#include "game/commands/PlayerCommand.hpp"
#include "types/script/ScriptEvent.hpp"

namespace YimMenu::Features
{
	class SendSext : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		static constexpr auto sm_Strippers = std::to_array({"HCH", "INF", "JUL", "NIK", "SAP", "TXI"});
		static constexpr auto sm_SextTypes = std::to_array({"1ST", "2ND", "NEED"});

		virtual void OnCall(Player player) override
		{
			OnCall(std::vector{player});
		}

		virtual void OnCall(const std::vector<Player>& players) override
		{
			SCRIPT_EVENT_SEND_TEXT_LABEL_MESSAGE message;
			strncpy(message.Label.Data, std::format("SXT_{}_{}", sm_Strippers[rand() % sm_Strippers.size()], sm_SextTypes[rand() % sm_SextTypes.size()]).c_str(), 24);
			for (auto& player : players)
				message.SetPlayer(player.GetId());
			message.Send();
		}
	};

	static SendSext _SendSext{"sendsext", "发送暧昧短信", "向指定玩家发送一条随机暧昧短信"};
}
