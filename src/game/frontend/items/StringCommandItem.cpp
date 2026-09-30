#include "Items.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/StringCommand.hpp"
#include "core/localization/Localization.hpp"
#include "imgui.h"

namespace YimMenu
{
	StringCommandItem::StringCommandItem(joaat_t id, std::optional<std::string> label_override) :
	    m_Command(Commands::GetCommand<StringCommand>(id)),
	    m_LabelOverride(label_override)
	{
	}

	void StringCommandItem::Draw()
	{
		if (!m_Command)
		{
			ImGui::Text("%s", "未知字符串命令！");
			return;
		}

		std::string current_value = m_Command->GetString();
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		memcpy(buffer, current_value.c_str(), current_value.size());

		ImGui::SetNextItemWidth(300.0f);
		const auto label = Localization::TranslateLabel(m_LabelOverride.value_or(m_Command->GetLabel()));
		if (ImGui::InputText(label.c_str(), buffer, sizeof(buffer)))
		{
			m_Command->SetStringValue(buffer);
		}
	}
}
