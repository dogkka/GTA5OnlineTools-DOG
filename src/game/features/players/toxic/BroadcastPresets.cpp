#include "core/commands/Commands.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/StringCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "types/script/ScriptEvent.hpp"

#include <cstring>
#include <string>

namespace YimMenu::Features
{
	static ListCommand _BroadcastPreset{"broadcastpreset", "播报预设", "选择要发送的假播报文本（选择“自定义”时使用“假通知文本”）", {{0, "自定义（用下方文本）"}, {1, "管理员警告"}, {2, "系统公告"}, {3, "反作弊警告"}, {4, "移出战局"}, {5, "击杀播报"}, {6, "促销广告"}}, 0};

	// 发送播报：以横幅样式给目标发送所选假播报
	class BroadcastSend : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			std::string text;
			switch (_BroadcastPreset.GetState())
			{
			case 1: text = "警告：管理员正在监视本战局"; break;
			case 2: text = "系统公告：服务器将于 5 分钟后维护"; break;
			case 3: text = "反作弊系统：检测到 1 名作弊者，正在处理"; break;
			case 4: text = "你已被移出此战局"; break;
			case 5: text = "击杀播报：玩家已被淘汰"; break;
			case 6: text = "限时促销：全部资产 1 折起！"; break;
			default:
				if (auto str_cmd = Commands::GetCommand<StringCommand>("fakenotiftext"_J))
					text = str_cmd->GetString();
				break;
			}

			if (text.empty())
				return;

			const int id   = player.GetId();
			const int bits = 1 << id;

			int64_t args[12]{};
			args[0] = static_cast<int64_t>(ScriptEventIndex::GtaBanner);
			args[1] = Self::GetPlayer().GetId();
			args[2] = bits;
			args[3] = 1;
			strncpy(reinterpret_cast<char*>(&args[4]), text.c_str(), 56);

			SCRIPT::_SEND_TU_SCRIPT_EVENT_NEW(1, args, 12, bits, static_cast<Hash>(args[0]));
		}
	};

	static BroadcastSend _BroadcastSend{"broadcastsend", "发送播报", "给目标发送所选假播报（横幅样式，实验性）"};
}
