#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	// spawns a health pickup at the target player's feet (server-synced pickup,
	// they can pick it up for real)
	class SupplyDrop : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto target = player.GetPed();
			if (!target)
				return;

			const auto pos = target.GetPosition();

			OBJECT::CREATE_PICKUP_ROTATE(0x8F707C18, // PICKUP_HEALTH_STANDARD
			    pos.x + 1.5f,
			    pos.y,
			    pos.z + 0.3f,
			    0.0f,
			    0.0f,
			    0.0f,
			    8,
			    1,
			    0,
			    false,
			    0);

			Notifications::Show("补给空投", "已在该玩家脚边投放一个医疗包。", NotificationType::Success);
		}
	};

	static SupplyDrop _SupplyDrop{"supplydrop", "补给空投", "在目标玩家脚边投放一个医疗包"};
}
