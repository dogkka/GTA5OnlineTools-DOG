#include "KortzCenterHeist.hpp"

namespace YimMenu::Submenus
{
	std::shared_ptr<TabItem> RenderKortzCenterHeistMenu()
	{
		auto tab = std::make_shared<TabItem>("科兹中心豪劫");

		auto target = std::make_shared<Group>("主要目标", 1);
		auto general = std::make_shared<Group>("常规采购", 2);
		auto preps = std::make_shared<Group>("前置任务", 2);
		auto scoping = std::make_shared<Group>("侦察", 2);
		auto finale = std::make_shared<Group>("终章辅助", 2);
		auto action = std::make_shared<Group>("", 1);

		target->AddItem(std::make_shared<ListCommandItem>("kortzcenterheistprimarytarget"_J));

		general->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistguardroutes"_J));
		general->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistglasscutter"_J));
		general->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistpowerdrills"_J));
		general->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistempcharges"_J));

		preps->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistscopeout"_J));
		preps->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistalphamail"_J));
		preps->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheisthazmat"_J));
		preps->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheiststaffkeycard"_J));
		preps->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheisttacticalequip"_J));
		preps->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheisthackingdevice"_J));
		preps->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistaccesscode"_J));
		preps->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistunmarkedweapons"_J));
		preps->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistcaracara"_J));
		preps->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistannihilator"_J));
		preps->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistmanchez"_J));
		preps->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistprepemp"_J));
		preps->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistguardshipments"_J));
		preps->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistguardroutesprep"_J));
		preps->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistglasscutterprep"_J));
		preps->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistpowerdrillsprep"_J));

		scoping->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistscopesecondary"_J));
		scoping->AddItem(std::make_shared<BoolCommandItem>("kortzcenterheistscopepoi"_J));

		action->AddItem(std::make_shared<CommandItem>("kortzcenterheistsetup"_J, "应用设置##kortz"));
		action->AddItem(std::make_shared<CommandItem>("kortzcenterheistrecommendedsetup"_J, "应用推荐配置##kortz"));

		finale->AddItem(std::make_shared<CommandItem>("kortzcenterheistcutglass"_J));
		finale->AddItem(std::make_shared<CommandItem>("kortzcenterheistdisablelasergrid"_J));
		finale->AddItem(std::make_shared<CommandItem>("kortzcenterheistskipdatacrack"_J));
		finale->AddItem(std::make_shared<CommandItem>("kortzcenterheistskipfingerprint"_J));
		finale->AddItem(std::make_shared<CommandItem>("kortzcenterheistskipsignalnodes"_J));
		finale->AddItem(std::make_shared<CommandItem>("kortzcenterheistautoenterpcaccesscode"_J));
		finale->AddItem(std::make_shared<CommandItem>("kortzcenterheisttakeprimary"_J));
		finale->AddItem(std::make_shared<CommandItem>("kortzcenterheisttakesecondary"_J));

		tab->AddItem(target);
		tab->AddItem(general);
		tab->AddItem(preps);
		tab->AddItem(scoping);
		tab->AddItem(finale);
		tab->AddItem(action);

		return tab;
	}
}
