#include "core/frontend/Notifications.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/commands/StringCommand.hpp"
#include "game/backend/Players.hpp"
#include "game/backend/Self.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "types/script/globals/GlobalPlayerBD.hpp"
#include "types/script/globals/GPBD_FM_3.hpp"
#include "types/script/ScriptEvent.hpp"

namespace YimMenu::Features
{
	static void SendScriptEvent(int64_t* args, int arg_count, int bits)
	{
		SCRIPT::_SEND_TU_SCRIPT_EVENT_NEW(1, args, arg_count, bits, static_cast<Hash>(args[0]));
	}

	class SendBounty : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			const int id   = player.GetId();
			const int bits = 1 << id;

			int64_t args[23]{};
			args[0] = static_cast<int64_t>(ScriptEventIndex::Bounty);
			args[1] = Self::GetPlayer().GetId();
			args[2] = bits;
			args[3] = id;    // target
			args[4] = 0;     // flags
			args[5] = 10000; // amount

			SendScriptEvent(args, 23, bits);
		}
	};

	class FakeBanMessage : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			const int id   = player.GetId();
			const int bits = 1 << id;

			int64_t args[9]{};
			args[0] = static_cast<int64_t>(ScriptEventIndex::SendTextLabelSMS);
			args[1] = Self::GetPlayer().GetId();
			args[2] = bits;
			strcpy(reinterpret_cast<char*>(&args[3]), "HUD_ROSBANPERM");

			SendScriptEvent(args, 9, bits);
		}
	};

	class VehicleKick : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			const int id   = player.GetId();
			const int bits = 1 << id;

			int64_t args[10]{};
			args[0] = static_cast<int64_t>(ScriptEventIndex::VehicleKick);
			args[1] = Self::GetPlayer().GetId();
			args[2] = bits;

			SendScriptEvent(args, 10, bits);
		}
	};

	class TriggerCeoRaid : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			const int id   = player.GetId();
			const int bits = 1 << id;

			int64_t args[4]{};
			args[0] = static_cast<int64_t>(ScriptEventIndex::TriggerCEORaid);
			args[1] = Self::GetPlayer().GetId();
			args[2] = bits;

			SendScriptEvent(args, 4, bits);
		}
	};

	class ForceMission : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			const int id   = player.GetId();
			const int bits = 1 << id;

			int64_t args[4]{};
			args[0] = static_cast<int64_t>(ScriptEventIndex::ForceMission);
			args[1] = Self::GetPlayer().GetId();
			args[2] = bits;

			SendScriptEvent(args, 4, bits);
		}
	};

	class ShowTransactionError : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			const int id   = player.GetId();
			const int bits = 1 << id;

			int64_t args[9]{};
			args[0] = static_cast<int64_t>(-830063381); // TransactionError (commented out in ScriptEventIndex)
			args[1] = Self::GetPlayer().GetId();
			args[2] = bits;
			args[3] = 1;
			args[6] = GPBD_FM_3::Get()->Entries[id].ScriptEventReplayProtectionCounter;

			SendScriptEvent(args, 9, bits);
		}
	};

	class KickFromInterior : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			const int id   = player.GetId();
			const int bits = 1 << id;

			const auto interior = GlobalPlayerBD::Get()->Entries[id].SimpleInteriorData;

			int64_t args[9]{};
			args[0] = static_cast<int64_t>(ScriptEventIndex::KickFromInterior);
			args[1] = Self::GetPlayer().GetId();
			args[2] = bits;
			args[3] = static_cast<int64_t>(interior.Index);
			args[4] = interior.InstanceId;

			SendScriptEvent(args, 9, bits);
		}
	};

	static SendBounty _SendBounty{"sendbounty", "送赏金", "给目标玩家挂上 10000 赏金"};
	static FakeBanMessage _FakeBanMessage{"fakeban", "假封号短信", "给目标玩家发送一条假的封号提示"};
	static VehicleKick _VehicleKick{"vehkick", "踢出载具", "把目标玩家从载具里踢出去"};
	static TriggerCeoRaid _TriggerCeoRaid{"ceoraid", "触发 CEO 突袭", "对目标玩家的 CEO 组织触发突袭"};
	static ForceMission _ForceMission{"forcemission", "强制任务", "强制目标玩家进入任务状态"};
	static ShowTransactionError _ShowTransactionError{"transerror", "假交易错误", "给目标玩家弹出交易错误提示"};
	static KickFromInterior _KickFromInterior{"intkick", "室内踢出", "把目标玩家从室内空间踢出去"};

	// ------------------------------------------------ 实验性：通知 / 横幅 / 声音轰炸
	// 以下事件的参数布局为社区常用发送端对齐版本（文本写入 args[3] 起的字节区，最多 56 字节）。
	// 如实测无效，可通过"调试 → 脚本事件发送器"或网页控制台调整布局。

	static StringCommand _FakeNotifText{"fakenotiftext", "假通知文本", "假通知 / 假横幅中显示的文本", "你被管理员盯上了"};
	static StringCommand _SoundSpamSound{"soundspamsound", "声音名称", "声音轰炸播放的音效名称", "CELL_APTINVYACHT"};

	class FakeNotification : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			const int id   = player.GetId();
			const int bits = 1 << id;
			const auto text = _FakeNotifText.GetString();

			int64_t args[12]{};
			args[0] = static_cast<int64_t>(ScriptEventIndex::Notification);
			args[1] = Self::GetPlayer().GetId();
			args[2] = bits;
			args[3] = 1; // 通知类型
			strncpy(reinterpret_cast<char*>(&args[4]), text.c_str(), 56);

			SendScriptEvent(args, 12, bits);
		}
	};

	class FakeBanner : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			const int id   = player.GetId();
			const int bits = 1 << id;
			const auto text = _FakeNotifText.GetString();

			int64_t args[12]{};
			args[0] = static_cast<int64_t>(ScriptEventIndex::GtaBanner);
			args[1] = Self::GetPlayer().GetId();
			args[2] = bits;
			args[3] = 1; // 横幅类型
			strncpy(reinterpret_cast<char*>(&args[4]), text.c_str(), 56);

			SendScriptEvent(args, 12, bits);
		}
	};

	class SoundSpam : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			const int id   = player.GetId();
			const int bits = 1 << id;
			const auto sound = _SoundSpamSound.GetString();

			int64_t args[12]{};
			args[0] = static_cast<int64_t>(ScriptEventIndex::SoundSpam);
			args[1] = Self::GetPlayer().GetId();
			args[2] = bits;
			strncpy(reinterpret_cast<char*>(&args[4]), sound.c_str(), 56);

			SendScriptEvent(args, 12, bits);
		}
	};

	class LoopSoundSpam : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 360)
				return;

			m_Timer = 0;

			auto player = Players::GetSelected();
			if (!player.IsValid() || player.IsLocal())
				return;

			const int id   = player.GetId();
			const int bits = 1 << id;
			const auto sound = _SoundSpamSound.GetString();

			int64_t args[12]{};
			args[0] = static_cast<int64_t>(ScriptEventIndex::SoundSpam);
			args[1] = Self::GetPlayer().GetId();
			args[2] = bits;
			strncpy(reinterpret_cast<char*>(&args[4]), sound.c_str(), 56);

			SendScriptEvent(args, 12, bits);
		}
	};

	static FakeNotification _FakeNotification{"fakenotification", "假通知", "给目标弹一条假的系统通知（实验性）"};
	static FakeBanner _FakeBanner{"fakebanner", "假横幅", "给目标屏幕上方弹一条假横幅（实验性）"};
	static SoundSpam _SoundSpam{"soundspam", "声音轰炸", "在目标处播放指定音效（实验性）"};
	static LoopSoundSpam _LoopSoundSpam{"loopsoundspam", "循环声音轰炸", "反复在目标处播放音效（每 6 秒）"};
}
