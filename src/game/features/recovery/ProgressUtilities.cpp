#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/gta/Stats.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	static bool RequireProgressSession(std::string_view title)
	{
		if (Pointers.IsSessionStarted && *Pointers.IsSessionStarted)
			return true;

		Notifications::Show(std::string(title), "请先进入 GTA 在线模式。", NotificationType::Error);
		return false;
	}

	class SaveOnlineProgress final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!RequireProgressSession("保存在线进度"))
				return;

			if (Stats::SaveStats())
				Notifications::Show("保存在线进度", "云端统计保存请求已提交。", NotificationType::Success);
			else
				Notifications::Show("保存在线进度", "保存请求未被立即接受，请稍后重试。", NotificationType::Warning);
		}
	};

	class CompleteMsBakerStory final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!RequireProgressSession("贝克女士任务"))
				return;

			const int previousFlow = Stats::GetInt("MPX_VCM_FLOW_PROGRESS");
			const int completedFlow = previousFlow | 1312735;
			LOG(INFO) << "MPX_VCM_FLOW_PROGRESS: " << previousFlow << " -> " << completedFlow;
			LOG(INFO) << "MPX_VCM_STORY_PROGRESS: " << Stats::GetInt("MPX_VCM_STORY_PROGRESS") << " -> 5";
			Stats::SetInt("MPX_VCM_FLOW_PROGRESS", completedFlow);
			Stats::SetInt("MPX_VCM_STORY_PROGRESS", 5);
			Notifications::Show("贝克女士任务", "故事任务已推进到终章完成状态。", NotificationType::Success);
		}
	};

	class CompleteAcidLabSetup final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!RequireProgressSession("致幻剂实验室"))
				return;

			LOG(INFO) << "MPX_XM22_FLOW: " << Stats::GetInt("MPX_XM22_FLOW") << " -> -1";
			Stats::SetInt("MPX_XM22_FLOW", -1);
			Notifications::Show("致幻剂实验室", "相关流程已标记为完成。", NotificationType::Success);
		}
	};

	static SaveOnlineProgress _SaveOnlineProgress{"saveonlineprogress", "保存在线进度", "请求将当前在线统计数据保存到云端。"};
	static CompleteMsBakerStory _CompleteMsBakerStory{"completemsbakerstory", "完成贝克女士任务", "将贝克女士故事任务推进到终章完成状态。"};
	static CompleteAcidLabSetup _CompleteAcidLabSetup{"completeacidlabsetup", "完成致幻剂实验室流程", "将致幻剂实验室相关流程标记为完成。"};
}
