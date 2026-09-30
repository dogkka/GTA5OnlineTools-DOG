#include "Vehicle.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/PersonalVehicles.hpp"
#include "game/features/vehicle/DeletePersonalVehicle.hpp"
#include "game/frontend/items/Items.hpp"
#include "game/frontend/submenus/Vehicle/SpawnVehicle.hpp"
#include "game/pointers/Pointers.hpp"
#include "Vehicle/VehicleEditor.hpp"
#include "Vehicle/SavedVehicles.hpp"

namespace YimMenu::Submenus
{
	Vehicle::Vehicle() :
#define ICON_FA_CAR "\xef\x86\xb9"
	    Submenu::Submenu("载具", ICON_FA_CAR)
	{
		auto main = std::make_shared<Category>("基础");

		auto globals = std::make_shared<Group>("全局");
		auto tools = std::make_shared<Group>("工具", 2);
		auto seatsAndDoors = std::make_shared<Group>("座位与车门", 2);
		auto hydraulics = std::make_shared<Group>("液压悬挂", 2);
		auto misc = std::make_shared<Group>("杂项");

		globals->AddItem(std::make_shared<BoolCommandItem>("vehiclegodmode"_J));
		globals->AddItem(std::make_shared<BoolCommandItem>("keepfixed"_J));
		globals->AddItem(std::make_shared<BoolCommandItem>("hornboost"_J));
		globals->AddItem(std::make_shared<BoolCommandItem>("modifyboostbehavior"_J));
		globals->AddItem(std::make_shared<ConditionalItem>("modifyboostbehavior"_J, std::make_shared<ListCommandItem>("boostbehavior"_J)));

		tools->AddItem(std::make_shared<CommandItem>("enterlastvehicle"_J));
		tools->AddItem(std::make_shared<CommandItem>("repairvehicle"_J));
		tools->AddItem(std::make_shared<CommandItem>("fixallvehicles"_J));
		tools->AddItem(std::make_shared<CommandItem>("callmechanic"_J));
		tools->AddItem(std::make_shared<CommandItem>("resetvehicledeliverycooldown"_J));
		tools->AddItem(std::make_shared<CommandItem>("requestpv"_J));
		tools->AddItem(std::make_shared<CommandItem>("despawnpv"_J));
		tools->AddItem(std::make_shared<CommandItem>("savepersonalvehicle"_J));
		tools->AddItem(std::make_shared<ImGuiItem>([] {
			static bool openConfirmation = false;
			static int vehicleId = -1;
			static std::string vehicleName;

			if (ImGui::Button("删除当前个人载具"))
			{
				if (!Pointers.IsSessionStarted || !*Pointers.IsSessionStarted)
				{
					Notifications::Show("删除个人载具", "请先进入 GTA 在线模式。", NotificationType::Error);
				}
				else if (!PersonalVehicles::GetCurrentHandle().IsValid())
				{
					Notifications::Show("删除个人载具", "请先呼出要删除的个人载具。", NotificationType::Error);
				}
				else if (auto vehicle = PersonalVehicles::GetCurrent())
				{
					vehicleId = vehicle->GetId();
					vehicleName = vehicle->GetName();
					if (const auto idSeparator = vehicleName.find("##"); idSeparator != std::string::npos)
						vehicleName.erase(idSeparator);
					openConfirmation = true;
				}
			}

			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("永久删除当前已呼出的个人载具，无法撤销。");

			if (openConfirmation)
				ImGui::OpenPopup("##delete_personal_vehicle");

			ImGui::SetNextWindowSize(ImVec2(460, 0), ImGuiCond_Appearing);
			if (ImGui::BeginPopupModal("##delete_personal_vehicle", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::TextUnformatted("确认永久删除以下个人载具？");
				ImGui::Spacing();
				ImGui::TextWrapped("%s", vehicleName.c_str());
				ImGui::Spacing();
				ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.30f, 1.0f), "此操作无法撤销。确认后请勿切换当前个人载具。");
				ImGui::Separator();

				if (ImGui::Button("永久删除"))
				{
					const int confirmedVehicleId = vehicleId;
					FiberPool::Push([confirmedVehicleId] {
						Features::DeletePersonalVehicle(confirmedVehicleId);
					});
					openConfirmation = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("取消"))
				{
					openConfirmation = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}));

		seatsAndDoors->AddItem(std::make_shared<ListCommandItem>("vehicleseat"_J));
		seatsAndDoors->AddItem(std::make_shared<CommandItem>("entervehicleseat"_J));
		seatsAndDoors->AddItem(std::make_shared<CommandItem>("openvehicledoors"_J));
		seatsAndDoors->AddItem(std::make_shared<CommandItem>("closevehicledoors"_J));

		hydraulics->AddItem(std::make_shared<ListCommandItem>("hydraulicwheel"_J));
		hydraulics->AddItem(std::make_shared<FloatCommandItem>("hydraulicfactor"_J));
		hydraulics->AddItem(std::make_shared<CommandItem>("raisehydraulicwheel"_J));
		hydraulics->AddItem(std::make_shared<CommandItem>("lowerhydraulicwheel"_J));

		misc->AddItem(std::make_shared<BoolCommandItem>("speedometer"_J));
		misc->AddItem(std::make_shared<BoolCommandItem>("seatbelt"_J));
		misc->AddItem(std::make_shared<BoolCommandItem>("lowervehiclestance"_J));
		misc->AddItem(std::make_shared<BoolCommandItem>("allowhatsinvehicles"_J));
		misc->AddItem(std::make_shared<BoolCommandItem>("lsccustomsbypass"_J));
		misc->AddItem(std::make_shared<BoolCommandItem>("dlcvehicles"_J));

		main->AddItem(globals);
		main->AddItem(tools);
		main->AddItem(seatsAndDoors);
		main->AddItem(hydraulics);
		main->AddItem(misc);

		AddCategory(std::move(main));
		AddCategory(BuildSpawnVehicleMenu());
		AddCategory(BuildVehicleEditorMenu());
		AddCategory(BuildSavedVehiclesMenu());
	}
}
