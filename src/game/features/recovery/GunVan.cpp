#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Tunables.hpp"
#include "game/gta/data/WeaponDisplayNames.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	static constexpr auto g_GunVanWeapons = std::to_array<const char*>({
	    "WEAPON_KNIFE",
	    "WEAPON_NIGHTSTICK",
	    "WEAPON_HAMMER",
	    "WEAPON_BOTTLE",
	    "WEAPON_DAGGER",
	    "WEAPON_KNUCKLE",
	    "WEAPON_MACHETE",
	    "WEAPON_FLASHLIGHT",
	    "WEAPON_SWITCHBLADE",
	    "WEAPON_BATTLEAXE",
	    "WEAPON_POOLCUE",
	    "WEAPON_WRENCH",
	    "WEAPON_HATCHET",
	    "WEAPON_BAT",
	    "WEAPON_CROWBAR",
	    "WEAPON_STUNROD",
	    "WEAPON_PISTOL",
	    "WEAPON_PISTOL_MK2",
	    "WEAPON_COMBATPISTOL",
	    "WEAPON_APPISTOL",
	    "WEAPON_PISTOL50",
	    "WEAPON_SNSPISTOL",
	    "WEAPON_SNSPISTOL_MK2",
	    "WEAPON_HEAVYPISTOL",
	    "WEAPON_VINTAGEPISTOL",
	    "WEAPON_FLAREGUN",
	    "WEAPON_MARKSMANPISTOL",
	    "WEAPON_REVOLVER",
	    "WEAPON_REVOLVER_MK2",
	    "WEAPON_DOUBLEACTION",
	    "WEAPON_RAYPISTOL",
	    "WEAPON_CERAMICPISTOL",
	    "WEAPON_NAVYREVOLVER",
	    "WEAPON_GADGETPISTOL",
	    "WEAPON_STUNGUN_MP",
	    "WEAPON_MICROSMG",
	    "WEAPON_SMG",
	    "WEAPON_SMG_MK2",
	    "WEAPON_ASSAULTSMG",
	    "WEAPON_COMBATPDW",
	    "WEAPON_MACHINEPISTOL",
	    "WEAPON_MINISMG",
	    "WEAPON_MG",
	    "WEAPON_COMBATMG",
	    "WEAPON_COMBATMG_MK2",
	    "WEAPON_GUSENBERG",
	    "WEAPON_RAYCARBINE",
	    "WEAPON_ASSAULTRIFLE",
	    "WEAPON_ASSAULTRIFLE_MK2",
	    "WEAPON_CARBINERIFLE",
	    "WEAPON_CARBINERIFLE_MK2",
	    "WEAPON_ADVANCEDRIFLE",
	    "WEAPON_SPECIALCARBINE",
	    "WEAPON_SPECIALCARBINE_MK2",
	    "WEAPON_BULLPUPRIFLE",
	    "WEAPON_BULLPUPRIFLE_MK2",
	    "WEAPON_COMPACTRIFLE",
	    "WEAPON_MILITARYRIFLE",
	    "WEAPON_HEAVYRIFLE",
	    "WEAPON_TACTICALRIFLE",
	    "WEAPON_PUMPSHOTGUN",
	    "WEAPON_PUMPSHOTGUN_MK2",
	    "WEAPON_SAWNOFFSHOTGUN",
	    "WEAPON_ASSAULTSHOTGUN",
	    "WEAPON_BULLPUPSHOTGUN",
	    "WEAPON_MUSKET",
	    "WEAPON_HEAVYSHOTGUN",
	    "WEAPON_DBSHOTGUN",
	    "WEAPON_AUTOSHOTGUN",
	    "WEAPON_COMBATSHOTGUN",
	    "WEAPON_SNIPERRIFLE",
	    "WEAPON_HEAVYSNIPER",
	    "WEAPON_HEAVYSNIPER_MK2",
	    "WEAPON_MARKSMANRIFLE",
	    "WEAPON_MARKSMANRIFLE_MK2",
	    "WEAPON_PRECISIONRIFLE",
	    "WEAPON_GRENADELAUNCHER",
	    "WEAPON_RPG",
	    "WEAPON_MINIGUN",
	    "WEAPON_HOMINGLAUNCHER",
	    "WEAPON_COMPACTLAUNCHER",
	    "WEAPON_RAYMINIGUN",
	    "WEAPON_EMPLAUNCHER",
	    "WEAPON_RAILGUNXM3",
	    "WEAPON_TECPISTOL",
	    "WEAPON_FIREWORK",
	    "WEAPON_BATTLERIFLE",
	    "WEAPON_SNOWLAUNCHER",
	});

	static std::vector<std::pair<int, const char*>> BuildGunVanWeaponList()
	{
		std::vector<std::pair<int, const char*>> result;
		result.reserve(g_GunVanWeapons.size());
		for (int i = 0; i < static_cast<int>(g_GunVanWeapons.size()); ++i)
		{
			const auto displayName = GetWeaponDisplayNameFallback(Joaat(g_GunVanWeapons[i]));
			result.emplace_back(i, displayName.empty() ? g_GunVanWeapons[i] : displayName.data());
		}
		return result;
	}

	static std::vector<std::pair<int, const char*>> g_GunVanSlots = {
	    {1, "槽位 1"},
	    {2, "槽位 2"},
	    {3, "槽位 3"},
	    {4, "槽位 4"},
	    {5, "槽位 5"},
	    {6, "槽位 6"},
	    {7, "槽位 7"},
	    {8, "槽位 8"},
	    {9, "槽位 9"},
	    {10, "槽位 10"},
	};

	static ListCommand _GunVanSlot{"gunvanslot", "槽位", "选择要修改的枪支厢型车武器槽位。", g_GunVanSlots, 1};
	static ListCommand _GunVanWeapon{"gunvanweapon", "武器", "选择要放入枪支厢型车槽位的武器。", BuildGunVanWeaponList(), 0};

	class SetGunVanWeapon final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
			{
				Notifications::Show("枪支厢型车", "请先进入 GTA 在线模式。", NotificationType::Error);
				return;
			}

			const int slot = _GunVanSlot.GetState();
			const int weaponIndex = _GunVanWeapon.GetState();
			if (slot < 1 || slot > 10 || weaponIndex < 0 || weaponIndex >= static_cast<int>(g_GunVanWeapons.size()))
			{
				Notifications::Show("枪支厢型车", "槽位或武器选择无效。", NotificationType::Error);
				return;
			}

			const auto tunableName = "XM22_GUN_VAN_SLOT_WEAPON_TYPE_" + std::to_string(slot);
			Tunable tunable{Joaat(tunableName)};
			if (!tunable.IsReady())
			{
				Notifications::Show("枪支厢型车", "可调参数缓存尚未完成，请稍后重试。", NotificationType::Error);
				return;
			}

			tunable.Set<int>(Joaat(g_GunVanWeapons[weaponIndex]));
			Notifications::Show("枪支厢型车", "武器槽位已更新。重新打开枪支厢型车商店后生效。", NotificationType::Success);
		}
	};

	static SetGunVanWeapon _SetGunVanWeapon{"setgunvanweapon", "应用武器", "修改所选枪支厢型车槽位的武器。"};
}
