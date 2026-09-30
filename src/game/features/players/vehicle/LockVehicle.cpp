#include "game/commands/PlayerCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static ListCommand _LockVehicleMode{"lockvehiclemode", "Lock Mode", "Whether to lock or unlock the player's vehicle", {{0, "Lock"}, {1, "Unlock"}}, 0};

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

	static LockVehicle _LockVehicle{"lockvehicle", "Lock Vehicle", "Locks or unlocks the player's vehicle"};
}
