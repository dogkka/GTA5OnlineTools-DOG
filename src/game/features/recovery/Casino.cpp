#include "core/commands/LoopedCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/Commands.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/ScriptLocal.hpp"
#include "game/gta/Stats.hpp"
#include "game/backend/Tunables.hpp"
#include "core/backend/ScriptMgr.hpp"

#include <cstdlib>
#include <ctime>

namespace YimMenu::Features
{
	namespace
	{
		// 老虎机随机结果表（Enhanced 当前版本：Local_1381.f_1[][]，共 194 项）
		constexpr int kSlotsResultsTable = 1381;
		constexpr int kSlotsFirstIndex   = 3;
		constexpr int kSlotsLastIndex    = 196;
		constexpr int kSlotsWinValue     = 6;
		constexpr int kSlotsLoseValue    = 0;

		// 这两项被游戏脚本用作特殊标记，不参与改写
		bool IsSkippedSlotIndex(int index)
		{
			return index == 67 || index == 132;
		}

		// 幸运转盘状态本地变量（Enhanced 当前版本：Local_311.f_14 = 奖品，Local_311.f_45 = 奖品状态）
		constexpr int kWheelBase        = 311;
		constexpr int kWheelPrizeOffset = 14;
		constexpr int kWheelStateOffset = 45;
		constexpr int kWheelStateValue  = 11;

		int WheelPrizeForMode(int mode)
		{
			switch (mode)
			{
			case 1: return 18; // 豪华跑车（当前展台载具）
			case 2: return 11; // 神秘奖品
			case 3: return 19; // $50,000 游戏币
			case 4: return 15; // 25,000 筹码
			case 5: return 17; // 15,000 声望
			case 6: return 4;  // 折扣券
			case 7: return 8;  // 衣服
			default: return -1;
			}
		}
	}

	// ============================ 老虎机控制 ============================
	class CasinoManipulateRigSlotMachines : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto mode_cmd  = Commands::GetCommand<ListCommand>("casinoslotmode"_J);
			const int mode = mode_cmd ? mode_cmd->GetState() : 0;
			const int desired = (mode == 1) ? kSlotsLoseValue : kSlotsWinValue;

			if (!Scripts::SafeToModifyFreemodeBroadcastGlobals() || !SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("casino_slots"_J))
				return;

			Player casinoSlotsScriptHostPlayer = NETWORK::NETWORK_GET_HOST_OF_SCRIPT("casino_slots", -1, 0);
			if (casinoSlotsScriptHostPlayer.GetId() != Self::GetPlayer().GetId())
				Scripts::ForceScriptHost(Scripts::FindScriptThread("casino_slots"_J));

			bool needs_run = false;
			for (int slots_iter = kSlotsFirstIndex; slots_iter <= kSlotsLastIndex; ++slots_iter)
			{
				if (IsSkippedSlotIndex(slots_iter))
					continue;

				if (*ScriptLocal("casino_slots"_J, kSlotsResultsTable + slots_iter).As<int*>() != desired)
				{
					needs_run = true;
					break;
				}
			}

			if (!needs_run)
				return;

			for (int slots_iter = kSlotsFirstIndex; slots_iter <= kSlotsLastIndex; ++slots_iter)
			{
				if (!IsSkippedSlotIndex(slots_iter))
					*ScriptLocal("casino_slots"_J, kSlotsResultsTable + slots_iter).As<int*>() = desired;
			}
		}

		virtual void OnDisable() override
		{
			if (!Scripts::SafeToModifyFreemodeBroadcastGlobals() || !SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("casino_slots"_J))
				return;

			Player casinoSlotsScriptHostPlayer = NETWORK::NETWORK_GET_HOST_OF_SCRIPT("casino_slots", -1, 0);
			if (casinoSlotsScriptHostPlayer.GetId() != Self::GetPlayer().GetId())
				Scripts::ForceScriptHost(Scripts::FindScriptThread("casino_slots"_J));

			// 仅当整张表仍是刚被改写过的状态时才复原（全部相同值 = 我们改的）
			int first_value = -1;
			bool rigged     = true;
			for (int slots_iter = kSlotsFirstIndex; slots_iter <= kSlotsLastIndex; ++slots_iter)
			{
				if (IsSkippedSlotIndex(slots_iter))
					continue;

				const int value = *ScriptLocal("casino_slots"_J, kSlotsResultsTable + slots_iter).As<int*>();
				if (first_value < 0)
					first_value = value;
				else if (value != first_value)
				{
					rigged = false;
					break;
				}
			}

			if (!rigged || (first_value != kSlotsWinValue && first_value != kSlotsLoseValue))
				return;

			std::srand(static_cast<unsigned int>(std::time(nullptr)));
			for (int slots_iter = kSlotsFirstIndex; slots_iter <= kSlotsLastIndex; ++slots_iter)
			{
				if (!IsSkippedSlotIndex(slots_iter))
					*ScriptLocal("casino_slots"_J, kSlotsResultsTable + slots_iter).As<int*>() = std::rand() % 8; // [0,7]
			}
		}
	};

	// ============================ 幸运转盘控制 ============================
	class CasinoLuckyWheelPrize : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto prize_cmd  = Commands::GetCommand<ListCommand>("casinoluckywheelprize"_J);
			const int mode  = prize_cmd ? prize_cmd->GetState() : 0;
			const int prize = WheelPrizeForMode(mode);
			if (prize < 0)
				return;

			if (!Scripts::SafeToModifyFreemodeBroadcastGlobals() || !SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("casino_lucky_wheel"_J))
				return;

			int* prize_local = ScriptLocal("casino_lucky_wheel"_J, kWheelBase + kWheelPrizeOffset).As<int*>();
			int* state_local = ScriptLocal("casino_lucky_wheel"_J, kWheelBase + kWheelStateOffset).As<int*>();
			if (!prize_local || !state_local)
				return;

			// 只在与目标不一致时写入，避免和脚本自身状态机互相抢占
			if (*prize_local != prize || *state_local != kWheelStateValue)
			{
				*prize_local = prize;
				*state_local = kWheelStateValue;
			}
		}
	};

	static ListCommand _CasinoSlotMode{"casinoslotmode", "老虎机结果", "选择老虎机结果：必中大奖 / 必输。恢复正常请在关闭上方开关前切回必中大奖。", {{0, "必中大奖"}, {1, "必输"}}, 0};

	static ListCommand _CasinoLuckyWheelPrizeSelect{"casinoluckywheelprize", "转盘奖品", "选择幸运转盘必中的奖品（需站在转盘旁、转动前生效；刷太多有被举报风险）。", {{0, "关闭"}, {1, "豪华跑车"}, {2, "神秘奖品"}, {3, "5万游戏币"}, {4, "2.5万筹码"}, {5, "1.5万声望"}, {6, "折扣券"}, {7, "衣服"}}, 0};

	static CasinoManipulateRigSlotMachines _CasinoManipulateRigSlotMachines{"casinomanipulaterigslotmachines", "老虎机控制", "控制老虎机结果：必中大奖或必输（需在赌场老虎机附近）。"};
	static CasinoLuckyWheelPrize _CasinoLuckyWheelPrize{"casinoluckywheel", "幸运转盘控制", "站在赌场幸运转盘旁，转动前自动将结果设定为所选奖品。"};
}
