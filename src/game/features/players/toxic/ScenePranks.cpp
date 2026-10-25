#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"

#include <cmath>
#include <cstdlib>

namespace YimMenu::Features
{
	// 城区大火：在目标周围点起一片火海（本地可见）
	class CityFire : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto target = player.GetPed();
			if (!target)
				return;

			const auto pos = target.GetPosition();

			for (int i = 0; i < 24; i++)
			{
				const float x = pos.x + (float)(rand() % 31 - 15);
				const float y = pos.y + (float)(rand() % 31 - 15);
				FIRE::START_SCRIPT_FIRE(x, y, pos.z + 0.5f, 1, false);
			}

			Notifications::Show("城区大火", "火势已蔓延（本地可见）。", NotificationType::Success);
		}
	};

	// 火圈：目标脚下围一圈火
	class FireRing : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto target = player.GetPed();
			if (!target)
				return;

			const auto pos = target.GetPosition();

			for (int i = 0; i < 12; i++)
			{
				const float angle = (float)i / 12.0f * 6.28318f;
				FIRE::START_SCRIPT_FIRE(pos.x + cosf(angle) * 4.0f, pos.y + sinf(angle) * 4.0f, pos.z + 0.5f, 1, false);
			}
		}
	};

	static CityFire _CityFire{"cityfire", "城区大火", "在目标周围点起一片火海（本地可见）"};
	static FireRing _FireRing{"firering", "火圈", "在目标脚下围一圈火（本地可见）"};
}
