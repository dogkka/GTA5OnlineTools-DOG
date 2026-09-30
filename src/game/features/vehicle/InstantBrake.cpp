#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class InstantBrake : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (!veh)
				return;

			VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh.GetHandle(), 0.0f);
		}
	};

	static InstantBrake _InstantBrake{"instantbrake", "Instant Brake", "Instantly stops the current vehicle"};
}
