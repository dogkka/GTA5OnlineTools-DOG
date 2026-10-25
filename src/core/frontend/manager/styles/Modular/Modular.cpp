#include "game/pointers/Pointers.hpp"
#include "game/frontend/Menu.hpp"
#include "core/frontend/manager/UIManager.hpp"
#include "core/localization/Localization.hpp"
#include "game/frontend/submenus/Settings/GUISettings.hpp"

#include <algorithm>

namespace YimMenu
{
	// 模块化：单窗口布局 —— 顶部子菜单模块卡片（3 列网格）+ 分类栏 + 内容，整窗一起拖动
	void RenderModularTheme()
	{
		YimMenu::SyncColorCommandsToStyle();

		auto& submenus = UIManager::GetSubmenus();
		auto activeSubmenu = UIManager::GetActiveSubmenu();

		const float windowWidth = *Pointers.ScreenResX / 2.5f;
		const ImVec2 windowSize(windowWidth, *Pointers.ScreenResY / 2.2f);

		ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSizeConstraints(ImVec2(560.0f, 320.0f), ImVec2(100000.0f, 100000.0f));
		ImGui::SetNextWindowPos(ImVec2((*Pointers.ScreenResX - windowWidth) * 0.5f, *Pointers.ScreenResY / 6.0f), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("YimMenuV2 · 模块化##ModularMenu", nullptr, ImGuiWindowFlags_NoCollapse))
		{
			// —— 子菜单模块卡片（3 列网格，与本窗口一体）——
			{
				ImDrawList* draw = ImGui::GetWindowDrawList();
				const int columns = 3;
				const float gap = 8.0f;
				const float cardHeight = 42.0f;
				const float availWidth = ImGui::GetContentRegionAvail().x;
				const float cardWidth = (availWidth - gap * static_cast<float>(columns - 1)) / static_cast<float>(columns);
				const ImVec2 gridMin = ImGui::GetCursorScreenPos();
				const int rows = static_cast<int>((submenus.size() + columns - 1) / columns);

				for (size_t i = 0; i < submenus.size(); ++i)
				{
					auto& submenu = submenus[i];
					const auto label = Localization::Translate(submenu->m_Name);
					const int col = static_cast<int>(i % columns);
					const int row = static_cast<int>(i / columns);
					const ImVec2 cardMin(gridMin.x + static_cast<float>(col) * (cardWidth + gap), gridMin.y + static_cast<float>(row) * (cardHeight + gap));
					const ImVec2 cardMax(cardMin.x + cardWidth, cardMin.y + cardHeight);

					ImGui::PushID(static_cast<int>(i));
					ImGui::SetCursorScreenPos(cardMin);
					ImGui::InvisibleButton("##module", ImVec2(cardWidth, cardHeight));
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

					const ImU32 fill = ImGui::GetColorU32(isActive ? ImGuiCol_ButtonActive : (hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg));
					draw->AddRectFilled(cardMin, cardMax, fill, 6.0f);
					draw->AddRect(cardMin, cardMax, ImGui::GetColorU32(isActive ? ImGuiCol_ButtonActive : ImGuiCol_Border), 6.0f, 0, isActive ? 2.0f : 1.0f);

					if (isActive)
						draw->AddRectFilled(cardMin, ImVec2(cardMin.x + 4.0f, cardMax.y), IM_COL32(96, 178, 255, 255), 6.0f, ImDrawFlags_RoundCornersLeft);

					// 图标 + 名称
					float textX = cardMin.x + 14.0f;
					if (ImFont* iconFont = Menu::Font::g_AwesomeFont)
					{
						const float iconSize = 18.0f;
						const ImVec2 textSize = iconFont->CalcTextSizeA(iconSize, 100000.0f, 0.0f, submenu->m_Icon.c_str());
						draw->AddText(iconFont, iconSize, ImVec2(textX, cardMin.y + (cardHeight - textSize.y) * 0.5f), ImGui::GetColorU32(ImGuiCol_Text), submenu->m_Icon.c_str());
						textX += textSize.x + 8.0f;
					}

					const ImVec2 labelSize = ImGui::CalcTextSize(label.c_str());
					draw->AddText(ImVec2(textX, cardMin.y + (cardHeight - labelSize.y) * 0.5f), ImGui::GetColorU32(isActive ? ImGuiCol_Text : ImGuiCol_TextDisabled), label.c_str());
				}

				ImGui::SetCursorScreenPos(ImVec2(gridMin.x, gridMin.y + static_cast<float>(rows) * (cardHeight + gap) + 2.0f));
				ImGui::Separator();
			}

			// —— 分类选择栏 ——
			if (ImGui::BeginChild("##categorySelectors", ImVec2(0, 56), true))
			{
				if (activeSubmenu)
					activeSubmenu->DrawCategorySelectors();
			}
			ImGui::EndChild();

			// —— 内容区 ——
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
		}
		ImGui::End();
	}
}
