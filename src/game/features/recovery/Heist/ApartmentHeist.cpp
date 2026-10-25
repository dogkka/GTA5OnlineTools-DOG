#include "core/commands/IntCommand.hpp"
#include "core/commands/Command.hpp"
#include "game/gta/Stats.hpp"
#include "game/backend/Players.hpp"
#include "game/gta/ScriptGlobal.hpp"
#include "game/gta/ScriptLocal.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "types/script/globals/GlobalPlayerBD.hpp"
#include "game/backend/Self.hpp"

namespace YimMenu::Features
{
	namespace ApartmentHeist
	{
		static IntCommand _ApartmentHeistCut1{"apartmentheistcut1", "玩家 1", "玩家 1 分红", std::nullopt, std::nullopt, 0};
		static IntCommand _ApartmentHeistCut2{"apartmentheistcut2", "玩家 2", "玩家 2 分红", std::nullopt, std::nullopt, 0};
		static IntCommand _ApartmentHeistCut3{"apartmentheistcut3", "玩家 3", "玩家 3 分红", std::nullopt, std::nullopt, 0};
		static IntCommand _ApartmentHeistCut4{"apartmentheistcut4", "玩家 4", "玩家 4 分红", std::nullopt, std::nullopt, 0};

		class SetCuts : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				auto base1 = ScriptGlobal(1936406).At(1);
				auto base2 = ScriptGlobal(1938374).At(3008);

				*base1.At(0, 1).As<int*>() = 100 - (_ApartmentHeistCut1.GetState() + _ApartmentHeistCut2.GetState() + _ApartmentHeistCut3.GetState() + _ApartmentHeistCut4.GetState());
				*base1.At(1, 1).As<int*>() = _ApartmentHeistCut2.GetState();
				*base1.At(2, 1).As<int*>() = _ApartmentHeistCut3.GetState();
				*base1.At(3, 1).As<int*>() = _ApartmentHeistCut4.GetState();

				ScriptMgr::Yield(500ms);

				*base2.At(0, 1).As<int*>() = -1 * (*base1.At(0, 1).As<int*>() + *base1.At(1, 1).As<int*>() + *base1.At(2, 1).As<int*>() + *base1.At(3, 1).As<int*>() - 100);
				
				for (int i = 1; i <= 3; i++)
				{
					*base2.At(i, 1).As<int*>() = *base1.At(i, 1).As<int*>();
				}
			}
		};

		class ForceReady : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto gpbd = GlobalPlayerBD::Get(); gpbd && Scripts::SafeToModifyFreemodeBroadcastGlobals())
				{
					for (auto& player : Players::GetPlayers())
					{
						gpbd->Entries[player.second.GetId()].HeistCutSelectionStage = 6;
					}
				}
			}
		};

		class Setup : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				Stats::SetInt("MPX_HEIST_PLANNING_STAGE", -1);
			}
		};

		class SkipHacking : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller"_J))
				{
					*ScriptLocal(thread, 12239).At(24).As<int*>() = 7;
					*ScriptLocal(thread, 10233).As<int*>() = *ScriptLocal(thread, 10233).As<int*>() | (1 << 9);
				}
			}
		};

		class SkipDrilling : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller"_J))
				{
					*ScriptLocal(thread, 10527).At(11).As<float*>() = 100.0f;
				}
			}
		};

		class SkipSwiping : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller"_J))
				{
					*ScriptLocal(thread, 32785).At(Self::GetPlayer().GetId(), 294).At(143).As<int*>() = 8;
					*ScriptLocal(thread, 64655).As<int*>() = 5;
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
				// TODO: find a way of getting current heist info so that InstantFinishPacific can be implemented here conditionally.
			}
		};

		class InstantFinishPacific : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller"_J))
				{
					Scripts::ForceScriptHost(thread);
					ScriptMgr::Yield(500ms);

					*ScriptLocal(thread, 20412).At(2686).As<int*>() = 1875000;
					*ScriptLocal(thread, 20412).At(1062).As<int*>() = 5;
					*ScriptLocal(thread, 20412).As<int*>() = 12;
					*ScriptLocal(thread, 29326).At(0, 1).As<int*>() = 99999;
					*ScriptLocal(thread, 32785).At(0, 294).At(68).As<int*>() = 99999;
				}
			}
		};

		static SetCuts _ApartmentHeistSetCuts{"apartmentheistsetcuts", "应用分红", "应用上方设置的抢劫分红比例。"};
		static ForceReady _ApartmentHeistForceReady{"apartmentheistforceready", "强制全员准备", "将所有玩家设为已准备。"};
		static Setup _ApartmentHeistSetup{"apartmentheistsetup", "配置任务", "配置当前公寓抢劫。"};
		static SkipHacking _ApartmentHeistSkipHacking{"apartmentheistskiphacking", "跳过破解", "跳过黑客破解流程。"};
		static SkipDrilling _ApartmentHeistSkipDrilling{"apartmentheistskipdrilling", "跳过钻孔", "跳过钻孔流程"};
		static SkipSwiping _ApartmentHeistSkipSwiping{"apartmentheistskipswiping", "跳过刷卡", "跳过刷卡流程"};
		static InstantFinish _ApartmentHeistInstantFinish{"apartmentheistinstantfinish", "立即完成", "立即完成当前抢劫。"};
		static InstantFinishPacific _ApartmentHeistInstantFinishPacific{"apartmentheistinstantfinishpacific", "立即完成（太平洋）", "立即完成太平洋标准银行差事。"};
	}
}
