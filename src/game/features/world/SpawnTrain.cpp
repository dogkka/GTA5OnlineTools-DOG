#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

#include <cmath>

namespace YimMenu::Features
{
	class SpawnTrain : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto ped = Self::GetPed();
			if (!ped)
				return;

			const auto pos     = ped.GetPosition();
			const auto heading = ped.GetHeading();
			const float rad    = heading * 3.14159265f / 180.0f;

			// spawn 60m ahead of the player
			const float x = pos.x - sinf(rad) * 60.0f;
			const float y = pos.y + cosf(rad) * 60.0f;

			const int variation = rand() % 12;
			const int train     = VEHICLE::CREATE_MISSION_TRAIN(variation, x, y, pos.z, true);

			if (train != 0)
				Notifications::Show("生成火车", "火车已生成（需要附近有铁轨）。", NotificationType::Success);
			else
				Notifications::Show("生成火车", "生成失败，请靠近铁轨再试。", NotificationType::Warning);
		}
	};

	static SpawnTrain _SpawnTrain{"spawntrain", "生成火车", "在你前方生成一节火车头（需靠近铁轨）"};
}
