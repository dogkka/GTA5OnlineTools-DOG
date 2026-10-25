#include "game/pointers/Pointers.hpp"
#include "game/frontend/Menu.hpp"
#include "core/frontend/manager/UIManager.hpp"
#include "core/localization/Localization.hpp"
#include "game/frontend/submenus/Settings/GUISettings.hpp"

#include <algorithm>

namespace YimMenu
{
	// 现代：单窗口布局 —— 顶部子菜单图标栏 + 分类栏 + 内容，整窗一起拖动
	void RenderModernTheme()
	{
		YimMenu::SyncColorCommandsToStyle();

		auto& submenus = UIManager::GetSubmenus();
		auto activeSubmenu = UIManager::GetActiveSubmenu();

		const float windowWidth = *Pointers.ScreenResX / 2.5f;
		const ImVec2 windowSize(windowWidth, *Pointers.ScreenResY / 2.2f);

		ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSizeConstraints(ImVec2(560.0f, 320.0f), ImVec2(100000.0f, 100000.0f));
		ImGui::SetNextWindowPos(ImVec2((*Pointers.ScreenResX - windowWidth) * 0.5f, *Pointers.ScreenResY / 6.0f), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("YimMenuV2 · 现代##ModernMenu", nullptr, ImGuiWindowFlags_NoCollapse))
		{
			// —— 子菜单图标栏（与本窗口一体，标题栏可直接拖动整窗）——
			{
				ImDrawList* draw = ImGui::GetWindowDrawList();
				const float bubbleSize = 38.0f;
				const float itemGap = 10.0f;
				const float barHeight = bubbleSize + ImGui::GetTextLineHeight() + 12.0f;

				std::vector<float> widths(submenus.size());
				float totalWidth = 0.0f;
				for (size_t i = 0; i < submenus.size(); ++i)
				{
					const auto label = Localization::Translate(submenus[i]->m_Name);
					widths[i] = std::max(bubbleSize, ImGui::CalcTextSize(label.c_str()).x + 18.0f);
					totalWidth += widths[i];
				}
				if (submenus.size() > 1)
					totalWidth += itemGap * static_cast<float>(submenus.size() - 1);

				const ImVec2 barMin = ImGui::GetCursorScreenPos();
				const float availWidth = ImGui::GetContentRegionAvail().x;
				float cursorX = std::max(0.0f, (availWidth - totalWidth) * 0.5f);

				for (size_t i = 0; i < submenus.size(); ++i)
				{
					auto& submenu = submenus[i];
					const auto label = Localization::Translate(submenu->m_Name);
					const float itemWidth = widths[i];
					const ImVec2 itemMin(barMin.x + cursorX, barMin.y);
					const ImVec2 bubbleMin(itemMin.x + (itemWidth - bubbleSize) * 0.5f, barMin.y);
					const ImVec2 bubbleMax(bubbleMin.x + bubbleSize, bubbleMin.y + bubbleSize);

					ImGui::PushID(static_cast<int>(i));
					ImGui::SetCursorScreenPos(itemMin);
					ImGui::InvisibleButton("##submenu", ImVec2(itemWidth, barHeight));
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
						draw->AddRectFilled(bubbleMin - ImVec2(2.0f, 2.0f), bubbleMax + ImVec2(2.0f, 2.0f), IM_COL32(96, 178, 255, 40), 10.0f);

					const ImU32 fill = ImGui::GetColorU32(isActive ? ImGuiCol_ButtonActive : (hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button));
					draw->AddRectFilled(bubbleMin, bubbleMax, fill, 8.0f);
					draw->AddRect(bubbleMin, bubbleMax, ImGui::GetColorU32(isActive ? ImGuiCol_ButtonActive : ImGuiCol_Border), 8.0f, 0, isActive ? 2.0f : 1.0f);

					// 图标（FontAwesome）
					if (ImFont* iconFont = Menu::Font::g_AwesomeFont)
					{
						const float iconSize = 20.0f;
						const ImVec2 textSize = iconFont->CalcTextSizeA(iconSize, 100000.0f, 0.0f, submenu->m_Icon.c_str());
						draw->AddText(iconFont, iconSize, ImVec2(bubbleMin.x + (bubbleSize - textSize.x) * 0.5f, bubbleMin.y + (bubbleSize - textSize.y) * 0.5f), ImGui::GetColorU32(ImGuiCol_Text), submenu->m_Icon.c_str());
					}

					// 名称
					const ImVec2 labelSize = ImGui::CalcTextSize(label.c_str());
					draw->AddText(ImVec2(itemMin.x + (itemWidth - labelSize.x) * 0.5f, bubbleMax.y + 4.0f), ImGui::GetColorU32(isActive ? ImGuiCol_Text : ImGuiCol_TextDisabled), label.c_str());

					cursorX += itemWidth + itemGap;
				}

				ImGui::SetCursorScreenPos(ImVec2(barMin.x, barMin.y + barHeight + 4.0f));
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
