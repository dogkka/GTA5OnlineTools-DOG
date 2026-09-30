#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "types/blip/BlipSprite.hpp"
#include "types/pad/ControllerInputs.hpp"

namespace YimMenu::Features
{
	static ListCommand _AutoDriveMode{"autodrivemode", "自动驾驶模式", "选择目的地：导航点 / 漫游 / 停止", {{0, "停止"}, {1, "导航点"}, {2, "漫游"}}, 0};

	class AutoDrive : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		static constexpr int s_DrivingStyle = 443; // law abiding
		static constexpr float s_DriveSpeed = 20.0f;

		int m_ActiveMode = 0;
		Vector3 m_LastTarget{};

		void StopDriving()
		{
			auto veh = Self::GetVehicle();
			auto ped = Self::GetPed();

			if (veh)
			{
				TASK::CLEAR_PRIMARY_VEHICLE_TASK(veh.GetHandle());
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh.GetHandle(), 0.0f);
			}

			if (ped)
				TASK::CLEAR_PED_TASKS(ped.GetHandle());
		}

		bool GetWaypoint(Vector3& out)
		{
			const int blip = HUD::GET_FIRST_BLIP_INFO_ID((int)BlipSprite::RADAR_WAYPOINT);
			if (!HUD::DOES_BLIP_EXIST(blip))
				return false;

			out = HUD::GET_BLIP_INFO_ID_COORD(blip);
			return true;
		}

		virtual void OnTick() override
		{
			const int mode = _AutoDriveMode.GetState();

			auto veh = Self::GetVehicle();
			auto ped = Self::GetPed();
			if (!veh || !ped)
			{
				m_ActiveMode = 0;
				return;
			}

			// stop when the player tries to take over
			const bool interrupted = PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_ACCELERATE)
			    || PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_BRAKE)
			    || PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_MOVE_LEFT_ONLY)
			    || PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_MOVE_RIGHT_ONLY);

			if (interrupted)
			{
				if (m_ActiveMode != 0)
				{
					StopDriving();
					m_ActiveMode = 0;
					_AutoDriveMode.SetState(0);
				}
				return;
			}

			if (mode == 0)
			{
				if (m_ActiveMode != 0)
				{
					StopDriving();
					m_ActiveMode = 0;
				}
				return;
			}

			if (mode == 1)
			{
				Vector3 target{};
				if (!GetWaypoint(target))
				{
					StopDriving();
					m_ActiveMode = 0;
					_AutoDriveMode.SetState(0);
					Notifications::Show("自动驾驶", "未设置导航点。", NotificationType::Warning);
					return;
				}

				const bool target_changed = m_ActiveMode != 1
				    || target.x != m_LastTarget.x || target.y != m_LastTarget.y || target.z != m_LastTarget.z;

				if (target_changed)
				{
					m_LastTarget = target;
					m_ActiveMode = 1;
					TASK::TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(ped.GetHandle(), veh.GetHandle(), target.x, target.y, target.z, s_DriveSpeed, s_DrivingStyle, 10.0f);
				}
			}
			else
			{
				if (m_ActiveMode != 2)
				{
					m_ActiveMode = 2;
					TASK::TASK_VEHICLE_DRIVE_WANDER(ped.GetHandle(), veh.GetHandle(), s_DriveSpeed, s_DrivingStyle);
				}
			}
		}

		virtual void OnDisable() override
		{
			StopDriving();
			m_ActiveMode = 0;
		}
	};

	static AutoDrive _AutoDrive{"autodrive", "自动驾驶", "自动开车前往导航点或随机漫游，按油门/刹车/转向接管即停止"};
}
