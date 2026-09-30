#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "types/pad/ControllerInputs.hpp"

namespace YimMenu::Features
{
	class VehicleJump : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			PAD::DISABLE_CONTROL_ACTION(0, (int)ControllerInputs::INPUT_VEH_HANDBRAKE, false);

			auto veh = Self::GetVehicle();
			if (!veh)
				return;

			if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, (int)ControllerInputs::INPUT_VEH_HANDBRAKE))
				ENTITY::APPLY_FORCE_TO_ENTITY(veh.GetHandle(), 1, 0.0f, 0.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0, 0, 1, 1, 0, 1);
		}
	};

	static VehicleJump _VehicleJump{"vehjump", "载具跳跃", "按手刹（空格）让载具跳跃，手刹功能被占用"};
}
