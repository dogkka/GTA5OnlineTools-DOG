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
				Notifications::Show("Remote Control", "The player is not in a vehicle.", NotificationType::Warning);
				return;
			}

			g_RemoteControlHandle = veh.GetHandle();

			if (auto cmd = Commands::GetCommand<BoolCommand>("remotecontrol"_J))
				cmd->SetState(true);

			Notifications::Show("Remote Control", "Now controlling the target vehicle. W/S to drive, A/D to steer.", NotificationType::Info);
		}
	};

	static RemoteControlTarget _RemoteControlTarget{"rcvehicle", "Remote Control", "Take control of the player's vehicle"};

	class RemoteControl : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnDisable() override
		{
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
				veh.RequestControl(0);
				return;
			}

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

	static RemoteControl _RemoteControl{"remotecontrol", "Remote Control", "Drive the last targeted player's vehicle with your controls"};
}
