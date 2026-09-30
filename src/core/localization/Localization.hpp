#pragma once

#include <string>
#include <string_view>

namespace YimMenu::Localization
{
	std::string Translate(std::string_view text);
	std::string TranslateLabel(std::string_view text);
	std::string FormatHotkeyWindowTitle(std::string_view label);
}
