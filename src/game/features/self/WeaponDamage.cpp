#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Pools.hpp"
#include <game/gta/Natives.hpp>
#include "core/commands/FloatCommand.hpp"

namespace YimMenu::Features
{
	static FloatCommand _WeaponDamageScale{"weapondamagescale", "武器伤害倍率", "设置武器伤害倍率（包括近战）", 0.0f, 10.0f, 1.0f};

	class WeaponDamage : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		void OnTick() override
		{
			float scale = _WeaponDamageScale.GetState();
			if (scale > 0.0f)
			{
				Hash weapon;
				if (WEAPON::GET_CURRENT_PED_WEAPON(Self::GetPed().GetHandle(), &weapon, 0))
				{
					WEAPON::SET_WEAPON_DAMAGE_MODIFIER(weapon, scale);
					WEAPON::_SET_WEAPON_PED_DAMAGE_MODIFIER(Self::GetPed().GetHandle(), scale);
				}
			}
		}

		void OnDisable() override
		{
			Hash weapon;
			if (WEAPON::GET_CURRENT_PED_WEAPON(Self::GetPed().GetHandle(), &weapon, 0))
			{
				WEAPON::SET_WEAPON_DAMAGE_MODIFIER(weapon, 1.0f);
				WEAPON::_SET_WEAPON_PED_DAMAGE_MODIFIER(Self::GetPed().GetHandle(), 1.0f);
			}
		}
	};

	static WeaponDamage _WeaponDamage{"weapondamage", "武器伤害", "允许修改你的武器伤害输出"};
}
