#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/features/players/vehicle/VehicleTrollHelpers.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static Vehicle GetTargetVehicle(Player player)
	{
		if (!player.IsValid() || player.IsLocal())
			return nullptr;

		auto ped = player.GetPed();
		if (!ped)
			return nullptr;

		return ped.GetVehicle();
	}

	class BreakDoors : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = GetTargetVehicle(player);
			if (!veh)
			{
				Notifications::Show("载具拆解", "目标不在载具中。", NotificationType::Warning);
				return;
			}

			ApplyVehicleTroll(veh.GetHandle(), [](int handle) {
				for (int door = 0; door <= 5; door++)
					VEHICLE::SET_VEHICLE_DOOR_BROKEN(handle, door, true);
			});
		}
	};

	class PopWindscreen : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = GetTargetVehicle(player);
			if (!veh)
			{
				Notifications::Show("载具拆解", "目标不在载具中。", NotificationType::Warning);
				return;
			}

			ApplyVehicleTroll(veh.GetHandle(), [](int handle) {
				VEHICLE::POP_OUT_VEHICLE_WINDSCREEN(handle);
			});
		}
	};

	class KillEngine : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = GetTargetVehicle(player);
			if (!veh)
			{
				Notifications::Show("载具拆解", "目标不在载具中。", NotificationType::Warning);
				return;
			}

			ApplyVehicleTroll(
			    veh.GetHandle(),
			    [](int handle) {
				VEHICLE::SET_VEHICLE_ENGINE_ON(handle, false, true, true);
				VEHICLE::SET_VEHICLE_OUT_OF_CONTROL(handle, false, false);
			    },
			    1100.0f);
		}
	};

	class LockdownVehicle : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = GetTargetVehicle(player);
			if (!veh)
			{
				Notifications::Show("载具拆解", "目标不在载具中。", NotificationType::Warning);
				return;
			}

			ApplyVehicleTroll(
			    veh.GetHandle(),
			    [](int handle) {
				VEHICLE::SET_VEHICLE_UNDRIVEABLE(handle, true);
				VEHICLE::SET_VEHICLE_OUT_OF_CONTROL(handle, false, false);
			    },
			    1500.0f);
		}
	};

	class IgniteVehicle : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = GetTargetVehicle(player);
			if (!veh)
			{
				Notifications::Show("载具拆解", "目标不在载具中。", NotificationType::Warning);
				return;
			}

			ApplyVehicleTroll(
			    veh.GetHandle(),
			    [](int handle) {
				FIRE::START_ENTITY_FIRE(handle);
			    },
			    300.0f);
		}
	};

	static BreakDoors _BreakDoors{"vehbreakdoors", "拆车门", "把目标玩家载具的车门全部拆掉。"};
	static PopWindscreen _PopWindscreen{"vehpopscreen", "掀挡风玻璃", "把目标玩家载具的挡风玻璃掀飞。"};
	static KillEngine _KillEngine{"vehkillengine", "熄火失控", "强制熄火并让目标载具失控。"};
	static LockdownVehicle _LockdownVehicle{"vehlockdown", "钉死载具", "让目标载具彻底无法驾驶（直到重新上车/重刷）。"};
	static IgniteVehicle _IgniteVehicle{"vehignite", "载具起火", "点燃目标玩家正在乘坐的载具。"};
}
