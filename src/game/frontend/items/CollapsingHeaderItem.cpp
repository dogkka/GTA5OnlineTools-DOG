#include "Items.hpp"
#include "core/localization/Localization.hpp"

namespace YimMenu
{
	CollapsingHeaderItem::CollapsingHeaderItem(const std::string& name) :
	    m_Name(name)
	{
	}

	void CollapsingHeaderItem::Draw()
	{
		const auto translatedName = Localization::Translate(m_Name);
		if (ImGui::CollapsingHeader(translatedName.c_str()))
		{
			for (auto& item : m_Items)
			{
				if (item->CanDraw())
					item->Draw();
			}
		}
	}
}
