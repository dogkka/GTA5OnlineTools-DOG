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

	// ---------------- 载具预览（悬停预览 / 点击生成） ----------------
	static std::atomic<int> g_PreviewGeneration{0};
	static int g_LastPreviewVehicle = 0; // 当前预览车（仅在游戏线程访问）
	static int g_LastPreviewHash    = 0; // 当前预览车的模型

	static void RemovePreviewVehicle()
	{
		if (g_LastPreviewVehicle != 0)
		{
			if (ENTITY::DOES_ENTITY_EXIST(g_LastPreviewVehicle))
				Vehicle(g_LastPreviewVehicle).Delete();
			g_LastPreviewVehicle = 0;
			g_LastPreviewHash    = 0;
		}
	}

	// 预览/生成落点：玩家正前方 6 米；若落点已有车（例如刚"转正"保留的车），往前挪到 11 米避免叠车
	static rage::fvector3 GetPreviewSpawnCoords(Ped ped)
	{
		const auto pos     = ped.GetPosition();
		const auto heading = ped.GetHeading();
		const float rad    = heading * 3.14159265f / 180.0f;

		float dist = 6.0f;
		float x    = pos.x - sinf(rad) * dist;
		float y    = pos.y + cosf(rad) * dist;

		if (VEHICLE::GET_CLOSEST_VEHICLE(x, y, pos.z, 4.0f, 0, 70))
		{
			dist = 11.0f;
			x    = pos.x - sinf(rad) * dist;
			y    = pos.y + cosf(rad) * dist;
		}

		return rage::fvector3{x, y, pos.z + 0.5f};
	}

	class SpawnPreviewModeCommand : public BoolCommand
	{
		using BoolCommand::BoolCommand;

		virtual void OnDisable() override
		{
			++g_PreviewGeneration; // 取消未完成的预览任务
			RemovePreviewVehicle();
		}
	};

	static SpawnPreviewModeCommand spawnPreviewMode{"spawnpreviewmode", "预览模式", "悬停预览：鼠标停在车名上约 0.3 秒即在正前方展示模型（自动替换）；点击则直接生成真车。", true};

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
			int hovered_hash = 0;
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
							const bool clicked = ImGui::Selectable(name.c_str());
							if (ImGui::IsItemHovered())
								hovered_hash = hash;

							if (clicked)
							{
								const int generation = ++g_PreviewGeneration;
								const bool preview   = spawnPreviewMode.GetState();
								FiberPool::Push([hash, name, generation, preview] {
									// 有更新的操作排队时，放弃本次
									if (generation != g_PreviewGeneration.load())
										return;

									auto ped = Self::GetPed();
									if (!ped)
										return;

									// 正在预览的就是这辆 → "转正"保留，不再重复生成
									if (preview && g_LastPreviewVehicle != 0 && g_LastPreviewHash == hash
									    && ENTITY::DOES_ENTITY_EXIST(g_LastPreviewVehicle))
									{
										auto promoted = Vehicle(g_LastPreviewVehicle);
										g_LastPreviewVehicle = 0;
										g_LastPreviewHash    = 0;

										if (generation != g_PreviewGeneration.load())
											return;

										if (spawnInsideVehicle.GetState())
											Self::GetPed().SetInVehicle(promoted);

										Notifications::Show("载具生成", "已生成：" + name, NotificationType::Success);
										return;
									}

									// 否则清掉旧预览车，重新生成一辆真车
									if (preview)
										RemovePreviewVehicle();

									// 在玩家正前方生成（6 米，落点被占则 11 米）
									auto handle = Vehicle::Create(hash, GetPreviewSpawnCoords(ped), ped.GetHeading());
									if (!handle)
									{
										Notifications::Show("载具生成", "生成失败：模型无法加载。", NotificationType::Error);
										return;
									}

									// 生成期间又被更新的操作取代：把刚生成的也清掉
									if (generation != g_PreviewGeneration.load())
									{
										handle.Delete();
										return;
									}

									if (spawnInsideVehicle.GetState())
										Self::GetPed().SetInVehicle(handle);

									if (spawnVehicleMaxed.GetState())
										handle.Upgrade();

									Notifications::Show("载具生成", "已生成：" + name, NotificationType::Success);
								});
							}
							ImGui::PopID();
						}
					}
				}

				ImGui::EndListBox();
			}

			// —— 悬停预览：鼠标停在某一项上约 0.3 秒即在车前展示该车（自动替换） ——
			static int s_HoverAccumHash  = 0;
			static float s_HoverAccumTime = 0.0f;

			if (spawnPreviewMode.GetState() && hovered_hash != 0)
			{
				if (hovered_hash == s_HoverAccumHash)
					s_HoverAccumTime += ImGui::GetIO().DeltaTime;
				else
				{
					s_HoverAccumHash = hovered_hash;
					s_HoverAccumTime = 0.0f;
				}

				if (s_HoverAccumTime >= 0.3f)
				{
					s_HoverAccumTime = 0.0f;
					s_HoverAccumHash = 0; // 触发后重臂（若仍停在同一辆，fiber 端会识别并直接返回）

					const int generation = ++g_PreviewGeneration;
					FiberPool::Push([hovered_hash, generation] {
						if (generation != g_PreviewGeneration.load())
							return;

						// 已经是当前预览车 → 无需重复生成
						if (g_LastPreviewVehicle != 0 && g_LastPreviewHash == hovered_hash
						    && ENTITY::DOES_ENTITY_EXIST(g_LastPreviewVehicle))
							return;

						RemovePreviewVehicle();

						auto ped = Self::GetPed();
						if (!ped)
							return;

						auto handle = Vehicle::Create(hovered_hash, GetPreviewSpawnCoords(ped), ped.GetHeading());
						if (!handle)
							return;

						if (generation != g_PreviewGeneration.load())
						{
							handle.Delete();
							return;
						}

						if (spawnVehicleMaxed.GetState())
							handle.Upgrade();

						g_LastPreviewVehicle = handle.GetHandle();
						g_LastPreviewHash    = hovered_hash;
						// 悬停预览不弹通知，避免刷屏
					});
				}
			}
			else
			{
				s_HoverAccumHash = 0;
				s_HoverAccumTime = 0.0f;
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
