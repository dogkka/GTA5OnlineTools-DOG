#include "SavedPlayerData.hpp"

namespace YimMenu
{
	std::string_view YimMenu::FetchedPlayerData::GameStateToString(GameState state)
	{
		if (state == GameState::INVALID)
			return "无效";

		if (state < GameState::INVALID || state >= GameState::MAX)
			return "被作弊玩家隐藏";

		static constexpr std::array game_states =
		    {
		        "仅邀请",
		        "仅好友",
		        "封闭帮会",
		        "帮会",
		        "任务",
		        "公开",
		    };

		return game_states[static_cast<int>(state)];
	}

	std::string_view FetchedPlayerData::MissionTypeToString(MissionType type)
	{
		switch (type)
		{
		case MissionType::NONE: return "无";
		case MissionType::MISSION: return "任务";
		case MissionType::DEATHMATCH: return "死斗";
		case MissionType::RACE: return "竞速";
		case MissionType::SURVIVAL: return "生存战";
		case MissionType::GANG_ATTACK: return "帮派攻击";
		case MissionType::GOLF: return "高尔夫";
		case MissionType::TENNIS: return "网球";
		case MissionType::SHOOTING_RANGE: return "射击场";
		}

		return "未知";
	}
}
