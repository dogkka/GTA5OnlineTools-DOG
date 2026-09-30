#include "Items.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/Commands.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/localization/Localization.hpp"
#include "game/frontend/Menu.hpp"

namespace YimMenu
{
	Group::Group(const std::string& name, int items_per_column) :
	    m_Name(name),
	    m_ItemsPerColumn(items_per_column)
	{
	}

	void Group::Draw()
	{
		if (!m_Name.empty())
		{
			ImGui::PushFont(Menu::Font::g_ChildTitleFont);
			const auto translatedName = Localization::Translate(m_Name);
			ImGui::Text("%s", translatedName.c_str());
			ImGui::PopFont();
			ImGui::Separator();
			ImGui::Spacing();
		}

		int item_count = 0;

		ImGui::BeginGroup();
		for (auto& item : m_Items)
		{
			if (!item->CanDraw())
				continue;

			item->Draw();
			item_count++;

			if (m_ItemsPerColumn != -1 && item_count % m_ItemsPerColumn == 0)
			{
				ImGui::EndGroup();
				ImGui::SameLine();
				ImGui::BeginGroup();
			}
		}
		ImGui::EndGroup();
	}
}
