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

		auto griefing = std::make_shared<Group>("骚扰", -1);
		griefing->AddItem(std::make_shared<PlayerCommandItem>("ceokick"_J));
		griefing->AddItem(std::make_shared<PlayerCommandItem>("sendsquad"_J));
		griefing->AddItem(std::make_shared<BoolCommandItem>("loopexplode"_J));
		griefing->AddItem(std::make_shared<BoolCommandItem>("loopragdoll"_J));
		griefing->AddItem(std::make_shared<BoolCommandItem>("loopkill"_J));
		griefing->AddItem(std::make_shared<BoolCommandItem>("loopbounty"_J));
		griefing->AddItem(std::make_shared<BoolCommandItem>("loopshake"_J));
		griefing->AddItem(std::make_shared<PlayerCommandItem>("animalattack"_J));
		griefing->AddItem(std::make_shared<PlayerCommandItem>("zombiehorde"_J));
		griefing->AddItem(std::make_shared<PlayerCommandItem>("ghostcar"_J));
		griefing->AddItem(std::make_shared<BoolCommandItem>("ghostcarhunt"_J));
		griefing->AddItem(std::make_shared<BoolCommandItem>("clownarmy"_J));
		griefing->AddItem(std::make_shared<PlayerCommandItem>("sharkattack"_J));
		griefing->AddItem(std::make_shared<PlayerCommandItem>("snowmanarmy"_J));
		griefing->AddItem(std::make_shared<PlayerCommandItem>("spawnbowlingball"_J));
		griefing->AddItem(std::make_shared<BoolCommandItem>("bowlingballhunt"_J));
		griefing->AddItem(std::make_shared<BoolCommandItem>("targetfireworks"_J));
		griefing->AddItem(std::make_shared<PlayerCommandItem>("pianodrop"_J));
		griefing->AddItem(std::make_shared<PlayerCommandItem>("giftrain"_J));
		griefing->AddItem(std::make_shared<PlayerCommandItem>("cagetarget"_J));
		griefing->AddItem(std::make_shared<PlayerCommandItem>("cagebox"_J));
		griefing->AddItem(std::make_shared<PlayerCommandItem>("safedrop"_J));

		auto events = std::make_shared<Group>("脚本事件", -1);
		events->AddItem(std::make_shared<PlayerCommandItem>("sendbounty"_J));
		events->AddItem(std::make_shared<PlayerCommandItem>("fakeban"_J));
		events->AddItem(std::make_shared<PlayerCommandItem>("vehkick"_J));
		events->AddItem(std::make_shared<PlayerCommandItem>("fakenotification"_J));
		events->AddItem(std::make_shared<PlayerCommandItem>("fakebanner"_J));
		events->AddItem(std::make_shared<PlayerCommandItem>("soundspam"_J));
		events->AddItem(std::make_shared<BoolCommandItem>("loopsoundspam"_J));
		events->AddItem(std::make_shared<StringCommandItem>("fakenotiftext"_J));
		events->AddItem(std::make_shared<StringCommandItem>("soundspamsound"_J));
		events->AddItem(std::make_shared<PlayerCommandItem>("ceoraid"_J));
		events->AddItem(std::make_shared<PlayerCommandItem>("forcemission"_J));
		events->AddItem(std::make_shared<PlayerCommandItem>("transerror"_J));
		events->AddItem(std::make_shared<PlayerCommandItem>("intkick"_J));
		events->AddItem(std::make_shared<PlayerCommandItem>("forceminigame"_J));
		events->AddItem(std::make_shared<ConditionalItem>("forceminigame"_J, std::make_shared<ListCommandItem>("minigame"_J)));
		events->AddItem(std::make_shared<PlayerCommandItem>("fakemoneybanked"_J));
		events->AddItem(std::make_shared<PlayerCommandItem>("fakemoneyremoved"_J));
		events->AddItem(std::make_shared<PlayerCommandItem>("fakemoneystolen"_J));
		events->AddItem(std::make_shared<IntCommandItem>("fakemoneyamount"_J, "金额", true));
		events->AddItem(std::make_shared<PlayerCommandItem>("moneyrain"_J));
		events->AddItem(std::make_shared<PlayerCommandItem>("supplydrop"_J));
		events->AddItem(std::make_shared<StringCommandItem>("trollplatetext"_J));
		events->AddItem(std::make_shared<PlayerCommandItem>("trollplate"_J));
		events->AddItem(std::make_shared<PlayerCommandItem>("forcecolor"_J));
		events->AddItem(std::make_shared<ConditionalItem>("forcecolor"_J, std::make_shared<ListCommandItem>("forcecolorselect"_J)));

		menu->AddItem(damage);
		menu->AddItem(griefing);
		menu->AddItem(events);

		return menu;
	}
}
