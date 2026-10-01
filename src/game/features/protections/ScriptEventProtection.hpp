#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace YimMenu::Features
{
	struct BlockedEventEntry
	{
		std::string Time;
		std::string Player;
		int PlayerId;
		std::string Event;
		std::uint32_t EventHash;
	};

	struct AttackerEntry
	{
		std::string Player;
		int PlayerId;
		unsigned long long Count;
	};

	void PushProtectionLog(const std::string& player, int player_id, const std::string& event, std::uint32_t event_hash);
	std::vector<BlockedEventEntry> GetProtectionLogSnapshot();
	std::vector<AttackerEntry> GetTopAttackers(int limit = 5);
	unsigned long long GetProtectionBlockCount();
	void ClearProtectionLog();
}
