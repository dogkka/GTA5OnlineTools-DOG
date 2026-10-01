#include "ScriptEventProtection.hpp"

#include <chrono>
#include <ctime>
#include <deque>
#include <mutex>
#include <unordered_map>
#include <algorithm>

#include "core/commands/BoolCommand.hpp"

namespace YimMenu::Features
{
	static BoolCommand _ScriptEventProtection{"scripteventprotection", "脚本事件防护", "拦截恶意的脚本事件攻击（声音轰炸、假通知、强制传送、踢出载具等）", true};

	static std::mutex g_LogMutex;
	static std::deque<BlockedEventEntry> g_Log;
	static std::unordered_map<int, unsigned long long> g_AttackerCounts;
	static std::unordered_map<int, std::string> g_AttackerNames;
	static unsigned long long g_BlockCount = 0;

	void PushProtectionLog(const std::string& player, int player_id, const std::string& event, std::uint32_t event_hash)
	{
		auto now = std::chrono::system_clock::now();
		const auto t = std::chrono::system_clock::to_time_t(now);
		char time_str[16]{};
		std::strftime(time_str, sizeof(time_str), "%H:%M:%S", std::localtime(&t));

		std::lock_guard lock(g_LogMutex);

		g_BlockCount++;
		g_AttackerCounts[player_id]++;
		if (!player.empty())
			g_AttackerNames[player_id] = player;

		if (g_Log.size() >= 100)
			g_Log.pop_front();

		g_Log.push_back({time_str, player, player_id, event, event_hash});
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
			out.push_back({name, id, count});
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
		g_BlockCount = 0;
	}
}
