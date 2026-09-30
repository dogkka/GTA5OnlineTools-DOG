#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Object.hpp"

namespace YimMenu::Features
{
	class MoneyRain : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto ped = player.GetPed();
			if (!ped)
				return;

			const auto pos = ped.GetPosition();

			for (int i = 0; i < 10; i++)
			{
				const float x = pos.x + (float)(rand() % 10 - 5);
				const float y = pos.y + (float)(rand() % 10 - 5);
				const float z = pos.z + 6.0f + (float)(rand() % 10);

				auto bag = Object::Create("prop_money_bag_01"_J, rage::fvector3{x, y, z});
				if (!bag)
					continue;

				ENTITY::SET_ENTITY_VELOCITY(bag.GetHandle(), (float)(rand() % 6 - 3), (float)(rand() % 6 - 3), -5.0f);
			}

			Notifications::Show("钱雨", "已在该玩家头顶撒下钱袋。", NotificationType::Success);
		}
	};

	static MoneyRain _MoneyRain{"moneyrain", "钱雨", "在目标玩家头顶掉下 10 个钱袋"};
}
