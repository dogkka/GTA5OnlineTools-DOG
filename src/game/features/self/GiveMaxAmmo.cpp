#include "core/commands/Command.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/data/Weapons.hpp"

namespace YimMenu::Features
{
	class GiveMaxAmmo : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			for (auto weapon : g_WeaponHashes)
			{
				Self::GetPed().SetMaxAmmoForWeapon(weapon);
			}
		}
	};

	static GiveMaxAmmo _GiveMaxAmmo{"givemaxammo", "补满弹药", "将你的弹药补充到上限。"};
}
