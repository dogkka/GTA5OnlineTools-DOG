#include "Items.hpp"
#include "core/localization/Localization.hpp"

namespace YimMenu
{
	TabItem::TabItem(const std::string& name) :
	    m_Name(name)
	{
	}

	void TabItem::Draw()
	{
		const auto translatedName = Localization::Translate(m_Name);
		if (ImGui::BeginTabItem(translatedName.c_str()))
		{
			for (auto& item : m_Items)
			{
				if (item->CanDraw())
					item->Draw();
			}
			ImGui::EndTabItem();
		}
	}
}
