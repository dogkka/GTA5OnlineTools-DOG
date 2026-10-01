#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Object.hpp"

namespace YimMenu::Features
{
	// drop a safe on the target's head
	class SafeDrop : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto target = player.GetPed();
			if (!target)
				return;

			std::uint32_t model = 0;
			for (auto candidate : {"h4_prop_h4_safe_01a"_J, "prop_ld_int_safe_01"_J, "h4_prop_h4_safe_01b"_J})
			{
				if (STREAMING::IS_MODEL_IN_CDIMAGE(candidate))
				{
					model = candidate;
					break;
				}
			}

			if (model == 0)
			{
				Notifications::Show("天降保险箱", "当前游戏版本没有可用的保险箱模型。", NotificationType::Warning);
				return;
			}

			const auto pos = target.GetPosition();

			auto safe = Object::Create(model, rage::fvector3{pos.x, pos.y, pos.z + 14.0f});
			if (!safe)
			{
				Notifications::Show("天降保险箱", "掉落失败，请重试。", NotificationType::Error);
				return;
			}

			ENTITY::SET_ENTITY_VELOCITY(safe.GetHandle(), 0.0f, 0.0f, -40.0f);

			Notifications::Show("天降保险箱", "保险箱砸向目标头顶。", NotificationType::Success);
		}
	};

	static SafeDrop _SafeDrop{"safedrop", "天降保险箱", "在目标头顶砸下一个保险箱"};
}
