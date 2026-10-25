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
		std::uint64_t Rid;
		std::string Event;
		std::uint32_t EventHash;
	};

	struct AttackerEntry
	{
		std::string Player;
		int PlayerId;
		std::uint64_t Rid;
		unsigned long long Count;
	};

	struct RidEntry
	{
		std::string Player;
		std::uint64_t Rid;
	};

	void PushProtectionLog(const std::string& player, int player_id, std::uint64_t rid, const std::string& event, std::uint32_t event_hash);
	std::vector<BlockedEventEntry> GetProtectionLogSnapshot();
	std::vector<AttackerEntry> GetTopAttackers(int limit = 5);
	unsigned long long GetProtectionBlockCount();
	void ClearProtectionLog();

	// 防护分组 / 只记录模式
	bool IsProtectionCategoryEnabled(const char* event_name);
	bool IsProtectionLogOnly();

	// 名单（按 RID）
	bool IsRidWhitelisted(std::uint64_t rid);
	bool IsRidBlacklisted(std::uint64_t rid);
	void AddRidToWhitelist(std::uint64_t rid, const std::string& name);
	void RemoveRidFromWhitelist(std::uint64_t rid);
	void AddRidToBlacklist(std::uint64_t rid, const std::string& name);
	void RemoveRidFromBlacklist(std::uint64_t rid);
	std::vector<RidEntry> GetWhitelistSnapshot();
	std::vector<RidEntry> GetBlacklistSnapshot();
}
