#include "Vehicle.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/PersonalVehicles.hpp"
#include "game/features/vehicle/DeletePersonalVehicle.hpp"
#include "game/frontend/items/Items.hpp"
#include "game/frontend/submenus/Vehicle/SpawnVehicle.hpp"
#include "game/pointers/Pointers.hpp"
#include "Vehicle/VehicleEditor.hpp"
#include "Vehicle/SavedVehicles.hpp"

#include <algorithm>

namespace YimMenu::Submenus
{
	Vehicle::Vehicle() :
#define ICON_FA_CAR "\xef\x86\xb9"
	    Submenu::Submenu("载具", ICON_FA_CAR)
	{
		auto main = std::make_shared<Category>("基础");

		auto globals = std::make_shared<Group>("全局");
		auto tools = std::make_shared<Group>("工具", -1);
		auto seatsAndDoors = std::make_shared<Group>("座位与车门", 2);
		auto hydraulics = std::make_shared<Group>("液压悬挂", 2);
		auto driving = std::make_shared<Group>("行驶辅助", -1);
		auto appearance = std::make_shared<Group>("外观与灯光", -1);
		auto misc = std::make_shared<Group>("安全与杂项", -1);
		auto tuning = std::make_shared<Group>("载具调校", -1);
		auto speedo = std::make_shared<Group>("速度表", 1);

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

		driving->AddItem(std::make_shared<BoolCommandItem>("instantbrake"_J));
		driving->AddItem(std::make_shared<BoolCommandItem>("keepenginerunning"_J));
		driving->AddItem(std::make_shared<BoolCommandItem>("turnsignals"_J));
		driving->AddItem(std::make_shared<ConditionalItem>("turnsignals"_J, std::make_shared<ListCommandItem>("turnsignalsmode"_J)));
		driving->AddItem(std::make_shared<BoolCommandItem>("fly"_J));
		driving->AddItem(std::make_shared<ConditionalItem>("fly"_J, std::make_shared<FloatCommandItem>("flyspeed"_J, std::nullopt, true)));
		driving->AddItem(std::make_shared<BoolCommandItem>("driveonwater"_J));
		driving->AddItem(std::make_shared<BoolCommandItem>("vehjump"_J));
		driving->AddItem(std::make_shared<BoolCommandItem>("autodrive"_J));
		driving->AddItem(std::make_shared<ConditionalItem>("autodrive"_J, std::make_shared<ListCommandItem>("autodrivemode"_J)));
		driving->AddItem(std::make_shared<BoolCommandItem>("keeponground"_J));

		appearance->AddItem(std::make_shared<BoolCommandItem>("rainbowpaint"_J));
		appearance->AddItem(std::make_shared<ConditionalItem>("rainbowpaint"_J, std::make_shared<IntCommandItem>("rainbowspeed"_J, "速度")));
		appearance->AddItem(std::make_shared<StringCommandItem>("platetext"_J));
		appearance->AddItem(std::make_shared<CommandItem>("plateeditor"_J));
		appearance->AddItem(std::make_shared<BoolCommandItem>("disablesiren"_J));
		appearance->AddItem(std::make_shared<BoolCommandItem>("vehicleinvis"_J));
		appearance->AddItem(std::make_shared<BoolCommandItem>("vehiclestrong"_J));

		misc->AddItem(std::make_shared<BoolCommandItem>("seatbelt"_J));
		misc->AddItem(std::make_shared<BoolCommandItem>("lowervehiclestance"_J));

		speedo->AddItem(std::make_shared<BoolCommandItem>("speedometer"_J));
		speedo->AddItem(std::make_shared<ConditionalItem>("speedometer"_J, std::make_shared<ListCommandItem>("speedostyle"_J)));
		speedo->AddItem(std::make_shared<ConditionalItem>("speedometer"_J, std::make_shared<ListCommandItem>("speedounits"_J)));
		speedo->AddItem(std::make_shared<ConditionalItem>("speedometer"_J, std::make_shared<FloatCommandItem>("speedox"_J, "横向 X", true)));
		speedo->AddItem(std::make_shared<ConditionalItem>("speedometer"_J, std::make_shared<FloatCommandItem>("speedoy"_J, "纵向 Y", true)));
		speedo->AddItem(std::make_shared<ConditionalItem>("speedometer"_J, std::make_shared<ImGuiItem>([] {
			auto x_cmd = Commands::GetCommand<FloatCommand>("speedox"_J);
			auto y_cmd = Commands::GetCommand<FloatCommand>("speedoy"_J);
			if (!x_cmd || !y_cmd)
				return;

			const ImVec2 size(300.0f, 168.0f); // 屏幕预览卡片
			const ImVec2 origin = ImGui::GetCursorScreenPos();
			ImGui::InvisibleButton("##speedodrag", size);

			float x = x_cmd->GetState();
			float y = y_cmd->GetState();

			// 映射区留边距：保证圆点在四个角也能完整显示
			const float inset = 14.0f;
			const ImVec2 inner0(origin.x + inset, origin.y + inset);
			const ImVec2 inner1(origin.x + size.x - inset, origin.y + size.y - inset);
			const float inner_w = inner1.x - inner0.x;
			const float inner_h = inner1.y - inner0.y;

			if (ImGui::IsItemActive())
			{
				const ImVec2 mouse = ImGui::GetIO().MousePos;
				x = std::clamp((mouse.x - inner0.x) / inner_w, 0.0f, 1.0f);
				y = std::clamp((mouse.y - inner0.y) / inner_h, 0.0f, 1.0f);
				x_cmd->SetState(x);
				y_cmd->SetState(y);
			}

			auto draw = ImGui::GetWindowDrawList();

			// 卡片 + 阴影
			draw->AddRectFilled(ImVec2(origin.x + 2.0f, origin.y + 3.0f), ImVec2(origin.x + size.x + 2.0f, origin.y + size.y + 3.0f), IM_COL32(0, 0, 0, 90), 8.0f);
			draw->AddRectFilled(origin, ImVec2(origin.x + size.x, origin.y + size.y), IM_COL32(26, 30, 40, 255), 8.0f);
			draw->AddRect(origin, ImVec2(origin.x + size.x, origin.y + size.y), IM_COL32(96, 106, 128, 255), 8.0f);

			// 屏幕映射区
			draw->AddRectFilled(inner0, inner1, IM_COL32(13, 15, 20, 255), 5.0f);
			draw->AddRect(inner0, inner1, IM_COL32(66, 74, 92, 255), 5.0f);
			for (float f = 0.25f; f < 1.0f; f += 0.25f)
			{
				draw->AddLine(ImVec2(inner0.x + inner_w * f, inner0.y), ImVec2(inner0.x + inner_w * f, inner1.y), IM_COL32(46, 52, 66, 255));
				draw->AddLine(ImVec2(inner0.x, inner0.y + inner_h * f), ImVec2(inner1.x, inner0.y + inner_h * f), IM_COL32(46, 52, 66, 255));
			}

			// 十字参考线
			const ImVec2 marker(inner0.x + inner_w * x, inner0.y + inner_h * y);
			draw->AddLine(ImVec2(marker.x, inner0.y), ImVec2(marker.x, inner1.y), IM_COL32(110, 170, 255, 70));
			draw->AddLine(ImVec2(inner0.x, marker.y), ImVec2(inner1.x, marker.y), IM_COL32(110, 170, 255, 70));

			// 圆点（发光圈 + 实心点 + 白芯）
			draw->AddCircleFilled(marker, 13.0f, IM_COL32(80, 160, 255, 60), 32);
			draw->AddCircleFilled(marker, 7.0f, IM_COL32(96, 178, 255, 255), 32);
			draw->AddCircleFilled(marker, 3.0f, IM_COL32(232, 242, 255, 255), 16);
			draw->AddCircle(marker, 9.5f, IM_COL32(170, 210, 255, 210), 32, 2.0f);

			ImGui::Text("位置   X %.2f    Y %.2f", x, y);
			ImGui::SameLine();
			if (ImGui::SmallButton("重置位置"))
			{
				x_cmd->SetState(1.0f);
				y_cmd->SetState(0.85f);
			}
			ImGui::TextDisabled("%s", "按住圆点拖动 = 屏幕落点（四角可到边）；菜单开着时会显示参考框");
		})));
		misc->AddItem(std::make_shared<BoolCommandItem>("allowhatsinvehicles"_J));
		misc->AddItem(std::make_shared<BoolCommandItem>("lsccustomsbypass"_J));
		misc->AddItem(std::make_shared<BoolCommandItem>("dlcvehicles"_J));
		misc->AddItem(std::make_shared<BoolCommandItem>("vehnocol"_J));
		misc->AddItem(std::make_shared<BoolCommandItem>("bulletprooftyres"_J));

		tuning->AddItem(std::make_shared<BoolCommandItem>("handlingeditor"_J));
		tuning->AddItem(std::make_shared<ConditionalItem>("handlingeditor"_J, std::make_shared<FloatCommandItem>("handlingmass"_J, "重量", true)));
		tuning->AddItem(std::make_shared<ConditionalItem>("handlingeditor"_J, std::make_shared<FloatCommandItem>("handlingaccel"_J, "动力", true)));
		tuning->AddItem(std::make_shared<ConditionalItem>("handlingeditor"_J, std::make_shared<FloatCommandItem>("handlingtopspeed"_J, "极速", true)));
		tuning->AddItem(std::make_shared<ConditionalItem>("handlingeditor"_J, std::make_shared<FloatCommandItem>("handlingtopspeedval"_J, "极速直设", true)));
		tuning->AddItem(std::make_shared<ConditionalItem>("handlingeditor"_J, std::make_shared<FloatCommandItem>("handlingbrake"_J, "刹车", true)));
		tuning->AddItem(std::make_shared<ConditionalItem>("handlingeditor"_J, std::make_shared<FloatCommandItem>("handlingtraction"_J, "抓地", true)));
		tuning->AddItem(std::make_shared<ConditionalItem>("handlingeditor"_J, std::make_shared<BoolCommandItem>("handlingnodeform"_J)));
		tuning->AddItem(std::make_shared<ConditionalItem>("handlingeditor"_J, std::make_shared<BoolCommandItem>("handlingengineimmune"_J)));

		main->AddItem(globals);
		main->AddItem(tools);
		main->AddItem(seatsAndDoors);
		main->AddItem(hydraulics);
		main->AddItem(driving);
		main->AddItem(appearance);
		main->AddItem(misc);
		main->AddItem(speedo);
		main->AddItem(tuning);

		AddCategory(std::move(main));
		auto presets = std::make_shared<Category>("预设车辆");
		auto presetGroup = std::make_shared<Group>("全改装神车", 1);
		presetGroup->AddItem(std::make_shared<ListCommandItem>("presetvehicle"_J));
		presetGroup->AddItem(std::make_shared<CommandItem>("spawnpresetvehicle"_J));
		presets->AddItem(presetGroup);
		AddCategory(std::move(presets));
		AddCategory(BuildSpawnVehicleMenu());
		AddCategory(BuildVehicleEditorMenu());
		AddCategory(BuildSavedVehiclesMenu());
	}
}
