#pragma once
#include <string>
#include <vector>

namespace YimMenu::Features
{
	struct BlockedEventEntry
	{
		std::string Time;
		std::string Player;
		std::string Event;
	};

	void PushProtectionLog(const std::string& player, const std::string& event);
	std::vector<BlockedEventEntry> GetProtectionLogSnapshot();
	unsigned long long GetProtectionBlockCount();
	void ClearProtectionLog();
}
