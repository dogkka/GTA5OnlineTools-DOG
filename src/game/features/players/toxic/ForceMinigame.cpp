#include <iterator>
#include <vector>

#include "core/commands/ListCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Scripts.hpp"

namespace YimMenu::Features
{
	static const struct
	{
		joaat_t Hash;
		const char* Name;
	} g_Minigames[] = {
	    {"golf_mp"_J, "高尔夫"},
	    {"AM_Darts"_J, "飞镖"},
	    {"Pilot_School_MP"_J, "飞行学校"},
	    {"gunslinger_arcade"_J, "街机：荒野枪手"},
	    {"ggsm_arcade"_J, "街机：太空猴"},
	    {"wizard_arcade"_J, "街机：巫师"},
	    {"camhedz_arcade"_J, "街机：拍照达人"},
	    {"puzzle"_J, "街机：Qub3D"},
	    {"fm_intro"_J, "新手教程"},
	};

	static std::vector<std::pair<int, const char*>> MakeMinigameList()
	{
		std::vector<std::pair<int, const char*>> list;
		for (int i = 0; i < (int)std::size(g_Minigames); i++)
			list.push_back({i, g_Minigames[i].Name});
		return list;
	}

	static ListCommand _Minigame{"minigame", "小游戏", "选择要强制目标进入的小游戏", MakeMinigameList(), 0};

	class ForceMinigame : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			const int index = _Minigame.GetState();
			if (index < 0 || index >= (int)std::size(g_Minigames))
				return;

			Scripts::ForceScriptOnPlayer(g_Minigames[index].Hash, 1 << player.GetId());
			Notifications::Show("强制小游戏", std::string("已让目标进入：") + g_Minigames[index].Name, NotificationType::Success);
		}
	};

	static ForceMinigame _ForceMinigame{"forceminigame", "强制小游戏", "把目标玩家拉进指定小游戏"};
}
