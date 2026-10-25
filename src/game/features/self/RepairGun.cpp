#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/RayCast.hpp"
#include "game/gta/Vehicle.hpp"
#include "types/pad/ControllerInputs.hpp"

namespace YimMenu::Features
{
	class RepairGun : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (!PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM))
				return;

			if (!PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_ATTACK))
				return;

			Entity target(nullptr);
			RayCast raycast(&target);
			if (!raycast.Cast() || !target.IsValid() || !target.IsVehicle())
				return;

			Vehicle(target.GetHandle()).Fix();
		}
	};

	static RepairGun _RepairGun{"repairgun", "修理枪", "瞄准载具按住射击键，持续修复"};
}
