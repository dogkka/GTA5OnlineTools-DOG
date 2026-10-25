#include "SalvageYardHeist.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Tunables.hpp"
#include "game/frontend/items/Items.hpp"
#include "game/gta/ScriptLocal.hpp"
#include "game/gta/Scripts.hpp"
#include "game/gta/Stats.hpp"

#include <vector>

namespace YimMenu::Submenus
{
	// [DOG] 赃车店槽位数据移植自 UltimateMenu V3（社区开源方案，GPL-2.0）
	static const std::pair<int, const char*> kSalvageRobberies[] = {
	    {0, "货船"},
	    {1, "街头帮派"},
	    {2, "杜根"},
	    {3, "领奖台"},
	    {4, "麦克托尼"},
	};

	static const std::pair<int, const char*> kSalvageVehicles[] = {
	{ 1, "LM87" },
	{ 2, "Cinquemila" },
	{ 3, "Autarch" },
	{ 4, "Tigon" },
	{ 5, "Champion" },
	{ 6, "10F" },
	{ 7, "SM722" },
	{ 8, "Omnis e-GT" },
	{ 9, "Growler" },
	{ 10, "Deity" },
	{ 11, "Itali RSX" },
	{ 12, "Coquette D10" },
	{ 13, "Jubilee" },
	{ 14, "Astron" },
	{ 15, "Comet S2 Cabr." },
	{ 16, "Torero" },
	{ 17, "Cheetah Classic" },
	{ 18, "Turismo Classic" },
	{ 19, "Infernus Classic" },
	{ 20, "Stafford" },
	{ 21, "GT500" },
	{ 22, "Viseris" },
	{ 23, "Mamba" },
	{ 24, "Coquette Black." },
	{ 25, "Stinger GT" },
	{ 26, "Z-Type" },
	{ 27, "Broadway" },
	{ 28, "Vigero ZX" },
	{ 29, "Buffalo STX" },
	{ 30, "Ruston" },
	{ 31, "Gauntl. Hellfire" },
	{ 32, "Dominator GTT" },
	{ 33, "Roosevelt Valor" },
	{ 34, "Swinger" },
	{ 35, "Stirling GT" },
	{ 36, "Omnis" },
	{ 37, "Tropos Rallye" },
	{ 38, "Jugular" },
	{ 39, "Patriot Mil-Spec" },
	{ 40, "Toros" },
	{ 41, "Caracara 4x4" },
	{ 42, "Sentinel Classic" },
	{ 43, "Weevil" },
	{ 44, "Blista Kanjo" },
	{ 45, "Eudora" },
	{ 46, "Kamacho" },
	{ 47, "Hellion" },
	{ 48, "Ellie" },
	{ 49, "Hermes" },
	{ 50, "Hustler" },
	{ 51, "Turismo Om." },
	{ 52, "Buffalo EVX" },
	{ 53, "Itali GTO St." },
	{ 54, "Virtue" },
	{ 55, "Ignus" },
	{ 56, "Zentorno" },
	{ 57, "Neon" },
	{ 58, "Furia" },
	{ 59, "Zorrusso" },
	{ 60, "Thrax" },
	{ 61, "Vagner" },
	{ 62, "Panthere" },
	{ 63, "Itali GTO" },
	{ 64, "S80RR" },
	{ 65, "Tyrant" },
	{ 66, "Entity MT" },
	{ 67, "Torero XO" },
	{ 68, "Neo" },
	{ 69, "Corsita" },
	{ 70, "Paragon R" },
	{ 71, "Franken Stange" },
	{ 72, "Comet Safari" },
	{ 73, "FR36" },
	{ 74, "Hotring Everon" },
	{ 75, "Komoda" },
	{ 76, "Tailgater S" },
	{ 77, "Jester Classic" },
	{ 78, "Jester RR" },
	{ 79, "Euros" },
	{ 80, "ZR350" },
	{ 81, "Cypher" },
	{ 82, "Dominator ASP" },
	{ 83, "Baller ST-D" },
	{ 84, "Casco" },
	{ 85, "Drift Yosemite" },
	{ 86, "Everon" },
	{ 87, "Penumbra FF" },
	{ 88, "V-STR" },
	{ 89, "Dominator GT" },
	{ 90, "Schlagen GT" },
	{ 91, "Cavalcade XL" },
	{ 92, "Clique" },
	{ 93, "Boor" },
	{ 94, "Sugoi" },
	{ 95, "Greenwood" },
	{ 96, "Brigham" },
	{ 97, "Issi Rally" },
	{ 98, "Seminole Fr." },
	{ 99, "Kanjo SJ" },
	{ 100, "Previon" },
	};

	static const std::pair<int, const char*> kSalvageMods[] = {
	    {0, "版本 1"},
	    {1, "版本 2"},
	    {2, "版本 3"},
	    {3, "版本 4"},
	    {4, "版本 5"},
	};

	static const std::pair<int, const char*> kSalvageKeeps[] = {
	    {0, "不可领走"},
	    {1, "可领走"},
	};

	template <typename T, std::size_t N>
	static std::vector<const char*> ToNames(const T (&items)[N])
	{
		std::vector<const char*> names;
		names.reserve(N);
		for (const auto& item : items)
			names.push_back(item.second);
		return names;
	}

	std::shared_ptr<TabItem> RenderSalvageYardMenu()
	{
		auto tab = std::make_shared<TabItem>("赃车店");

		auto preps = std::make_shared<Group>("前置", 1);
		auto payouts = std::make_shared<Group>("收益（出售价值）", 1);
		auto slots = std::make_shared<Group>("槽位配置", 1);
		auto missions = std::make_shared<Group>("任务", 1);

		preps->AddItem(std::make_shared<CommandItem>("salvagecompletepreps"_J, "完整前置"));
		preps->AddItem(std::make_shared<CommandItem>("salvageresetpreps"_J, "重置前置"));
		preps->AddItem(std::make_shared<CommandItem>("salvagereloadscreen"_J, "重载规划板"));
		preps->AddItem(std::make_shared<CommandItem>("salvageskipweeklycooldown"_J, "跳过每周冷却"));

		payouts->AddItem(std::make_shared<IntCommandItem>("salvagesellvalue1"_J));
		payouts->AddItem(std::make_shared<IntCommandItem>("salvagesellvalue2"_J));
		payouts->AddItem(std::make_shared<IntCommandItem>("salvagesellvalue3"_J));
		payouts->AddItem(std::make_shared<CommandItem>("salvageapplysellvalues"_J, "应用出售价值"));

		slots->AddItem(std::make_shared<ImGuiItem>([] {
			static const auto robberyNames = ToNames(kSalvageRobberies);
			static const auto vehicleNames = ToNames(kSalvageVehicles);
			static const auto modNames = ToNames(kSalvageMods);
			static const auto keepNames = ToNames(kSalvageKeeps);

			static int slot = 0;
			static int robberySel[3]{};
			static int vehicleSel[3]{};
			static int modSel[3]{};
			static int keepSel[3]{};

			ImGui::TextDisabled("%s", "选择每期抢劫的抢劫类型与载具（应用后回规划板查看）：");

			ImGui::SetNextItemWidth(120.0f);
			ImGui::Combo("槽位", &slot, "槽位 1\0槽位 2\0槽位 3\0");

			ImGui::SetNextItemWidth(180.0f);
			ImGui::Combo("抢劫", &robberySel[slot], robberyNames.data(), static_cast<int>(robberyNames.size()));

			ImGui::SetNextItemWidth(240.0f);
			ImGui::Combo("载具", &vehicleSel[slot], vehicleNames.data(), static_cast<int>(vehicleNames.size()));

			ImGui::SetNextItemWidth(140.0f);
			ImGui::Combo("改装版本", &modSel[slot], modNames.data(), static_cast<int>(modNames.size()));

			ImGui::SetNextItemWidth(140.0f);
			ImGui::Combo("可否领走", &keepSel[slot], keepNames.data(), static_cast<int>(keepNames.size()));

			auto applyTunable = [](int hash, int value) {
				Tunable tunable{static_cast<joaat_t>(hash)};
				if (tunable.IsReady())
					tunable.Set(value);
			};

			auto reloadPlanning = [] {
				if (auto thread = Scripts::FindScriptThread("vehrob_planning"_J))
					*ScriptLocal(thread, 545).As<int*>() = 2;
			};

			if (ImGui::Button("应用槽位配置##salvageslot"))
			{
				static constexpr int kRobberyTypes[3] = {1152433341, 852564222, 552662330};
				static constexpr int kVehicleTypes[3] = {-1012732012, 1366330161, 1806057372};
				static constexpr int kCanKeep[3] = {-1700733442, -1547046832, 1830093543};

				applyTunable(kRobberyTypes[slot], kSalvageRobberies[robberySel[slot]].first);
				applyTunable(kVehicleTypes[slot],
				    kSalvageVehicles[vehicleSel[slot]].first + kSalvageMods[modSel[slot]].first * 100);
				applyTunable(kCanKeep[slot], kSalvageKeeps[keepSel[slot]].first);
				reloadPlanning();

				Notifications::Show("赃车店", "槽位配置已应用，规划板已刷新。", NotificationType::Success);
			}

			ImGui::SameLine();

			if (ImGui::Button("置为可用（清除占用）##salvageslot"))
			{
				Stats::SetInt("MPX_SALV23_VEHROB_STATUS" + std::to_string(slot), 0);
				reloadPlanning();
				Notifications::Show("赃车店", "该槽位已置为可用。", NotificationType::Success);
			}
		}));

		missions->AddItem(std::make_shared<CommandItem>("salvageinstantfinish"_J, "立即完成（需在任务中）"));
		missions->AddItem(std::make_shared<CommandItem>("salvageinstantsell"_J, "瞬间出售（需在出售任务中、人在车上）"));

		tab->AddItem(preps);
		tab->AddItem(payouts);
		tab->AddItem(slots);
		tab->AddItem(missions);

		return tab;
	}
}
