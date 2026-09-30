#include "ApartmentHeist.hpp"

namespace YimMenu::Submenus
{
	std::shared_ptr<TabItem> RenderApartmentHeistMenu()
	{
		auto tab = std::make_shared<TabItem>("公寓抢劫");

		auto cuts = std::make_shared<Group>("抢劫分红", 2);
		auto setups = std::make_shared<Group>("抢劫设置");
		auto misc = std::make_shared<Group>("其他", 1);

		cuts->AddItem(std::make_shared<IntCommandItem>("apartmentheistcut1"_J));
		cuts->AddItem(std::make_shared<IntCommandItem>("apartmentheistcut3"_J));
		cuts->AddItem(std::make_shared<IntCommandItem>("apartmentheistcut2"_J));
		cuts->AddItem(std::make_shared<IntCommandItem>("apartmentheistcut4"_J));
		cuts->AddItem(std::make_shared<CommandItem>("apartmentheistforceready"_J));
		cuts->AddItem(std::make_shared<CommandItem>("apartmentheistsetcuts"_J));

		setups->AddItem(std::make_shared<CommandItem>("apartmentheistsetup"_J));

		misc->AddItem(std::make_shared<CommandItem>("apartmentheistskiphacking"_J));
		misc->AddItem(std::make_shared<CommandItem>("apartmentheistskipdrilling"_J));
		misc->AddItem(std::make_shared<CommandItem>("apartmentheistskipswiping"_J));
		misc->AddItem(std::make_shared<CommandItem>("apartmentheistinstantfinish"_J));
		misc->AddItem(std::make_shared<CommandItem>("apartmentheistinstantfinishpacific"_J));

		tab->AddItem(cuts);
		tab->AddItem(setups);
		tab->AddItem(misc);

		return tab;
	}
}
