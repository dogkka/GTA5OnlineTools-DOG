#include "core/commands/Command.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Pools.hpp"

#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace YimMenu::Features
{
	static ListCommand _BlipColour{"vehblipcolour", "光点颜色", "载具光点的颜色。",
	    {{0, "蓝"}, {1, "红"}, {2, "绿"}, {3, "黄"}, {4, "紫"}, {5, "青"}, {6, "粉"}}, 0};
	static FloatCommand _BlipRadius{"vehblipradius", "光点范围", "给这个范围内的载具加光点（米）。", 50.0f, 400.0f, 150.0f};
	static FloatCommand _BlipScale{"vehblipscale", "光点大小", "载具光点的缩放。", 0.5f, 2.0f, 0.8f};

	static const int kBlipColours[] = {38, 1, 2, 5, 7, 3, 27};
	static std::unordered_map<int, int> g_VehBlips{};

	static void ClearVehicleBlips()
	{
		for (auto& [veh, blip] : g_VehBlips)
		{
			if (blip)
				HUD::REMOVE_BLIP(&blip);
		}
		g_VehBlips.clear();
	}

	// 附近载具光点：给半径内的载具加小地图光点（自动跟随、离开范围自动清理）
	class VehicleBlips : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 30)
				return;
			m_Timer = 0;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			const auto selfPos = ped.GetPosition();
			const float radius = _BlipRadius.GetState();
			const float scale  = _BlipScale.GetState();
			const int colour   = kBlipColours[std::clamp(_BlipColour.GetState(), 0, 6)];

			// 清理：实体消失或超出范围
			for (auto it = g_VehBlips.begin(); it != g_VehBlips.end();)
			{
				bool remove = false;
				if (!ENTITY::DOES_ENTITY_EXIST(it->first))
				{
					remove = true;
				}
				else
				{
					const auto pos = ENTITY::GET_ENTITY_COORDS(it->first, false);
					const float dx = pos.x - selfPos.x;
					const float dy = pos.y - selfPos.y;
					if (std::sqrt(dx * dx + dy * dy) > radius * 1.2f)
						remove = true;
				}

				if (remove)
				{
					int blip = it->second;
					if (blip)
						HUD::REMOVE_BLIP(&blip);
					it = g_VehBlips.erase(it);
				}
				else
				{
					++it;
				}
			}

			// 添加：半径内的载具
			for (Vehicle v : Pools::GetVehicles())
			{
				if (!v || !v.IsValid())
					continue;
				if (g_VehBlips.size() > 250)
					break;

				const int handle = v.GetHandle();
				if (g_VehBlips.contains(handle))
					continue;

				const auto pos = v.GetPosition();
				const float dx = pos.x - selfPos.x;
				const float dy = pos.y - selfPos.y;
				if (std::sqrt(dx * dx + dy * dy) > radius)
					continue;

				const int blip = HUD::ADD_BLIP_FOR_ENTITY(handle);
				if (!blip)
					continue;

				HUD::SET_BLIP_SPRITE(blip, 225);
				HUD::SET_BLIP_COLOUR(blip, colour);
				HUD::SET_BLIP_SCALE(blip, scale);
				HUD::SET_BLIP_AS_SHORT_RANGE(blip, true);
				HUD::SET_BLIP_HIDDEN_ON_LEGEND(blip, true);

				g_VehBlips.emplace(handle, blip);
			}
		}

		virtual void OnDisable() override
		{
			ClearVehicleBlips();
		}
	};

	class ClearVehicleBlipsCommand : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			ClearVehicleBlips();
		}
	};

	static VehicleBlips _VehicleBlips{"vehblips", "附近载具光点", "给半径内的载具加小地图光点，离开范围自动清理。"};
	static ClearVehicleBlipsCommand _ClearVehicleBlips{"clearvehblips", "清除全部光点", "立即移除本功能添加的所有载具光点。"};
}
