#include "game/commands/PlayerCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class SmashWindows : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = player.GetPed().GetVehicle();
			if (!veh)
				return;

			for (int i = 0; i <= 7; i++)
				VEHICLE::SMASH_VEHICLE_WINDOW(veh.GetHandle(), i);
		}
	};

	class KillEngine : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = player.GetPed().GetVehicle();
			if (!veh)
				return;

			VEHICLE::SET_VEHICLE_ENGINE_HEALTH(veh.GetHandle(), -4000.0f);
		}
	};

	class FlipVehicle : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = player.GetPed().GetVehicle();
			if (!veh)
				return;

			auto rot = ENTITY::GET_ENTITY_ROTATION(veh.GetHandle(), 2);
			ENTITY::SET_ENTITY_ROTATION(veh.GetHandle(), rot.x, rot.y, rot.z + 180.0f, 2, true);
		}
	};

	class TeleportIntoVehicle : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = player.GetPed().GetVehicle();
			auto ped = Self::GetPed();
			if (!veh || !ped)
				return;

			PED::SET_PED_INTO_VEHICLE(ped.GetHandle(), veh.GetHandle(), -1);
		}
	};

	static SmashWindows _SmashWindows{"smashwindows", "砸碎车窗", "砸碎目标玩家载具的所有车窗"};
	static KillEngine _KillEngine{"killengine", "破坏引擎", "报废目标玩家载具的引擎"};
	static FlipVehicle _FlipVehicle{"flipvehicle", "掉头翻转", "将目标玩家载具原地旋转 180 度"};
	static TeleportIntoVehicle _TeleportIntoVehicle{"tpintovehicle", "传送进车", "把自己传送进目标玩家的载具"};
}
