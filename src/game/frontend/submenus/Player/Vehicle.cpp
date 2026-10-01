#include "Vehicle.hpp"

#include "game/frontend/items/Items.hpp"

namespace YimMenu::Submenus
{
	std::shared_ptr<Category> BuildVehicleMenu()
	{
		auto menu = std::make_shared<Category>("载具");

		auto group = std::make_shared<Group>("载具选项");
		group->AddItem(std::make_shared<PlayerCommandItem>("tyreburst"_J));
		group->AddItem(std::make_shared<PlayerCommandItem>("lockvehicle"_J));
		group->AddItem(std::make_shared<ConditionalItem>("lockvehicle"_J, std::make_shared<ListCommandItem>("lockvehiclemode"_J)));
		group->AddItem(std::make_shared<PlayerCommandItem>("rcvehicle"_J));
		group->AddItem(std::make_shared<BoolCommandItem>("remotecontrol"_J));
		group->AddItem(std::make_shared<PlayerCommandItem>("smashwindows"_J));
		group->AddItem(std::make_shared<PlayerCommandItem>("killengine"_J));
		group->AddItem(std::make_shared<PlayerCommandItem>("flipvehicle"_J));
		group->AddItem(std::make_shared<PlayerCommandItem>("tpintovehicle"_J));
		group->AddItem(std::make_shared<PlayerCommandItem>("downgradevehicle"_J));
		group->AddItem(std::make_shared<PlayerCommandItem>("upgradevehicle"_J));
		group->AddItem(std::make_shared<PlayerCommandItem>("explodevehicle"_J));

		menu->AddItem(group);

		return menu;
	}
}
