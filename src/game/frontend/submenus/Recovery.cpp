#include "Recovery.hpp"
#include "Recovery/HeistModifier.hpp"
#include "Recovery/StatEditor.hpp"
#include "Recovery/Transactions.hpp"
#include "Recovery/DailyActivities.hpp"
#include "game/features/recovery/ClaimSafeEarnings.hpp"
#include "game/features/recovery/UnlockCenter.hpp"
#include "game/frontend/items/Items.hpp"

namespace YimMenu::Submenus
{
	Recovery::Recovery() :
#define ICON_FA_SACK_DOLLAR "\xef\x93\x80"
	    Submenu::Submenu("账号进度", ICON_FA_SACK_DOLLAR)
	{
		auto main = std::make_shared<Category>("基础");
		auto businesses = std::make_shared<Category>("产业");
		auto shopping = std::make_shared<Category>("商店");
		auto contracts = std::make_shared<Category>("合约");
		auto casino = std::make_shared<Category>("赌场");
		auto unlocks = std::make_shared<Category>("解锁中心");

		auto generalGroup = std::make_shared<Group>("常规");
		auto businessGroup = std::make_shared<Group>("常规");
		auto businessBalances = std::make_shared<Group>("保险箱余额", -1);
		auto businessProgress = std::make_shared<Group>("产业进度", 2);
		auto gunVanGroup = std::make_shared<Group>("枪支厢型车", 3);
		auto contractGroup = std::make_shared<Group>("任务前置", 2);
		auto autoShopGroup = std::make_shared<Group>("改装铺合约", 3);
		auto casinoSlots = std::make_shared<Group>("老虎机");
		auto casinoWheel = std::make_shared<Group>("幸运转盘");
		auto limitedUnlocks = std::make_shared<Group>("限时物品与涂装", 3);
		auto careerUnlocks = std::make_shared<Group>("职业进度", -1);
		auto awardUnlocks = std::make_shared<Group>("奖章", -1);
		auto achievementUnlocks = std::make_shared<Group>("平台成就", 2);
		auto unlockMaintenance = std::make_shared<Group>("批处理", -1);
		//auto casinoBlackJack = std::make_shared<Group>("二十一点");
		//auto casinoRoulette = std::make_shared<Group>("轮盘");

		generalGroup->AddItem(std::make_shared<BoolCommandItem>("playallmissionssolo"_J));
		generalGroup->AddItem(std::make_shared<BoolCommandItem>("unlockgtaplus"_J));
		generalGroup->AddItem(std::make_shared<BoolCommandItem>("overriderpmultiplier"_J));
		generalGroup->AddItem(std::make_shared<ConditionalItem>("overriderpmultiplier"_J, std::make_shared<FloatCommandItem>("rpmultiplierinput"_J)));
		generalGroup->AddItem(std::make_shared<IntCommandItem>("setranklevel"_J));
		generalGroup->AddItem(std::make_shared<CommandItem>("setrankapply"_J));
		generalGroup->AddItem(std::make_shared<BoolCommandItem>("freechangeappearance"_J));
		generalGroup->AddItem(std::make_shared<BoolCommandItem>("nochangeappearancecooldown"_J));
		generalGroup->AddItem(std::make_shared<BoolCommandItem>("allowgenderchange"_J));
		generalGroup->AddItem(std::make_shared<CommandItem>("saveonlineprogress"_J));

		businessGroup->AddItem(std::make_shared<ListCommandItem>("businesssafe"_J));
		businessGroup->AddItem(std::make_shared<CommandItem>("claimsafeearnings"_J));
		businessGroup->AddItem(std::make_shared<CommandItem>("maxnightclubpopularity"_J));
		businessGroup->AddItem(std::make_shared<BoolCommandItem>("keepnightclubpopularity"_J));
		businessGroup->AddItem(std::make_shared<ConditionalItem>("keepnightclubpopularity"_J, std::make_shared<IntCommandItem>("nightclubpopularityinterval"_J)));
		businessGroup->AddItem(std::make_shared<BoolCommandItem>("autocollectnightclubsafe"_J));
		businessGroup->AddItem(std::make_shared<ConditionalItem>("autocollectnightclubsafe"_J, std::make_shared<IntCommandItem>("nightclubsafeinterval"_J)));
		businessGroup->AddItem(std::make_shared<BoolCommandItem>("autoresetnightclubpaytime"_J));
		businessGroup->AddItem(std::make_shared<ConditionalItem>("autoresetnightclubpaytime"_J, std::make_shared<IntCommandItem>("nightclubpaytimeinterval"_J)));
		businessGroup->AddItem(std::make_shared<CommandItem>("clearmoneyfrontsheat"_J));

		businessBalances->AddItem(std::make_shared<ImGuiItem>([] {
			const auto balances = Features::BusinessSafes::GetBalances();
			if (!balances)
				return ImGui::TextDisabled("%s", "进入 GTA 在线模式后显示实时余额。");

			for (std::size_t index = 0; index < balances->size(); index++)
				ImGui::Text("%s：$%d", Features::BusinessSafes::Names[index].data(), (*balances)[index]);
		}));

		businessProgress->AddItem(std::make_shared<CommandItem>("completemsbakerstory"_J));
		businessProgress->AddItem(std::make_shared<CommandItem>("completeacidlabsetup"_J));

		gunVanGroup->AddItem(std::make_shared<ListCommandItem>("gunvanslot"_J));
		gunVanGroup->AddItem(std::make_shared<ListCommandItem>("gunvanweapon"_J));
		gunVanGroup->AddItem(std::make_shared<CommandItem>("setgunvanweapon"_J));

		contractGroup->AddItem(std::make_shared<CommandItem>("skipdrdrepreps"_J));
		contractGroup->AddItem(std::make_shared<CommandItem>("skipcluckinbellpreps"_J));
		contractGroup->AddItem(std::make_shared<CommandItem>("skiposcarguzmanpreps"_J));
		contractGroup->AddItem(std::make_shared<CommandItem>("skipknowayoutpreps"_J));
		// [DOG] 佩里科岛前置跳过入口（与 抢劫 → 佩里科岛 →「配置任务」同一命令）
		contractGroup->AddItem(std::make_shared<CommandItem>("cayopericoheistsetup"_J, "跳过佩里科岛前置"));

		autoShopGroup->AddItem(std::make_shared<ListCommandItem>("autoshopcontract"_J));
		autoShopGroup->AddItem(std::make_shared<CommandItem>("skipautoshoppreps"_J));

		casinoSlots->AddItem(std::make_shared<BoolCommandItem>("casinomanipulaterigslotmachines"_J));
		casinoSlots->AddItem(std::make_shared<ConditionalItem>("casinomanipulaterigslotmachines"_J, std::make_shared<ListCommandItem>("casinoslotmode"_J)));
		casinoSlots->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextDisabled("%s", "需在赌场老虎机附近生效。");
		}));

		casinoWheel->AddItem(std::make_shared<BoolCommandItem>("casinoluckywheel"_J));
		casinoWheel->AddItem(std::make_shared<ConditionalItem>("casinoluckywheel"_J, std::make_shared<ListCommandItem>("casinoluckywheelprize"_J)));
		casinoWheel->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextDisabled("%s", "站在幸运转盘旁、转动前生效。");
		}));

		limitedUnlocks->AddItem(std::make_shared<CommandItem>("unlocklimiteditems"_J));
		limitedUnlocks->AddItem(std::make_shared<CommandItem>("unlockvehiclepaints"_J));
		limitedUnlocks->AddItem(std::make_shared<CommandItem>("skipbusinessintros"_J));
		limitedUnlocks->AddItem(std::make_shared<ImGuiItem>([] {
			const auto progress = Features::UnlockCenter::GetLimitedProgress();
			if (!progress.Initialized)
				return ImGui::TextDisabled("%s", "限时物品：尚未建立批次");
			ImGui::Text("限时物品：%zu/%zu%s", progress.Processed, progress.Total, progress.Running ? "（处理中）" : "");
		}));

		careerUnlocks->AddItem(std::make_shared<CommandItem>("processcareerunlocks"_J));
		careerUnlocks->AddItem(std::make_shared<ImGuiItem>([] {
			const auto progress = Features::UnlockCenter::GetCareerProgress();
			if (!progress.Initialized)
				return ImGui::TextDisabled("%s", "状态：尚未扫描");
			ImGui::Text("状态：%zu/%zu%s", progress.Processed, progress.Total, progress.Running ? "（处理中）" : "");
		}));

		awardUnlocks->AddItem(std::make_shared<CommandItem>("processawardunlock"_J));
		awardUnlocks->AddItem(std::make_shared<ImGuiItem>([] {
			const auto progress = Features::UnlockCenter::GetAwardProgress();
			if (!progress.Initialized)
				return ImGui::TextDisabled("%s", "状态：尚未扫描");
			ImGui::Text("状态：%zu/%zu%s", progress.Processed, progress.Total, progress.Running ? "（处理中）" : "");
		}));

		achievementUnlocks->AddItem(std::make_shared<IntCommandItem>("achievementid"_J, std::nullopt, false));
		achievementUnlocks->AddItem(std::make_shared<CommandItem>("unlockachievement"_J));
		unlockMaintenance->AddItem(std::make_shared<CommandItem>("resetunlockprogress"_J));

		auto orgTools = std::make_shared<Group>("组织工具（实验性）", -1);
		orgTools->AddItem(std::make_shared<IntCommandItem>("ceoslot"_J, "颜色槽位"));
		orgTools->AddItem(std::make_shared<IntCommandItem>("ceocolour"_J, "颜色编号"));
		orgTools->AddItem(std::make_shared<CommandItem>("ceocolourapply"_J));
		orgTools->AddItem(std::make_shared<CommandItem>("ceoappeal"_J));

		auto carMeet = std::make_shared<Group>("车友会", -1);
		carMeet->AddItem(std::make_shared<IntCommandItem>("carclublevel"_J, "等级"));
		carMeet->AddItem(std::make_shared<CommandItem>("setcarclublelevel"_J));
		carMeet->AddItem(std::make_shared<CommandItem>("unlockprizeride"_J));

		main->AddItem(generalGroup);
		main->AddItem(orgTools);
		main->AddItem(carMeet);
		businesses->AddItem(businessGroup);
		businesses->AddItem(businessBalances);
		businesses->AddItem(businessProgress);
		shopping->AddItem(gunVanGroup);
		contracts->AddItem(contractGroup);
		contracts->AddItem(autoShopGroup);
		auto casinoTables = std::make_shared<Group>("赌桌控制（实验性）", -1);
		casinoTables->AddItem(std::make_shared<BoolCommandItem>("rigblackjack"_J));
		casinoTables->AddItem(std::make_shared<BoolCommandItem>("rigroulette"_J));
		casinoTables->AddItem(std::make_shared<ConditionalItem>("rigroulette"_J, std::make_shared<IntCommandItem>("rigroulettevalue"_J, "轮盘结果")));
		casino->AddItem(casinoTables);
		casino->AddItem(casinoSlots);
		casino->AddItem(casinoWheel);
		unlocks->AddItem(limitedUnlocks);
		unlocks->AddItem(careerUnlocks);
		unlocks->AddItem(awardUnlocks);
		unlocks->AddItem(achievementUnlocks);
		unlocks->AddItem(unlockMaintenance);

		AddCategory(std::move(main));
		AddCategory(std::move(businesses));
		AddCategory(std::move(shopping));
		AddCategory(std::move(contracts));
		AddCategory(std::move(casino));
		AddCategory(std::move(unlocks));
		AddCategory(BuildStatEditorMenu());
		AddCategory(BuildTransactionsMenu());
		AddCategory(BuildHeistModifierMenu());
		AddCategory(BuildDailyActivitiesMenu());
	}
}
