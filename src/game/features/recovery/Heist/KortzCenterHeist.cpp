#include "core/commands/Command.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/filemgr/FileMgr.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/ScriptGlobal.hpp"
#include "game/gta/ScriptLocal.hpp"
#include "game/gta/Scripts.hpp"
#include "game/gta/Stats.hpp"
#include "game/pointers/Pointers.hpp"

#include <ctime>
#include <fstream>

namespace YimMenu::Features
{
	namespace KortzCenterHeist
	{
		static constexpr int kRecommendedGeneralBits = -1929380361;
		static constexpr int kRecommendedGeneralBits2 = 254984;
		static constexpr int kRecommendedRobberyProg = 65535;
		static constexpr int kRecommendedPoiBits = 65439;

		static bool RequireSession(std::string_view title)
		{
			if (Pointers.IsSessionStarted && *Pointers.IsSessionStarted)
				return true;

			Notifications::Show(std::string(title), "请先进入 GTA 在线模式。", NotificationType::Error);
			return false;
		}

		static rage::scrThread* RequireFinaleThread(std::string_view title)
		{
			if (!RequireSession(title))
				return nullptr;

			auto thread = Scripts::FindScriptThread("fm_mission_controller_v3"_J);
			if (!thread)
				Notifications::Show(std::string(title), "请先进入科兹中心豪劫终章。", NotificationType::Error);
			return thread;
		}

		// --- Primary Target ---
		static std::vector<std::pair<int, const char*>> kortzCenterTargets = {
		    {0, "最后的放纵"},
		    {1, "野兔独思"},
		    {2, "罗马的覆灭"},
		    {3, "兄弟兄弟"},
		    {4, "众生相"},
		    {5, "日渐衰败"},
		    {6, "真爱"},
		    {7, "屏息"},
		    {8, "圆满"},
		    {9, "我听见了声音"},
		    {10, "冬日，某个无名之地"},
		    {11, "戴珍珠项链的少女"},
		    {12, "水果闲谈"},
		    {13, "南瓜"},
		    {14, "双生之异"},
		    {15, "堆叠习作 V"},
		    {16, "我，水果"},
		    {17, "拐弯抹角"},
		    {18, "成功过剩"},
		    {19, "榨汁"},
		    {20, "蜿蜒归途"},
		    {21, "腊肠犬"},
		    {22, "信任"},
		    {23, "至死不渝"},
		    {24, "什么是甜瓜？"},
		    {25, "奋斗的成果"},
		    {26, "米奥混战"}};
		static ListCommand _KortzCenterPrimaryTarget{"kortzcenterheistprimarytarget", "主要目标", "选择科兹中心豪劫的主要目标艺术品。", kortzCenterTargets, 0};

		// --- General Purchases (MPX_K26_GENERAL_BS bits 5-8) ---
		static BoolCommand _KortzCenterGuardRoutes{"kortzcenterheistguardroutes", "保安巡逻路线", "将保安巡逻路线标记为已购买。", true};
		static BoolCommand _KortzCenterGlassCutter{"kortzcenterheistglasscutter", "玻璃切割器", "将玻璃切割器标记为已购买。", true};
		static BoolCommand _KortzCenterPowerDrills{"kortzcenterheistpowerdrills", "电钻", "将电钻标记为已购买。", true};
		static BoolCommand _KortzCenterEMPCharges{"kortzcenterheistempcharges", "电磁脉冲炸药", "将电磁脉冲炸药标记为已购买。", true};

		// --- Prep Work (MPX_K26_ROBBERY_PROG bits 0-15) ---
		static BoolCommand _KortzCenterScopeOut{"kortzcenterheistscopeout", "前期侦察", "将科兹中心侦察任务标记为完成。", true};
		static BoolCommand _KortzCenterAlphaMail{"kortzcenterheistalphamail", "阿尔法邮政伪装", "将阿尔法邮政伪装准备标记为完成。", true};
		static BoolCommand _KortzCenterHazmat{"kortzcenterheisthazmat", "防化服", "将防化服准备标记为完成。", true};
		static BoolCommand _KortzCenterStaffKeycard{"kortzcenterheiststaffkeycard", "工作人员钥匙卡", "将工作人员钥匙卡准备标记为完成。", true};
		static BoolCommand _KortzCenterTacticalEquip{"kortzcenterheisttacticalequip", "战术装备", "将战术装备准备标记为完成。", true};
		static BoolCommand _KortzCenterHackingDevice{"kortzcenterheisthackingdevice", "入侵设备", "将入侵设备准备标记为完成。", true};
		static BoolCommand _KortzCenterAccessCode{"kortzcenterheistaccesscode", "访问代码", "将访问代码准备标记为完成。", true};
		static BoolCommand _KortzCenterUnmarkedWeapons{"kortzcenterheistunmarkedweapons", "无标记武器", "将无标记武器准备标记为完成。", true};
		static BoolCommand _KortzCenterCaracara{"kortzcenterheistcaracara", "装甲卡拉卡拉", "将装甲卡拉卡拉准备标记为完成。", true};
		static BoolCommand _KortzCenterAnnihilator{"kortzcenterheistannihilator", "歼灭者隐形版", "将歼灭者隐形版准备标记为完成。", true};
		static BoolCommand _KortzCenterManchez{"kortzcenterheistmanchez", "曼切兹", "将曼切兹准备标记为完成。", true};
		static BoolCommand _KortzCenterPrepEMP{"kortzcenterheistprepemp", "电磁脉冲炸药（准备）", "将电磁脉冲炸药准备任务标记为完成。", true};
		static BoolCommand _KortzCenterGuardShipments{"kortzcenterheistguardshipments", "保安装备货物", "将保安装备货物准备标记为完成。", true};
		static BoolCommand _KortzCenterGuardRoutesPrep{"kortzcenterheistguardroutesprep", "保安巡逻路线（准备）", "将保安巡逻路线准备标记为完成。", true};
		static BoolCommand _KortzCenterGlassCutterPrep{"kortzcenterheistglasscutterprep", "玻璃切割器（准备）", "将玻璃切割器准备标记为完成。", true};
		static BoolCommand _KortzCenterPowerDrillsPrep{"kortzcenterheistpowerdrillsprep", "电钻（准备）", "将电钻准备标记为完成。", true};

		// --- Scoping ---
		static BoolCommand _KortzCenterScopeSecondary{"kortzcenterheistscopesecondary", "次要目标", "将全部次要目标标记为已侦察。", true};
		static BoolCommand _KortzCenterScopePOI{"kortzcenterheistscopepoi", "兴趣点", "将全部兴趣点标记为已侦察。", true};

		// --- Setup ---
		class Setup : public Command
		{
			using Command::Command;

			virtual void OnCall() override
			{
				if (!RequireSession("科兹中心豪劫"))
					return;

				int generalBits = kRecommendedGeneralBits;
				if (!_KortzCenterGuardRoutes.GetState())
					generalBits &= ~(1 << 5);
				if (!_KortzCenterGlassCutter.GetState())
					generalBits &= ~(1 << 6);
				if (!_KortzCenterPowerDrills.GetState())
					generalBits &= ~(1 << 7);
				if (!_KortzCenterEMPCharges.GetState())
					generalBits &= ~(1 << 8);

				int robberyProg = kRecommendedRobberyProg;
				if (!_KortzCenterScopeOut.GetState())
					robberyProg &= ~(1 << 0);
				if (!_KortzCenterAlphaMail.GetState())
					robberyProg &= ~(1 << 1);
				if (!_KortzCenterHazmat.GetState())
					robberyProg &= ~(1 << 2);
				if (!_KortzCenterStaffKeycard.GetState())
					robberyProg &= ~(1 << 3);
				if (!_KortzCenterTacticalEquip.GetState())
					robberyProg &= ~(1 << 4);
				if (!_KortzCenterHackingDevice.GetState())
					robberyProg &= ~(1 << 5);
				if (!_KortzCenterAccessCode.GetState())
					robberyProg &= ~(1 << 6);
				if (!_KortzCenterUnmarkedWeapons.GetState())
					robberyProg &= ~(1 << 7);
				if (!_KortzCenterCaracara.GetState())
					robberyProg &= ~(1 << 8);
				if (!_KortzCenterAnnihilator.GetState())
					robberyProg &= ~(1 << 9);
				if (!_KortzCenterManchez.GetState())
					robberyProg &= ~(1 << 10);
				if (!_KortzCenterPrepEMP.GetState())
					robberyProg &= ~(1 << 11);
				if (!_KortzCenterGuardShipments.GetState())
					robberyProg &= ~(1 << 12);
				if (!_KortzCenterGuardRoutesPrep.GetState())
					robberyProg &= ~(1 << 13);
				if (!_KortzCenterGlassCutterPrep.GetState())
					robberyProg &= ~(1 << 14);
				if (!_KortzCenterPowerDrillsPrep.GetState())
					robberyProg &= ~(1 << 15);

				int scopingBS = _KortzCenterScopeSecondary.GetState() ? -1 : 0;
				int poiBS = _KortzCenterScopePOI.GetState() ? kRecommendedPoiBits : 0;

				LOG(INFO) << "科兹中心豪劫配置：GENERAL_BS=" << generalBits << ", GENERAL_BS2=" << kRecommendedGeneralBits2 << ", ROBBERY_PROG=" << robberyProg
				          << ", TARGET=" << _KortzCenterPrimaryTarget.GetState() << ", SCOPING_BS=" << scopingBS << ", POI_BS=" << poiBS;
				Stats::SetInt("MPX_K26_GENERAL_BS", generalBits);
				Stats::SetInt("MPX_K26_GENERAL_BS2", kRecommendedGeneralBits2);
				Stats::SetInt("MPX_K26_ROBBERY_PROG", robberyProg);
				Stats::SetInt("MPX_K26_HEIST_TARGET", _KortzCenterPrimaryTarget.GetState());
				Stats::SetInt("MPX_K26_SCOPING_BS", scopingBS);
				Stats::SetInt("MPX_K26_POI_BS", poiBS);
				Notifications::Show("科兹中心豪劫", "所选配置已写入。", NotificationType::Success);
			}
		};

		class ApplyRecommendedSetup final : public Command
		{
			using Command::Command;

			void OnCall() override
			{
				if (!RequireSession("科兹中心豪劫"))
					return;

				LOG(INFO) << "正在应用科兹中心豪劫推荐配置，目标=" << _KortzCenterPrimaryTarget.GetState();
				Stats::SetInt("MPX_K26_GENERAL_BS", kRecommendedGeneralBits);
				Stats::SetInt("MPX_K26_GENERAL_BS2", kRecommendedGeneralBits2);
				Stats::SetInt("MPX_K26_ROBBERY_PROG", kRecommendedRobberyProg);
				Stats::SetInt("MPX_K26_HEIST_TARGET", _KortzCenterPrimaryTarget.GetState());
				Stats::SetInt("MPX_K26_SCOPING_BS", -1);
				Stats::SetInt("MPX_K26_POI_BS", kRecommendedPoiBits);
				Notifications::Show("科兹中心豪劫", "推荐配置已写入。", NotificationType::Success);
			}
		};

		class CutGlass final : public Command
		{
			using Command::Command;

			void OnCall() override
			{
				auto thread = RequireFinaleThread("快速切割玻璃");
				if (!thread)
					return;

				auto progress = ScriptLocal(thread, 32855).At(4, 13).At(3);
				if (!progress.CanAccess())
				{
					Notifications::Show("快速切割玻璃", "任务局部变量当前不可访问。", NotificationType::Error);
					return;
				}

				*progress.As<float*>() = 100.0f;
				Notifications::Show("快速切割玻璃", "玻璃切割进度已完成。", NotificationType::Success);
			}
		};

		class DisableLaserGrid final : public Command
		{
			using Command::Command;

			void OnCall() override
			{
				auto thread = RequireFinaleThread("关闭激光网格");
				if (!thread)
					return;

				auto local = ScriptLocal(thread, 70416);
				auto state = ScriptGlobal(1935711);
				if (!local.CanAccess() || !state.CanAccess())
				{
					Notifications::Show("关闭激光网格", "任务变量当前不可访问。", NotificationType::Error);
					return;
				}

				*local.As<int*>() = 4294784;
				*state.As<int*>() |= 1;
				Notifications::Show("关闭激光网格", "激光网格已关闭。", NotificationType::Success);
			}
		};

		class SkipDataCrack final : public Command
		{
			using Command::Command;

			void OnCall() override
			{
				auto thread = RequireFinaleThread("跳过数据破解");
				if (!thread)
					return;

				auto state = ScriptLocal(thread, 1388);
				if (!state.CanAccess())
				{
					Notifications::Show("跳过数据破解", "任务局部变量当前不可访问。", NotificationType::Error);
					return;
				}

				for (int index = 0; index < 8; index++)
					*state.At(index, 4).As<int*>() = 1;
				Notifications::Show("跳过数据破解", "数据破解已完成。", NotificationType::Success);
			}
		};

		class SkipFingerprint final : public Command
		{
			using Command::Command;

			void OnCall() override
			{
				auto thread = RequireFinaleThread("跳过指纹破解");
				if (!thread)
					return;

				auto state = ScriptLocal(thread, 26866);
				if (!state.CanAccess())
				{
					Notifications::Show("跳过指纹破解", "任务局部变量当前不可访问。", NotificationType::Error);
					return;
				}

				*state.As<int*>() = 5;
				Notifications::Show("跳过指纹破解", "指纹破解已完成。", NotificationType::Success);
			}
		};

		class SkipSignalNodes final : public Command
		{
			using Command::Command;

			void OnCall() override
			{
				auto thread = RequireFinaleThread("跳过信号节点");
				if (!thread)
					return;

				auto state = ScriptLocal(thread, 27914);
				if (!state.CanAccess())
				{
					Notifications::Show("跳过信号节点", "任务局部变量当前不可访问。", NotificationType::Error);
					return;
				}

				*state.As<int*>() = 5;
				Notifications::Show("跳过信号节点", "信号节点破解已完成。", NotificationType::Success);
			}
		};

		class TakePrimaryTarget final : public Command
		{
			using Command::Command;

			void OnCall() override
			{
				auto thread = RequireFinaleThread("拿取主要目标");
				if (!thread)
					return;

				auto target = ScriptLocal(thread, 29355).At(11);
				if (!target.CanAccess())
				{
					Notifications::Show("拿取主要目标", "任务局部变量当前不可访问。", NotificationType::Error);
					return;
				}

				*target.As<int*>() = 10;
				Notifications::Show("拿取主要目标", "已触发主要目标拿取。", NotificationType::Success);
			}
		};

		class TakeSecondaryTarget final : public Command
		{
			using Command::Command;

			void OnCall() override
			{
				auto thread = RequireFinaleThread("拿取次要目标");
				if (!thread)
					return;

				auto target = ScriptLocal(thread, 29355).At(11);
				if (!target.CanAccess())
				{
					Notifications::Show("拿取次要目标", "任务局部变量当前不可访问。", NotificationType::Error);
					return;
				}

				*target.As<int*>() = 3;
				Notifications::Show("拿取次要目标", "已触发次要目标拿取。", NotificationType::Success);
			}
		};

		class AutoEnterPcAccessCode final : public Command
		{
			using Command::Command;

			void OnCall() override
			{
				for (int index = 0; index < 3; index++)
				{
					auto thread = RequireFinaleThread("自动输入电脑访问代码");
					if (!thread)
						return;

					auto digit = ScriptLocal(thread, 32818).At(1).At(index, 2).At(1);
					if (!digit.CanAccess())
					{
						Notifications::Show("自动输入电脑访问代码", "任务局部变量当前不可访问。", NotificationType::Error);
						return;
					}

					*digit.As<int*>() = 0;
					ScriptMgr::Yield(100ms);
					PAD::SET_CONTROL_VALUE_NEXT_FRAME(0, 237, 1.0f);
				}

				Notifications::Show("自动输入电脑访问代码", "访问代码已自动输入。", NotificationType::Success);
			}
		};

		static Setup _KortzCenterSetup{"kortzcenterheistsetup", "应用设置", "写入所选目标、采购、准备任务和侦察状态。"};
		static ApplyRecommendedSetup _KortzCenterRecommendedSetup{"kortzcenterheistrecommendedsetup", "应用推荐配置", "使用经筛选的统计掩码完成全部采购、前置和侦察，同时保留所选主要目标。"};
		static CutGlass _KortzCenterCutGlass{"kortzcenterheistcutglass", "快速切割玻璃", "在科兹中心豪劫终章中完成玻璃切割进度。"};
		static DisableLaserGrid _KortzCenterDisableLaserGrid{"kortzcenterheistdisablelasergrid", "关闭激光网格", "在科兹中心豪劫终章中关闭激光防护网格。"};
		static SkipDataCrack _KortzCenterSkipDataCrack{"kortzcenterheistskipdatacrack", "跳过数据破解", "在科兹中心豪劫终章中完成数据破解。"};
		static SkipFingerprint _KortzCenterSkipFingerprint{"kortzcenterheistskipfingerprint", "跳过指纹破解", "在科兹中心豪劫终章中完成指纹破解。"};
		static SkipSignalNodes _KortzCenterSkipSignalNodes{"kortzcenterheistskipsignalnodes", "跳过信号节点", "在科兹中心豪劫终章中完成信号节点破解。"};
		static TakePrimaryTarget _KortzCenterTakePrimaryTarget{"kortzcenterheisttakeprimary", "拿取主要目标", "站在主要目标旁，在科兹中心豪劫终章中触发目标拿取。"};
		static TakeSecondaryTarget _KortzCenterTakeSecondaryTarget{"kortzcenterheisttakesecondary", "拿取次要目标", "站在次要目标旁，在科兹中心豪劫终章中触发目标拿取。"};
		static AutoEnterPcAccessCode _KortzCenterAutoEnterPcAccessCode{"kortzcenterheistautoenterpcaccesscode", "自动输入电脑访问代码", "在科兹中心豪劫终章中自动填写三位电脑访问代码。"};

		// [DOG] 开启单人次要目标（二层展厅）：移植自 UC 帖 4745112-post761（tidmouth）+ UltimateMenu
		// 说明：UC 原式为 Global_4980736.f_29174[i*333].f_68/.f_143 = 0，i ∈ {0,1} 高柜 / {5,6,7} 横柜 / {20,21} 艺术品。
		// 由于"数组跳表头 +1"的地址约定在增强/传承两版存在 ±1 不确定，这里两种候选地址都写入（均为 0，低风险）。
		class EnableSoloSecondaries final : public Command
		{
		public:
			using Command::Command;

			void OnCall() override
			{
				if (!RequireSession("单人次要目标"))
					return;

				static constexpr int kIndices[] = {0, 1, 5, 6, 7, 20, 21};
				static constexpr int kFields[] = {68, 143};

				bool anyAccessible = false;
				for (const int i : kIndices)
				{
					for (const int field : kFields)
					{
						// 候选 A（按 UC 原式直译）
						auto a = ScriptGlobal(4980736 + 29174 + i * 333 + field);
						if (a.CanAccess())
						{
							*a.As<int*>() = 0;
							anyAccessible = true;
						}

						// 候选 B（数组跳表头 +1）
						auto b = ScriptGlobal(4980736 + 1 + 29174 + i * 333 + field);
						if (b.CanAccess())
							*b.As<int*>() = 0;
					}
				}

				if (!anyAccessible)
				{
					Notifications::Show("单人次要目标", "全局变量当前不可访问。", NotificationType::Error);
					return;
				}

				Notifications::Show("单人次要目标", "已写入二层展厅单人拿取（实验性）。进入展厅验证：可正常打开原本需要多人的玻璃柜即成功。", NotificationType::Success);
			}
		};

		// [DOG] 立即完成终章（实验性）：数值来自 UltimateMenu a28c577（KCIf1/KCIf2，含传承版 +1 约定），
		// 增强版地址存在 ±1 不确定，两个候选都写入（状态=9 / 进度=50，与其他抢劫的立即完成同构）。
		class InstantFinish final : public Command
		{
		public:
			using Command::Command;

			void OnCall() override
			{
				auto thread = RequireFinaleThread("立即完成终章（实验性）");
				if (!thread)
					return;

				// 状态候选：56716 / 56717（±1 两种地址约定）
				*ScriptLocal(thread, 56716).As<int*>() = 9;
				*ScriptLocal(thread, 56717).As<int*>() = 9;

				// 进度候选：58492 / 58493 / 58494（对应 56716/56717 + 1776（±1））
				*ScriptLocal(thread, 56716).At(1776).As<int*>() = 50;
				*ScriptLocal(thread, 56716).At(1776).At(0, 1).As<int*>() = 50;
				*ScriptLocal(thread, 56717).At(1776).At(0, 1).As<int*>() = 50;

				Notifications::Show("立即完成终章", "已请求完成终章（实验性）。若数秒内无反应，请用「转储终章局部变量」前后各 dump 一份发作者。", NotificationType::Success);
			}
		};

		static EnableSoloSecondaries _KortzCenterSoloSecondaries{"kortzcentersolosecondaries", "开启单人次要目标（实验性）", "让二层展厅的次要目标可以单人拿取（社区方案移植，待实机验证）。"};
		static InstantFinish _KortzCenterInstantFinish{"kortzcenterinstantfinish", "立即完成终章（实验性）", "请求立即结算科兹终章（社区数值移植，待实机验证）。"};

		// [DOG] 研发工具：转储科兹终章脚本的全部局部变量（供定位“秒过”所需变量；有界遍历 m_StackSize/8）
		class DumpFinaleLocals : public Command
		{
		public:
			using Command::Command;

			virtual void OnCall() override
			{
				auto thread = Scripts::FindScriptThread("fm_mission_controller_v3"_J);
				if (!thread)
				{
					Notifications::Show("局部变量转储", "未找到科兹终章脚本（需在终章任务进行中）。", NotificationType::Error);
					return;
				}

				const auto count = thread->m_Context.m_StackSize / sizeof(std::uintptr_t);
				if (count == 0 || count > 200000)
				{
					Notifications::Show("局部变量转储", "脚本栈大小异常，已放弃转储。", NotificationType::Error);
					return;
				}

				std::time_t now = std::time(nullptr);
				char name[64]{};
				std::strftime(name, sizeof(name), "kortz_locals_%Y%m%d_%H%M%S.txt", std::localtime(&now));

				auto file = FileMgr::GetProjectFile(name);
				std::ofstream out(file.Path(), std::ios::trunc);
				if (!out.is_open())
				{
					Notifications::Show("局部变量转储", "无法写入转储文件。", NotificationType::Error);
					return;
				}

				out << "script=fm_mission_controller_v3 count=" << count << "\n";
				for (std::size_t i = 0; i < count; ++i)
				{
					const auto raw = *ScriptLocal(thread, i).As<std::uintptr_t*>();
					const int value = static_cast<int>(raw);
					float fvalue = 0.0f;
					std::memcpy(&fvalue, &value, sizeof(float));
					out << i << " 0x" << std::hex << raw << std::dec << " " << value << " " << fvalue << "\n";
				}

				out.close();
				Notifications::Show("局部变量转储",
				    std::format("已转储 {} 个局部变量 → {}", count, file.Path().string()),
				    NotificationType::Success);
			}
		};

		static DumpFinaleLocals _KortzCenterDumpLocals{"kortzdumplocals", "转储终章局部变量（研发）", "将科兹终章脚本的全部局部变量写入配置文件目录，用于定位“秒过”所需变量。"};
	}
}
