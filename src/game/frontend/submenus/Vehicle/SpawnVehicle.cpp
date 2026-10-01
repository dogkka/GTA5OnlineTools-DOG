#include "SpawnVehicle.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/localization/Localization.hpp"
#include "game/backend/Self.hpp"
#include "game/backend/PersonalVehicles.hpp"
#include "game/gta/data/Vehicles.hpp"
#include "game/gta/Natives.hpp"

#include <atomic>
#include <cmath>

namespace YimMenu::Submenus
{
	static BoolCommand spawnInsideVehicle{"spawninsideveh", "车内生成", "在载具内部生成。"};
	static BoolCommand spawnVehicleMaxed{"spawnvehmaxed", "满改生成", "生成满改载具。"};
	static BoolCommand spawnInsidePersonalVehicle{"spawninsidepv", "车内生成", "在个人载具内部生成。"};
	static BoolCommand spawnClonePersonalVehicle{"spawnclonepv", "生成复制品", "生成个人载具的复制品。"};
	static BoolCommand spawnPreviewMode{"spawnpreviewmode", "预览模式", "点击列表中的车辆会在你正前方 6 米处展示模型，再次点击自动替换上一辆。", true};
	static std::atomic<int> g_PreviewGeneration{0};
	static int g_LastPreviewVehicle = 0;

	std::shared_ptr<TabItem> RenderSpawnNewVehicle()
	{
		auto tab = std::make_shared<TabItem>("新载具");

		auto spawn = std::make_shared<Group>("生成");
		auto settings = std::make_shared<Group>("设置");

		static std::vector<std::string> vehicleNames{};
		static std::vector<int> vehicleClasses{};
		static int selectedClass{-1};

		spawn->AddItem(std::make_unique<ImGuiItem>([] {
			static bool init = [] {
				FiberPool::Push([] {
					std::unordered_map<std::string, int> nameCount;

					for (auto& veh : g_VehicleHashes)
					{
						std::string finalName = Vehicle::GetLocalizedDisplayName(veh);
						int& count = nameCount[finalName];
						if (count > 0)
						{
							finalName += " " + std::to_string(count + 1);
						}
						++count;

						vehicleNames.push_back(finalName);

						int id = VEHICLE::GET_VEHICLE_CLASS_FROM_NAME(veh);
						vehicleClasses.push_back(id);
					}
				});

				return true;
			}();

			static char search[64];
			ImGui::SetNextItemWidth(300.f);
			ImGui::InputTextWithHint("名称", "搜索", search, sizeof(search));

			ImGui::SetNextItemWidth(300.f);
			constexpr auto allText = "全部";
			if (ImGui::BeginCombo("类别", selectedClass == -1 ? allText : g_VehicleClassNames[selectedClass]))
			{
				if (ImGui::Selectable(allText, selectedClass == -1))
				{
					selectedClass = -1;
				}

				for (int i = 0; i < g_VehicleClassNames.size(); i++)
				{
					if (ImGui::Selectable(g_VehicleClassNames[i], selectedClass == i))
					{
						selectedClass = i;
					}
				}

				ImGui::EndCombo();
			}

			const int visible = std::min(20, static_cast<int>(vehicleNames.size()));
			const float height = visible * ImGui::GetTextLineHeightWithSpacing();
			if (ImGui::BeginListBox("##vehicles", {300.f, height}))
			{
				if (vehicleNames.empty())
				{
					ImGui::Text("%s", "原生函数缓存尚未完成。");
				}
				else
				{
					std::string lower = search;
					std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
					for (int veh = 0; veh < vehicleNames.size(); veh++)
					{
						auto hash = g_VehicleHashes[veh];
						auto name = vehicleNames[veh];
						auto lowerName = name;
						std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

						bool matchesSearch = lowerName.find(lower) != std::string::npos;
						bool matchesClass = selectedClass == -1 || vehicleClasses[veh] == selectedClass;
						if (matchesSearch && matchesClass)
						{
							ImGui::PushID(hash);
							if (ImGui::Selectable(name.c_str()))
							{
								const int generation = ++g_PreviewGeneration;
								const bool preview   = spawnPreviewMode.GetState();
								FiberPool::Push([hash, name, generation, preview] {
									// 连点去抖：有更新的点击排队时，放弃本次
									if (generation != g_PreviewGeneration.load())
										return;

									auto ped = Self::GetPed();
									if (!ped)
										return;

									// 移除上一辆预览车（只有最新一次点击动旧车）
									if (preview && g_LastPreviewVehicle != 0)
									{
										if (ENTITY::DOES_ENTITY_EXIST(g_LastPreviewVehicle))
											Vehicle(g_LastPreviewVehicle).Delete();
										g_LastPreviewVehicle = 0;
									}

									// 在玩家正前方 6 米生成（与"预设车辆"同款定位，避免贴脸生成失败/看不见）
									const auto pos     = ped.GetPosition();
									const auto heading = ped.GetHeading();
									const float rad    = heading * 3.14159265f / 180.0f;
									const float spawn_x = pos.x - sinf(rad) * 6.0f;
									const float spawn_y = pos.y + cosf(rad) * 6.0f;

									auto handle = Vehicle::Create(hash, rage::fvector3{spawn_x, spawn_y, pos.z + 0.5f}, heading);
									if (!handle)
									{
										if (generation == g_PreviewGeneration.load() && preview)
											Notifications::Show("载具预览", "生成失败：模型无法加载。", NotificationType::Error);
										return;
									}

									// 生成期间又被更新的点击取代：把刚生成的也清掉
									if (generation != g_PreviewGeneration.load())
									{
										handle.Delete();
										return;
									}

									if (spawnInsideVehicle.GetState())
										Self::GetPed().SetInVehicle(handle);

									if (spawnVehicleMaxed.GetState())
										handle.Upgrade();

									if (preview)
									{
										g_LastPreviewVehicle = handle.GetHandle();
										Notifications::Show("载具预览", "已展示：" + name, NotificationType::Info);
									}
								});
							}
							ImGui::PopID();
						}
					}
				}

				ImGui::EndListBox();
			}
		}));

		settings->AddItem(std::make_shared<BoolCommandItem>("spawninsideveh"_J));
		settings->AddItem(std::make_shared<BoolCommandItem>("spawnvehmaxed"_J));
		settings->AddItem(std::make_shared<BoolCommandItem>("spawnpreviewmode"_J));

		tab->AddItem(spawn);
		tab->AddItem(settings);
		return tab;
	}

	std::shared_ptr<TabItem> RenderSpawnPersonalVehicle()
	{
		auto tab = std::make_shared<TabItem>("个人载具");

		auto spawn = std::make_shared<Group>("生成");
		auto settings = std::make_shared<Group>("设置");

		static std::string selectedGarageStr{""};

		spawn->AddItem(std::make_unique<ImGuiItem>([] {
			if (!*Pointers.IsSessionStarted)
				return ImGui::TextDisabled("%s", "请先进入 GTA 在线模式。");

			PersonalVehicles::Update();

			static char search[64];
			ImGui::SetNextItemWidth(300.f);
			ImGui::InputTextWithHint("名称", "搜索", search, sizeof(search));

			ImGui::SetNextItemWidth(300.f);
			constexpr auto allText = "全部";
			if (ImGui::BeginCombo("车库", selectedGarageStr.empty() ? allText : selectedGarageStr.c_str()))
			{
				if (ImGui::Selectable(allText, selectedGarageStr.empty()))
				{
					selectedGarageStr.clear();
				}
				for (auto garage : PersonalVehicles::GetGarages())
				{
					if (ImGui::Selectable(garage.c_str(), garage == selectedGarageStr))
					{
						selectedGarageStr = garage;
					}
				}

				ImGui::EndCombo();
			}

			const int visible = std::min(20, static_cast<int>(PersonalVehicles::GetPersonalVehicles().size()));
			const float height = visible * ImGui::GetTextLineHeightWithSpacing();
			if (ImGui::BeginListBox("##personalvehicles", {300.f, height}))
			{
				if (PersonalVehicles::GetPersonalVehicles().empty())
				{
					ImGui::Text("%s", "统计数据尚未加载。");
				}
				else
				{
					std::string lowerSearch = search;
					std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), tolower);
					for (const auto& it : PersonalVehicles::GetPersonalVehicles())
					{
						const auto& label = it.first;
						const auto& personalVeh = it.second;

						auto lowerName = label;
						std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

						bool matchesSearch = lowerName.find(lowerSearch) != std::string::npos;
						bool matchesGarage = selectedGarageStr.empty() || personalVeh->GetGarage() == selectedGarageStr;
						if (matchesSearch && matchesGarage)
						{
							ImGui::PushID(personalVeh->GetId());
							if (ImGui::Selectable(label.c_str()))
							{
								FiberPool::Push([&personalVeh] {
									if (spawnClonePersonalVehicle.GetState())
									{
										auto coords  = Vehicle::GetSpawnLocRelToPed(Self::GetPed().GetHandle(), personalVeh->GetModel());
										auto heading = Self::GetPed().GetHeading();
										auto handle  = personalVeh->Clone(coords, heading);
										
										if (spawnInsidePersonalVehicle.GetState())
											Self::GetPed().SetInVehicle(handle);
									}
									else
									{
										if (!personalVeh->Request(spawnInsidePersonalVehicle.GetState()))
											Notifications::Show("生成个人载具", "生成个人载具失败。", NotificationType::Error);
									}
								});
							}
							ImGui::PopID();
						}
					}
				}

				ImGui::EndListBox();
			}
		}));

		settings->AddItem(std::make_shared<BoolCommandItem>("spawninsidepv"_J));
		settings->AddItem(std::make_shared<BoolCommandItem>("spawnclonepv"_J));


		tab->AddItem(spawn);
		tab->AddItem(settings);
		return tab;
	}

	std::shared_ptr<Category> BuildSpawnVehicleMenu()
	{
		auto menu = std::make_shared<Category>("生成");

		auto tabBar = std::make_shared<TabBarItem>("生成");

		tabBar->AddItem(RenderSpawnNewVehicle());
		tabBar->AddItem(RenderSpawnPersonalVehicle());

		menu->AddItem(std::move(tabBar));

		return menu;
	}
}
