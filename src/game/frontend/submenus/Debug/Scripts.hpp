#pragma once
#include "core/frontend/manager/Category.hpp"
#include "game/frontend/items/Items.hpp"

#include <string>
#include <string_view>

namespace YimMenu::Submenus
{
	std::shared_ptr<Category> BuildScriptsMenu();
	std::string LocalizeScriptDisplayName(std::string_view scriptName);
}
