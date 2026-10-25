#include "core/commands/ListCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/features/players/vehicle/VehicleTrollHelpers.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static ListCommand _LockVehicleMode{"lockvehiclemode", "锁定模式", "选择锁定还是解锁", {{0, "锁定"}, {1, "解锁"}}, 0};

	class LockVehicle : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = player.GetPed().GetVehicle();
			if (!veh)
			{
				Notifications::Show("锁车", "该玩家不在载具中。", NotificationType::Warning);
				return;
			}

			const int lock_state = _LockVehicleMode.GetState() == 0 ? 2 : 1;

			ApplyVehicleTroll(veh.GetHandle(), [lock_state](int handle) {
				VEHICLE::SET_VEHICLE_DOORS_LOCKED(handle, lock_state);
			});
		}
	};

	static LockVehicle _LockVehicle{"lockvehicle", "锁车", "锁定或解锁目标玩家的载具"};
}
