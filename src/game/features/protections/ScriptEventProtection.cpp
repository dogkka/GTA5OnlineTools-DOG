#include "ScriptEventProtection.hpp"

#include <chrono>
#include <ctime>
#include <deque>
#include <mutex>

#include "core/commands/BoolCommand.hpp"

namespace YimMenu::Features
{
	static BoolCommand _ScriptEventProtection{"scripteventprotection", "脚本事件防护", "拦截恶意的脚本事件攻击（声音轰炸、假通知、强制传送、踢出载具等）", true};

	static std::mutex g_LogMutex;
	static std::deque<BlockedEventEntry> g_Log;
	static unsigned long long g_BlockCount = 0;

	void PushProtectionLog(const std::string& player, const std::string& event)
	{
		auto now = std::chrono::system_clock::now();
		const auto t = std::chrono::system_clock::to_time_t(now);
		char time_str[16]{};
		std::strftime(time_str, sizeof(time_str), "%H:%M:%S", std::localtime(&t));

		std::lock_guard lock(g_LogMutex);

		g_BlockCount++;

		if (g_Log.size() >= 100)
			g_Log.pop_front();

		g_Log.push_back({time_str, player, event});
	}

	std::vector<BlockedEventEntry> GetProtectionLogSnapshot()
	{
		std::lock_guard lock(g_LogMutex);
		return {g_Log.begin(), g_Log.end()};
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
		g_BlockCount = 0;
	}
}
