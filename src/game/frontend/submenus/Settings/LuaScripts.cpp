#include "LuaScripts.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/localization/Localization.hpp"
#include "core/scripting/LuaManager.hpp"
#include "core/frontend/widgets/imgui_colors.h"

namespace YimMenu::Submenus
{
	std::shared_ptr<Category> BuildLuaScriptsMenu()
	{
		auto menu = std::make_shared<Category>("Lua 脚本");

		static std::shared_ptr<LuaScript> selectedScript;

		menu->AddItem(std::make_unique<ImGuiItem>([] {
			const float height = 15 * ImGui::GetTextLineHeightWithSpacing();
			if (ImGui::BeginListBox("##luascripts", {300.f, height}))
			{
				static std::optional<std::string> loadingScript;

				LuaManager::ForAllLoadedScripts([](std::shared_ptr<LuaScript>& script) {
					if (ImGui::Selectable(script->GetName().data(), script == selectedScript))
					{
						selectedScript = script;
					}
				});

				LuaManager::ForAllUnloadedScripts([](LuaManager::UnloadedScript& script) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::Colors::Gray.Value);
					if (ImGui::Selectable(script.m_Name.data(), false))
					{
						loadingScript = script.m_Path; // can't load the script directly from this callback or it'll deadlock
					}
					ImGui::PopStyleColor();

					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip("%s", "点击以加载该脚本");
					}
				});

				if (loadingScript)
				{
					LuaManager::LoadScript(*loadingScript);
					loadingScript = std::nullopt;
				}

				ImGui::EndListBox();
			}

			ImGui::SameLine();
		
			ImGui::BeginGroup();
			if (selectedScript && selectedScript->GetLoadState() == LuaScript::LoadState::UNLOADED)
				selectedScript.reset();

			if (selectedScript)
			{
				ImGui::Text("%s", selectedScript->GetName().data());

				bool paused = selectedScript->GetLoadState() == LuaScript::LoadState::PAUSED;
				if (ImGui::Button(paused ? "继续" : "暂停"))
				{
					if (paused)
						selectedScript->Resume();
					else
						selectedScript->Pause();
				}
				ImGui::SameLine();
				if (ImGui::Button("重新加载"))
				{
					selectedScript->Reload();
				}
				ImGui::SameLine();
				if (ImGui::Button("卸载"))
				{
					selectedScript->Unload();
				}
			}
			ImGui::EndGroup();
		}));

		return menu;
	}
}
