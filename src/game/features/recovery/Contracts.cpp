#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/gta/Stats.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	static bool RequireContractSession(std::string_view title)
	{
		if (*Pointers.IsSessionStarted)
			return true;

		Notifications::Show(std::string(title), "请先进入 GTA 在线模式。", NotificationType::Error);
		return false;
	}

	class SkipDrDrePreps final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!RequireContractSession("德瑞博士合约"))
				return;
			LOG(INFO) << "MPX_FIXER_STORY_BS: " << Stats::GetInt("MPX_FIXER_STORY_BS") << " -> 4095";
			Stats::SetInt("MPX_FIXER_STORY_BS", 4095);
			Notifications::Show("德瑞博士合约", "前置任务已标记为完成。", NotificationType::Success);
		}
	};

	class SkipCluckinBellPreps final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!RequireContractSession("当当钟农场突袭"))
				return;

			LOG(INFO) << "当当钟农场突袭打包布尔项 42108：" << Stats::GetPackedBool(42108) << " -> 0";
			LOG(INFO) << "当当钟农场突袭打包整数项 51019/51021/51023：" << Stats::GetPackedInt(51019) << "/" << Stats::GetPackedInt(51021) << "/" << Stats::GetPackedInt(51023) << " -> 2/2/4";
			LOG(INFO) << "MPX_SALV23_INST_PROG: " << Stats::GetInt("MPX_SALV23_INST_PROG") << " -> 31";
			Stats::SetPackedBool(42108, false);
			Stats::SetPackedInt(51019, 2);
			Stats::SetPackedInt(51021, 2);
			Stats::SetPackedInt(51023, 4);
			Stats::SetInt("MPX_SALV23_INST_PROG", 31);
			Notifications::Show("当当钟农场突袭", "前置任务已标记为完成。", NotificationType::Success);
		}
	};

	class SkipOscarGuzmanPreps final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!RequireContractSession("奥斯卡·古兹曼再度起航"))
				return;

			LOG(INFO) << "奥斯卡·古兹曼打包布尔项 51272: " << Stats::GetPackedBool(51272) << " -> 1";
			LOG(INFO) << "MPX_HACKER24_INST_BS: " << Stats::GetInt("MPX_HACKER24_INST_BS") << " -> 31";
			Stats::SetPackedBool(51272, true);
			Stats::SetInt("MPX_HACKER24_INST_BS", 31);
			Notifications::Show("奥斯卡·古兹曼再度起航", "前置任务已标记为完成。", NotificationType::Success);
		}
	};

	class SkipKnoWayOutPreps final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!RequireContractSession("无路可逃"))
				return;
			LOG(INFO) << "MPX_M25_AVI_MISSION_CURRENT: " << Stats::GetInt("MPX_M25_AVI_MISSION_CURRENT") << " -> 4";
			Stats::SetInt("MPX_M25_AVI_MISSION_CURRENT", 4);
			Notifications::Show("无路可逃", "前置任务已标记为完成。", NotificationType::Success);
		}
	};

	static std::vector<std::pair<int, const char*>> g_AutoShopContracts = {
	    {0, "联合储蓄合约"},
	    {1, "超级美钞交易"},
	    {2, "银行合约"},
	    {3, "电控单元差事"},
	    {4, "监狱合约"},
	    {5, "事务所交易"},
	    {6, "失落摩托帮合约"},
	    {7, "数据合约"},
	};

	static ListCommand _AutoShopContract{"autoshopcontract", "改装铺合约", "选择已在合约面板上接取的改装铺合约。", g_AutoShopContracts, 0};

	class SkipAutoShopPreps final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!RequireContractSession("改装铺合约"))
				return;

			const int contract = _AutoShopContract.GetState();
			if (contract < 0 || contract >= static_cast<int>(g_AutoShopContracts.size()))
			{
				Notifications::Show("改装铺合约", "合约选择无效。", NotificationType::Error);
				return;
			}

			LOG(INFO) << "MPX_TUNER_CURRENT: " << Stats::GetInt("MPX_TUNER_CURRENT") << " -> " << contract;
			LOG(INFO) << "MPX_TUNER_GEN_BS: " << Stats::GetInt("MPX_TUNER_GEN_BS") << " -> -1";
			Stats::SetInt("MPX_TUNER_CURRENT", contract);
			Stats::SetInt("MPX_TUNER_GEN_BS", -1);
			Notifications::Show("改装铺合约", "所选合约的前置任务已标记为完成。", NotificationType::Success);
		}
	};

	static SkipDrDrePreps _SkipDrDrePreps{"skipdrdrepreps", "跳过德瑞博士前置", "将德瑞博士合约的前置任务标记为完成。"};
	static SkipCluckinBellPreps _SkipCluckinBellPreps{"skipcluckinbellpreps", "跳过当当钟农场突袭前置", "将当当钟农场突袭的前置任务标记为完成。"};
	static SkipOscarGuzmanPreps _SkipOscarGuzmanPreps{"skiposcarguzmanpreps", "跳过奥斯卡前置", "将“奥斯卡·古兹曼再度起航”的前置任务标记为完成。"};
	static SkipKnoWayOutPreps _SkipKnoWayOutPreps{"skipknowayoutpreps", "跳过无路可逃前置", "将“无路可逃”的任务进度推进到终章前。"};
	static SkipAutoShopPreps _SkipAutoShopPreps{"skipautoshoppreps", "跳过所选合约前置", "先在改装铺面板接取并选择同一合约，再标记其前置任务为完成。"};
}
