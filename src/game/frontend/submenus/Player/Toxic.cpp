#include "Toxic.hpp"

namespace YimMenu::Submenus
{
	std::shared_ptr<Category> BuildToxicMenu()
	{
		auto menu = std::make_shared<Category>("恶意");

		auto damage = std::make_shared<Group>("伤害", 1);
		damage->AddItem(std::make_shared<PlayerCommandItem>("kill"_J));
		damage->AddItem(std::make_shared<PlayerCommandItem>("killexploit"_J));
		damage->AddItem(std::make_shared<PlayerCommandItem>("explode"_J));

		auto griefing = std::make_shared<Group>("骚扰");
		griefing->AddItem(std::make_shared<PlayerCommandItem>("ceokick"_J));

		menu->AddItem(damage);
		menu->AddItem(griefing);

		return menu;
	}
}
