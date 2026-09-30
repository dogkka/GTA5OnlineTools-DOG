#include "Onboarding.hpp"
#include "GUI.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/localization/Localization.hpp"
#include "game/backend/AnticheatBypass.hpp"
#include "game/pointers/Pointers.hpp"
#include <shellapi.h>

namespace YimMenu
{
	static BoolCommand _OnboardingComplete{"$onboardingcomplete", "", ""};

	void ProcessOnboarding()
	{
		if (_OnboardingComplete.GetState())
			return;

		static bool ensure_popup_open = [] {
			ImGui::OpenPopup("重要！请务必阅读！");
			GUI::SetOnboarding(true);
			return true;
		}();

		const auto window_size = ImVec2{700, 500};
		const auto window_position = ImVec2{(*Pointers.ScreenResX - window_size.x) / 2, (*Pointers.ScreenResY - window_size.y) / 2};

		ImGui::SetNextWindowSize(window_size, ImGuiCond_Once);
		ImGui::SetNextWindowPos(window_position, ImGuiCond_Once);

		if (ImGui::BeginPopupModal("重要！请务必阅读！", nullptr, ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::TextWrapped("%s",
			    "欢迎使用 YimMenuV2。你可以按插入键或控制键+\\ 打开菜单。随着 BattlEye 的加入，加入并停留在公开战局的能力已受到很大限制。"
			    "你可以选择只与其他 YimMenu 用户一起游玩，也可以连接到常规的 BattlEye 保护战局。"
			    "如果你加入常规战局，通常会在三分钟内被自动踢出；即使重新启用 BattlEye，也可能在最多两天内被暂时限制加入。");
			static int value = 0;
			ImGui::RadioButton("仅与 YimMenu 用户游玩", &value, 0);
			ImGui::SameLine();
			ImGui::RadioButton("与所有人游玩（不可用！）", &value, 1);
			ImGui::TextWrapped("%s",
			    "你之后可以随时在“网络 > 伪装”里修改这个选项。官方仓库地址是 https://github.com/YimMenu/YimMenuV2。"
			    "请只从 GitHub 下载，以避免恶意软件。你也可以在仓库中提交问题、提出功能建议，或者通过拉取请求参与贡献。"
			    "另外我们还有 Matrix 社区服务器：https://matrix.to/#/#yimmenu:matrix.org，便于更快地与开发者和其他用户交流。");
			if (ImGui::Button("打开 GitHub"))
			{
				ShellExecuteA(NULL, "open", "https://github.com/YimMenu/YimMenuV2", NULL, NULL, SW_SHOWNORMAL);
			}
			ImGui::SameLine();
			if (ImGui::Button("打开 Matrix 服务器"))
			{
				ShellExecuteA(NULL, "open", "https://matrix.to/#/#yimmenu:matrix.org", NULL, NULL, SW_SHOWNORMAL);
			}
			ImGui::TextWrapped("%s",
			    "请定期检查更新；我们通常每晚都会发布新构建。最重要的是，注意风险，合理使用。");
			if (ImGui::Button("关闭"))
			{
				Commands::GetCommand<BoolCommand>("cheaterpool"_J)->SetState(!value);
				_OnboardingComplete.SetState(true);
				GUI::SetOnboarding(false);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}
