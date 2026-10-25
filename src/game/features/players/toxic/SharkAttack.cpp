#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Ped.hpp"

namespace YimMenu::Features
{
	// beached sharks flopping around the target player
	class SharkAttack : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto target = player.GetPed();
			if (!target)
				return;

			const auto pos = target.GetPosition();

			static const std::uint32_t models[] = {"a_c_sharktiger"_J, "a_c_sharkhammer"_J};

			int spawned = 0;
			for (int i = 0; i < 4; i++)
			{
				auto model = models[i % 2];
				if (!STREAMING::IS_MODEL_IN_CDIMAGE(model))
					continue;

				auto shark = Ped::Create(model,
				    rage::fvector3{pos.x + (float)(rand() % 14 - 7), pos.y + (float)(rand() % 14 - 7), pos.z + 0.5f},
				    (float)(rand() % 360));
				if (!shark)
					continue;

				const int handle = shark.GetHandle();

				// let them flop: random tilt + a bit of velocity so they wiggle around
				ENTITY::SET_ENTITY_ROTATION(handle, (float)(rand() % 40 - 20), 180.0f, (float)(rand() % 360), 0, true);
				ENTITY::SET_ENTITY_VELOCITY(handle, (float)(rand() % 6 - 3), (float)(rand() % 6 - 3), 0.5f);

				spawned++;
			}

			if (spawned > 0)
				Notifications::Show("鲨鱼上岸", std::to_string(spawned) + " 条鲨鱼在目标脚边搁浅扑腾。", NotificationType::Success);
			else
				Notifications::Show("鲨鱼上岸", "鲨鱼模型不可用（当前游戏版本）。", NotificationType::Warning);
		}
	};

	static SharkAttack _SharkAttack{"sharkattack", "鲨鱼上岸", "在目标玩家周围扔下 4 条搁浅的鲨鱼"};
}
