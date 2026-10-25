#include "Items.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/localization/Localization.hpp"
#include "core/frontend/widgets/toggle/imgui_toggle.hpp"

namespace YimMenu
{
	IntCommandItem::IntCommandItem(joaat_t id, std::optional<std::string> label_override, bool use_slider) :
	    m_useSlider(use_slider),
	    m_Command(Commands::GetCommand<IntCommand>(id)),
	    m_LabelOverride(label_override)
	{
	}

	void IntCommandItem::Draw()
	{
		if (!m_Command)
		{
			ImGui::Text("%s", "未知！");
			return;
		}

		int value = m_Command->GetState();
		const auto label = Localization::TranslateLabel(m_LabelOverride.has_value() ? m_LabelOverride.value() : m_Command->GetLabel());
		if (!m_Command->GetMinimum().has_value() || !m_Command->GetMaximum().has_value() || !m_useSlider)
		{
			ImGui::SetNextItemWidth(150);
			if (ImGui::InputInt(label.c_str(), &value))
			{
				m_Command->SetState(value);
			}
		}
		else
		{
			ImGui::SetNextItemWidth(150);
			if (ImGui::SliderInt(label.c_str(), &value, m_Command->GetMinimum().value(), m_Command->GetMaximum().value()))
			{
				m_Command->SetState(value);
			}
		}
	}
}
