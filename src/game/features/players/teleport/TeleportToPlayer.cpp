#include "game/backend/Self.hpp"
#include "game/commands/PlayerCommand.hpp"

namespace YimMenu::Features
{
	class TeleportToPlayer : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			if (auto ped = player.GetPed())
				Self::GetPed().TeleportTo(ped.GetPosition());
		}
	};

	static TeleportToPlayer _TeleportToPlayer{"tptoplayer", "传送到玩家", "将你传送到所选玩家身边"};
}
