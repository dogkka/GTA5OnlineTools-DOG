#include "game/commands/PlayerCommand.hpp"
#include "types/script/ScriptEvent.hpp"

namespace YimMenu::Features
{
	class CeoKick : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			OnCall(std::vector{player});
		}

		virtual void OnCall(const std::vector<Player>& players) override
		{
			SCRIPT_EVENT_KICK_FROM_CEO message;
			for (auto& player : players)
				message.SetPlayer(player.GetId());
			message.Send();
		}
	};

	static CeoKick _CeoKick{"ceokick", "CEO 踢出", "将玩家踢出其组织或摩托帮"};
}
