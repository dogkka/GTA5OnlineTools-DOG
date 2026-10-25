#include "core/commands/Command.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/gta/Stats.hpp"
#include "game/gta/ScriptGlobal.hpp"
#include "game/gta/ScriptLocal.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "types/script/globals/GPBD_FM_2.hpp"

namespace YimMenu::Features
{
	namespace DoomsdayHeist
	{
		static IntCommand _DoomsdayHeistCut1{"doomsdayheistcut1", "玩家 1", "玩家 1 分红", std::nullopt, std::nullopt, 0};
		static IntCommand _DoomsdayHeistCut2{"doomsdayheistcut2", "玩家 2", "玩家 2 分红", std::nullopt, std::nullopt, 0};
		static IntCommand _DoomsdayHeistCut3{"doomsdayheistcut3", "玩家 3", "玩家 3 分红", std::nullopt, std::nullopt, 0};
		static IntCommand _DoomsdayHeistCut4{"doomsdayheistcut4", "玩家 4", "玩家 4 分红", std::nullopt, std::nullopt, 0};

		class SetCuts : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				auto base = ScriptGlobal(1969071).At(812).At(50);

				*base.At(0, 1).As<int*>() = _DoomsdayHeistCut1.GetState();
				*base.At(1, 1).As<int*>() = _DoomsdayHeistCut2.GetState();
				*base.At(2, 1).As<int*>() = _DoomsdayHeistCut3.GetState();
				*base.At(3, 1).As<int*>() = _DoomsdayHeistCut4.GetState();
			}
		};

		class ForceReady : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto gpbd_fm_2 = GPBD_FM_2::Get())
				{
					gpbd_fm_2->Entries[0].GangopsData.ArePlayersReady[0] = TRUE;
					gpbd_fm_2->Entries[1].GangopsData.ArePlayersReady[1] = TRUE;
					gpbd_fm_2->Entries[2].GangopsData.ArePlayersReady[2] = TRUE;
					gpbd_fm_2->Entries[3].GangopsData.ArePlayersReady[3] = TRUE;
				}
			}
		};

		static std::vector<std::pair<int, const char*>> doomsdayHeistCategory = {
			{0, "数据泄露"},
			{1, "波格丹危机"},
			{2, "末日将至"}
		};
		static ListCommand _DoomsdayHeistCategory{"doomsdayheistcategory", "抢劫章节", "要配置的末日豪劫章节", doomsdayHeistCategory, 0};

		class Setup : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				switch (_DoomsdayHeistCategory.GetState())
				{
				case 0:
					Stats::SetInt("MPX_GANGOPS_FLOW_MISSION_PROG", 503);
					Stats::SetInt("MPX_GANGOPS_HEIST_STATUS", -229383);
					Stats::SetInt("MPX_GANGOPS_FLOW_NOTIFICATIONS", 1557);
					break;
				case 1:
					Stats::SetInt("MPX_GANGOPS_FLOW_MISSION_PROG", 240);
					Stats::SetInt("MPX_GANGOPS_HEIST_STATUS", -229378);
					Stats::SetInt("MPX_GANGOPS_FLOW_NOTIFICATIONS", 1557);
					break;
				case 2:
					Stats::SetInt("MPX_GANGOPS_FLOW_MISSION_PROG", 16368);
					Stats::SetInt("MPX_GANGOPS_HEIST_STATUS", -229380);
					Stats::SetInt("MPX_GANGOPS_FLOW_NOTIFICATIONS", 1557);
					break;
				default: break;
				}

				ScriptMgr::Yield(500ms);

				if (auto thread = Scripts::FindScriptThread("gb_gang_ops_planning"_J))
					*ScriptLocal(thread, 218).As<int*>() = 6;
			}
		};

		class SkipHacking : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller"_J))
				{
					*ScriptLocal(thread, 1588).As<int*>() = 2;
					*ScriptLocal(thread, 1557).As<int*>() = 3;
					*ScriptLocal(thread, 1314).At(135).As<int*>() = 3;
				}
			}
		};

		class InstantFinish : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller"_J))
				{
					Scripts::ForceScriptHost(thread);
					ScriptMgr::Yield(500ms);

					*ScriptLocal(thread, 20412).At(1725).At(0, 1).As<int*>() = 80;
					*ScriptLocal(thread, 20412).As<int*>() = 12;
					*ScriptLocal(thread, 29326).At(0, 1).As<int*>() = 99999;
					*ScriptLocal(thread, 32785).At(0, 294).At(68).As<int*>() = 99999;
				}
				// TODO: find a way of getting current heist info so that InstantFinishAct3 can be implemented here conditionally.
			}
		};

		class InstantFinishAct3 : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller"_J))
				{
					Scripts::ForceScriptHost(thread);
					ScriptMgr::Yield(500ms);

					*ScriptLocal(thread, 20412).As<int*>() = 12;
					*ScriptLocal(thread, 20412).At(1740).At(0, 1).As<int*>() = 150;
					*ScriptLocal(thread, 20412).At(1062).As<int*>() = 5;
					*ScriptLocal(thread, 29326).At(0, 1).As<int*>() = 99999;
					*ScriptLocal(thread, 32785).At(0, 294).At(68).As<int*>() = 99999;
				}
			}
		};

		static SetCuts _DoomsdayHeistSetCuts{"doomsdayheistsetcuts", "应用分红", "应用上方设置的抢劫分红比例。"};
		static ForceReady _DoomsdayHeistForceReady{"doomsdayheistforceready", "强制全员准备", "将所有玩家设为已准备。"};
		static Setup _DoomsdayHeistSetup{"doomsdayheistsetup", "配置任务", "配置所选末日豪劫章节。"};
		static SkipHacking _DoomsdayHeistSkipHacking{"doomsdayheistskiphacking", "跳过破解", "跳过黑客破解流程。"};
		static InstantFinish _DoomsdayHeistInstantFinish{"doomsdayheistinstantfinish", "立即完成", "立即完成当前抢劫。"};
		static InstantFinishAct3 _DoomsdayHeistInstantFinishAct3{"doomsdayheistinstantfinishact3", "立即完成（第三章）", "立即完成末日将至第三章。"};
	}
}
