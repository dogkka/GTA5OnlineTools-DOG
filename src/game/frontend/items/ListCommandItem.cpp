#include "Items.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/localization/Localization.hpp"
#include "core/frontend/widgets/toggle/imgui_toggle.hpp"

namespace YimMenu
{
	ListCommandItem::ListCommandItem(joaat_t id, std::optional<std::string> label_override) :
	    m_Command(Commands::GetCommand<ListCommand>(id)),
	    m_LabelOverride(label_override)
	{
	}

	void ListCommandItem::Draw()
	{
		if (!m_Command)
		{
			ImGui::Text("%s", "未知列表！");
			return;
		}

		int current_val = m_Command->GetState();
		auto& list = m_Command->GetList();
		std::string largestString;
		std::size_t largestStringLen = 0;
		const auto label = Localization::TranslateLabel(m_LabelOverride.value_or(m_Command->GetLabel()));

		m_SelectedItem.reset();
		for (auto& item : list)
		{
			if (item.first == current_val)
			{
				m_SelectedItem = Localization::Translate(item.second);
			}

			auto translatedItem = Localization::Translate(item.second);
			if (translatedItem.length() > largestStringLen)
			{
				largestString = std::move(translatedItem);
				largestStringLen = largestString.length();
			}
		}

		if (!m_SelectedItem.has_value())
			m_SelectedItem = "";

		const auto previewText = m_SelectedItem->length() > largestStringLen ? *m_SelectedItem : largestString;
		const auto previewSize = ImGui::CalcTextSize(previewText.c_str());
		const auto labelSize = label.empty() ? ImVec2{} : ImGui::CalcTextSize(label.c_str());
		const auto framePadding = ImGui::GetStyle().FramePadding.x * 2.0f;
		const auto arrowWidth = ImGui::GetFrameHeight();
		const auto spacing = label.empty() ? 0.0f : ImGui::GetStyle().ItemInnerSpacing.x;
		m_ItemWidth = static_cast<int>(std::max(220.0f, previewSize.x + labelSize.x + framePadding + arrowWidth + spacing + 48.0f));

		ImGui::SetNextItemWidth(m_ItemWidth.value());
		if (ImGui::BeginCombo(label.c_str(), m_SelectedItem.value().c_str()))
		{
			for (auto& el : list)
			{
				const auto translatedItem = Localization::Translate(el.second);
				if (ImGui::Selectable(translatedItem.c_str(), el.first == current_val))
				{
					current_val = el.first;
					m_Command->SetState(el.first);
				}

				if (el.first == current_val)
				{
					m_SelectedItem = translatedItem; // just in case
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}
}
