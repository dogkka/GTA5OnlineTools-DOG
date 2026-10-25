#include "game/commands/PlayerCommand.hpp"

namespace YimMenu::Features
{
	class Kill : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			if (auto ped = player.GetPed())
				ped.Kill();
		}
	};

	static Kill _Kill{"kill", "击杀", "击杀该玩家"};
}
