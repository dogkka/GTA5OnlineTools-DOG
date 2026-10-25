#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/features/players/vehicle/VehicleTrollHelpers.hpp"
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
			{
				Notifications::Show("击穿玻璃", "该玩家不在载具中。", NotificationType::Warning);
				return;
			}

			// 先抢控制权（空车同步生效）；对方驾驶中拿不到控制权时走轻伤伤害事件兜底
			ApplyVehicleTroll(
			    veh.GetHandle(),
			    [](int handle) {
				    for (int window = 0; window <= 7; window++)
					    VEHICLE::SMASH_VEHICLE_WINDOW(handle, window);
			    },
			    150.0f);
		}
	};

	class KillEngine : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = player.GetPed().GetVehicle();
			if (!veh)
			{
				Notifications::Show("破坏引擎", "目标不在载具中。", NotificationType::Warning);
				return;
			}

			// 先抢控制权（空车可同步生效）；被目标驾驶时走爆炸伤害事件兜底
			ApplyVehicleTroll(
			    veh.GetHandle(),
			    [](int handle) {
				    VEHICLE::SET_VEHICLE_ENGINE_HEALTH(handle, -4000.0f);
				    VEHICLE::SET_VEHICLE_ENGINE_ON(handle, false, true, true);
			    },
			    1100.0f);
		}
	};

	class FlipVehicle : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = player.GetPed().GetVehicle();
			if (!veh)
			{
				Notifications::Show("掉头翻转", "该玩家不在载具中。", NotificationType::Warning);
				return;
			}

			ApplyVehicleTroll(
			    veh.GetHandle(),
			    [](int handle) {
				    auto rot     = ENTITY::GET_ENTITY_ROTATION(handle, 2);
				    float heading = rot.z + 180.0f;
				    if (heading > 360.0f)
					    heading -= 360.0f;
				    ENTITY::SET_ENTITY_ROTATION(handle, rot.x, rot.y, heading, 2, true);
			    },
			    600.0f);
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

	static SmashWindows _SmashWindows{"smashwindows", "击穿玻璃", "击碎目标玩家载具的所有车窗"};
	static KillEngine _KillEngine{"killengine", "破坏引擎", "报废目标玩家载具的引擎"};
	static FlipVehicle _FlipVehicle{"flipvehicle", "掉头翻转", "将目标玩家载具原地旋转 180 度"};
	static TeleportIntoVehicle _TeleportIntoVehicle{"tpintovehicle", "传送进车", "把自己传送进目标玩家的载具"};
}
