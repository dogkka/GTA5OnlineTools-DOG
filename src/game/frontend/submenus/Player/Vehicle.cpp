#include "Vehicle.hpp"

#include "game/frontend/items/Items.hpp"

namespace YimMenu::Submenus
{
	std::shared_ptr<Category> BuildVehicleMenu()
	{
		auto menu = std::make_shared<Category>("Vehicle");

		auto group = std::make_shared<Group>("Vehicle Options");
		group->AddItem(std::make_shared<PlayerCommandItem>("tyreburst"_J));
		group->AddItem(std::make_shared<PlayerCommandItem>("lockvehicle"_J));
		group->AddItem(std::make_shared<ConditionalItem>("lockvehicle"_J, std::make_shared<ListCommandItem>("lockvehiclemode"_J)));

		menu->AddItem(group);

		return menu;
	}
}
