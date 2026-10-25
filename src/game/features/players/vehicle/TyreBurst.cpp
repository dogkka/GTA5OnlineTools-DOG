#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/features/players/vehicle/VehicleTrollHelpers.hpp"
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
			{
				Notifications::Show("爆胎", "该玩家不在载具中。", NotificationType::Warning);
				return;
			}

			// 先抢控制权（空车同步生效）；对方驾驶中拿不到控制权时走轻伤伤害事件兜底
			ApplyVehicleTroll(
			    veh.GetHandle(),
			    [](int handle) {
				    for (int wheel = 0; wheel <= 5; wheel++)
					    VEHICLE::SET_VEHICLE_TYRE_BURST(handle, wheel, true, 1000.0f);
			    },
			    150.0f);
		}
	};

	static TyreBurst _TyreBurst{"tyreburst", "爆胎", "爆掉目标玩家载具的所有轮胎"};
}
