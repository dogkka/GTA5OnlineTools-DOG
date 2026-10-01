#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Object.hpp"

namespace YimMenu::Features
{
	// cartoon-style piano (or vending machine) dropped on the target's head
	class PianoDrop : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto target = player.GetPed();
			if (!target)
				return;

			std::uint32_t model = 0;
			for (auto candidate : {"prop_piano_01"_J, "h4_prop_h4_piano_01a"_J, "prop_vend_soda_02"_J, "prop_ld_int_safe_01"_J})
			{
				if (STREAMING::IS_MODEL_IN_CDIMAGE(candidate))
				{
					model = candidate;
					break;
				}
			}

			if (model == 0)
			{
				Notifications::Show("天降钢琴", "当前游戏版本没有可用的钢琴模型。", NotificationType::Warning);
				return;
			}

			const auto pos = target.GetPosition();

			auto piano = Object::Create(model, rage::fvector3{pos.x, pos.y, pos.z + 15.0f});
			if (!piano)
			{
				Notifications::Show("天降钢琴", "掉落失败，请重试。", NotificationType::Error);
				return;
			}

			ENTITY::SET_ENTITY_VELOCITY(piano.GetHandle(), 0.0f, 0.0f, -45.0f);

			Notifications::Show("天降钢琴", "咣——天降大礼砸向目标。", NotificationType::Success);
		}
	};

	static PianoDrop _PianoDrop{"pianodrop", "天降钢琴", "在目标头顶砸下一架大钢琴（没有钢琴时掉落自动贩卖机）"};
}
