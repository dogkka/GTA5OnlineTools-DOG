#include "core/commands/Command.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/backend/Tunables.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/ScriptLocal.hpp"
#include "game/gta/Scripts.hpp"
#include "game/gta/Stats.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	// [DOG] 赃车店（Salvage Yard）：数据与流程移植自 UltimateMenu V3（社区开源方案，GPL-2.0）
	namespace SalvageYardHeist
	{
		static bool RequireSession(std::string_view title)
		{
			if (Pointers.IsSessionStarted && *Pointers.IsSessionStarted)
				return true;

			Notifications::Show(std::string(title), "请先进入 GTA 在线模式。", NotificationType::Error);
			return false;
		}

		// vehrob_planning 规划板刷新（UM: SYRl1 = 545）
		static void ReloadPlanningBoard()
		{
			if (auto thread = Scripts::FindScriptThread("vehrob_planning"_J))
				*ScriptLocal(thread, 545).As<int*>() = 2;
		}

		class CompletePreps : public Command
		{
		public:
			using Command::Command;

			virtual void OnCall() override
			{
				if (!RequireSession("赃车店前置"))
					return;

				Stats::SetInt("MPX_SALV23_GEN_BS", -1);
				Stats::SetInt("MPX_SALV23_SCOPE_BS", -1);
				Stats::SetInt("MPX_SALV23_FM_PROG", -1);
				Stats::SetInt("MPX_SALV23_INST_PROG", -1);
				ReloadPlanningBoard();

				Notifications::Show("赃车店前置", "已标记全部前置完成；回规划板查看（建议换装或购买任意物品触发一次存档）。", NotificationType::Success);
			}
		};

		class ResetPreps : public Command
		{
		public:
			using Command::Command;

			virtual void OnCall() override
			{
				if (!RequireSession("赃车店前置"))
					return;

				Stats::SetInt("MPX_SALV23_GEN_BS", 0);
				Stats::SetInt("MPX_SALV23_SCOPE_BS", 0);
				Stats::SetInt("MPX_SALV23_FM_PROG", 0);
				Stats::SetInt("MPX_SALV23_INST_PROG", 0);
				ReloadPlanningBoard();

				Notifications::Show("赃车店前置", "已重置全部前置。", NotificationType::Warning);
			}
		};

		class ReloadScreen : public Command
		{
		public:
			using Command::Command;

			virtual void OnCall() override
			{
				ReloadPlanningBoard();
				Notifications::Show("赃车店", "规划板已刷新。", NotificationType::Info);
			}
		};

		class SkipWeeklyCooldown : public Command
		{
		public:
			using Command::Command;

			virtual void OnCall() override
			{
				if (!RequireSession("赃车店冷却"))
					return;

				static Tunable weekId{"SALV23_VEH_ROBBERY_WEEK_ID"_J};
				if (weekId.IsReady())
					weekId.Set(Stats::GetInt("MPX_SALV23_WEEK_SYNC") + 1);

				ReloadPlanningBoard();
				Notifications::Show("赃车店", "已推进每周冷却并刷新规划板。", NotificationType::Success);
			}
		};

		// --- 出售价值（三档） ---
		static IntCommand _SellValue1{"salvagesellvalue1", "出售价值 槽位 1", "第一个槽位的出售价值。", 0, 2000000000, 0};
		static IntCommand _SellValue2{"salvagesellvalue2", "出售价值 槽位 2", "第二个槽位的出售价值。", 0, 2000000000, 0};
		static IntCommand _SellValue3{"salvagesellvalue3", "出售价值 槽位 3", "第三个槽位的出售价值。", 0, 2000000000, 0};

		class ApplySellValues : public Command
		{
		public:
			using Command::Command;

			virtual void OnCall() override
			{
				if (!RequireSession("赃车店收益"))
					return;

				static Tunable slot1{static_cast<joaat_t>(-1699398139)};
				static Tunable slot2{static_cast<joaat_t>(-1997104504)};
				static Tunable slot3{static_cast<joaat_t>(-1704051341)};

				if (slot1.IsReady())
					slot1.Set(_SellValue1.GetState());
				if (slot2.IsReady())
					slot2.Set(_SellValue2.GetState());
				if (slot3.IsReady())
					slot3.Set(_SellValue3.GetState());

				ReloadPlanningBoard();
				Notifications::Show("赃车店收益", "已写入三档出售价值并刷新规划板。", NotificationType::Success);
			}
		};

		// --- 任务 ---
		struct SalvageMission
		{
			joaat_t m_Script;
			int m_Step1;
			int m_Step2;
		};

		static const SalvageMission kMissions[] = {
		    {"fm_content_vehrob_cargo_ship"_J, 7186, 8579},
		    {"fm_content_vehrob_police"_J, 9012, 10449},
		    {"fm_content_vehrob_arena"_J, 7913, 9346},
		    {"fm_content_vehrob_casino_prize"_J, 9192, 10586},
		    {"fm_content_vehrob_submarine"_J, 6219, 7515},
		};

		class InstantFinish : public Command
		{
		public:
			using Command::Command;

			virtual void OnCall() override
			{
				bool found = false;

				for (const auto& mission : kMissions)
				{
					if (auto thread = Scripts::FindScriptThread(mission.m_Script))
					{
						found = true;

						*ScriptLocal(thread, mission.m_Step1).As<int*>() = *ScriptLocal(thread, mission.m_Step1).As<int*>() | (1 << 11);
						*ScriptLocal(thread, mission.m_Step2).As<int*>() = 2;
					}
				}

				Notifications::Show("赃车店",
				    found ? "已请求秒过当前赃车店任务。" : "未找到进行中的赃车店任务（需在任务中）。",
				    found ? NotificationType::Success : NotificationType::Error);
			}
		};

		class InstantSell : public Command
		{
		public:
			using Command::Command;

			virtual void OnCall() override
			{
				auto ped = Self::GetPed();
				if (!ped)
				{
					Notifications::Show("赃车店", "未找到本地玩家。", NotificationType::Error);
					return;
				}

				PED::SET_PED_COORDS_KEEP_VEHICLE(ped.GetHandle(), 1169.0f, -2976.0f, 6.0f);
				Notifications::Show("赃车店", "已传送到出售点（需在出售任务中、且人在车上）。", NotificationType::Success);
			}
		};

		static CompletePreps _CompletePreps{"salvagecompletepreps", "完整前置", "将赃车店全部前置标记为完成，并刷新规划板。"};
		static ResetPreps _ResetPreps{"salvageresetpreps", "重置前置", "将赃车店全部前置重置为未完成。"};
		static ReloadScreen _ReloadScreen{"salvagereloadscreen", "重载规划板", "刷新赃车店规划板显示。"};
		static SkipWeeklyCooldown _SkipWeeklyCooldown{"salvageskipweeklycooldown", "跳过每周冷却", "推进赃车店每周抢劫冷却并刷新规划板。"};
		static ApplySellValues _ApplySellValues{"salvageapplysellvalues", "应用出售价值", "将三档出售价值写入游戏并刷新规划板。"};
		static InstantFinish _InstantFinish{"salvageinstantfinish", "立即完成", "秒过当前进行中的赃车店任务（需在任务中）。"};
		static InstantSell _InstantSell{"salvageinstantsell", "瞬间出售", "把玩家（连同载具）传送到出售点，快速完成出售（需在出售任务中）。"};
	}
}