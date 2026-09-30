#include "game/commands/PlayerCommand.hpp"
#include "core/commands/ListCommand.hpp"
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
				return;

			VEHICLE::SET_VEHICLE_DOORS_LOCKED(veh.GetHandle(), _LockVehicleMode.GetState() == 0 ? 2 : 1);
		}
	};

	static LockVehicle _LockVehicle{"lockvehicle", "锁车门", "锁定或解锁目标玩家的载具"};
}
