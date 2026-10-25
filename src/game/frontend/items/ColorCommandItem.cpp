#include "Items.hpp"
#include "core/commands/ColorCommand.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/Commands.hpp"
#include "core/localization/Localization.hpp"
#include "core/frontend/widgets/toggle/imgui_toggle.hpp"

namespace YimMenu
{
	ColorCommandItem::ColorCommandItem(joaat_t id, std::optional<std::string> labelOverride) :
	    m_Command(Commands::GetCommand<ColorCommand>(id)),
	    m_LabelOverride(labelOverride)
	{
	}

	void ColorCommandItem::Draw()
	{
		if (!m_Command)
		{
			ImGui::Text("%s", "未知颜色选择器！");
			return;
		}

		auto color = m_Command->GetState();
		const auto label = Localization::TranslateLabel(m_LabelOverride.has_value() ? m_LabelOverride.value() : m_Command->GetLabel());

		ImGui::SameLine();

		ImGui::SetNextItemWidth(150);
		if (ImGui::ColorButton(label.c_str(), color))
		{
			ImGui::OpenPopup(label.c_str());
		}

		if (ImGui::BeginPopup(label.c_str()))
		{
			if (ImGui::ColorPicker4("##picker", (float*)&color))
			{
				m_Command->SetState(color);
			}
			ImGui::EndPopup();
		}
	}
}
