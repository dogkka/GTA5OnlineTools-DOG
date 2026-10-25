#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/data/Weapons.hpp"

#include <utility>
#include <vector>

namespace YimMenu::Features
{
	// 武器防清空：持续跟踪武器与弹药快照，被恶意清空后自动恢复。
	class WeaponProtection : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;
		int m_LastOwned = 0;
		std::vector<std::pair<std::uint32_t, int>> m_Snapshot;

		virtual void OnTick() override
		{
			if (++m_Timer < 60) // 约 1 秒
				return;

			m_Timer = 0;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			int owned = 0;
			for (auto hash : g_WeaponHashes)
			{
				if (hash == "WEAPON_UNARMED"_J)
					continue;

				if (WEAPON::HAS_PED_GOT_WEAPON(ped.GetHandle(), hash, false))
					owned++;
			}

			if (owned >= m_LastOwned)
			{
				// 更新快照（吸收新捡到的武器/弹药）
				m_Snapshot.clear();
				for (auto hash : g_WeaponHashes)
				{
					if (hash == "WEAPON_UNARMED"_J)
						continue;

					if (WEAPON::HAS_PED_GOT_WEAPON(ped.GetHandle(), hash, false))
						m_Snapshot.emplace_back(hash, WEAPON::GET_AMMO_IN_PED_WEAPON(ped.GetHandle(), hash));
				}

				m_LastOwned = owned;
				return;
			}

			// 武器数骤降 → 判定被清空，恢复
			if (m_LastOwned - owned >= 3 && !m_Snapshot.empty())
			{
				for (const auto& [hash, ammo] : m_Snapshot)
				{
					if (!WEAPON::HAS_PED_GOT_WEAPON(ped.GetHandle(), hash, false))
						WEAPON::GIVE_WEAPON_TO_PED(ped.GetHandle(), hash, ammo > 0 ? ammo : 1, false, false);
				}

				Notifications::Show("武器防护", "检测到武器被清空，已自动恢复装备。", NotificationType::Warning);
				m_LastOwned = static_cast<int>(m_Snapshot.size());
			}
		}

		virtual void OnDisable() override
		{
			m_Snapshot.clear();
			m_LastOwned = 0;
		}
	};

	static WeaponProtection _WeaponProtection{"weaponprotection", "武器防清空", "被恶意清空武器后自动恢复（含弹药快照）"};
}
