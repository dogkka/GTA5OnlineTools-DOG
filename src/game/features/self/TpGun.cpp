#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/RayCast.hpp"
#include "types/pad/ControllerInputs.hpp"

namespace YimMenu::Features
{
	class TpGun : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (!PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM))
				return;

			if (!PAD::IS_DISABLED_CONTROL_JUST_RELEASED(0, (int)ControllerInputs::INPUT_ATTACK))
				return;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			Vector3 coords{};
			RayCast raycast(&coords);
			if (!raycast.Cast())
				return;

			ped.SetPosition(rage::fvector3{coords.x, coords.y, coords.z});
		}
	};

	static TpGun _TpGun{"tpgun", "传送枪", "瞄准后松开射击键，传送到准星落点"};
}
