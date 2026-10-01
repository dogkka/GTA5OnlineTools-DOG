#include <cmath>

#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Object.hpp"

namespace YimMenu::Features
{
	// classic "cage": drop a golden container on top of the target
	class CageTarget : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto target = player.GetPed();
			if (!target)
				return;

			const auto pos = target.GetPosition();

			auto cage = Object::Create("prop_gold_cont_01"_J, rage::fvector3{pos.x, pos.y, pos.z - 1.0f});
			if (!cage)
			{
				Notifications::Show("加笼子", "生成失败，请重试。", NotificationType::Error);
				return;
			}

			Notifications::Show("加笼子", "目标已被黄金集装箱扣在里面。", NotificationType::Success);
		}
	};

	// bigger frame: four containers boxing the target in
	class CageBoxTarget : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto target = player.GetPed();
			if (!target)
				return;

			const auto pos = target.GetPosition();

			struct Placement
			{
				float dx, dy, heading;
			};

			static const Placement placements[] = {
			    {0.0f, 6.0f, 0.0f},
			    {0.0f, -6.0f, 0.0f},
			    {-6.0f, 0.0f, 90.0f},
			    {6.0f, 0.0f, 90.0f},
			};

			int placed = 0;
			for (const auto& p : placements)
			{
				auto wall = Object::Create("prop_gold_cont_01"_J, rage::fvector3{pos.x + p.dx, pos.y + p.dy, pos.z - 1.0f});
				if (!wall)
					continue;

				ENTITY::SET_ENTITY_HEADING(wall.GetHandle(), p.heading);
				placed++;
			}

			Notifications::Show("集装箱围墙", std::to_string(placed) + "/4 面集装箱已围住目标。", NotificationType::Success);
		}
	};

	static CageTarget _CageTarget{"cagetarget", "加笼子", "在目标玩家身上扣一个黄金集装箱"};
	static CageBoxTarget _CageBoxTarget{"cagebox", "集装箱围墙", "用 4 个集装箱把目标围在中间"};
}
