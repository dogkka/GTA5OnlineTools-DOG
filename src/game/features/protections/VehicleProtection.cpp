#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Vehicle.hpp"

namespace YimMenu::Features
{
	// 载具防篡改：车窗被砸自动修复；载具被恶意删除时按原位置重刷并坐回。
	class VehicleProtection : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;
		int m_LastVehicle = 0;
		int m_MissingTicks = 0;
		std::uint32_t m_LastModel = 0;
		rage::fvector3 m_LastPos{};
		float m_LastHeading = 0.0f;

		virtual void OnTick() override
		{
			if (++m_Timer < 15)
				return;

			m_Timer = 0;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			if (auto veh = ped.GetVehicle())
			{
				// 车窗自动修复
				for (int window = 0; window < 8; window++)
				{
					if (!VEHICLE::IS_VEHICLE_WINDOW_INTACT(veh.GetHandle(), window))
						VEHICLE::FIX_VEHICLE_WINDOW(veh.GetHandle(), window);
				}

				m_LastVehicle  = veh.GetHandle();
				m_LastModel    = ENTITY::GET_ENTITY_MODEL(veh.GetHandle());
				m_LastPos      = veh.GetPosition();
				m_LastHeading  = ENTITY::GET_ENTITY_HEADING(veh.GetHandle());
				m_MissingTicks = 0;
				return;
			}

			if (m_LastVehicle != 0 && !ENTITY::DOES_ENTITY_EXIST(m_LastVehicle))
			{
				// 载具在"仍在车上"期间消失 → 判定被删，重刷
				if (++m_MissingTicks >= 10)
				{
					m_MissingTicks = 0;

					auto newVeh = Vehicle::Create(m_LastModel, m_LastPos, m_LastHeading);
					if (newVeh)
					{
						PED::SET_PED_INTO_VEHICLE(ped.GetHandle(), newVeh.GetHandle(), -1);
						m_LastVehicle = newVeh.GetHandle();
						Notifications::Show("载具防护", "载具被删除，已按原位置重新刷出并坐回。", NotificationType::Warning);
					}
				}
			}
			else
			{
				// 正常下车（车还在）→ 停止追踪
				m_LastVehicle  = 0;
				m_MissingTicks = 0;
			}
		}
	};

	static VehicleProtection _VehicleProtection{"vehicleprotection", "载具防篡改", "车窗被砸自动修复；载具被恶意删除时自动按原位重刷"};
}
