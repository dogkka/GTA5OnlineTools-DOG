#include "Items.hpp"
#include "core/localization/Localization.hpp"
#include "misc/cpp/imgui_stdlib.h"


namespace YimMenu
{
	InputTextWithHint::InputTextWithHint(std::string label, std::string hint, std::string* buf, int flags, std::function<void()> cb, ImGuiInputTextCallback inputCallback) :
	    m_Id(label),
	    m_Hint(hint),
	    m_Buf(buf),
	    m_Flags(flags),
	    m_Callback(cb),
	    m_ImGuiInputTextCallback(inputCallback)
	{
	}

	void InputTextWithHint::Draw()
	{
		const auto translatedId = Localization::TranslateLabel(m_Id);
		const auto translatedHint = Localization::Translate(m_Hint);
		if (ImGui::InputTextWithHint(translatedId.data(), translatedHint.data(), m_Buf, m_Flags, m_ImGuiInputTextCallback))
			;
		{
			if (m_Callback != nullptr)
			{
				m_Callback();
			}
		}
	}
}
