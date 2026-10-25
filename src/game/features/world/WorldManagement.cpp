#include "core/backend/FiberPool.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/commands/ColorCommand.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Entity.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Pools.hpp"

#include <chrono>
#include <cmath>

namespace YimMenu::Features
{
	static ColorCommand _TrafficColour{"trafficcolour", "交通颜色", "把所有 NPC 车辆染成指定颜色（100 米内，本地可见）。", ImVec4{1.0f, 0.2f, 0.8f, 1.0f}};
	static FloatCommand _ClearAreaRadius{"cleararearadius", "清场半径", "清场/惩罚的生效半径（米）。", 20.0f, 300.0f, 60.0f};

	static float Dist2D(const rage::fvector3& a, const rage::fvector3& b)
	{
		const float dx = a.x - b.x;
		const float dy = a.y - b.y;
		return std::sqrt(dx * dx + dy * dy);
	}

	static bool HasPlayerInside(int veh)
	{
		const auto driver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(veh, -1, true);
		if (driver && PED::IS_PED_A_PLAYER(driver))
			return true;

		for (int seat = 0; seat < 3; seat++)
		{
			const auto passenger = VEHICLE::GET_PED_IN_VEHICLE_SEAT(veh, seat, true);
			if (passenger && PED::IS_PED_A_PLAYER(passenger))
				return true;
		}

		return false;
	}

	class NoTraffic : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			VEHICLE::SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0f);
			VEHICLE::SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0f);
			VEHICLE::SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0f);
		}
	};

	class TrafficColour : public LoopedCommand
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
			const auto colour  = _TrafficColour.GetState();
			const int r = (int)(colour.x * 255.0f);
			const int g = (int)(colour.y * 255.0f);
			const int b = (int)(colour.z * 255.0f);

			for (Vehicle v : Pools::GetVehicles())
			{
				if (!v || !v.IsValid())
					continue;

				const int handle = v.GetHandle();
				if (HasPlayerInside(handle))
					continue;
				if (DECORATOR::DECOR_EXIST_ON(handle, "Player_Vehicle"))
					continue;
				if (Dist2D(selfPos, v.GetPosition()) > 100.0f)
					continue;

				// 已经是目标颜色则跳过，避免每隔 0.5 秒重复同步
				int pr, pg, pb, sr, sg, sb;
				VEHICLE::GET_VEHICLE_CUSTOM_PRIMARY_COLOUR(handle, &pr, &pg, &pb);
				VEHICLE::GET_VEHICLE_CUSTOM_SECONDARY_COLOUR(handle, &sr, &sg, &sb);
				if (pr == r && pg == g && pb == b && sr == r && sg == g && sb == b)
					continue;

				if (!NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(handle))
				{
					NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(handle);
					continue;
				}

				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(handle, r, g, b);
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(handle, r, g, b);
			}
		}
	};

	class ClearArea : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			const float radius = _ClearAreaRadius.GetState();

			FiberPool::Push([radius] {
				auto ped = Self::GetPed();
				if (!ped)
					return;

				const auto selfPos = ped.GetPosition();
				int processed = 0;

				for (Ped p : Pools::GetPeds())
				{
					if (!p || !p.IsValid() || p.GetHandle() == ped.GetHandle())
						continue;
					if (PED::IS_PED_A_PLAYER(p.GetHandle()))
						continue;
					if (Dist2D(selfPos, p.GetPosition()) > radius)
						continue;

					p.Delete();

					if (++processed % 20 == 0)
						ScriptMgr::Yield(std::chrono::milliseconds(5));
				}

				for (Vehicle v : Pools::GetVehicles())
				{
					if (!v || !v.IsValid())
						continue;
					if (HasPlayerInside(v.GetHandle()))
						continue;
					if (Dist2D(selfPos, v.GetPosition()) > radius)
						continue;

					v.Delete();

					if (++processed % 20 == 0)
						ScriptMgr::Yield(std::chrono::milliseconds(5));
				}

				Notifications::Show("清场", "半径内 NPC 与车辆已清理。", NotificationType::Success);
			});
		}
	};

	class NpcPunishment : public LoopedCommand
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
			const int self     = ped.GetHandle();
			const float radius = _ClearAreaRadius.GetState();

			for (Ped p : Pools::GetPeds())
			{
				if (!p || !p.IsValid() || p.GetHandle() == self)
					continue;
				if (PED::IS_PED_A_PLAYER(p.GetHandle()))
					continue;
				if (ENTITY::IS_ENTITY_DEAD(p.GetHandle(), false))
					continue;
				if (Dist2D(selfPos, p.GetPosition()) > radius)
					continue;

				// 跳过带光点的任务 NPC，避免误杀任务/盟友单位（Stand 同款保护）
				if (ENTITY::IS_ENTITY_A_MISSION_ENTITY(p.GetHandle()) && HUD::GET_BLIP_FROM_ENTITY(p.GetHandle()) != 0)
					continue;

				// 正在射击我们、或已与我们交战的 NPC → 直接清除
				if (PED::IS_PED_SHOOTING(p.GetHandle()) || PED::IS_PED_IN_COMBAT(p.GetHandle(), self))
					p.Kill();
			}
		}
	};

	static NoTraffic _NoTraffic{"notraffic", "禁用交通", "街上不再刷出新车（本地视角）。"};
	static TrafficColour _TrafficColourCmd{"colourtraffic", "交通染色", "把 100 米内的 NPC 车辆染成指定颜色。"};
	static ClearArea _ClearArea{"cleararea", "半径清场", "删除半径内所有 NPC 与无玩家乘坐的车辆。"};
	static NpcPunishment _NpcPunishment{"npcpunish", "NPC 反击惩罚", "自动击毙半径内正在向你射击或与你交战的 NPC。"};
}
