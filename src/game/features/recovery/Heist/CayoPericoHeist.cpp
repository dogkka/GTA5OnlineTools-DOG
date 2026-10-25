#include "core/commands/Command.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Stats.hpp"
#include "game/gta/ScriptGlobal.hpp"
#include "game/gta/ScriptLocal.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "game/backend/Tunables.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
	namespace CayoPericoHeist
	{
		static IntCommand _CayoPericoHeistCut1{"cayopericoheistcut1", "玩家 1", "玩家 1 分红", std::nullopt, std::nullopt, 0};
		static IntCommand _CayoPericoHeistCut2{"cayopericoheistcut2", "玩家 2", "玩家 2 分红", std::nullopt, std::nullopt, 0};
		static IntCommand _CayoPericoHeistCut3{"cayopericoheistcut3", "玩家 3", "玩家 3 分红", std::nullopt, std::nullopt, 0};
		static IntCommand _CayoPericoHeistCut4{"cayopericoheistcut4", "玩家 4", "玩家 4 分红", std::nullopt, std::nullopt, 0};

		class SetCuts : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				auto base = ScriptGlobal(1980570).At(831).At(56);

				*base.At(0, 1).As<int*>() = _CayoPericoHeistCut1.GetState();
				*base.At(1, 1).As<int*>() = _CayoPericoHeistCut2.GetState();
				*base.At(2, 1).As<int*>() = _CayoPericoHeistCut3.GetState();
				*base.At(3, 1).As<int*>() = _CayoPericoHeistCut4.GetState();
			}
		};

		class ForceReady : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				auto base = ScriptGlobal(1973762);

				for (int i = 0; i <= 3; i++)
				{
					*base.At(i, 27).At(7).At(i, 1).As<int*>() = 1;
				}
			}
		};

		static std::vector<std::pair<int, const char*>> cayoPericoHeistDifficulty = {
			{126823, "普通"},
			{131055, "困难"}
		};
		static ListCommand _CayoPericoHeistDifficulty{"cayopericoheistdifficulty", "难度", "抢劫难度", cayoPericoHeistDifficulty, 126823};

		static std::vector<std::pair<int, const char*>> cayoPericoHeistPrimaryTarget = {
			{5, "黑豹雕像"},
			{3, "粉钻"},
			{4, "马德拉索文件"},
			{2, "债券"},
			{1, "红宝石项链"},
			{0, "西西米托龙舌兰"}
		};
		static ListCommand _CayoPericoHeistPrimaryTarget{"cayopericoheistprimarytarget", "主要目标", "主要目标", cayoPericoHeistPrimaryTarget, 5};

		static std::vector<std::pair<int, const char*>> cayoPericoHeistWeapon = {
			{1, "侵略者"},
			{2, "同谋者"},
			{3, "神枪手"},
			{4, "破坏者"},
			{5, "特等射手"}
		};
		static ListCommand _CayoPericoHeistWeapon{"cayopericoheistweapon", "武器", "武器类别", cayoPericoHeistWeapon, 1};

		class Setup : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				Stats::SetInt("MPX_H4CNF_TARGET", _CayoPericoHeistPrimaryTarget.GetState()); // Primary target
				Stats::SetInt("MPX_H4LOOT_CASH_I", 16711680);
				Stats::SetInt("MPX_H4LOOT_CASH_I_SCOPED", 16711680);
				Stats::SetInt("MPX_H4LOOT_CASH_C", 0);
				Stats::SetInt("MPX_H4LOOT_CASH_C_SCOPED", 0);
				Stats::SetInt("MPX_H4LOOT_COKE_I", 255);
				Stats::SetInt("MPX_H4LOOT_COKE_I_SCOPED", 255);
				Stats::SetInt("MPX_H4LOOT_COKE_C", 0);
				Stats::SetInt("MPX_H4LOOT_COKE_C_SCOPED", 0);
				Stats::SetInt("MPX_H4LOOT_GOLD_I", 0);
				Stats::SetInt("MPX_H4LOOT_GOLD_I_SCOPED", 0);
				Stats::SetInt("MPX_H4LOOT_GOLD_C", 255);
				Stats::SetInt("MPX_H4LOOT_GOLD_C_SCOPED", 255);
				Stats::SetInt("MPX_H4LOOT_WEED_I", 65280);
				Stats::SetInt("MPX_H4LOOT_WEED_I_SCOPED", 65280);
				Stats::SetInt("MPX_H4LOOT_WEED_C", 0);
				Stats::SetInt("MPX_H4LOOT_WEED_C_SCOPED", 0);
				Stats::SetInt("MPX_H4LOOT_PAINT", 127);
				Stats::SetInt("MPX_H4LOOT_PAINT_SCOPED", 127);
				Stats::SetInt("MPX_H4LOOT_CASH_V", 83250);
				Stats::SetInt("MPX_H4LOOT_COKE_V", 202500);
				Stats::SetInt("MPX_H4LOOT_GOLD_V", 333333);
				Stats::SetInt("MPX_H4LOOT_WEED_V", 135000);
				Stats::SetInt("MPX_H4LOOT_PAINT_V", 180000);
				Stats::SetInt("MPX_H4_PROGRESS", _CayoPericoHeistDifficulty.GetState()); // Difficulty
				// [DOG] 2026-10 修复：BS_GEN/UNIFORM/GRAPPEL 改用现代值 -1（全部位置置位）。
				// 旧固定掩码（0x3FFFF / 5256 / 5156）在 R* 持续扩充准备任务位后无法通过游戏校验，
				// 导致「配置任务」不再跳过佩里科岛前置（多源交叉验证：UltimateMenu V3 / Silent-Night / sussyspt）。
				Stats::SetInt("MPX_H4CNF_BS_GEN", -1);
				Stats::SetInt("MPX_H4CNF_BS_ENTR", 63);
				Stats::SetInt("MPX_H4CNF_BS_ABIL", 63);
				Stats::SetInt("MPX_H4CNF_WEP_DISRP", 3);
				Stats::SetInt("MPX_H4CNF_ARM_DISRP", 3);
				Stats::SetInt("MPX_H4CNF_HEL_DISRP", 3);
				Stats::SetInt("MPX_H4CNF_APPROACH", -1);
				Stats::SetInt("MPX_H4CNF_BOLTCUT", 4424);
				Stats::SetInt("MPX_H4CNF_UNIFORM", -1); // [DOG] 原 5256（旧装备码已失效）
				Stats::SetInt("MPX_H4CNF_GRAPPEL", -1); // [DOG] 原 5156（旧装备码已失效）
				Stats::SetInt("MPX_H4_MISSIONS", -1);
				Stats::SetInt("MPX_H4CNF_WEAPONS", _CayoPericoHeistWeapon.GetState()); // Weapon
				Stats::SetInt("MPX_H4CNF_TROJAN", 5);
				Stats::SetInt("MPX_H4_PLAYTHROUGH_STATUS", 10); // [DOG] 原 100（现代参考实现统一为 10）

				ScriptMgr::Yield(500ms);

				if (auto thread = Scripts::FindScriptThread("heist_island_planning"_J))
					*ScriptLocal(thread, 1580).As<int*>() = 2;

					// [DOG] 提示：换装 / 购买任意物品可触发一次存档，保证配置持久化（社区经验，上游 issue #538）
					Notifications::Show("佩里科岛抢劫", "已应用配置并跳过全部前置；建议换装或购买任意物品触发一次存档", NotificationType::Success);
			}
		};

		static IntCommand _CayoPericoHeistPrimaryTargetValue{"cayopericoheistprimarytargetvalue", "主要目标价值", "更新主要目标价值", std::nullopt, std::nullopt, 0};

		class SetPrimaryTargetValue : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				auto primary_target = Stats::GetInt("MPX_H4CNF_TARGET");

				switch (primary_target)
				{
				case 0:
				{
					static Tunable tunable{"IH_PRIMARY_TARGET_VALUE_TEQUILA"_J};
					if (tunable.IsReady())
						tunable.Set(_CayoPericoHeistPrimaryTargetValue.GetState());
					break;
				}
				case 1:
				{
					static Tunable tunable{"IH_PRIMARY_TARGET_VALUE_PEARL_NECKLACE"_J};
					if (tunable.IsReady())
						tunable.Set(_CayoPericoHeistPrimaryTargetValue.GetState());
					break;
				}
				case 2:
				{
					static Tunable tunable{"IH_PRIMARY_TARGET_VALUE_BEARER_BONDS"_J};
					if (tunable.IsReady())
						tunable.Set(_CayoPericoHeistPrimaryTargetValue.GetState());
					break;
				}
				case 3:
				{
					static Tunable tunable{"IH_PRIMARY_TARGET_VALUE_PINK_DIAMOND"_J};
					if (tunable.IsReady())
						tunable.Set(_CayoPericoHeistPrimaryTargetValue.GetState());
					break;
				}
				case 4:
				{
					static Tunable tunable{"IH_PRIMARY_TARGET_VALUE_MADRAZO_FILES"_J};
					if (tunable.IsReady())
						tunable.Set(_CayoPericoHeistPrimaryTargetValue.GetState());
					break;
				}
				case 5:
				{
					static Tunable tunable{"IH_PRIMARY_TARGET_VALUE_SAPPHIRE_PANTHER_STATUE"_J};
					if (tunable.IsReady())
						tunable.Set(_CayoPericoHeistPrimaryTargetValue.GetState());
					break;
				}
				default: break;
				}
			}
		};

		static IntCommand _CayoPericoHeistSecondaryTakeValue{"cayopericoheistsecondarytakevalue", "次要目标收益", "要设置的次要目标总收益", std::nullopt, std::nullopt, 0};

		class SetSecondaryTakeValue : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller_2020"_J))
					*ScriptLocal(thread, 59986).At(1376).At(53).As<int*>() = _CayoPericoHeistSecondaryTakeValue.GetState();
			}
		};

		class SkipHacking : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller_2020"_J))
					*ScriptLocal(thread, 26619).As<int*>() = 5;
			}
		};

		class CutSewer : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller_2020"_J))
					*ScriptLocal(thread, 31511).As<int*>() = 6;
			}
		};

		class CutGlass : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller_2020"_J))
					*ScriptLocal(thread, 32751).At(3).As<float*>() = 100.0f;
			}
		};

		class TakePrimaryTarget : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller_2020"_J))
				{
					auto ped = Self::GetPed();

					if (!ped)
						return;

					auto pos = ped.GetPosition();
					auto heading = ped.GetHeading();
					auto primary_target = Stats::GetInt("MPX_H4CNF_TARGET");

					FiberPool::Push([&] {
						TASK::TASK_GO_STRAIGHT_TO_COORD(ped.GetHandle(), 5006.917, -5755.931, 15.484, 1.0, 3, 15, 5);

						switch (primary_target)
						{
						case 0:
						case 1:
						case 3:
						case 5:
							*ScriptLocal(thread, 32750).As<int*>() = 5;
							*ScriptLocal(thread, 32751).As<int*>() = 3;
							break;
						case 2:
						case 4:
							*ScriptLocal(thread, 32725).As<int*>() = 7;
							break;
						default: break;
						}

						ScriptMgr::Yield(4000ms);

						TASK::TASK_GO_STRAIGHT_TO_COORD(ped.GetHandle(), pos.x, pos.y, pos.z, 1.0, 3, heading, 5);
					});
				}
			}
		};

		class InstantFinish : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller_2020"_J))
				{
					Scripts::ForceScriptHost(thread);
					ScriptMgr::Yield(500ms);

					*ScriptLocal(thread, 56504).As<int*>() = 9;
					*ScriptLocal(thread, 56504).At(1776).At(0, 1).As<int*>() = 50;
				}
			}
		};

		static SetCuts _CayoPericoHeistSetCuts{"cayopericoheistsetcuts", "应用分红", "应用上方设置的抢劫分红比例。"};
		// [DOG] 击杀冷却清除：移植自 UltimateMenu V3（社区方案）。单人/组队对应不同的 H4_TARGET_POSIX 值；
		// 点击后需切换一次战局（或离线再上线）生效。
		class KillCooldown : public Command
		{
			const int m_TargetPosix;

		public:
			KillCooldown(std::string name, std::string label, std::string description, int targetPosix) :
			    Command(name, label, description),
			    m_TargetPosix(targetPosix)
			{
			}

			virtual void OnCall() override
			{
				Stats::SetInt("MPX_H4_TARGET_POSIX", m_TargetPosix);
				Stats::SetInt("MPX_H4_COOLDOWN", 0);
				Stats::SetInt("MPX_H4_COOLDOWN_HARD", 0);

				Notifications::Show("佩里科岛冷却", "已清除抢劫冷却；切换一次战局后生效。", NotificationType::Success);
			}
		};

		static KillCooldown _CayoPericoHeistKillCooldownSolo{"cayopericoheistkillcooldownsolo", "击杀冷却（单人结算后）", "清除佩里科岛抢劫冷却（单人通关后使用；点击后切换一次战局生效）。", 1659643454};
		static KillCooldown _CayoPericoHeistKillCooldownTeam{"cayopericoheistkillcooldownteam", "击杀冷却（组队结算后）", "清除佩里科岛抢劫冷却（组队通关后使用；点击后切换一次战局生效）。", 1659429119};

		static ForceReady _CayoPericoHeistForceReady{"cayopericoheistforceready", "强制全员准备", "将所有玩家设为已准备。"};
		static Setup _CayoPericoHeistSetup{"cayopericoheistsetup", "配置任务", "一键配置佩里科岛抢劫并跳过全部前置（应用后建议换装触发一次存档）。"};
		static SetPrimaryTargetValue _CayoPericoHeistSetPrimaryTargetValue{"cayopericoheistsetprimarytargetvalue", "设置主要目标价值", "更新主要目标价值"};
		static SetSecondaryTakeValue _CayoPericoHeistSetSecondaryTakeValue{"cayopericoheistsetsecondarytakevalue", "应用次要目标收益", "应用设定的次要目标总收益。"};
		static SkipHacking _CayoPericoHeistSkipHacking{"cayopericoheistskiphacking", "跳过破解", "跳过黑客破解流程。"};
		static CutSewer _CayoPericoHeistCutSewer{"cayopericoheistcutsewer", "切开下水道", "切开下水道入口"};
		static CutGlass _CayoPericoHeistCutGlass{"cayopericoheistcutglass", "切开玻璃", "切开玻璃"};
		static TakePrimaryTarget _CayoPericoHeistTakePrimaryTarget{"cayopericoheisttakeprimarytarget", "获取主要目标", "直接将主要目标标记为已获取。"};
		static InstantFinish _CayoPericoHeistInstantFinish{"cayopericoheistinstantfinish", "立即完成", "立即完成当前抢劫。"};
	}
}
