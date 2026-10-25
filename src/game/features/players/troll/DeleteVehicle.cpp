#include "game/commands/PlayerCommand.hpp"

namespace YimMenu::Features
{
	class DeleteVehicle : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			if (auto ped = player.GetPed())
				Entity::DeleteNetwork(ped.GetVehicleObjectId());
		}
	};

	static DeleteVehicle _DeleteVehicle{"deleteveh", "删除载具", "删除玩家当前所在的载具"};
}
