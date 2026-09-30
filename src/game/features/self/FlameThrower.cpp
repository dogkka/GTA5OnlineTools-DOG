#include "core/commands/LoopedCommand.hpp"
#include "core/util/Math.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/RayCast.hpp"
#include "types/pad/ControllerInputs.hpp"

namespace YimMenu::Features
{
	class FlameThrower : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (!PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM))
				return;

			if (!PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_ATTACK))
				return;

			if (++m_Timer < 4)
				return;

			m_Timer = 0;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			const auto cam     = CAMERA::GET_GAMEPLAY_CAM_COORD();
			const auto rot     = CAMERA::GET_GAMEPLAY_CAM_ROT(0);
			const auto dir     = Math::RotationToDirection(rage::fvector3{rot.x, rot.y, rot.z});

			Vector3 end{cam.x + dir.x * 40.0f, cam.y + dir.y * 40.0f, cam.z + dir.z * 40.0f};

			Entity target(nullptr);
			Vector3 hit{};
			RayCast raycast(&target, &hit);
			if (raycast.Cast())
				end = hit;

			// lay a stream of fire along the aim line
			for (int i = 1; i <= 5; i++)
			{
				const float t  = (float)i / 5.0f;
				const float fx = cam.x + (end.x - cam.x) * t;
				const float fy = cam.y + (end.y - cam.y) * t;
				const float fz = cam.z + (end.z - cam.z) * t;
				FIRE::START_SCRIPT_FIRE(fx, fy, fz, 0, false);
			}

			// ignite whatever is at the end of the stream
			if (target.IsValid() && (target.IsPed() || target.IsVehicle()))
				FIRE::START_ENTITY_FIRE(target.GetHandle());
		}
	};

	static FlameThrower _FlameThrower{"flamethrower", "喷火器", "瞄准并按住射击键向前方喷射火焰"};
}
