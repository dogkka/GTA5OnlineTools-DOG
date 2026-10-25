#include "TunablesEditor.hpp"

#include "core/commands/IntCommand.hpp"
#include "core/commands/StringCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/util/Joaat.hpp"
#include "game/backend/Tunables.hpp"

namespace YimMenu::Features
{
	static StringCommand _TunableName{"tunablename", "Tunable 名称", "tunable 的名称，例如 IDLEKICK_KICK"};
	static IntCommand _TunableValue{"tunablevalue", "Tunable 值", "要写入的整数值"};

	static std::optional<ScriptGlobal> FindTunable()
	{
		const auto name = _TunableName.GetString();
		if (name.empty())
			return std::nullopt;

		return Tunables::GetTunable(Joaat(name));
	}

	void RenderTunableEditor()
	{
		const auto name = _TunableName.GetString();
		if (name.empty())
		{
			ImGui::TextUnformatted("输入 tunable 名称后显示当前值。");
			return;
		}

		if (auto global = FindTunable())
			ImGui::Text("当前值：%d", *global->As<int*>());
		else
			ImGui::TextUnformatted("未找到该 tunable（进入战局加载完成后才可用）。");

		if (ImGui::Button("读取"))
		{
			if (auto global = FindTunable())
			{
				_TunableValue.SetState(*global->As<int*>());
				Notifications::Show("Tunables", "已读取当前值。", NotificationType::Info);
			}
			else
			{
				Notifications::Show("Tunables", "未找到该 tunable。", NotificationType::Warning);
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("写入"))
		{
			if (auto global = FindTunable())
			{
				*global->As<int*>() = _TunableValue.GetState();
				Notifications::Show("Tunables", "已写入新值。", NotificationType::Success);
			}
			else
			{
				Notifications::Show("Tunables", "未找到该 tunable。", NotificationType::Warning);
			}
		}
	}
}
