#include "Settings.hpp"

#include "core/commands/Commands.hpp"
#include "core/commands/HotkeySystem.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/localization/Localization.hpp"
#include "game/backend/Self.hpp"
#include "game/backend/AnticheatBypass.hpp"
#include "game/frontend/items/Items.hpp"
#include "game/frontend/items/DrawHotkey.hpp"
#include "game/frontend/submenus/Settings/LuaScripts.hpp"
#include "game/frontend/submenus/Settings/GUISettings.hpp"
#include "game/features/protections/ScriptEventProtection.hpp"
#include "game/features/system/TunablesEditor.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Submenus
{
	// TODO: refactor this
	static void Hotkeys()
	{
		ImGui::BulletText("%s", "按住带有命令名称的按钮，然后输入按键即可修改热键。");
		ImGui::BulletText("%s", "如果命令已有热键，再次点击该按钮会移除它。");

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		
		// this assumes we can't add new commands in runtime, but a lot of other subsystems assume that too
		static std::map<std::string, CommandLink*> sortedCommands;
		static bool commandsSorted = []() {
			for (auto& [hash, command] : Commands::GetCommands())
			{
				if (auto it = g_HotkeySystem.m_CommandHotkeys.find(hash); it != g_HotkeySystem.m_CommandHotkeys.end())
					sortedCommands.emplace(command->GetLabel(), &it->second);
			}
			return true;
		}();

		HotkeySystem::SetBeingModifed(false);

		for (auto& [name, link] : sortedCommands)
		{
			if (name.empty())
				continue;
			DrawHotkey(link, name);
		}
	};

	Settings::Settings() :
	#define ICON_FA_GEARS "\xef\x80\x93"
	    Submenu::Submenu("设置", ICON_FA_GEARS)
	{
		auto hotkeys = std::make_shared<Category>("热键");
		auto gui = std::make_shared<Category>("界面");
		auto game = std::make_shared<Category>("游戏");

		auto uiStyle = std::make_shared<Group>("界面");
		auto playerEsp = std::make_shared<Group>("玩家 ESP", 10);
		auto pedEsp = std::make_shared<Group>("行人 ESP", 10);
		auto objectEsp = std::make_shared<Group>("物体 ESP");
		auto overlay = std::make_shared<Group>("叠加层");
		auto chat = std::make_shared<Group>("聊天");
		auto protection = std::make_shared<Group>("防护");
		auto tunables = std::make_shared<Group>("Tunables 编辑器");
		auto envCheck = std::make_shared<Group>("环境自检");

		hotkeys->AddItem(std::make_shared<ImGuiItem>(Hotkeys));

		// Players
		uiStyle->AddItem(std::make_shared<ListCommandItem>("styleselector"_J));

		playerEsp->AddItem(std::make_shared<BoolCommandItem>("espdrawplayers"_J));
		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<BoolCommandItem>("espdrawdeadplayers"_J)));

		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<BoolCommandItem>("espnameplayers"_J)));
		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<ColorCommandItem>("namecolorplayers"_J)));

		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<BoolCommandItem>("espdistanceplayers"_J)));

		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<BoolCommandItem>("espskeletonplayers"_J)));
		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<ColorCommandItem>("skeletoncolorplayers"_J)));
		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<BoolCommandItem>("esphealthplayers"_J)));
		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<BoolCommandItem>("espboxplayers"_J)));

		// Peds
		pedEsp->AddItem(std::make_shared<BoolCommandItem>("espdrawpeds"_J));
		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espdrawdeadpeds"_J)));

		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espmodelspeds"_J)));
		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<ColorCommandItem>("hashcolorpeds"_J)));

		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espnetinfopeds"_J)));
		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espscriptinfopeds"_J)));

		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espdistancepeds"_J)));

		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espskeletonpeds"_J)));
		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<ColorCommandItem>("skeletoncolorpeds"_J)));

		objectEsp->AddItem(std::make_shared<BoolCommandItem>("espdrawobjects"_J));
		objectEsp->AddItem(std::make_shared<ConditionalItem>("espdrawobjects"_J, std::make_shared<ColorCommandItem>("hashcolorobjects"_J)));
		objectEsp->AddItem(std::make_shared<ConditionalItem>("espdrawobjects"_J, std::make_shared<BoolCommandItem>("espnetinfoobjects"_J)));
		objectEsp->AddItem(std::make_shared<ConditionalItem>("espdrawobjects"_J, std::make_shared<BoolCommandItem>("espscriptinfoobjects"_J)));

		objectEsp->AddItem(std::make_shared<ConditionalItem>("espdrawobjects"_J, std::make_shared<BoolCommandItem>("espdistanceobjects"_J)));


		overlay->AddItem(std::make_shared<BoolCommandItem>("overlay"_J));
		overlay->AddItem(std::make_shared<ConditionalItem>("overlay"_J, std::make_shared<BoolCommandItem>("overlayfps"_J)));

		chat->AddItem(std::make_shared<CommandItem>("clearchat"_J));
		protection->AddItem(std::make_shared<BoolCommandItem>("scripteventprotection"_J));
		protection->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::Text("累计拦截：%llu", Features::GetProtectionBlockCount());

			if (ImGui::BeginChild("##protechistory", ImVec2(0, 140), true))
			{
				for (auto& entry : Features::GetProtectionLogSnapshot())
					ImGui::Text("%s  %s  %s", entry.Time.c_str(), entry.Player.c_str(), entry.Event.c_str());
			}
			ImGui::EndChild();

			if (ImGui::Button("清空记录"))
				Features::ClearProtectionLog();
		}));
		tunables->AddItem(std::make_shared<StringCommandItem>("tunablename"_J));
		tunables->AddItem(std::make_shared<IntCommandItem>("tunablevalue"_J, "值", false));
		tunables->AddItem(std::make_shared<ImGuiItem>([] { Features::RenderTunableEditor(); }));

		envCheck->AddItem(std::make_shared<ImGuiItem>([] {
			ImGui::Text("BattlEye：%s", AnticheatBypass::IsBattlEyeRunning() ? "运行中（危险！请立即退出并关闭 BE）" : "已关闭");
			ImGui::Text("FSL：%s", AnticheatBypass::IsFSLLoaded() ? "已加载" : "未加载");
			if (AnticheatBypass::IsFSLLoaded())
			{
				ImGui::Text("  FSL 版本：%d", AnticheatBypass::GetFSLVersion());
				ImGui::Text("  本地存档：%s", AnticheatBypass::IsFSLProvidingLocalSaves() ? "已启用" : "未启用");
				ImGui::Text("  BE 绕过标志：%s", AnticheatBypass::IsFSLProvidingBattlEyeBypass() ? "已提供" : "未提供");
			}
			ImGui::Separator();
			ImGui::Text("游戏版本：%s", Pointers.GameVersion ? Pointers.GameVersion : "未知");
			ImGui::Text("补丁版本：%s", AnticheatBypass::GetSupportedGameVersion());
			if (AnticheatBypass::IsOutdated())
			{
				ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "版本不匹配：函数还原已跳过，菜单可能异常，请等待适配新版");
			}
			else
			{
				ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.35f, 1.0f), "版本匹配：函数还原补丁可用");
			}
		}));

		auto executor = std::make_shared<Group>("命令执行器", -1);
		executor->AddItem(std::make_shared<StringCommandItem>("commandinput"_J));
		executor->AddItem(std::make_shared<CommandItem>("executecommand"_J));

		game->AddItem(playerEsp);
		game->AddItem(pedEsp);
		game->AddItem(objectEsp);
		game->AddItem(protection);
		game->AddItem(envCheck);
		game->AddItem(tunables);
		game->AddItem(executor);

		gui->AddItem(uiStyle);
		gui->AddItem(overlay);
		gui->AddItem(chat);

		AddCategory(std::move(hotkeys));
		AddCategory(std::move(gui));
		AddCategory(std::move(game));
		AddCategory(DrawGUISettingsMenu());
		AddCategory(BuildLuaScriptsMenu());
	}
}
