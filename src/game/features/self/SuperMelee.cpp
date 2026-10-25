#include "core/commands/IntCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Entity.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Pools.hpp"

#include <cmath>

namespace YimMenu::Features
{
	static IntCommand _SuperMeleePower{"supermeleepower", "击飞力度", "近战击飞被打中目标的力度。", 1, 10, 5};

	static float Dist2D(const rage::fvector3& a, const rage::fvector3& b)
	{
		const float dx = a.x - b.x;
		const float dy = a.y - b.y;
		return std::sqrt(dx * dx + dy * dy);
	}

	static bool WasMeleedByUs(Entity ent, int self)
	{
		return WEAPON::HAS_ENTITY_BEEN_DAMAGED_BY_WEAPON(ent.GetHandle(), 0, 1)
		    && ENTITY::HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(ent.GetHandle(), self, true);
	}

	static void ClearDamageMarkers(Entity ent)
	{
		WEAPON::CLEAR_ENTITY_LAST_WEAPON_DAMAGE(ent.GetHandle());
		ENTITY::CLEAR_ENTITY_LAST_DAMAGE_ENTITY(ent.GetHandle());
	}

	static void ApplyLaunch(Entity ent, const rage::fvector3& selfPos, float power)
	{
		const auto pos = ent.GetPosition();
		rage::fvector3 dir{pos.x - selfPos.x, pos.y - selfPos.y, 0.0f};
		const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
		if (len > 0.001f)
		{
			dir.x /= len;
			dir.y /= len;
		}

		ENTITY::APPLY_FORCE_TO_ENTITY(ent.GetHandle(), 5, 0.0f, 0.0f, 15.0f, 0.0f, 0.0f, 0.0f, 0, true, false, true, false, true);
		ENTITY::APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(ent.GetHandle(), 1, dir.x * power * 25.0f, dir.y * power * 25.0f, -1.0f, true, false, true, true);
	}

	// 超级近战击飞：近战打中谁，谁就被弹飞（NPC 与无人载具，Stand 同款伤害标记判定）
	class SuperMelee : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 3)
				return;
			m_Timer = 0;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			const float power  = (float)_SuperMeleePower.GetState();
			const auto selfPos = ped.GetPosition();
			const int self     = ped.GetHandle();

			for (Ped p : Pools::GetPeds())
			{
				if (!p || !p.IsValid() || p.GetHandle() == self)
					continue;
				if (ENTITY::IS_ENTITY_DEAD(p.GetHandle(), false))
					continue;
				if (PED::IS_PED_A_PLAYER(p.GetHandle()))
					continue;
				if (Dist2D(selfPos, p.GetPosition()) > 20.0f)
					continue;

				if (WasMeleedByUs(p, self))
					ApplyLaunch(p, selfPos, power);

				ClearDamageMarkers(p);
			}

			for (Vehicle v : Pools::GetVehicles())
			{
				if (!v || !v.IsValid())
					continue;

				// 有玩家在任意座位（司机或乘客）都跳过，防止把玩家载具弹飞
				bool occupied_by_player = false;
				for (int seat = -1; seat < 4; seat++)
				{
					const auto occupant = VEHICLE::GET_PED_IN_VEHICLE_SEAT(v.GetHandle(), seat, true);
					if (occupant && PED::IS_PED_A_PLAYER(occupant))
					{
						occupied_by_player = true;
						break;
					}
				}
				if (occupied_by_player)
					continue;

				if (Dist2D(selfPos, v.GetPosition()) > 20.0f)
					continue;

				if (WasMeleedByUs(v, self))
				{
					NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(v.GetHandle());
					ApplyLaunch(v, selfPos, power);
				}

				ClearDamageMarkers(v);
			}
		}
	};

	static SuperMelee _SuperMelee{"supermelee", "超级近战击飞", "近战打中的 NPC/载具会被直接弹飞（对玩家无效，力度可调）。"};
}
