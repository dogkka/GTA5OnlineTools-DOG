#include "core/backend/ScriptMgr.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Players.hpp"
#include "game/backend/Self.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "types/script/ScriptEvent.hpp"
#include "types/script/globals/GPBD_FM_3.hpp"
#include "types/script/globals/GSBD_FM_Events.hpp"

#include <chrono>
#include <cstdint>
#include <cstdlib>

namespace YimMenu::Features
{
	// Stand 同款：负数取低 32 位后零扩展，脚本事件里的 int32 校验参数需要这种形式
	static uint64_t Trickery32(std::int32_t value)
	{
		return static_cast<uint64_t>(static_cast<std::uint32_t>(value));
	}

	static void SendRawEvent(std::int64_t* args, int arg_count, int bits)
	{
		SCRIPT::_SEND_TU_SCRIPT_EVENT_NEW(1, args, arg_count, bits, static_cast<Hash>(args[0]));
	}

	// 单次经验事件：GiveCollectible（收藏品被拾取），每次 +1000 RP
	// 参数布局与 Stand 的 Give RP 完全一致，type 8 + 索引 -5 会绕过"已收集"检查
	static void SendGiveRpOnce(Player player, std::int64_t type, std::int64_t index)
	{
		const int id   = player.GetId();
		const int bits = 1 << id;

		std::int64_t args[8]{};
		args[0] = static_cast<std::int64_t>(Trickery32(static_cast<std::int32_t>(ScriptEventIndex::GiveCollectible)));
		args[1] = Self::GetPlayer().GetId();
		args[2] = bits;
		args[3] = type;
		args[4] = index;
		args[5] = 1;
		args[6] = 1;
		args[7] = 1;

		SendRawEvent(args, 8, bits);
	}

	// CEO 发薪事件：args 布局与 Stand 的 CEO Money Loop 一致
	static void SendCeoPay(Player player, std::int64_t amount, std::int32_t guard, std::int64_t alt)
	{
		auto gpbd = GPBD_FM_3::Get();
		auto gsbd = GSBD_FM_Events::Get();
		if (!gpbd || !gsbd)
			return;

		const int id   = player.GetId();
		const int bits = 1 << id;

		std::int64_t args[9]{};
		args[0] = static_cast<std::int64_t>(Trickery32(static_cast<std::int32_t>(ScriptEventIndex::CeoMoney)));
		args[1] = Self::GetPlayer().GetId();
		args[2] = bits;
		args[3] = amount;
		args[4] = static_cast<std::int64_t>(Trickery32(guard));
		args[5] = alt;
		args[6] = gpbd->Entries[id].ScriptEventReplayProtectionCounter;
		args[7] = gsbd->AntiReplayValue1;
		args[8] = gsbd->AntiReplayValue2;

		SendRawEvent(args, 9, bits);
	}

	class GiveRp : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			if (!player.IsValid() || player.IsLocal())
				return;

			// 两个分支与 Stand 相同：随机走南瓜收藏品或普通收藏品路线
			if (std::rand() % 2 == 0)
			{
				for (int i = 0; i < 20; i++)
				{
					SendGiveRpOnce(player, 8, -5); // 南瓜收藏品：-5 绕过已收集检查
					ScriptMgr::Yield(std::chrono::milliseconds(120));
				}
			}
			else
			{
				for (int i = 20; i <= 24; i++)
				{
					SendGiveRpOnce(player, 4, i);
					ScriptMgr::Yield(std::chrono::milliseconds(120));
				}
			}

			Notifications::Show("刷经验", "已向目标发放经验。", NotificationType::Success);
		}
	};

	class LoopGiveRp : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;
		int m_Index = 20;

		virtual void OnTick() override
		{
			if (++m_Timer < 180) // 约 3 秒
				return;

			m_Timer = 0;

			auto player = Players::GetSelected();
			if (!player.IsValid() || player.IsLocal())
				return;

			// 交替使用两种收藏品路线，每次 +1000 RP
			if (m_Index == -5)
			{
				SendGiveRpOnce(player, 8, -5);
				m_Index = 20;
			}
			else
			{
				SendGiveRpOnce(player, 4, m_Index);
				m_Index++;
				if (m_Index > 24)
					m_Index = -5;
			}
		}
	};

	class LoopCeoPay : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer     = 0;
		int m_SlowTimer = 0;
		bool m_Alt      = false;

		virtual void OnTick() override
		{
			m_Timer++;
			m_SlowTimer++;

			auto player = Players::GetSelected();
			if (!player.IsValid() || player.IsLocal())
				return;

			if (m_Timer >= 1800) // 约 30 秒
			{
				m_Timer = 0;
				SendCeoPay(player, 10000, -1292453789, m_Alt ? 1 : 0);
				m_Alt = !m_Alt;
			}

			if (m_SlowTimer >= 7200) // 约 120 秒
			{
				m_SlowTimer = 0;
				SendCeoPay(player, 30000, 198210293, 1);
			}
		}
	};

	static GiveRp _GiveRp{"giverp", "给他刷经验", "给选中玩家发放一批经验（事件方式，约 2 万 RP）。"};
	static LoopGiveRp _LoopGiveRp{"loopgiverp", "循环刷经验", "持续给选中玩家刷经验（每约 3 秒 +1000 RP）。"};
	static LoopCeoPay _LoopCeoPay{"loopceopay", "循环刷钱", "持续给选中玩家发钱（CEO 薪资事件，30 秒一次）。连续使用有账号风险，仅供对局整蛊研究。"};
}
