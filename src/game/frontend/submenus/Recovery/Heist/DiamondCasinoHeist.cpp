#include "DiamondCasinoHeist.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/backend/FiberPool.hpp"
#include "game/frontend/items/Items.hpp"

namespace YimMenu::Submenus
{
	// [DOG] 收益预设：写入输入框命令的状态并触发“应用”命令（社区通用数值）
	static void ApplyTakePreset(joaat_t inputCommand, joaat_t applyCommand, int value)
	{
		if (auto input = Commands::GetCommand<IntCommand>(inputCommand))
			input->SetState(value);

		if (auto apply = Commands::GetCommand(applyCommand))
			FiberPool::Push([apply] { apply->Call(); });
	}

	std::shared_ptr<TabItem> RenderDiamondCasinoHeistMenu()
	{
		auto tab = std::make_shared<TabItem>("名钻赌场豪劫");

		auto cuts = std::make_shared<Group>("抢劫分红", -1);
		auto setups = std::make_shared<Group>("抢劫设置");
		auto loots = std::make_shared<Group>("收益", 2);
		auto misc = std::make_shared<Group>("其他", 1);

		cuts->AddItem(std::make_shared<IntCommandItem>("diamondcasinoheistcut1"_J));
		cuts->AddItem(std::make_shared<IntCommandItem>("diamondcasinoheistcut3"_J));
		cuts->AddItem(std::make_shared<IntCommandItem>("diamondcasinoheistcut2"_J));
		cuts->AddItem(std::make_shared<IntCommandItem>("diamondcasinoheistcut4"_J));
		cuts->AddItem(std::make_shared<CommandItem>("diamondcasinoheistforceready"_J));
		cuts->AddItem(std::make_shared<CommandItem>("diamondcasinoheistsetcuts"_J));

		auto gunmanAndDriver = std::make_shared<Group>("", 2);
		gunmanAndDriver->AddItem(std::make_shared<ListCommandItem>("diamondcasinoheistgunman"_J));
		gunmanAndDriver->AddItem(std::make_shared<ListCommandItem>("diamondcasinoheistdriver"_J));
		gunmanAndDriver->AddItem(std::make_shared<ListCommandItem>("diamondcasinoheistweapon"_J));
		gunmanAndDriver->AddItem(std::make_shared<ListCommandItem>("diamondcasinoheistvehicle"_J));

		setups->AddItem(std::make_shared<ListCommandItem>("diamondcasinoheistdifficulty"_J));
		setups->AddItem(std::make_shared<ListCommandItem>("diamondcasinoheistprimarytarget"_J));
		setups->AddItem(std::make_shared<ListCommandItem>("diamondcasinoheistapproach"_J));
		setups->AddItem(std::move(gunmanAndDriver));
		setups->AddItem(std::make_shared<ListCommandItem>("diamondcasinoheisthacker"_J));
		setups->AddItem(std::make_shared<CommandItem>("diamondcasinoheistsetup"_J));

		loots->AddItem(std::make_shared<IntCommandItem>("diamondcasinoheistpotentialtake"_J));
		loots->AddItem(std::make_shared<IntCommandItem>("diamondcasinoheistactualtake"_J));
		loots->AddItem(std::make_shared<CommandItem>("diamondcasinoheistsetpotentialtake"_J, "设置##potentialtake"));
		loots->AddItem(std::make_shared<CommandItem>("diamondcasinoheistsetactualtake"_J, "设置##actualtake"));
		loots->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextDisabled("%s", "实际收益预设（单击 = 填入并应用）：");
			ImGui::SameLine();
			if (ImGui::Button("100万##casinopreset"))
				ApplyTakePreset("diamondcasinoheistactualtake"_J, "diamondcasinoheistsetactualtake"_J, 1000000);
			ImGui::SameLine();
			if (ImGui::Button("1000万##casinopreset"))
				ApplyTakePreset("diamondcasinoheistactualtake"_J, "diamondcasinoheistsetactualtake"_J, 10000000);
			ImGui::SameLine();
			if (ImGui::Button("1亿##casinopreset"))
				ApplyTakePreset("diamondcasinoheistactualtake"_J, "diamondcasinoheistsetactualtake"_J, 100000000);
			ImGui::SameLine();
			if (ImGui::Button("1.5亿##casinopreset"))
				ApplyTakePreset("diamondcasinoheistactualtake"_J, "diamondcasinoheistsetactualtake"_J, 150000000);
		}));

		misc->AddItem(std::make_shared<CommandItem>("diamondcasinoheistskiphacking"_J));
		misc->AddItem(std::make_shared<CommandItem>("diamondcasinoheistskipdrilling"_J));
		misc->AddItem(std::make_shared<CommandItem>("diamondcasinoheistsolomantrap"_J));
		misc->AddItem(std::make_shared<CommandItem>("diamondcasinoheistinstantfinish"_J));

		tab->AddItem(cuts);
		tab->AddItem(setups);
		tab->AddItem(loots);
		tab->AddItem(misc);

		return tab;
	}
}
