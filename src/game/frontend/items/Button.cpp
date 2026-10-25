#include "Items.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/localization/Localization.hpp"

namespace YimMenu
{
	Button::Button(const std::string_view& name, std::function<void()> onClick, const ImVec2 size, const std::string_view& information) :
	    m_Name(name),
	    m_Information(information),
	    m_OnClick(onClick),
	    m_Size(size)
	{
	}

	void Button::Draw()
	{
		const auto translatedName = Localization::TranslateLabel(m_Name);
		if (ImGui::Button(translatedName.c_str(), m_Size))
		{
			if (m_OnClick != nullptr)
			{
				FiberPool::Push([&] {
					m_OnClick();
				});
			}
		}
	}
}
