#include "Items.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/Vector3Command.hpp"
#include "core/localization/Localization.hpp"
#include "game/backend/SavedLocations.hpp"
#include "game/backend/Self.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu
{
	Vector3CommandItem::Vector3CommandItem(joaat_t id, std::optional<std::string> label_override) :
	    m_Command(Commands::GetCommand<Vector3Command>(id)),
	    m_LabelOverride(label_override)
	{
	}

	void Vector3CommandItem::Draw()
	{
		if (!m_Command)
		{
			ImGui::Text("%s", "未知！");
			return;
		}

		auto value = m_Command->GetState();
		ImGui::PushID(m_Command);
		ImGui::SetNextItemWidth(180);
		if (ImGui::InputFloat3("##coord_inp", &value.x, "%.1f"))
			m_Command->SetState(value);

		if (Self::GetPed())
		{
			ImGui::SameLine();
			if (ImGui::Button("当前位置"))
				m_Command->SetState(Self::GetPed().GetPosition());
		}
		ImGui::SameLine();
		if (ImGui::Button("已保存位置..."))
			ImGui::OpenPopup("##saved");

		if (ImGui::BeginPopup("##saved", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			ImGui::Text("%s", "点击一个地点即可选择。可在 传送 > 已保存 中添加更多位置。");
			InputTextWithHint("##filter", "搜索", &m_CurrentFilter).Draw();

			const float max_length = *Pointers.ScreenResY / 3.2;

			// TODO: duplicated code
			ImGui::BeginGroup();
			ImGui::Text("%s", "分类");

			if (ImGui::BeginListBox("##categories", {200, max_length}))
			{
				for (auto& l : SavedLocations::GetAllSavedLocations() | std::ranges::views::keys)
				{
					const auto translatedCategory = Localization::Translate(l);
					if (ImGui::Selectable(translatedCategory.c_str(), l == m_CurrentCategory))
					{
						m_CurrentCategory = l;
					}

					if (m_CurrentCategory.empty())
					{
						m_CurrentCategory = l;
					}
				}
				ImGui::EndListBox();
			}
			ImGui::EndGroup();
			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::Text("%s", "地点");
			if (ImGui::BeginListBox("##saved_locs", {200, max_length}))
			{
				if (SavedLocations::GetAllSavedLocations().find(m_CurrentCategory) != SavedLocations::GetAllSavedLocations().end())
				{
					std::vector<SavedLocation> current_list{};

					if (!m_CurrentFilter.empty())
						current_list = SavedLocations::SavedLocationsFilteredList(m_CurrentFilter);
					else
						current_list = SavedLocations::GetAllSavedLocations().at(m_CurrentCategory);

					for (const auto& l : current_list)
					{
						if (ImGui::Selectable(l.name.data(), false, ImGuiSelectableFlags_AllowDoubleClick))
						{
							m_Command->SetState({l.x, l.y, l.z});
							ImGui::CloseCurrentPopup();
						}

						if (ImGui::IsItemHovered() && l.name.length() > 27)
						{
							ImGui::BeginTooltip();
							ImGui::Text("%s", l.name.data());
							ImGui::EndTooltip();
						}
					}
				}

				ImGui::EndListBox();
			}

			ImGui::EndGroup();

			if (ImGui::Button("关闭"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		const auto label = Localization::Translate(m_LabelOverride.has_value() ? m_LabelOverride.value() : m_Command->GetLabel());
		if (!label.empty())
		{
			ImGui::SameLine();
			ImGui::Text("%s", label.c_str());
		}

		ImGui::PopID();
	}
}
