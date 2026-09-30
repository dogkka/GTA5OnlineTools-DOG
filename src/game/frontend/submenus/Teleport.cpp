#include "Teleport.hpp"

#include "core/frontend/Notifications.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/localization/Localization.hpp"
#include "game/backend/SavedLocations.hpp"
#include "game/backend/Self.hpp"
#include "game/frontend/items/Items.hpp"

namespace YimMenu::Submenus
{
	namespace
	{
		std::string NormalizeSavedLocationCategory(std::string_view category)
		{
			const auto localizedDefault = Localization::Translate("Default");
			if (category.empty() || category == localizedDefault)
				return "Default";

			return std::string(category);
		}
	}

	static float GetDistanceFromLocation(const SavedLocation& t)
	{
		return rage::fvector3(t.x, t.y, t.z).GetDistance(Self::GetPed().GetPosition());
	}

	void RenderCustomTeleport()
	{
		ImGui::BeginGroup();
		static std::string newLocationName{};
		static std::string category = Localization::Translate("Default");
		static SavedLocation locationToDelete;

		if (!std::string(locationToDelete.name).empty())
			ImGui::OpenPopup("##deletelocation");

		if (ImGui::BeginPopupModal("##deletelocation", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("确定要删除 %s 吗？", locationToDelete.name.data());

			ImGui::Spacing();

			if (ImGui::Button("是"))
			{
				SavedLocations::DeleteSavedLocation(NormalizeSavedLocationCategory(category), locationToDelete.name);
				locationToDelete.name = "";
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("否"))
			{
				locationToDelete.name = "";
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PushItemWidth(300);
		InputTextWithHint("分类", "分类", &category).Draw();

		ImGui::PushItemWidth(200);
		InputTextWithHint("地点名称", "新地点", &newLocationName).Draw();
		ImGui::PopItemWidth();

		if (ImGui::Button("保存当前位置")) // Button widget still crashes
		{
			FiberPool::Push([=] {
				if (newLocationName.empty())
				{
					Notifications::Show("自定义传送", "请输入有效名称", NotificationType::Warning);
				}
				else if (SavedLocations::GetSavedLocationByName(newLocationName))
				{
					Notifications::Show("自定义传送", std::format("名为 {} 的地点已存在", newLocationName));
				}
				else
				{
					SavedLocation teleportLocation;
					Entity teleportEntity = Self::GetPed();
					if (auto vehicle = Self::GetVehicle())
						teleportEntity = vehicle;

					auto coords = teleportEntity.GetPosition();
					teleportLocation.name = newLocationName;
					teleportLocation.x = coords.x;
					teleportLocation.y = coords.y;
					teleportLocation.z = coords.z;
					teleportLocation.yaw = teleportEntity.GetHeading();
					teleportLocation.pitch = 0.0f; // why do we need pitch and roll anyway?
					teleportLocation.roll = 0.0f;
					SavedLocations::SaveNewLocation(NormalizeSavedLocationCategory(category), teleportLocation);
				}
			});
		};


		ImGui::Separator();

		ImGui::Text("%s", "双击可传送\n按住 Shift 点击可删除");

		ImGui::Spacing();

		static std::string filter{};
		InputTextWithHint("##filter", "搜索", &filter).Draw();

		ImGui::BeginGroup();
		ImGui::Text("%s", "分类");
		if (ImGui::BeginListBox("##categories", {200, -1}))
		{
			for (auto& l : SavedLocations::GetAllSavedLocations() | std::ranges::views::keys)
			{
				const auto translatedCategory = Localization::Translate(l);
				if (ImGui::Selectable(translatedCategory.c_str(), l == NormalizeSavedLocationCategory(category)))
				{
					category = translatedCategory;
				}

				if (category.empty())
				{
					category = translatedCategory;
				}
			}
			ImGui::EndListBox();
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::Text("%s", "地点");
		if (ImGui::BeginListBox("##saved_locs", {200, -1})) // Need automatic dimensions instead of hard coded
		{
			const auto categoryKey = NormalizeSavedLocationCategory(category);
			if (SavedLocations::GetAllSavedLocations().find(categoryKey) != SavedLocations::GetAllSavedLocations().end())
			{
				std::vector<SavedLocation> current_list{};

				if (!filter.empty())
					current_list = SavedLocations::SavedLocationsFilteredList(filter);
				else
					current_list = SavedLocations::GetAllSavedLocations().at(categoryKey);

				for (const auto& l : current_list)
				{
					if (ImGui::Selectable(l.name.data(), false, ImGuiSelectableFlags_AllowDoubleClick))
					{
						if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
						{
							locationToDelete = l;
						}
						else
						{
							if (ImGui::IsMouseDoubleClicked(0))
							{
								FiberPool::Push([l] {
									rage::fvector3 l_ = {l.x, l.y, l.z};
									Self::GetPed().TeleportTo(l_);
								});
							}
						}
					}

					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						if (l.name.length() > 27)
							ImGui::Text("%s", l.name.data());
						ImGui::Text("距离：%f", GetDistanceFromLocation(l));
						ImGui::EndTooltip();
					}
				}
			}

			ImGui::EndListBox();
		}

		ImGui::EndGroup();

		ImGui::EndGroup();
	}

	void RenderDirectionalTp()
	{
		FloatCommandItem("directionaltpdistance"_J).Draw();

		ImGui::BeginGroup();
		CommandItem("directionaltpforward"_J).Draw();
		CommandItem("directionaltpbackward"_J).Draw();
		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginGroup();
		CommandItem("directionaltpright"_J).Draw();
		CommandItem("directionaltpleft"_J).Draw();
		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginGroup();
		CommandItem("directionaltpup"_J).Draw();
		CommandItem("directionaltpdown"_J).Draw();
		ImGui::EndGroup();
	}

	Teleport::Teleport() :
		#define ICON_FA_TELEPORT "\xef\x8f\x85"
	    Submenu::Submenu("传送", ICON_FA_TELEPORT)
	{
		auto main = std::make_shared<Category>("基础");
		auto miscGroup = std::make_shared<Group>("杂项");

		miscGroup->AddItem(std::make_shared<ConditionalItem>("autotptowaypoint"_J, std::make_shared<CommandItem>("tptowaypoint"_J), true));
		miscGroup->AddItem(std::make_shared<BoolCommandItem>("autotptowaypoint"_J));
		miscGroup->AddItem(std::make_shared<CommandItem>("tptoobjective"_J));
		miscGroup->AddItem(std::make_shared<ImGuiItem>([] {
			RenderDirectionalTp();
		}));

		main->AddItem(miscGroup);

		auto customteleport = std::make_shared<Category>("已保存");
		customteleport->AddItem(std::make_shared<ImGuiItem>([] {
			RenderCustomTeleport();
		}));


		AddCategory(std::move(main));
		AddCategory(std::move(customteleport));
	}
}
