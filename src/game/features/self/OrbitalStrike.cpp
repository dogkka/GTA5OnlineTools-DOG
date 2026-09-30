#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/RayCast.hpp"
#include "types/fx/ExplosionType.hpp"
#include "types/pad/ControllerInputs.hpp"

namespace YimMenu::Features
{
	class OrbitalStrike : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (!PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM))
				return;

			if (!PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, (int)ControllerInputs::INPUT_ATTACK))
				return;

			Vector3 hit{};
			RayCast raycast(&hit);
			if (!raycast.Cast())
				return;

			FIRE::ADD_EXPLOSION(hit.x, hit.y, hit.z, (int)ExplosionType::ORBITAL_CANNON, 10.0f, true, false, 2.0f, false);
		}
	};

	static OrbitalStrike _OrbitalStrike{"orbitalstrike", "天基炮", "瞄准后点击射击，在准星落点降下轨道炮打击"};
}
