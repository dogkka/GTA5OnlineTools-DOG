#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Object.hpp"

namespace YimMenu::Features
{
	// shower of gift boxes around the target player
	class GiftRain : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto target = player.GetPed();
			if (!target)
				return;

			std::uint32_t model = 0;
			for (auto candidate : {"hei_prop_heist_box"_J, "prop_cs_giftbox"_J, "prop_gift_box_01"_J})
			{
				if (STREAMING::IS_MODEL_IN_CDIMAGE(candidate))
				{
					model = candidate;
					break;
				}
			}

			if (model == 0)
			{
				Notifications::Show("礼物雨", "当前游戏版本没有可用的礼物盒模型。", NotificationType::Warning);
				return;
			}

			const auto pos = target.GetPosition();

			int spawned = 0;
			for (int i = 0; i < 10; i++)
			{
				const float x = pos.x + (float)(rand() % 14 - 7);
				const float y = pos.y + (float)(rand() % 14 - 7);
				const float z = pos.z + 8.0f + (float)(rand() % 8);

				auto gift = Object::Create(model, rage::fvector3{x, y, z});
				if (!gift)
					continue;

				ENTITY::SET_ENTITY_VELOCITY(gift.GetHandle(), (float)(rand() % 4 - 2), (float)(rand() % 4 - 2), -8.0f);
				spawned++;
			}

			Notifications::Show("礼物雨", std::to_string(spawned) + " 个礼物盒从天而降。", NotificationType::Success);
		}
	};

	static GiftRain _GiftRain{"giftrain", "礼物雨", "在目标玩家周围空投一堆礼物盒"};
}
