#include "Vehicle.hpp"

#include "game/frontend/items/Items.hpp"

namespace YimMenu::Submenus
{
	std::shared_ptr<Category> BuildVehicleMenu()
	{
		auto menu = std::make_shared<Category>("载具");

		auto control = std::make_shared<Group>("载具控制");
		control->AddItem(std::make_shared<PlayerCommandItem>("rcvehicle"_J));
		control->AddItem(std::make_shared<BoolCommandItem>("remotecontrol"_J));
		control->AddItem(std::make_shared<ConditionalItem>("remotecontrol"_J, std::make_shared<FloatCommandItem>("remotecontrolspeed"_J, "遥控速度")));
		control->AddItem(std::make_shared<PlayerCommandItem>("tpintovehicle"_J));
		control->AddItem(std::make_shared<PlayerCommandItem>("lockvehicle"_J));
		control->AddItem(std::make_shared<ListCommandItem>("lockvehiclemode"_J));

		auto troll = std::make_shared<Group>("载具整蛊");
		troll->AddItem(std::make_shared<PlayerCommandItem>("tyreburst"_J));
		troll->AddItem(std::make_shared<PlayerCommandItem>("smashwindows"_J));
		troll->AddItem(std::make_shared<PlayerCommandItem>("killengine"_J));
		troll->AddItem(std::make_shared<PlayerCommandItem>("flipvehicle"_J));
		troll->AddItem(std::make_shared<PlayerCommandItem>("vehbreakdoors"_J));
		troll->AddItem(std::make_shared<PlayerCommandItem>("vehpopscreen"_J));
		troll->AddItem(std::make_shared<PlayerCommandItem>("vehkillengine"_J));
		troll->AddItem(std::make_shared<PlayerCommandItem>("vehlockdown"_J));
		troll->AddItem(std::make_shared<PlayerCommandItem>("vehignite"_J));

		auto exterior = std::make_shared<Group>("外观与改装");
		exterior->AddItem(std::make_shared<PlayerCommandItem>("upgradevehicle"_J));
		exterior->AddItem(std::make_shared<PlayerCommandItem>("downgradevehicle"_J));
		exterior->AddItem(std::make_shared<PlayerCommandItem>("explodevehicle"_J));

		menu->AddItem(control);
		menu->AddItem(troll);
		menu->AddItem(exterior);

		return menu;
	}
}
