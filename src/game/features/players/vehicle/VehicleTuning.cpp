#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class DowngradeVehicle : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = player.GetPed().GetVehicle();
			if (!veh)
				return;

			VEHICLE::SET_VEHICLE_MOD_KIT(veh.GetHandle(), 0);

			for (int slot = 0; slot <= 49; slot++)
				VEHICLE::REMOVE_VEHICLE_MOD(veh.GetHandle(), slot);

			VEHICLE::TOGGLE_VEHICLE_MOD(veh.GetHandle(), 18, false); // turbo
			VEHICLE::TOGGLE_VEHICLE_MOD(veh.GetHandle(), 20, false); // tyre smoke
			VEHICLE::TOGGLE_VEHICLE_MOD(veh.GetHandle(), 22, false); // xenon lights
		}
	};

	class UpgradeVehicle : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = player.GetPed().GetVehicle();
			if (!veh)
				return;

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
	};

	class ExplodeVehicle : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = player.GetPed().GetVehicle();
			if (!veh)
				return;

			NETWORK::NETWORK_EXPLODE_VEHICLE(veh.GetHandle(), true, false, false);
		}
	};

	static DowngradeVehicle _DowngradeVehicle{"downgradevehicle", "拆除改装", "拆除目标玩家载具的全部改装"};
	static UpgradeVehicle _UpgradeVehicle{"upgradevehicle", "全改装", "给目标玩家载具装上全部最高级改装"};
	static ExplodeVehicle _ExplodeVehicle{"explodevehicle", "引爆载具", "直接引爆目标玩家的载具"};
}
