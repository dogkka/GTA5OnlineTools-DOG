#include "game/commands/PlayerCommand.hpp"
#include "types/script/globals/GSBD_Kicking.hpp"
#include "game/gta/Scripts.hpp"

namespace YimMenu::Features
{
	class ScriptHostKick : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			if (auto freemode = Scripts::FindScriptThread("freemode"_J); freemode && Scripts::SafeToModifyFreemodeBroadcastGlobals())
			{
				Scripts::ForceScriptHost(freemode);
				GSBD_Kicking::Get()->KickedPlayers[player.GetId()] = true;
			}
		}
	};

	static ScriptHostKick _ScriptHostKick{"shkick", "脚本主机踢出", "通过伪造踢出投票将玩家移出战局。"};
}
