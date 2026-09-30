#include "DoomsdayHeist.hpp"

namespace YimMenu::Submenus
{
	std::shared_ptr<TabItem> RenderDoomsdayHeistMenu()
	{
		auto tab = std::make_shared<TabItem>("末日豪劫");

		auto cuts = std::make_shared<Group>("抢劫分红", 2);
		auto setups = std::make_shared<Group>("抢劫设置");
		auto misc = std::make_shared<Group>("其他", 1);

		cuts->AddItem(std::make_shared<IntCommandItem>("doomsdayheistcut1"_J));
		cuts->AddItem(std::make_shared<IntCommandItem>("doomsdayheistcut3"_J));
		cuts->AddItem(std::make_shared<IntCommandItem>("doomsdayheistcut2"_J));
		cuts->AddItem(std::make_shared<IntCommandItem>("doomsdayheistcut4"_J));
		cuts->AddItem(std::make_shared<CommandItem>("doomsdayheistforceready"_J));
		cuts->AddItem(std::make_shared<CommandItem>("doomsdayheistsetcuts"_J));

		setups->AddItem(std::make_shared<ListCommandItem>("doomsdayheistcategory"_J));
		setups->AddItem(std::make_shared<CommandItem>("doomsdayheistsetup"_J));

		misc->AddItem(std::make_shared<CommandItem>("doomsdayheistskiphacking"_J));
		misc->AddItem(std::make_shared<CommandItem>("doomsdayheistinstantfinish"_J));
		misc->AddItem(std::make_shared<CommandItem>("doomsdayheistinstantfinishact3"_J));

		tab->AddItem(cuts);
		tab->AddItem(setups);
		tab->AddItem(misc);

		return tab;
	}
}
