#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class TyreBurst : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = player.GetPed().GetVehicle();
			if (!veh)
				return;

			for (int i = 0; i <= 5; i++)
				VEHICLE::SET_VEHICLE_TYRE_BURST(veh.GetHandle(), i, true, 1000.0f);
		}
	};

	static TyreBurst _TyreBurst{"tyreburst", "爆胎", "爆掉目标玩家载具的所有轮胎"};
}
