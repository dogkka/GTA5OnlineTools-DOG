#include <cmath>
#include <iterator>
#include <vector>

#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Vehicle.hpp"

namespace YimMenu::Features
{
	struct PresetVehicle
	{
		std::uint32_t Model;
		const char* Name;
	};

	static const PresetVehicle g_PresetVehicles[] = {
	    {"zentorno"_J, "桑托劳（超跑）"},
	    {"t20"_J, "T20（超跑）"},
	    {"italirsx"_J, "意大利 RSX"},
	    {"kuruma2"_J, "装甲骷髅马"},
	    {"deluxo"_J, "德罗索（飞行车）"},
	    {"vigilante"_J, "义警（蝙蝠车）"},
	    {"oppressor2"_J, "暴君 MK2"},
	    {"scramjet"_J, "冲锋号"},
	};

	static std::vector<std::pair<int, const char*>> MakePresetList()
	{
		std::vector<std::pair<int, const char*>> list;
		for (int i = 0; i < (int)std::size(g_PresetVehicles); i++)
			list.push_back({i, g_PresetVehicles[i].Name});
		return list;
	}

	static ListCommand _PresetVehicle{"presetvehicle", "预设车辆", "选择要刷出的全改装车辆", MakePresetList(), 0};

	static void ApplyMaxUpgrades(Vehicle veh)
	{
		VEHICLE::SET_VEHICLE_MOD_KIT(veh.GetHandle(), 0);

		VEHICLE::TOGGLE_VEHICLE_MOD(veh.GetHandle(), 18, true); // turbo
		VEHICLE::TOGGLE_VEHICLE_MOD(veh.GetHandle(), 20, true); // tyre smoke
		VEHICLE::TOGGLE_VEHICLE_MOD(veh.GetHandle(), 22, true); // xenon lights
		VEHICLE::SET_VEHICLE_WINDOW_TINT(veh.GetHandle(), 1);

		for (int slot = 0; slot <= 49; slot++)
		{
			if (slot == 48) // livery
				continue;

			const int count = VEHICLE::GET_NUM_VEHICLE_MODS(veh.GetHandle(), slot);
			if (count <= 0)
				continue;

			int selected = -1;
			for (int mod = count - 1; mod >= 0; mod--)
			{
				if (VEHICLE::IS_VEHICLE_MOD_GEN9_EXCLUSIVE(veh.GetHandle(), slot, mod))
					continue;

				selected = mod;
				break;
			}

			if (selected != -1)
				VEHICLE::SET_VEHICLE_MOD(veh.GetHandle(), slot, selected, false);
		}
	}

	class SpawnPresetVehicle : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			const int index = _PresetVehicle.GetState();
			if (index < 0 || index >= (int)std::size(g_PresetVehicles))
				return;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			const auto& preset = g_PresetVehicles[index];
			const auto pos     = ped.GetPosition();
			const auto heading = ped.GetHeading();
			const float rad    = heading * 3.14159265f / 180.0f;

			const float x = pos.x - sinf(rad) * 6.0f;
			const float y = pos.y + cosf(rad) * 6.0f;

			auto veh = Vehicle::Create(preset.Model, rage::fvector3{x, y, pos.z + 0.5f}, heading);
			if (!veh)
			{
				Notifications::Show("预设车辆", "刷出失败，请稍后重试。", NotificationType::Error);
				return;
			}

			ApplyMaxUpgrades(veh);

			Notifications::Show("预设车辆", std::string("已刷出全改装：") + preset.Name, NotificationType::Success);
		}
	};

	static SpawnPresetVehicle _SpawnPresetVehicle{"spawnpresetvehicle", "刷出预设车", "在前方刷出所选的全改装车辆"};
}
