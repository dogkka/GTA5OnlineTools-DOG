#include <string>

#include "core/commands/IntCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "types/script/ScriptEvent.hpp"

namespace YimMenu::Features
{
	static IntCommand _PWantedStars{"pwantedstars", "通缉星级", "要设置给目标玩家的通缉等级（0~5）", 0, 5, 3};

	static void SendScriptEvent(int64_t* args, int arg_count, int bits)
	{
		SCRIPT::_SEND_TU_SCRIPT_EVENT_NEW(1, args, arg_count, bits, static_cast<Hash>(args[0]));
	}

	// 设置通缉（本机视角）：直接改本机对目标玩家的通缉状态 —— 星标与警察追捕在你这边立即生效。
	// 目标客户端的自身通缉由对方的游戏权威同步，是否跟随取决于其客户端与防护设置。
	class PlayerSetWanted : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			if (!player.IsValid() || player.IsLocal())
			{
				Notifications::Show("通缉管理", "请先选择一名其他玩家。", NotificationType::Warning);
				return;
			}

			const int level = _PWantedStars.GetState();
			const int id    = player.GetId();

			PLAYER::SET_PLAYER_WANTED_LEVEL(id, level, false);
			PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(id, false);

			Notifications::Show("通缉管理", "已在本机把目标设为 " + std::to_string(level) + " 星（对方是否同步取决于其客户端与防护）。", NotificationType::Success);
		}
	};

	// 清除通缉（远程事件）：发送 ClearWantedLevel 脚本事件，由对方客户端清除自身通缉。
	// 对方开启“事件防护/拦截脚本事件”时会被拦截。
	class PlayerClearWanted : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			if (!player.IsValid() || player.IsLocal())
			{
				Notifications::Show("通缉管理", "请先选择一名其他玩家。", NotificationType::Warning);
				return;
			}

			const int id   = player.GetId();
			const int bits = 1 << id;

			int64_t args[5]{};
			args[0] = static_cast<int64_t>(ScriptEventIndex::ClearWantedLevel);
			args[1] = Self::GetPlayer().GetId();
			args[2] = bits;

			SendScriptEvent(args, 5, bits);

			// 本机视角也清一遍，保证立即看到效果
			PLAYER::SET_PLAYER_WANTED_LEVEL(id, 0, false);
			PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(id, false);

			Notifications::Show("通缉管理", "已发送“清除通缉”事件（对方开事件防护时会被拦截）。", NotificationType::Success);
		}
	};

	static PlayerSetWanted _PlayerSetWanted{"pwantedapply", "设置通缉", "把目标玩家的通缉等级设为本机滑块数值（本机视角立即生效）"};
	static PlayerClearWanted _PlayerClearWanted{"pwantedclear", "清除通缉（事件）", "给目标玩家发送“清除通缉”事件，由对方客户端清除自身通缉"};
}
