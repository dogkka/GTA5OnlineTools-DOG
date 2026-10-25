#include "game/pointers/Pointers.hpp"
#include "game/frontend/Menu.hpp"
#include "core/frontend/manager/UIManager.hpp"
#include "core/localization/Localization.hpp"
#include "game/frontend/submenus/Settings/GUISettings.hpp"

#include <algorithm>

namespace YimMenu
{
	// 现代（纵向）：单窗口布局 —— 左侧子菜单栏 + 右侧分类栏/内容，整窗一起拖动
	void RenderModernVTheme()
	{
		YimMenu::SyncColorCommandsToStyle();

		auto& submenus = UIManager::GetSubmenus();
		auto activeSubmenu = UIManager::GetActiveSubmenu();

		const float windowWidth = *Pointers.ScreenResX / 2.5f;
		const ImVec2 windowSize(windowWidth, *Pointers.ScreenResY / 2.2f);

		ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSizeConstraints(ImVec2(560.0f, 320.0f), ImVec2(100000.0f, 100000.0f));
		ImGui::SetNextWindowPos(ImVec2((*Pointers.ScreenResX - windowWidth) * 0.5f, *Pointers.ScreenResY / 6.0f), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("YimMenuV2 · 现代（纵向）##ModernVerticalMenu", nullptr, ImGuiWindowFlags_NoCollapse))
		{
			// —— 左侧子菜单栏（纵向列表，与本窗口一体）——
			const float sideBarWidth = 140.0f;
			if (ImGui::BeginChild("##submenuSideBar", ImVec2(sideBarWidth, 0), true))
			{
				ImDrawList* draw = ImGui::GetWindowDrawList();
				const float rowHeight = 34.0f;

				for (size_t i = 0; i < submenus.size(); ++i)
				{
					auto& submenu = submenus[i];
					const auto label = Localization::Translate(submenu->m_Name);
					const ImVec2 rowMin = ImGui::GetCursorScreenPos();
					const ImVec2 rowSize(ImGui::GetContentRegionAvail().x, rowHeight);

					ImGui::PushID(static_cast<int>(i));
					ImGui::InvisibleButton("##submenu", rowSize);
					const bool hovered = ImGui::IsItemHovered();
					const bool clicked = ImGui::IsItemClicked();
					ImGui::PopID();

					if (clicked)
					{
						UIManager::SetActiveSubmenu(submenu);
						UIManager::SetShowContentWindow(true);
						activeSubmenu = submenu;
					}

					const bool isActive = (submenu == activeSubmenu);

					if (isActive)
						draw->AddRectFilled(rowMin, rowMin + rowSize, ImGui::GetColorU32(ImGuiCol_ButtonActive), 6.0f);
					else if (hovered)
						draw->AddRectFilled(rowMin, rowMin + rowSize, ImGui::GetColorU32(ImGuiCol_ButtonHovered), 6.0f);

					if (isActive)
						draw->AddRectFilled(ImVec2(rowMin.x + 2.0f, rowMin.y + 6.0f), ImVec2(rowMin.x + 5.0f, rowMin.y + rowSize.y - 6.0f), IM_COL32(96, 178, 255, 255), 2.0f);

					// 图标 + 名称
					float textX = rowMin.x + 12.0f;
					if (ImFont* iconFont = Menu::Font::g_AwesomeFont)
					{
						const float iconSize = 18.0f;
						const ImVec2 textSize = iconFont->CalcTextSizeA(iconSize, 100000.0f, 0.0f, submenu->m_Icon.c_str());
						draw->AddText(iconFont, iconSize, ImVec2(textX, rowMin.y + (rowHeight - textSize.y) * 0.5f), ImGui::GetColorU32(ImGuiCol_Text), submenu->m_Icon.c_str());
						textX += textSize.x + 8.0f;
					}

					const ImVec2 labelSize = ImGui::CalcTextSize(label.c_str());
					draw->AddText(ImVec2(textX, rowMin.y + (rowHeight - labelSize.y) * 0.5f), ImGui::GetColorU32(isActive ? ImGuiCol_Text : ImGuiCol_TextDisabled), label.c_str());
				}
			}
			ImGui::EndChild();
			ImGui::SameLine();

			// —— 右侧：分类栏 + 内容 ——
			ImGui::BeginGroup();
			if (ImGui::BeginChild("##categorySelectors", ImVec2(0, 56), true))
			{
				if (activeSubmenu)
					activeSubmenu->DrawCategorySelectors();
			}
			ImGui::EndChild();

			if (ImGui::BeginChild("##options", ImVec2(0, 0), true))
			{
				ImFont* optionsFont = UIManager::GetOptionsFont();
				if (optionsFont)
					ImGui::PushFont(optionsFont);

				if (activeSubmenu)
					activeSubmenu->Draw();

				if (optionsFont)
					ImGui::PopFont();
			}
			ImGui::EndChild();
			ImGui::EndGroup();
		}
		ImGui::End();
	}
}
