#include "Troll.hpp"

namespace YimMenu::Submenus
{
	std::shared_ptr<Category> BuildTrollMenu()
	{
		auto menu = std::make_shared<Category>("整蛊");

		auto general = std::make_shared<Group>("常规");
		auto bounty = std::make_shared<Group>("悬赏", 1);
		auto vehicle = std::make_shared<Group>("载具");

		general->AddItem(std::make_shared<PlayerCommandItem>("sendsext"_J));
		general->AddItem(std::make_shared<PlayerCommandItem>("sendphantomcar"_J));
		general->AddItem(std::make_shared<PlayerCommandItem>("sendgooch"_J));
		general->AddItem(std::make_shared<PlayerCommandItem>("giveweapons"_J));

		bounty->AddItem(std::make_shared<IntCommandItem>("bountyamount"_J));
		bounty->AddItem(std::make_shared<BoolCommandItem>("anonymousbounty"_J));
		bounty->AddItem(std::make_shared<PlayerCommandItem>("setbounty"_J));

		vehicle->AddItem(std::make_shared<PlayerCommandItem>("deleteveh"_J));

		menu->AddItem(general);
		menu->AddItem(bounty);
		menu->AddItem(vehicle);

		return menu;
	}
}
