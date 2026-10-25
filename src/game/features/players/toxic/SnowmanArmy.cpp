#include <cmath>

#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Object.hpp"

namespace YimMenu::Features
{
	// ring of snowmen around the target player
	class SnowmanArmy : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto target = player.GetPed();
			if (!target)
				return;

			// try several known snowman / decoration models
			std::uint32_t model = 0;
			for (auto candidate : {"prop_snowman_01"_J, "prop_snowman_02"_J, "prop_mannequin_01"_J, "prop_statue_01"_J})
			{
				if (STREAMING::IS_MODEL_IN_CDIMAGE(candidate))
				{
					model = candidate;
					break;
				}
			}

			if (model == 0)
			{
				Notifications::Show("雪人军团", "当前游戏版本没有可用的雪人模型。", NotificationType::Warning);
				return;
			}

			const auto pos = target.GetPosition();

			int placed = 0;
			for (int i = 0; i < 8; i++)
			{
				const float angle = (float)i / 8.0f * 6.28318f;
				const float x     = pos.x + cosf(angle) * 3.5f;
				const float y     = pos.y + sinf(angle) * 3.5f;

				auto snowman = Object::Create(model, rage::fvector3{x, y, pos.z});
				if (!snowman)
					continue;

				// face the target
				const float heading = atan2f(-(pos.x - x), pos.y - y) * 180.0f / 3.14159265f;
				ENTITY::SET_ENTITY_HEADING(snowman.GetHandle(), heading);

				placed++;
			}

			Notifications::Show("雪人军团", std::to_string(placed) + " 个雪人把目标围了起来。", NotificationType::Success);
		}
	};

	static SnowmanArmy _SnowmanArmy{"snowmanarmy", "雪人军团", "在目标玩家周围摆下一圈雪人"};
}
