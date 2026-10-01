#include <winsock2.h>
#include <ws2tcpip.h>

#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Version.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/commands/StringCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/scripting/LuaManager.hpp"
#include "core/util/Joaat.hpp"
#include "game/backend/Players.hpp"
#include "game/backend/Self.hpp"
#include "game/features/protections/ScriptEventProtection.hpp"
#include "game/gta/Natives.hpp"
#include "types/script/ScriptEvent.hpp"

namespace YimMenu::Features
{
	// ---------------------------------------------------------------- commands

	static BoolCommand _WebConsoleLan{"webconsolelan", "允许局域网访问", "开启后同一局域网内的手机/电脑也能访问（建议同时设置访问令牌）", false};
	static IntCommand _WebConsolePort{"webconsoleport", "控制台端口", "网页控制台的监听端口", 1024, 65535, 7777};
	static StringCommand _WebConsoleToken{"webconsoletoken", "访问令牌", "访问网页控制台所需的令牌；留空则不校验（仅本机访问时可留空）", ""};

	// ---------------------------------------------------------------- helpers

	static std::string JsonEscape(std::string_view s)
	{
		std::string out;
		out.reserve(s.size() + 8);
		for (char c : s)
		{
			switch (c)
			{
			case '"': out += "\\\""; break;
			case '\\': out += "\\\\"; break;
			case '\n': out += "\\n"; break;
			case '\r': out += "\\r"; break;
			case '\t': out += "\\t"; break;
			default:
				if (static_cast<unsigned char>(c) < 0x20)
				{
					char buf[8];
					std::snprintf(buf, sizeof(buf), "\\u%04x", c);
					out += buf;
				}
				else
					out += c;
			}
		}
		return out;
	}

	static std::string UrlDecode(std::string_view in)
	{
		std::string out;
		out.reserve(in.size());
		auto hex = [](char ch) -> int {
			if (ch >= '0' && ch <= '9') return ch - '0';
			if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
			if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
			return -1;
		};

		for (size_t i = 0; i < in.size(); i++)
		{
			char c = in[i];
			if (c == '+')
				out += ' ';
			else if (c == '%' && i + 2 < in.size())
			{
				int hi = hex(in[i + 1]), lo = hex(in[i + 2]);
				if (hi >= 0 && lo >= 0)
				{
					out += static_cast<char>((hi << 4) | lo);
					i += 2;
				}
				else
					out += c;
			}
			else
				out += c;
		}
		return out;
	}

	static std::unordered_map<std::string, std::string> ParseQuery(const std::string& query)
	{
		std::unordered_map<std::string, std::string> out;
		size_t pos = 0;
		while (pos < query.size())
		{
			auto amp = query.find('&', pos);
			if (amp == std::string::npos)
				amp = query.size();

			auto pair = query.substr(pos, amp - pos);
			if (auto eq = pair.find('='); eq != std::string::npos)
				out[UrlDecode(pair.substr(0, eq))] = UrlDecode(pair.substr(eq + 1));
			else if (!pair.empty())
				out[UrlDecode(pair)] = "";

			pos = amp + 1;
		}
		return out;
	}

	// 纯数字解析（支持十进制与 0x 十六进制）
	static int64_t ParseNumber(const std::string& s)
	{
		if (s.empty())
			return 0;

		char* end = nullptr;
		long long value = std::strtoll(s.c_str(), &end, 0);
		if (end && *end == '\0' && end != s.c_str())
			return value;
		return 0;
	}

	// 事件哈希解析：数字优先，否则当作名称做 Joaat
	static int64_t ParseHash(const std::string& s)
	{
		if (s.empty())
			return 0;

		char* end = nullptr;
		long long value = std::strtoll(s.c_str(), &end, 0);
		if (end && *end == '\0' && end != s.c_str())
			return value;
		return static_cast<int64_t>(static_cast<int32_t>(Joaat(s)));
	}

	// 在游戏脚本线程（fiber）上执行并等待结果
	static std::string RunOnGameThread(std::function<std::string()> fn, int timeout_ms = 2500)
	{
		struct Sync
		{
			std::mutex m;
			std::condition_variable cv;
			bool done = false;
			std::string value = "{\"ok\":false,\"error\":\"no result\"}";
		};

		auto sync = std::make_shared<Sync>();
		FiberPool::Push([sync, fn = std::move(fn)] {
			std::string value;
			try
			{
				value = fn();
			}
			catch (const std::exception& e)
			{
				value = "{\"ok\":false,\"error\":\"exception: " + JsonEscape(e.what()) + "\"}";
			}
			catch (...)
			{
				value = "{\"ok\":false,\"error\":\"unknown exception\"}";
			}

			{
				std::lock_guard lock(sync->m);
				sync->value = std::move(value);
				sync->done = true;
			}
			sync->cv.notify_all();
		});

		std::unique_lock lock(sync->m);
		if (!sync->cv.wait_for(lock, std::chrono::milliseconds(timeout_ms), [&] { return sync->done; }))
			return "{\"ok\":false,\"error\":\"游戏繁忙或未进入战局（等待超时）\"}";
		return sync->value;
	}

	static void RunOnGameThreadFire(std::function<void()> fn)
	{
		FiberPool::Push(std::move(fn));
	}

	// 玩家列表 JSON（必须在游戏线程上构建）
	static std::string PlayersJson()
	{
		rage::fvector3 self_pos{};
		bool have_self_pos = false;
		if (auto self_ped = Self::GetPed())
		{
			self_pos = self_ped.GetPosition();
			have_self_pos = true;
		}

		std::string out = "{\"ok\":true,\"players\":[";
		bool first = true;
		for (auto& [id, player] : Players::GetPlayers())
		{
			if (!player.IsValid())
				continue;

			std::string dist = "-1";
			if (!player.IsLocal() && have_self_pos)
			{
				if (auto ped = player.GetPed())
					dist = std::to_string(static_cast<int>(self_pos.GetDistance(ped.GetPosition())));
			}

			if (!first)
				out += ",";
			first = false;

			out += "{\"id\":" + std::to_string(player.GetId())
			    + ",\"name\":\"" + JsonEscape(player.GetName())
			    + "\",\"rid\":" + std::to_string(player.GetRID())
			    + ",\"local\":" + (player.IsLocal() ? "true" : "false")
			    + ",\"host\":" + (player.IsHost() ? "true" : "false")
			    + ",\"dist\":" + dist + "}";
		}
		out += "]}";
		return out;
	}

	// 执行命令（支持数值/文本参数），必须在游戏线程上执行
	static std::string ExecCommandJson(const std::string& name, const std::string& arg)
	{
		Command* target = Commands::GetCommand(Joaat(name));
		if (!target)
		{
			for (auto& [hash, command] : Commands::GetCommands())
			{
				if (command->GetLabel() == name)
				{
					target = command;
					break;
				}
			}
		}

		if (!target)
			return "{\"ok\":false,\"error\":\"未找到命令\"}";

		if (!arg.empty())
		{
			if (auto int_cmd = dynamic_cast<IntCommand*>(target))
				int_cmd->SetState(std::atoi(arg.c_str()));
			else if (auto float_cmd = dynamic_cast<FloatCommand*>(target))
				float_cmd->SetState(static_cast<float>(std::atof(arg.c_str())));
			else if (auto string_cmd = dynamic_cast<StringCommand*>(target))
				string_cmd->SetStringValue(arg);
		}

		const std::string label = target->GetLabel();
		target->Call();
		return "{\"ok\":true,\"executed\":\"" + JsonEscape(label) + "\"}";
	}

	// 脚本事件参数构造：支持整数（十进制/0x）与 s: 前缀的原始字符串
	struct EventArgsBuilder
	{
		int64_t args[32]{};
		size_t slot      = 3;
		size_t byte_off  = 0;

		void WriteInt(int64_t value)
		{
			if (slot >= std::size(args))
				return;

			if (byte_off != 0)
			{
				byte_off = 0;
				slot++;
			}

			if (slot < std::size(args))
				args[slot++] = value;
		}

		void WriteBytes(const std::string& text)
		{
			for (char c : text)
			{
				if (slot >= std::size(args))
					return;

				reinterpret_cast<char*>(&args[slot])[byte_off++] = c;
				if (byte_off == 8)
				{
					byte_off = 0;
					slot++;
				}
			}

			if (slot >= std::size(args))
				return;

			reinterpret_cast<char*>(&args[slot])[byte_off++] = '\0';
			if (byte_off == 8)
			{
				byte_off = 0;
				slot++;
			}
		}
	};

	// 发送脚本事件，必须在游戏线程上执行
	static std::string SendScriptEventJson(int64_t event_hash, int target_id, const std::vector<std::string>& params)
	{
		if (target_id < 0 || target_id >= 32)
			return "{\"ok\":false,\"error\":\"目标 ID 无效\"}";

		if (!Self::GetPlayer().IsValid())
			return "{\"ok\":false,\"error\":\"未进入战局\"}";

		if (target_id == Self::GetPlayer().GetId())
			return "{\"ok\":false,\"error\":\"不能对自己发送\"}";

		const int bits = 1 << target_id;

		EventArgsBuilder builder{};
		builder.args[0] = event_hash;
		builder.args[1] = Self::GetPlayer().GetId();
		builder.args[2] = bits;

		for (auto& param : params)
		{
			if (param.rfind("s:", 0) == 0)
				builder.WriteBytes(param.substr(2));
			else
				builder.WriteInt(ParseNumber(param));
		}

		int count = static_cast<int>(builder.slot);
		if (builder.byte_off != 0)
			count++;
		if (count < 4)
			count = 4;
		if (count > 32)
			count = 32;

		SCRIPT::_SEND_TU_SCRIPT_EVENT_NEW(1, builder.args, count, bits, static_cast<Hash>(event_hash));
		return "{\"ok\":true,\"event\":" + std::to_string(event_hash) + ",\"target\":" + std::to_string(target_id) + ",\"count\":" + std::to_string(count) + "}";
	}

	// ---------------------------------------------------------------- web page

	static const char* HtmlPage()
	{
		return R"HTML(<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>狗哥远程控制台</title>
<style>
body{background:#12141a;color:#e8e8e8;font-family:system-ui,sans-serif;margin:0;padding:14px}
h2{margin:4px 0 14px}
.card{background:#1a1d25;border:1px solid #2a2e3a;border-radius:10px;padding:12px;margin-bottom:12px}
button{background:#2d6cdf;color:#fff;border:0;border-radius:6px;padding:6px 10px;margin:2px;cursor:pointer;font-size:13px}
button.g{background:#3a4150}
button.r{background:#c0392b}
input{background:#12141a;color:#eee;border:1px solid #2f3542;border-radius:6px;padding:6px;margin:2px}
.dim{color:#8b93a1;font-size:12px;margin-top:6px;word-break:break-all;white-space:pre-wrap}
.player{border-top:1px solid #262b36;padding:8px 0}
.badge{background:#2d6cdf;border-radius:4px;padding:1px 6px;font-size:12px;margin-left:6px}
.badge.host{background:#c9a227;color:#111}
</style>
</head>
<body>
<h2>🐶 狗哥远程控制台</h2>

<div class="card" id="status">加载中…</div>

<div class="card">
  <b>玩家列表</b>
  <div id="players" class="dim">加载中…</div>
  <div class="dim">先点玩家右边的「选中」，再点动作按钮。</div>
  <span id="selinfo" class="dim"></span>
  <div>
    <button onclick="act('explode')">爆炸</button>
    <button onclick="act('loopshake')">循环摇晃</button>
    <button onclick="act('loopexplode')">循环爆炸</button>
    <button onclick="act('loopragdoll')">循环跌倒</button>
    <button onclick="act('cagetarget')">加笼子</button>
    <button onclick="act('sendbounty')">送赏金</button>
    <button onclick="act('vehkick')">踢出载具</button>
    <button class="r" onclick="act('loopkill')">循环击杀</button>
  </div>
</div>

<div class="card">
  <b>命令执行</b><br>
  <input id="cmd" placeholder="命令名 / 中文名" style="min-width:200px">
  <input id="cmdarg" placeholder="数值/文本参数(可选)" style="width:150px">
  <button onclick="execCmd()">执行</button>
  <div class="dim" id="cmdout"></div>
</div>

<div class="card">
  <b>脚本事件发送器</b><br>
  <input id="evhash" placeholder="事件哈希 (0x… 或十进制)" style="width:190px">
  <input id="evtarget" placeholder="目标ID" style="width:70px">
  <input id="evparams" placeholder="参数: 1 2 3 或 s:文本" style="min-width:200px">
  <button onclick="sendEvent()">发送</button>
  <div class="dim" id="evout"></div>
</div>

<div class="card">
  <b>快捷</b><br>
  <button class="g" onclick="api('/api/lua/reload')">重载全部 Lua</button>
  <button class="g" onclick="api('/api/exec?name=mmfulllobby')">一键满员模式</button>
  <button class="g" onclick="api('/api/exec?name=locklobby')">锁战局 开/关</button>
  <button class="g" onclick="api('/api/exec?name=anticage')">防笼子 开/关</button>
  <button class="g" onclick="api('/api/exec?name=scripteventprotection')">脚本防护 开/关</button>
  <div class="dim" id="out"></div>
</div>

<script>
var T = new URLSearchParams(location.search).get('t') || '';
var SEL = -1;
function u(x){ return x + (x.indexOf('?') >= 0 ? '&' : '?') + 't=' + encodeURIComponent(T); }
function api(x){
  return fetch(u(x)).then(function(r){ return r.json(); }).then(function(j){
    document.getElementById('out').textContent = JSON.stringify(j);
    return j;
  }).catch(function(e){ document.getElementById('out').textContent = '错误: ' + e; });
}
function pick(id){
  SEL = id;
  document.getElementById('selinfo').textContent = '已选中玩家 ID ' + id;
}
function act(name){
  if (SEL < 0) { document.getElementById('out').textContent = '请先选中一名玩家'; return; }
  fetch(u('/api/select?id=' + SEL)).then(function(){ return api('/api/exec?name=' + name); });
}
function execCmd(){
  var n = document.getElementById('cmd').value.trim();
  var a = document.getElementById('cmdarg').value.trim();
  if (!n) return;
  api('/api/exec?name=' + encodeURIComponent(n) + '&arg=' + encodeURIComponent(a)).then(function(j){
    document.getElementById('cmdout').textContent = JSON.stringify(j);
  });
}
function sendEvent(){
  var h = document.getElementById('evhash').value.trim();
  var t = document.getElementById('evtarget').value.trim();
  var ps = document.getElementById('evparams').value.trim().split(/\s+/);
  var q = '/api/scriptevent?event=' + encodeURIComponent(h) + '&target=' + encodeURIComponent(t);
  for (var i = 0; i < ps.length && i < 8; i++)
    if (ps[i]) q += '&p' + (i + 1) + '=' + encodeURIComponent(ps[i]);
  api(q).then(function(j){ document.getElementById('evout').textContent = JSON.stringify(j); });
}
function refresh(){
  fetch(u('/api/status')).then(function(r){ return r.json(); }).then(function(s){
    document.getElementById('status').textContent =
      '版本 ' + s.version + ' ｜ 玩家 ' + (s.player || '?') + ' (ID ' + s.selfId + ') ｜ 战局人数 ' + s.players + ' ｜ ' + (s.inSession ? '在线' : '不在战局');
  }).catch(function(){});
  fetch(u('/api/players')).then(function(r){ return r.json(); }).then(function(j){
    var el = document.getElementById('players');
    el.innerHTML = '';
    var list = j.players || [];
    if (!list.length) { el.textContent = '战局内没有其他玩家'; return; }
    list.forEach(function(p){
      var d = document.createElement('div');
      d.className = 'player';
      var name = document.createElement('span');
      name.textContent = p.name;
      var head = document.createElement('b');
      head.textContent = p.id + ' ';
      d.appendChild(head);
      d.appendChild(name);
      if (p.local) d.insertAdjacentHTML('beforeend', '<span class="badge">我</span>');
      if (p.host) d.insertAdjacentHTML('beforeend', '<span class="badge host">主机</span>');
      d.insertAdjacentHTML('beforeend', '<span class="dim"> RID ' + p.rid + ' 距离 ' + (p.dist >= 0 ? p.dist + 'm' : '-') + '</span>');
      if (!p.local) {
        var b = document.createElement('button');
        b.className = 'g';
        b.textContent = '选中';
        b.onclick = function(){ pick(p.id); };
        d.appendChild(b);
      }
      el.appendChild(d);
    });
  }).catch(function(){});
}
refresh();
setInterval(refresh, 5000);
</script>
</body>
</html>
)HTML";
	}

	// ---------------------------------------------------------------- server

	class WebConsoleServer
	{
		std::thread m_Thread;
		SOCKET m_Listen = INVALID_SOCKET;
		std::atomic<bool> m_Running{false};

	public:
		bool Start(int port, bool lan)
		{
			if (m_Running)
				return true;

			WSADATA wsa{};
			if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
				return false;

			m_Listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (m_Listen == INVALID_SOCKET)
			{
				WSACleanup();
				return false;
			}

			BOOL reuse = TRUE;
			setsockopt(m_Listen, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuse), sizeof(reuse));

			sockaddr_in addr{};
			addr.sin_family = AF_INET;
			addr.sin_port   = htons(static_cast<u_short>(port));
			if (lan)
				addr.sin_addr.s_addr = INADDR_ANY;
			else
				inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

			if (bind(m_Listen, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR
			    || listen(m_Listen, 8) == SOCKET_ERROR)
			{
				closesocket(m_Listen);
				m_Listen = INVALID_SOCKET;
				WSACleanup();
				return false;
			}

			m_Running = true;
			m_Thread  = std::thread([this] { Loop(); });
			return true;
		}

		void Stop()
		{
			if (!m_Running)
				return;

			m_Running = false;
			if (m_Thread.joinable())
				m_Thread.join();

			if (m_Listen != INVALID_SOCKET)
			{
				closesocket(m_Listen);
				m_Listen = INVALID_SOCKET;
			}

			WSACleanup();
		}

		bool IsRunning() const
		{
			return m_Running;
		}

	private:
		void Loop()
		{
			while (m_Running)
			{
				fd_set fds;
				FD_ZERO(&fds);
				FD_SET(m_Listen, &fds);

				timeval tv{};
				tv.tv_usec = 300000;

				const int ready = select(0, &fds, nullptr, nullptr, &tv);
				if (ready <= 0)
					continue;

				SOCKET client = accept(m_Listen, nullptr, nullptr);
				if (client == INVALID_SOCKET)
					continue;

				HandleClient(client);
				closesocket(client);
			}
		}

		void HandleClient(SOCKET client)
		{
			char buffer[8192]{};
			const int received = recv(client, buffer, sizeof(buffer) - 1, 0);
			if (received <= 0)
				return;

			std::string request(buffer, received);
			const auto line_end = request.find("\r\n");
			const std::string request_line = request.substr(0, line_end == std::string::npos ? request.size() : line_end);

			std::string path, query;
			{
				const auto first_space = request_line.find(' ');
				if (first_space == std::string::npos)
					return;

				const auto second_space = request_line.find(' ', first_space + 1);
				std::string target = request_line.substr(first_space + 1,
				    second_space == std::string::npos ? std::string::npos : second_space - first_space - 1);

				const auto qpos = target.find('?');
				path  = qpos == std::string::npos ? target : target.substr(0, qpos);
				query = qpos == std::string::npos ? "" : target.substr(qpos + 1);
			}

			auto params = ParseQuery(query);

			std::string status       = "200 OK";
			std::string content_type = "application/json; charset=utf-8";
			std::string body;

			const auto token = _WebConsoleToken.GetString();
			if (!token.empty() && params["t"] != token)
			{
				status = "403 Forbidden";
				body   = "{\"ok\":false,\"error\":\"访问令牌无效\"}";
			}
			else if (path == "/" || path == "/index.html")
			{
				content_type = "text/html; charset=utf-8";
				body         = HtmlPage();
			}
			else if (path == "/api/status")
			{
				body = RunOnGameThread([]() -> std::string {
					std::string player_name = "未进入战局";
					int self_id             = -1;
					if (auto self = Self::GetPlayer())
					{
						player_name = self.GetName();
						self_id     = self.GetId();
					}

					return "{\"ok\":true,\"version\":\"" + std::string(Build::Version)
					    + "\",\"tag\":\"" + JsonEscape(Build::Tag)
					    + "\",\"player\":\"" + JsonEscape(player_name)
					    + "\",\"selfId\":" + std::to_string(self_id)
					    + ",\"players\":" + std::to_string(Players::GetPlayers().size())
					    + ",\"inSession\":" + (NETWORK::NETWORK_IS_SESSION_STARTED() ? "true" : "false") + "}";
				});
			}
			else if (path == "/api/players")
			{
				body = RunOnGameThread([]() -> std::string { return PlayersJson(); });
			}
			else if (path == "/api/select")
			{
				const int id = std::atoi(params["id"].c_str());
				body = RunOnGameThread([id]() -> std::string {
					if (!Players::GetPlayers().count(static_cast<uint8_t>(id)))
						return "{\"ok\":false,\"error\":\"玩家不存在\"}";

					Players::SetSelected(Player(static_cast<uint8_t>(id)));
					return std::string("{\"ok\":true,\"selected\":") + std::to_string(id) + "}";
				});
			}
			else if (path == "/api/exec")
			{
				const std::string name = params["name"];
				const std::string arg  = params["arg"];
				body = RunOnGameThread([name, arg]() -> std::string { return ExecCommandJson(name, arg); });
			}
			else if (path == "/api/notify")
			{
				const std::string text = params["text"];
				if (text.empty())
				{
					body = "{\"ok\":false,\"error\":\"缺少 text 参数\"}";
				}
				else
				{
					RunOnGameThreadFire([text] { Notifications::Show("远程消息", text, NotificationType::Info); });
					body = "{\"ok\":true}";
				}
			}
			else if (path == "/api/lua/reload")
			{
				// 先在锁内快照（回调内不能直接触发 Lua 操作，会与 LuaManager 的死锁保护冲突）
				static std::vector<std::shared_ptr<LuaScript>> s_ScriptsToReload;
				s_ScriptsToReload.clear();
				LuaManager::ForAllLoadedScripts(+[](std::shared_ptr<LuaScript>& script) {
					s_ScriptsToReload.push_back(script);
				});
				for (auto& script : s_ScriptsToReload)
					script->Reload();
				s_ScriptsToReload.clear();
				body = "{\"ok\":true,\"action\":\"reload-all-lua\"}";
			}
			else if (path == "/api/scriptevent")
			{
				const int64_t event_hash = ParseHash(params["event"]);
				const int target_id      = std::atoi(params["target"].c_str());

				std::vector<std::string> values;
				for (int i = 1; i <= 8; i++)
				{
					const auto key = "p" + std::to_string(i);
					if (auto it = params.find(key); it != params.end() && !it->second.empty())
						values.push_back(it->second);
				}

				body = RunOnGameThread([event_hash, target_id, values]() -> std::string {
					return SendScriptEventJson(event_hash, target_id, values);
				});
			}
			else if (path == "/api/log")
			{
				std::string out = "{\"ok\":true,\"count\":" + std::to_string(GetProtectionBlockCount()) + ",\"entries\":[";
				bool first = true;
				for (auto& entry : GetProtectionLogSnapshot())
				{
					if (!first)
						out += ",";
					first = false;
					out += "{\"time\":\"" + JsonEscape(entry.Time) + "\",\"player\":\"" + JsonEscape(entry.Player)
					    + "\",\"id\":" + std::to_string(entry.PlayerId) + ",\"rid\":" + std::to_string(entry.Rid)
					    + ",\"event\":\"" + JsonEscape(entry.Event) + "\",\"hash\":" + std::to_string(entry.EventHash) + "}";
				}
				out += "]}";
				body = out;
			}
			else
			{
				status = "404 Not Found";
				body   = "{\"ok\":false,\"error\":\"未知接口\"}";
			}

			std::string response = "HTTP/1.1 " + status + "\r\nContent-Type: " + content_type
			    + "\r\nContent-Length: " + std::to_string(body.size()) + "\r\nConnection: close\r\n\r\n" + body;

			size_t sent = 0;
			while (sent < response.size())
			{
				const int n = send(client, response.c_str() + sent, static_cast<int>(response.size() - sent), 0);
				if (n <= 0)
					break;
				sent += n;
			}
		}
	};

	static WebConsoleServer g_WebConsoleServer;

	class WebConsoleToggle : public BoolCommand
	{
		using BoolCommand::BoolCommand;

		virtual void OnEnable() override
		{
			if (!g_WebConsoleServer.Start(_WebConsolePort.GetState(), _WebConsoleLan.GetState()))
			{
				Notifications::Show("远程控制台", "启动失败：端口被占用？请换个端口再试。", NotificationType::Error);
				SetState(false);
				return;
			}

			Notifications::Show("远程控制台", "已启动：http://127.0.0.1:" + std::to_string(_WebConsolePort.GetState()) + "（浏览器打开即可）", NotificationType::Success);
		}

		virtual void OnDisable() override
		{
			g_WebConsoleServer.Stop();
			Notifications::Show("远程控制台", "已停止。", NotificationType::Warning);
		}
	};

	static WebConsoleToggle _WebConsole{"webconsole", "远程控制台", "开启内置网页控制台：用浏览器或手机远程查看战局、执行菜单功能（仅本机监听，可在下方开放局域网）", true};
}
