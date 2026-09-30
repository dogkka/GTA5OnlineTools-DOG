#include "core/commands/LoopedCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/RayCast.hpp"
#include "types/pad/ControllerInputs.hpp"

namespace YimMenu::Features
{
	class DeleteGun : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (!PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM))
				return;

			if (!PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, (int)ControllerInputs::INPUT_ATTACK))
				return;

			Entity target(nullptr);
			RayCast raycast(&target);
			if (!raycast.Cast() || !target.IsValid())
				return;

			target.Delete();
		}
	};

	static DeleteGun _DeleteGun{"deletegun", "删除枪", "瞄准后射击删除准星落点的实体"};
}
