#include "ScriptEventProtection.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <ctime>
#include <deque>
#include <mutex>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include "core/commands/BoolCommand.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/util/Joaat.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static BoolCommand _ScriptEventProtection{"scripteventprotection", "脚本事件防护", "拦截恶意的脚本事件攻击（声音轰炸、假通知、强制传送、踢出载具等）", true};

	static BoolCommand _ProtKick{"protkick", "拦截 · 踢出类", "拦截 CEO/室内/载具踢出、强制断线等", true};
	static BoolCommand _ProtTransport{"prottransport", "拦截 · 传送类", "拦截强制传送 / 室内控制", true};
	static BoolCommand _ProtAnnoy{"protannoy", "拦截 · 骚扰类", "拦截声音轰炸、假通知/短信/横幅、赏金、观战等", true};
	static BoolCommand _ProtMission{"protmission", "拦截 · 任务类", "拦截强制任务/活动/脚本、CEO 突袭等", true};
	static BoolCommand _ProtAsset{"protasset", "拦截 · 资产类", "拦截 CEO 资金、个人载具、收集品等事件", true};
	static BoolCommand _ProtCrash{"protcrash", "拦截 · 崩溃类", "拦截撞库崩溃类事件", true};
	static BoolCommand _ProtLogOnly{"protlogonly", "只记录不拦截（观察）", "命中防护时只写日志和通知，但放行事件；名单不受影响", false};

	static IntCommand _ProtTriggerCount{"prottriggercount", "自动应对 · 阈值", "同一玩家被拦截达到该次数后触发自动应对", 1, 500, 15};
	static ListCommand _ProtAutoResponse{"protautoresponse", "自动应对 · 动作", "达到阈值后的动作：仅警告 / 拉黑名单（无条件拦截其事件）/ 自动退出战局", {{0, "关闭"}, {1, "仅警告"}, {2, "拉黑名单"}, {3, "自动退出战局"}}, 0};

	static std::atomic<bool> g_RequestLeaveSession{false};

	static std::mutex g_LogMutex;
	static std::deque<BlockedEventEntry> g_Log;
	static std::unordered_map<int, unsigned long long> g_AttackerCounts;
	static std::unordered_map<int, std::string> g_AttackerNames;
	static std::unordered_map<int, std::uint64_t> g_AttackerRids;
	static std::unordered_map<std::uint64_t, std::string> g_Whitelist;
	static std::unordered_map<std::uint64_t, std::string> g_Blacklist;
	static std::unordered_set<int> g_Responded;
	static unsigned long long g_BlockCount = 0;

	bool IsRidWhitelisted(std::uint64_t rid)
	{
		std::lock_guard lock(g_LogMutex);
		return g_Whitelist.contains(rid);
	}

	bool IsRidBlacklisted(std::uint64_t rid)
	{
		std::lock_guard lock(g_LogMutex);
		return g_Blacklist.contains(rid);
	}

	void AddRidToWhitelist(std::uint64_t rid, const std::string& name)
	{
		std::lock_guard lock(g_LogMutex);
		g_Whitelist[rid] = name;
	}

	void RemoveRidFromWhitelist(std::uint64_t rid)
	{
		std::lock_guard lock(g_LogMutex);
		g_Whitelist.erase(rid);
	}

	void AddRidToBlacklist(std::uint64_t rid, const std::string& name)
	{
		std::lock_guard lock(g_LogMutex);
		g_Blacklist[rid] = name;
	}

	void RemoveRidFromBlacklist(std::uint64_t rid)
	{
		std::lock_guard lock(g_LogMutex);
		g_Blacklist.erase(rid);
	}

	std::vector<RidEntry> GetWhitelistSnapshot()
	{
		std::lock_guard lock(g_LogMutex);
		std::vector<RidEntry> out;
		out.reserve(g_Whitelist.size());
		for (auto& [rid, name] : g_Whitelist)
			out.push_back({name, rid});
		return out;
	}

	std::vector<RidEntry> GetBlacklistSnapshot()
	{
		std::lock_guard lock(g_LogMutex);
		std::vector<RidEntry> out;
		out.reserve(g_Blacklist.size());
		for (auto& [rid, name] : g_Blacklist)
			out.push_back({name, rid});
		return out;
	}

	bool IsProtectionCategoryEnabled(const char* event_name)
	{
		static const std::unordered_map<std::string_view, std::string_view> s_Categories = {
		    {"CeoKick", "protkick"},
		    {"KickFromInterior", "protkick"},
		    {"VehicleKick", "protkick"},
		    {"NetworkBail", "protkick"},
		    {"Remote Teleport", "prottransport"},
		    {"InteriorControl", "prottransport"},
		    {"SendTextLabelSMS", "protannoy"},
		    {"Fake Money Notification", "protannoy"},
		    {"Notification", "protannoy"},
		    {"GtaBanner", "protannoy"},
		    {"SoundSpam", "protannoy"},
		    {"Bounty", "protannoy"},
		    {"RemoteOffradar", "protannoy"},
		    {"Spectate", "protannoy"},
		    {"RequestRandomEvent", "protannoy"},
		    {"ClearWantedLevel", "protannoy"},
		    {"SetSkydiveCompleted", "protannoy"},
		    {"ForceMission", "protmission"},
		    {"StartActivity", "protmission"},
		    {"StartScript", "protmission"},
		    {"TSECommand", "protmission"},
		    {"MarkPlayerAsBeast", "protmission"},
		    {"TriggerCEORaid", "protmission"},
		    {"CeoMoney", "protasset"},
		    {"CeoBan", "protasset"},
		    {"PersonalVehicle", "protasset"},
		    {"Collectible", "protasset"},
		    {"Crash", "protcrash"},
		    {"NotificationCrash", "protcrash"},
		};

		if (auto it = s_Categories.find(event_name); it != s_Categories.end())
		{
			if (const auto cmd = Commands::GetCommand<BoolCommand>(Joaat(it->second)))
				return cmd->GetState();
		}

		return true;
	}

	bool IsProtectionLogOnly()
	{
		return _ProtLogOnly.GetState();
	}

	static void EvaluateAutoResponse(const std::string& player, int player_id, std::uint64_t rid, unsigned long long count)
	{
		const int mode = _ProtAutoResponse.GetState();
		if (mode <= 0)
			return;

		const unsigned long long threshold = static_cast<unsigned long long>(_ProtTriggerCount.GetState());
		if (count < threshold)
			return;

		{
			std::lock_guard lock(g_LogMutex);
			if (!g_Responded.insert(player_id).second)
				return;
		}

		const std::string name = player.empty() ? ("ID " + std::to_string(player_id)) : player;

		switch (mode)
		{
		case 1:
			Notifications::Show("防护提醒", name + " 正在持续攻击你（已拦截 " + std::to_string(count) + " 次事件）", NotificationType::Error);
			break;
		case 2:
			if (rid)
				AddRidToBlacklist(rid, name);
			Notifications::Show("防护", "已将 " + name + " 加入黑名单，其事件将被无条件拦截。", NotificationType::Error);
			break;
		case 3:
			if (rid)
				AddRidToBlacklist(rid, name);
			g_RequestLeaveSession = true;
			Notifications::Show("防护", "遭到 " + name + " 持续攻击，正在自动退出战局…", NotificationType::Error);
			break;
		}
	}

	void PushProtectionLog(const std::string& player, int player_id, std::uint64_t rid, const std::string& event, std::uint32_t event_hash)
	{
		auto now = std::chrono::system_clock::now();
		const auto t = std::chrono::system_clock::to_time_t(now);
		char time_str[16]{};
		std::strftime(time_str, sizeof(time_str), "%H:%M:%S", std::localtime(&t));

		unsigned long long count = 0;
		{
			std::lock_guard lock(g_LogMutex);

			g_BlockCount++;
			count = ++g_AttackerCounts[player_id];
			if (!player.empty())
				g_AttackerNames[player_id] = player;
			if (rid)
				g_AttackerRids[player_id] = rid;

			if (g_Log.size() >= 100)
				g_Log.pop_front();

			g_Log.push_back({time_str, player, player_id, rid, event, event_hash});
		}

		EvaluateAutoResponse(player, player_id, rid, count);
	}

	std::vector<BlockedEventEntry> GetProtectionLogSnapshot()
	{
		std::lock_guard lock(g_LogMutex);
		return {g_Log.begin(), g_Log.end()};
	}

	std::vector<AttackerEntry> GetTopAttackers(int limit)
	{
		std::lock_guard lock(g_LogMutex);

		std::vector<AttackerEntry> out;
		for (auto& [id, count] : g_AttackerCounts)
		{
			std::string name = "<未知>";
			if (auto it = g_AttackerNames.find(id); it != g_AttackerNames.end())
				name = it->second;
			std::uint64_t rid = 0;
			if (auto it = g_AttackerRids.find(id); it != g_AttackerRids.end())
				rid = it->second;
			out.push_back({name, id, rid, count});
		}

		std::sort(out.begin(), out.end(), [](const AttackerEntry& a, const AttackerEntry& b) {
			return a.Count > b.Count;
		});

		if (static_cast<int>(out.size()) > limit)
			out.resize(limit);

		return out;
	}

	unsigned long long GetProtectionBlockCount()
	{
		std::lock_guard lock(g_LogMutex);
		return g_BlockCount;
	}

	void ClearProtectionLog()
	{
		std::lock_guard lock(g_LogMutex);
		g_Log.clear();
		g_AttackerCounts.clear();
		g_AttackerNames.clear();
		g_AttackerRids.clear();
		g_Responded.clear();
		g_BlockCount = 0;
	}

	// 内部：处理"自动退出战局"请求（在菜单线程执行，避免在网络事件回调里直接离局）
	class ProtectionAutoResponder : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (!g_RequestLeaveSession.exchange(false))
				return;

			if (NETWORK::NETWORK_IS_IN_SESSION())
				NETWORK::NETWORK_SESSION_LEAVE(0);
		}
	};

	static ProtectionAutoResponder _ProtectionAutoResponder{"protautoresponder", "防护 · 自动应对处理器", "内部功能：处理自动退出战局请求"};
}
