#include "CayoPericoHeist.hpp"
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

	std::shared_ptr<TabItem> RenderCayoPericoHeistMenu()
	{
		auto tab = std::make_shared<TabItem>("佩里科岛抢劫");

		auto cuts = std::make_shared<Group>("抢劫分红", -1);
		auto setups = std::make_shared<Group>("抢劫设置");
		auto loots = std::make_shared<Group>("收益", 2);
		auto misc = std::make_shared<Group>("其他", 1);

		cuts->AddItem(std::make_shared<IntCommandItem>("cayopericoheistcut1"_J));
		cuts->AddItem(std::make_shared<IntCommandItem>("cayopericoheistcut3"_J));
		cuts->AddItem(std::make_shared<IntCommandItem>("cayopericoheistcut2"_J));
		cuts->AddItem(std::make_shared<IntCommandItem>("cayopericoheistcut4"_J));
		cuts->AddItem(std::make_shared<CommandItem>("cayopericoheistforceready"_J));
		cuts->AddItem(std::make_shared<CommandItem>("cayopericoheistsetcuts"_J));

		setups->AddItem(std::make_shared<ListCommandItem>("cayopericoheistdifficulty"_J));
		setups->AddItem(std::make_shared<ListCommandItem>("cayopericoheistprimarytarget"_J));
		setups->AddItem(std::make_shared<ListCommandItem>("cayopericoheistweapon"_J));
		setups->AddItem(std::make_shared<CommandItem>("cayopericoheistsetup"_J));

		loots->AddItem(std::make_shared<IntCommandItem>("cayopericoheistprimarytargetvalue"_J));
		loots->AddItem(std::make_shared<IntCommandItem>("cayopericoheistsecondarytakevalue"_J));
		loots->AddItem(std::make_shared<CommandItem>("cayopericoheistsetprimarytargetvalue"_J, "设置##primarytargetvalue"));
		loots->AddItem(std::make_shared<CommandItem>("cayopericoheistsetsecondarytakevalue"_J, "设置##secondarytakevalue"));
		loots->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextDisabled("%s", "次要收益预设（单击 = 填入并应用）：");
			ImGui::SameLine();
			if (ImGui::Button("100万##cayopreset"))
				ApplyTakePreset("cayopericoheistsecondarytakevalue"_J, "cayopericoheistsetsecondarytakevalue"_J, 1000000);
			ImGui::SameLine();
			if (ImGui::Button("1000万##cayopreset"))
				ApplyTakePreset("cayopericoheistsecondarytakevalue"_J, "cayopericoheistsetsecondarytakevalue"_J, 10000000);
			ImGui::SameLine();
			if (ImGui::Button("1亿##cayopreset"))
				ApplyTakePreset("cayopericoheistsecondarytakevalue"_J, "cayopericoheistsetsecondarytakevalue"_J, 100000000);
			ImGui::SameLine();
			if (ImGui::Button("1.5亿##cayopreset"))
				ApplyTakePreset("cayopericoheistsecondarytakevalue"_J, "cayopericoheistsetsecondarytakevalue"_J, 150000000);
		}));

		misc->AddItem(std::make_shared<CommandItem>("cayopericoheistskiphacking"_J));
		misc->AddItem(std::make_shared<CommandItem>("cayopericoheistcutsewer"_J));
		misc->AddItem(std::make_shared<CommandItem>("cayopericoheistcutglass"_J));
		misc->AddItem(std::make_shared<CommandItem>("cayopericoheisttakeprimarytarget"_J));
		misc->AddItem(std::make_shared<CommandItem>("cayopericoheistinstantfinish"_J));
		misc->AddItem(std::make_shared<CommandItem>("cayopericoheistkillcooldownsolo"_J));
		misc->AddItem(std::make_shared<CommandItem>("cayopericoheistkillcooldownteam"_J));
		misc->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::TextDisabled("%s", "冷却清除：点击后切换一次战局生效（再刷一轮前用）。");
		}));

		tab->AddItem(cuts);
		tab->AddItem(setups);
		tab->AddItem(loots);
		tab->AddItem(misc);

		return tab;
	}
}
