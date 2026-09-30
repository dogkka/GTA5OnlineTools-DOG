#include "core/commands/BoolCommand.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Vehicle.hpp"
#include "types/pad/ControllerInputs.hpp"

namespace YimMenu::Features
{
	static int g_RemoteControlHandle = 0;

	class RemoteControlTarget : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = player.GetPed().GetVehicle();
			if (!veh)
			{
				Notifications::Show("遥控载具", "该玩家不在载具中。", NotificationType::Warning);
				return;
			}

			g_RemoteControlHandle = veh.GetHandle();

			if (auto cmd = Commands::GetCommand<BoolCommand>("remotecontrol"_J))
				cmd->SetState(true);

			Notifications::Show("遥控载具", "已夺取目标载具控制权。W/S 前进后退，A/D 转向。", NotificationType::Info);
		}
	};

	static RemoteControlTarget _RemoteControlTarget{"rcvehicle", "遥控载具", "夺取目标玩家载具的控制权"};

	class RemoteControl : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_ControlRequestTimer = 0;

		virtual void OnDisable() override
		{
			if (g_RemoteControlHandle != 0 && ENTITY::DOES_ENTITY_EXIST(g_RemoteControlHandle))
				VEHICLE::SET_VEHICLE_DOORS_LOCKED(g_RemoteControlHandle, 1);

			g_RemoteControlHandle = 0;
		}

		virtual void OnTick() override
		{
			if (g_RemoteControlHandle == 0)
				return;

			if (!ENTITY::DOES_ENTITY_EXIST(g_RemoteControlHandle))
			{
				g_RemoteControlHandle = 0;
				return;
			}

			Vehicle veh(g_RemoteControlHandle);
			if (!veh.HasControl())
			{
				if (++m_ControlRequestTimer >= 30)
				{
					m_ControlRequestTimer = 0;
					veh.RequestControl(0);
				}
				return;
			}

			m_ControlRequestTimer = 0;

			VEHICLE::SET_VEHICLE_ENGINE_ON(veh.GetHandle(), true, true, false);
			VEHICLE::SET_VEHICLE_DOORS_LOCKED(veh.GetHandle(), 4);

			constexpr float speed = 25.0f;
			const bool accel     = PAD::IS_CONTROL_PRESSED(2, (int)ControllerInputs::INPUT_VEH_ACCELERATE);
			const bool brake     = PAD::IS_CONTROL_PRESSED(2, (int)ControllerInputs::INPUT_VEH_BRAKE);

			if (accel && !brake)
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh.GetHandle(), speed);
			else if (brake && !accel)
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh.GetHandle(), -speed * 0.5f);
			else
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh.GetHandle(), 0.0f);

			if (PAD::IS_CONTROL_PRESSED(2, (int)ControllerInputs::INPUT_VEH_MOVE_LEFT_ONLY))
				ENTITY::APPLY_FORCE_TO_ENTITY(veh.GetHandle(), 1, speed * 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 1, 1, 1, 0, 1);

			if (PAD::IS_CONTROL_PRESSED(2, (int)ControllerInputs::INPUT_VEH_MOVE_RIGHT_ONLY))
				ENTITY::APPLY_FORCE_TO_ENTITY(veh.GetHandle(), 1, -speed * 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 1, 1, 1, 0, 1);
		}
	};

	static RemoteControl _RemoteControl{"remotecontrol", "遥控开关", "用你的按键驾驶上次锁定的目标载具"};
}
