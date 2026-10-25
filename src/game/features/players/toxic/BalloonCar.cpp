#include "core/backend/ScriptMgr.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Object.hpp"

#include <chrono>
#include <cstdlib>

namespace YimMenu::Features
{
	// 气球挂车：给目标载具挂上一串气球，并持续施加升力让其缓缓升空（本地可见）
	class BalloonCar : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto target = player.GetPed();
			if (!target)
				return;

			auto veh = target.GetVehicle();
			if (!veh)
			{
				Notifications::Show("气球挂车", "目标不在载具中。", NotificationType::Warning);
				return;
			}

			const auto vehPos = veh.GetPosition();

			for (int i = 0; i < 6; i++)
			{
				const float x = vehPos.x + (float)(rand() % 7 - 3);
				const float y = vehPos.y + (float)(rand() % 7 - 3);
				const float z = vehPos.z + 3.0f + (float)(rand() % 2);
				Object::Create("xm3_prop_xm3_balloon_01a"_J, rage::fvector3{x, y, z});
			}

			Notifications::Show("气球挂车", "气球已就位，准备升空——", NotificationType::Success);

			for (int i = 0; i < 100; i++)
			{
				if (!ENTITY::DOES_ENTITY_EXIST(veh.GetHandle()))
					return;

				ENTITY::SET_ENTITY_VELOCITY(veh.GetHandle(), 0.0f, 0.0f, 2.0f + static_cast<float>(i) * 0.06f);
				ScriptMgr::Yield(std::chrono::milliseconds(100));
			}
		}
	};

	static BalloonCar _BalloonCar{"ballooncar", "气球挂车", "给目标载具挂上气球并缓缓升空（本地可见）"};
}
