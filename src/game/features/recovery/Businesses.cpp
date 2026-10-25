#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/gta/Stats.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	static bool RequireBusinessSession(std::string_view title)
	{
		if (*Pointers.IsSessionStarted)
			return true;

		Notifications::Show(std::string(title), "请先进入 GTA 在线模式。", NotificationType::Error);
		return false;
	}

	class MaxNightclubPopularity final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!RequireBusinessSession("夜总会人气"))
				return;

			const int previous = Stats::GetInt("MPX_CLUB_POPULARITY");
			LOG(INFO) << "夜总会人气：" << previous << " -> 1000";
			Stats::SetInt("MPX_CLUB_POPULARITY", 1000);
			Notifications::Show("夜总会人气", "夜总会人气已设为最高。", NotificationType::Success);
		}
	};

	class ClearMoneyFrontsHeat final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!RequireBusinessSession("洗钱生意热度"))
				return;

			for (int index = 24924; index <= 24926; ++index)
			{
				LOG(INFO) << "洗钱生意打包统计项 " << index << ": " << Stats::GetPackedInt(index) << " -> 0";
				Stats::SetPackedInt(index, 0);
			}
			Notifications::Show("洗钱生意热度", "三项洗钱生意热度已清零。", NotificationType::Success);
		}
	};

	static MaxNightclubPopularity _MaxNightclubPopularity{"maxnightclubpopularity", "补满夜总会人气", "将夜总会人气设置为最高值。"};
	static ClearMoneyFrontsHeat _ClearMoneyFrontsHeat{"clearmoneyfrontsheat", "清除洗钱生意热度", "将三项洗钱生意的热度清零。"};
}
