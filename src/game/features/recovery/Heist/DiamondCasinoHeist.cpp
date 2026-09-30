#include "core/commands/Command.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/gta/Stats.hpp"
#include "core/backend/FiberPool.hpp"
#include "game/gta/ScriptGlobal.hpp"
#include "game/gta/ScriptLocal.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "game/backend/Tunables.hpp"
#include "game/backend/Self.hpp"

namespace YimMenu::Features
{
	namespace DiamondCasinoHeist
	{
		static IntCommand _DiamondCasinoHeistCut1{"diamondcasinoheistcut1", "玩家 1", "玩家 1 分红", std::nullopt, std::nullopt, 0};
		static IntCommand _DiamondCasinoHeistCut2{"diamondcasinoheistcut2", "玩家 2", "玩家 2 分红", std::nullopt, std::nullopt, 0};
		static IntCommand _DiamondCasinoHeistCut3{"diamondcasinoheistcut3", "玩家 3", "玩家 3 分红", std::nullopt, std::nullopt, 0};
		static IntCommand _DiamondCasinoHeistCut4{"diamondcasinoheistcut4", "玩家 4", "玩家 4 分红", std::nullopt, std::nullopt, 0};

		class SetCuts : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				auto base = ScriptGlobal(1973762).At(1497).At(736).At(92);

				*base.At(0, 1).As<int*>() = _DiamondCasinoHeistCut1.GetState();
				*base.At(1, 1).As<int*>() = _DiamondCasinoHeistCut2.GetState();
				*base.At(2, 1).As<int*>() = _DiamondCasinoHeistCut3.GetState();
				*base.At(3, 1).As<int*>() = _DiamondCasinoHeistCut4.GetState();
			}
		};

		class ForceReady : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				auto base = ScriptGlobal(1978125);

				for (int i = 0; i <= 3; i++)
				{
					*base.At(i, 68).At(7).At(i, 1).As<int*>() = 1;
				}
			}
		};

		static std::vector<std::pair<int, const char*>> diamondCasinoHeistDifficulty = {
			{0, "普通"},
			{1, "困难"}
		};
		static ListCommand _DiamondCasinoHeistDifficulty{"diamondcasinoheistdifficulty", "难度", "抢劫难度", diamondCasinoHeistDifficulty, 0};

		static std::vector<std::pair<int, const char*>> diamondCasinoHeistPrimaryTarget = {
			{3, "钻石"},
			{1, "黄金"},
			{2, "艺术品"},
			{0, "现金"}
		};
		static ListCommand _DiamondCasinoHeistPrimaryTarget{"diamondcasinoheistprimarytarget", "主要目标", "主要目标", diamondCasinoHeistPrimaryTarget, 3};

		static ListCommand* _DiamondCasinoHeistGunmanPtr = nullptr;
		static ListCommand* _DiamondCasinoHeistApproachPtr = nullptr;

		static std::vector<std::vector<std::vector<std::pair<int, const char*>>>> diamondCasinoHeistWeapon = {
			{
				{{0, "Mk II 霰弹枪配置"}, {1, "Mk II 步枪配置"}},
				{{0, "Mk II 冲锋枪配置"}, {1, "Mk II 步枪配置"}},
				{{0, "Mk II 霰弹枪配置"}, {1, "Mk II 步枪配置"}}
			},
			{
				{{0, "步枪配置"}, {1, "霰弹枪配置"}},
				{{0, "步枪配置"}, {1, "霰弹枪配置"}},
				{{0, "步枪配置"}, {1, "霰弹枪配置"}}
			},
			{
				{{0, "战斗 PDW 配置"}, {1, "步枪配置"}},
				{{0, "霰弹枪配置"}, {1, "步枪配置"}},
				{{0, "霰弹枪配置"}, {1, "战斗机枪配置"}}
			},
			{
				{{0, "冲锋枪配置"}, {1, "霰弹枪配置"}},
				{{0, "冲锋手枪配置"}, {1, "霰弹枪配置"}},
				{{0, "冲锋枪配置"}, {1, "霰弹枪配置"}}
			},
			{
				{{0, "微型冲锋枪配置"}, {1, "冲锋手枪配置"}},
				{{0, "微型冲锋枪配置"}, {1, "霰弹枪配置"}},
				{{0, "霰弹枪配置"}, {1, "左轮手枪配置"}}},
			{
				{{0, "                                        "}, {1, ""}},
				{{0, ""}, {1, ""}},
				{{0, ""}, {1, ""}}
			}
		};
		static ListCommand _DiamondCasinoHeistWeapon{"diamondcasinoheistweapon", "武器", "武器配置", diamondCasinoHeistWeapon[5][0], 0};

		static std::vector<std::pair<int, const char*>> diamondCasinoHeistGunman = {
			{0, "切斯特·麦考伊"},
			{1, "古斯塔沃·莫塔"},
			{2, "帕特里克·麦克瑞利"},
			{3, "查理·里德"},
			{4, "卡尔·阿博拉吉"},
			{5, "移除枪手"}
		};

		class Gunman : public ListCommand
		{
			using ListCommand::ListCommand;

			virtual void OnChange() override
			{
				_DiamondCasinoHeistWeapon.SetList(diamondCasinoHeistWeapon[this->GetState()][_DiamondCasinoHeistApproachPtr->GetState()]);
				_DiamondCasinoHeistWeapon.SetState(0);
			}
		};
		static Gunman _DiamondCasinoHeistGunman{"diamondcasinoheistgunman", "枪手", "枪手", diamondCasinoHeistGunman, 5};

		static std::vector<std::pair<int, const char*>> diamondCasinoHeistApproach = {
			{0, "隐迹潜踪"},
			{1, "兵不厌诈"},
			{2, "气势汹汹"}
		};

		class Approach : public ListCommand
		{
			using ListCommand::ListCommand;

			virtual void OnChange() override
			{
				_DiamondCasinoHeistWeapon.SetList(diamondCasinoHeistWeapon[_DiamondCasinoHeistGunmanPtr->GetState()][this->GetState()]);
				_DiamondCasinoHeistWeapon.SetState(0);
			}
		};
		static Approach _DiamondCasinoHeistApproach{"diamondcasinoheistapproach", "路线", "抢劫路线", diamondCasinoHeistApproach, 0};

		static struct InitPtrs
		{
			InitPtrs()
			{
				_DiamondCasinoHeistGunmanPtr = &_DiamondCasinoHeistGunman;
				_DiamondCasinoHeistApproachPtr = &_DiamondCasinoHeistApproach;
			}
		} _initPtrs;

		static std::vector<std::vector<std::pair<int, const char*>>> diamondCasinoHeistVehicle = {
			{{0, "炸吧"}, {1, "流浪者"}, {2, "不法之徒"}, {3, "埃弗伦"}},
			{{0, "王者经典版"}, {1, "铁腕经典版"}, {2, "爱利"}, {3, "科莫达"}},
			{{0, "随行者 Mk II"}, {1, "漂移约塞米蒂"}, {2, "斯国一"}, {3, "扼喉"}},
			{{0, "曼切兹"}, {1, "斯特德"}, {2, "亵渎者"}, {3, "雷克托"}},
			{{0, "天威经典版"}, {1, "埃斯波"}, {2, "羽黑"}, {3, "卫士经典款"}},
			{{0, "                           "}, {1, ""}, {2, ""}, {3, ""}}
		};
		static ListCommand _DiamondCasinoHeistVehicle{"diamondcasinoheistvehicle", "载具", "逃离载具", diamondCasinoHeistVehicle[5], 0};

		static std::vector<std::pair<int, const char*>> diamondCasinoHeistDriver = {
			{0, "切斯特·麦考伊"},
			{1, "艾迪·托"},
			{2, "塔莉安娜·马丁内斯"},
			{3, "扎克·尼尔森"},
			{4, "卡里姆·登兹"},
			{5, "移除车手"}
		};

		class Driver : public ListCommand
		{
			using ListCommand::ListCommand;

			virtual void OnChange() override
			{
				_DiamondCasinoHeistVehicle.SetList(diamondCasinoHeistVehicle[this->GetState()]);
				_DiamondCasinoHeistVehicle.SetState(0);
			}
		};
		static Driver _DiamondCasinoHeistDriver{"diamondcasinoheistdriver", "车手", "车手", diamondCasinoHeistDriver, 5};

		static std::vector<std::pair<int, const char*>> diamondCasinoHeistHacker = {
			{4, "艾维·施瓦茨曼"},
			{5, "佩姬·哈里斯"},
			{2, "克里斯蒂安·费尔茨"},
			{3, "约翰·布莱尔"},
			{1, "里奇·卢肯斯"},
			{6, "移除黑客"}
		};
		static ListCommand _DiamondCasinoHeistHacker{"diamondcasinoheisthacker", "黑客", "黑客", diamondCasinoHeistHacker, 6};

		class Setup : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				Stats::SetInt("MPX_H3_COMPLETEDPOSIX", -1);
				Stats::SetInt("MPX_H3OPT_MASKS", 4);
				Stats::SetInt("MPX_H3OPT_WEAPS", _DiamondCasinoHeistWeapon.GetState());
				Stats::SetInt("MPX_H3OPT_VEHS", _DiamondCasinoHeistVehicle.GetState());
				Stats::SetInt("MPX_CAS_HEIST_FLOW", -1);
				Stats::SetInt("MPX_H3_LAST_APPROACH", 0);
				Stats::SetInt("MPX_H3OPT_APPROACH", _DiamondCasinoHeistApproach.GetState() + 1); // 1 = Silent & Sneaky, 2 = The Big Con, 3 = Aggressive

				switch (_DiamondCasinoHeistDifficulty.GetState())
				{
				case 0: Stats::SetInt("MPX_H3_HARD_APPROACH", 0); break;
				case 1: Stats::SetInt("MPX_H3_HARD_APPROACH", _DiamondCasinoHeistApproach.GetState() + 1); break;
				default: break;
				}

				Stats::SetInt("MPX_H3OPT_TARGET", _DiamondCasinoHeistPrimaryTarget.GetState());
				Stats::SetInt("MPX_H3OPT_POI", 1023);
				Stats::SetInt("MPX_H3OPT_ACCESSPOINTS", 2047);

				switch (_DiamondCasinoHeistGunman.GetState())
				{
				case 0: Stats::SetInt("MPX_H3OPT_CREWWEAP", 4); break;
				case 1: Stats::SetInt("MPX_H3OPT_CREWWEAP", 2); break;
				case 2: Stats::SetInt("MPX_H3OPT_CREWWEAP", 5); break;
				case 3: Stats::SetInt("MPX_H3OPT_CREWWEAP", 3); break;
				case 4: Stats::SetInt("MPX_H3OPT_CREWWEAP", 1); break;
				case 5: Stats::SetInt("MPX_H3OPT_CREWWEAP", 6); break;
				default: break;
				}

				switch (_DiamondCasinoHeistDriver.GetState())
				{
				case 0: Stats::SetInt("MPX_H3OPT_CREWDRIVER", 5); break;
				case 1: Stats::SetInt("MPX_H3OPT_CREWDRIVER", 3); break;
				case 2: Stats::SetInt("MPX_H3OPT_CREWDRIVER", 2); break;
				case 3: Stats::SetInt("MPX_H3OPT_CREWDRIVER", 4); break;
				case 4: Stats::SetInt("MPX_H3OPT_CREWDRIVER", 1); break;
				case 5: Stats::SetInt("MPX_H3OPT_CREWDRIVER", 6); break;
				default: break;
				}

				Stats::SetInt("MPX_H3OPT_CREWHACKER", _DiamondCasinoHeistHacker.GetState());
				Stats::SetInt("MPX_H3OPT_DISRUPTSHIP", 3); // Security Strength: 1 = Strong, 2 = Medium, 3 = Weak
				Stats::SetInt("MPX_H3OPT_BODYARMORLVL", -1);
				Stats::SetInt("MPX_H3OPT_KEYLEVELS", 2); // 1 = Level1 Security Pass, 2 = Level2 Security Pass

				Stats::SetInt("MPX_H3OPT_BITSET0", rand() % (INT_MAX)); // Refresh board
				Stats::SetInt("MPX_H3OPT_BITSET1", rand() % (INT_MAX)); // Refresh board

				ScriptMgr::Yield(500ms);

				Stats::SetInt("MPX_H3OPT_BITSET0", -1); // Refresh board
				Stats::SetInt("MPX_H3OPT_BITSET1", -1); // Refresh board
			}
		};

		static IntCommand _DiamondCasinoHeistPotentialTake{"diamondcasinoheistpotentialtake", "预计收益", "更新预计收益", std::nullopt, std::nullopt, 0};

		class SetPotentialTake : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				int primary_target = Stats::GetInt("MPX_H3OPT_TARGET");

				switch (primary_target)
				{
				case 0:
				{
					static Tunable tunable{static_cast<joaat_t>(-1638885821)};
					if (tunable.IsReady())
						tunable.Set(_DiamondCasinoHeistPotentialTake.GetState());
					break;
				} // "Cash"
				case 1:
				{
					static Tunable tunable{static_cast<joaat_t>(-582734553)};
					if (tunable.IsReady())
						tunable.Set(_DiamondCasinoHeistPotentialTake.GetState());
					break;
				} // "Gold"
				case 2:
				{
					static Tunable tunable{1934398910};
					if (tunable.IsReady())
						tunable.Set(_DiamondCasinoHeistPotentialTake.GetState());
					break;
				} // "Artwork"
				case 3:
				{
					static Tunable tunable{1277889925};
					if (tunable.IsReady())
						tunable.Set(_DiamondCasinoHeistPotentialTake.GetState());
					break;
				} // "Diamond"
				default: break;
				}
			}
		};

		static IntCommand _DiamondCasinoHeistActualTake{"diamondcasinoheistactualtake", "实际收益", "更新实际收益", std::nullopt, std::nullopt, 0};

		class SetActualTake : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller"_J))
					*ScriptLocal(thread, 20412).At(2686).As<int*>() = _DiamondCasinoHeistActualTake.GetState();
			}
		};

		class SkipHacking : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller"_J))
				{
					*ScriptLocal(thread, 55028).As<int*>() = 5;
					*ScriptLocal(thread, 56098).As<int*>() = 5;
				}
			}
		};

		class SkipDrilling : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (auto thread = Scripts::FindScriptThread("fm_mission_controller"_J))
					*ScriptLocal(thread, 10567).At(7).As<int*>() = *ScriptLocal(thread, 10567).At(37).As<int*>();
			}
		};

		class SoloMantrap : public Command
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

					*ScriptLocal(thread, 20412).At(1740).At(0, 1).As<int*>() = 80;
					*ScriptLocal(thread, 20412).At(2686).As<int*>() = 4443220;
					*ScriptLocal(thread, 20412).At(1062).As<int*>() = 5;
					*ScriptLocal(thread, 20412).As<int*>() = 12;
					*ScriptLocal(thread, 29326).At(0, 1).As<int*>() = 99999;
					*ScriptLocal(thread, 32785).At(0, 294).At(68).As<int*>() = 99999;
				}
			}
		};

		static SetCuts _DiamondCasinoHeistSetCuts{"diamondcasinoheistsetcuts", "应用分红", "应用上方设置的抢劫分红比例。"};
		static ForceReady _DiamondCasinoHeistForceReady{"diamondcasinoheistforceready", "强制全员准备", "将所有玩家设为已准备。"};
		static Setup _DiamondCasinoHeistSetup{"diamondcasinoheistsetup", "配置任务", "配置名钻赌场豪劫。"};
		static SetPotentialTake _DiamondCasinoHeistSetPotentialTake{"diamondcasinoheistsetpotentialtake", "设置预计收益", "更新预计收益"};
		static SetActualTake _DiamondCasinoHeistSetActualTake{"diamondcasinoheistsetactualtake", "设置实际收益", "更新实际收益"};
		static SkipHacking _DiamondCasinoHeistSkipHacking{"diamondcasinoheistskiphacking", "跳过破解", "跳过黑客破解流程。"};
		static SkipDrilling _DiamondCasinoHeistSkipDrilling{"diamondcasinoheistskipdrilling", "跳过钻孔", "跳过钻孔流程"};
		static SoloMantrap _DiamondCasinoHeistSoloMantrap{"diamondcasinoheistsolomantrap", "单人通过安检门", "允许单人通过原本需要双人刷卡的安检门。"};
		static InstantFinish _DiamondCasinoHeistInstantFinish{"diamondcasinoheistinstantfinish", "立即完成", "立即完成当前抢劫。"};
	}
}
