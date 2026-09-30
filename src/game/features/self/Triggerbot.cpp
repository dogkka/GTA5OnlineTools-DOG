#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/RayCast.hpp"
#include "types/pad/ControllerInputs.hpp"

namespace YimMenu::Features
{
	class Triggerbot : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (!PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM))
				return;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			Entity target(nullptr);
			RayCast raycast(&target);

			if (!raycast.Cast() || !target.IsValid() || !target.IsPed() || target.IsDead())
				return;

			if (target.GetHandle() == ped.GetHandle())
				return;

			PED::SET_PED_RESET_FLAG(ped.GetHandle(), 65, true);
		}
	};

	static Triggerbot _Triggerbot{"triggerbot", "自动开火", "准星对准人形时自动开火"};
}
