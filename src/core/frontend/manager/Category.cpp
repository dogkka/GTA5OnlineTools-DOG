#include "Category.hpp"
#include "core/localization/Localization.hpp"

namespace YimMenu
{
	void Category::Draw()
	{
		for (auto& item : m_Items)
			item->Draw();
	}

	int Category::GetLength()
	{
		if (m_Length.has_value())
			return m_Length.value();

		const auto translatedName = Localization::Translate(m_Name);
		m_Length = std::max(ImGui::CalcTextSize(translatedName.c_str()).x + 25.0f, 75.0f);
		return m_Length.value();
	}
}
