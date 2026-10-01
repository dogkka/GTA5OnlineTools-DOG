#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "types/blip/BlipSprite.hpp"
#include "types/pad/ControllerInputs.hpp"

namespace YimMenu::Features
{
	static ListCommand _AutoDriveMode{"autodrivemode", "自动驾驶模式", "选择驾驶风格：导航点 / 漫游（遵守交规）/ 横冲直撞（无视交规·疯狂超车·高速）", {{0, "停止"}, {1, "导航点"}, {2, "漫游"}, {3, "横冲直撞"}}, 0};

	class AutoDrive : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		static constexpr int s_LawDrivingStyle = 443;          // 遵守交规（礼让、等灯、慢速）
		static constexpr int s_RecklessDrivingStyle = 1081445; // 横冲直撞：匆忙 + 疯狂变道（社区实测最快风格族）
		static constexpr float s_LawSpeed = 20.0f;             // ≈72 km/h
		static constexpr float s_RecklessSpeed = 35.0f;        // ≈126 km/h

		int m_ActiveMode = 0;
		int m_ActiveStyle = 0;
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

			const bool reckless = (mode == 3);
			const int style     = reckless ? s_RecklessDrivingStyle : s_LawDrivingStyle;
			const float speed   = reckless ? s_RecklessSpeed : s_LawSpeed;

			if (mode == 1 || mode == 3)
			{
				Vector3 target{};
				if (!GetWaypoint(target))
				{
					if (mode == 1)
					{
						StopDriving();
						m_ActiveMode = 0;
						_AutoDriveMode.SetState(0);
						Notifications::Show("自动驾驶", "未设置导航点。", NotificationType::Warning);
						return;
					}

					// 横冲直撞且未设导航点：直接莽着漫游
					if (m_ActiveMode != 3 || m_ActiveStyle != style)
					{
						m_ActiveMode = 3;
						m_ActiveStyle = style;
						TASK::TASK_VEHICLE_DRIVE_WANDER(ped.GetHandle(), veh.GetHandle(), speed, style);
						Notifications::Show("自动驾驶", "横冲直撞：未设导航点，已开始疯狂漫游。", NotificationType::Info);
					}
					return;
				}

				const bool target_changed = m_ActiveMode != mode
				    || m_ActiveStyle != style
				    || target.x != m_LastTarget.x || target.y != m_LastTarget.y || target.z != m_LastTarget.z;

				if (target_changed)
				{
					m_LastTarget = target;
					m_ActiveMode = mode;
					m_ActiveStyle = style;
					TASK::TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(ped.GetHandle(), veh.GetHandle(), target.x, target.y, target.z, speed, style, 10.0f);
				}
			}
			else
			{
				if (m_ActiveMode != 2 || m_ActiveStyle != style)
				{
					m_ActiveMode = 2;
					m_ActiveStyle = style;
					TASK::TASK_VEHICLE_DRIVE_WANDER(ped.GetHandle(), veh.GetHandle(), speed, style);
				}
			}
		}

		virtual void OnDisable() override
		{
			StopDriving();
			m_ActiveMode = 0;
		}
	};

	static AutoDrive _AutoDrive{"autodrive", "自动驾驶", "自动开车前往导航点或随机漫游：导航点/漫游遵守交规，横冲直撞无视交规疯狂超车；按油门/刹车/转向接管即停止"};
}
