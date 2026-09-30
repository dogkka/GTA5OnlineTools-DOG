#include "core/frontend/Notifications.hpp"
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
}
