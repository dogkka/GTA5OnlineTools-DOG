#include "core/commands/LoopedCommand.hpp"
#include "core/commands/FloatCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "types/pad/ControllerInputs.hpp"

namespace YimMenu::Features
{
	static FloatCommand _FlySpeed{"flyspeed", "飞行速度", "飞行时的前进速度", 1.0f, 150.0f, 20.0f};

	class Fly : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_LastVehicle = 0;

		void CleanupVehicle(int handle)
		{
			if (handle == 0 || !ENTITY::DOES_ENTITY_EXIST(handle))
				return;

			VEHICLE::SET_VEHICLE_GRAVITY(handle, true);
			ENTITY::SET_ENTITY_COLLISION(handle, true, true);
			VEHICLE::SET_VEHICLE_FORWARD_SPEED(handle, 0.0f);
		}

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (!veh)
			{
				if (m_LastVehicle != 0)
				{
					CleanupVehicle(m_LastVehicle);
					m_LastVehicle = 0;
				}
				return;
			}

			if (veh.GetHandle() != m_LastVehicle)
			{
				CleanupVehicle(m_LastVehicle);
				m_LastVehicle = veh.GetHandle();
				ENTITY::SET_ENTITY_COLLISION(veh.GetHandle(), false, false);
			}

			auto cam_rot = CAMERA::GET_GAMEPLAY_CAM_ROT(0);
			ENTITY::SET_ENTITY_ROTATION(veh.GetHandle(), cam_rot.x, cam_rot.y, cam_rot.z, 0, true);

			const float speed = _FlySpeed.GetState();
			const bool up     = PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_MOVE_UP_ONLY);
			const float locspeed = up ? speed * 2.0f : speed;

			if (PAD::IS_CONTROL_PRESSED(2, (int)ControllerInputs::INPUT_VEH_ACCELERATE))
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh.GetHandle(), locspeed);
			else if (PAD::IS_CONTROL_PRESSED(2, (int)ControllerInputs::INPUT_VEH_BRAKE))
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh.GetHandle(), -locspeed);
			else
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh.GetHandle(), 0.0f);

			if (PAD::IS_CONTROL_PRESSED(2, (int)ControllerInputs::INPUT_VEH_MOVE_LEFT_ONLY))
				ENTITY::APPLY_FORCE_TO_ENTITY(veh.GetHandle(), 1, -locspeed, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 1, 1, 1, 0, 1);

			if (PAD::IS_CONTROL_PRESSED(2, (int)ControllerInputs::INPUT_VEH_MOVE_RIGHT_ONLY))
				ENTITY::APPLY_FORCE_TO_ENTITY(veh.GetHandle(), 1, locspeed, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 1, 1, 1, 0, 1);

			VEHICLE::SET_VEHICLE_GRAVITY(veh.GetHandle(), false);
		}

		virtual void OnDisable() override
		{
			CleanupVehicle(m_LastVehicle);
			m_LastVehicle = 0;
		}
	};

	static Fly _Fly{"fly", "飞天模式", "让当前载具飞行，方向跟随视角"};
}
