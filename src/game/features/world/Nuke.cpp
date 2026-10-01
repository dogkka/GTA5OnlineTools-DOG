#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "types/fx/ExplosionType.hpp"

namespace YimMenu::Features
{
	class Nuke : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto ped = Self::GetPed();
			if (!ped)
				return;

			const auto pos = ped.GetPosition();

			// pretend we're far enough away to be safe: strike 40m ahead
			const auto heading = ped.GetHeading();
			const float rad    = heading * 3.14159265f / 180.0f;
			const float x      = pos.x - sinf(rad) * 40.0f;
			const float y      = pos.y + cosf(rad) * 40.0f;
			const float z      = pos.z;

			// mushroom cloud: stacked delayed explosions
			for (int i = 0; i < 8; i++)
			{
				const float ox = (float)(rand() % 16 - 8);
				const float oy = (float)(rand() % 16 - 8);
				const float oz = (float)(i * 6);

				FIRE::ADD_EXPLOSION(x + ox, y + oy, z + oz, (int)ExplosionType::ORBITAL_CANNON, 30.0f, true, false, 4.0f, false);
			}

			CAMERA::SHAKE_GAMEPLAY_CAM("LARGE_EXPLOSION_SHAKE", 2.0f);

			Notifications::Show("核爆", "前方 40 米已升起蘑菇云。", NotificationType::Warning);
		}
	};

	static Nuke _Nuke{"nuke", "核爆", "在前方 40 米降下核打击（超大爆炸+镜头震动）"};
}
