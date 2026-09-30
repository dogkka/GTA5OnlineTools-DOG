#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "types/pad/ControllerInputs.hpp"

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

			// only fire the instant stop at the moment brake / handbrake is pressed,
			// so the vehicle can still drive normally otherwise
			const bool triggered = PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, (int)ControllerInputs::INPUT_VEH_BRAKE)
			    || PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, (int)ControllerInputs::INPUT_VEH_HANDBRAKE);

			if (!triggered)
				return;

			ENTITY::SET_ENTITY_VELOCITY(veh.GetHandle(), 0.0f, 0.0f, 0.0f);
			VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh.GetHandle(), 0.0f);
		}
	};

	static InstantBrake _InstantBrake{"instantbrake", "瞬间刹车", "行驶中按下刹车（S）或手刹（空格）时瞬间停车"};
}
