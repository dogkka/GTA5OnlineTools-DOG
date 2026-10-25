#include "game/commands/PlayerCommand.hpp"
#include "game/backend/Self.hpp"

namespace YimMenu::Features
{
	class Bring : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			player.TeleportTo(Self::GetPed().GetPosition());
		}
	};

	static Bring _Bring{"bring", "传送到身边", "将玩家传送到你身边。"};
}
