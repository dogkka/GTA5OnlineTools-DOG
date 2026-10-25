#include "Items.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/HotkeySystem.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/localization/Localization.hpp"
#include "DrawHotkey.hpp"

namespace YimMenu
{
	CommandItem::CommandItem(joaat_t id, std::optional<std::string> label_override) :
	    m_Command(Commands::GetCommand<Command>(id)),
	    m_LabelOverride(label_override)
	{
	}

	void CommandItem::Draw()
	{
		if (!m_Command)
		{
			ImGui::Text("%s", "未知！");
			return;
		}

		const auto label = Localization::TranslateLabel(m_LabelOverride.has_value() ? m_LabelOverride.value() : m_Command->GetLabel());
		const auto description = Localization::Translate(m_Command->GetDescription());
		if (ImGui::Button(label.c_str()))
		{
			// Capture the command by value, not 'this'. This item may be drawn from a 
			// Lua command handle's :draw()) and gets destroyed before the FiberPool task runs.
			auto command = m_Command;
			FiberPool::Push([command] {
				command->Call();
			});
		}

		// TODO: refactor this

		auto windowLabel = Localization::FormatHotkeyWindowTitle(m_Command->GetLabel());

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("%s", description.c_str());
			if (GetAsyncKeyState(VK_CAPITAL) & 0x8000)
				ImGui::OpenPopup(windowLabel.data());
		}

		ImGui::SetNextWindowSize(ImVec2(500, 120));
		if (ImGui::BeginPopupModal(windowLabel.data(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar))
		{
			ImGui::BulletText("%s", "输入一个按键。");

			ImGui::Separator();

			HotkeySystem::SetBeingModifed(true);

			if (auto it = g_HotkeySystem.m_CommandHotkeys.find(m_Command->GetHash()); it != g_HotkeySystem.m_CommandHotkeys.end())
				DrawHotkey(&it->second, label);

			ImGui::Spacing();
			if (ImGui::Button("关闭") || ((!ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered()) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)))
			{
				HotkeySystem::SetBeingModifed(false);
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}
