#include <cmath>

#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Pools.hpp"

namespace YimMenu::Features
{
	class Blackhole : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 2)
				return;

			m_Timer = 0;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			const auto pos     = ped.GetPosition();
			const auto heading = ped.GetHeading();
			const float rad    = heading * 3.14159265f / 180.0f;

			// singularity 12m in front of the player
			const float cx = pos.x - sinf(rad) * 12.0f;
			const float cy = pos.y + cosf(rad) * 12.0f;
			const float cz = pos.z + 2.0f;

			auto pull = [&](int handle) {
				if (handle == ped.GetHandle())
					return;

				const auto p    = ENTITY::GET_ENTITY_COORDS(handle, true);
				const float dx  = cx - p.x;
				const float dy  = cy - p.y;
				const float dz  = cz - p.z;
				const float dsq = dx * dx + dy * dy + dz * dz;

				if (dsq > 60.0f * 60.0f || dsq < 1.0f)
					return;

				const float dist = sqrtf(dsq);
				const float k    = 1500.0f / dsq;

				ENTITY::APPLY_FORCE_TO_ENTITY(handle, 1, dx / dist * k, dy / dist * k, dz / dist * k, 0.0f, 0.0f, 0.0f, 0, false, false, false, false, true);
			};

			for (auto veh : Pools::GetVehicles())
				pull(veh.GetHandle());

			for (auto iter : Pools::GetPeds())
			{
				if (!iter.IsPlayer())
					pull(iter.GetHandle());
			}

			for (auto obj : Pools::GetObjects())
				pull(obj.GetHandle());
		}
	};

	static Blackhole _Blackhole{"blackhole", "黑洞", "在前方制造引力奇点，吸附附近的载具、行人与物体"};
}
