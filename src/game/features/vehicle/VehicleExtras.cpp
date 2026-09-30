#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class DisableSiren : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (!veh)
				return;

			VEHICLE::SET_VEHICLE_SIREN(veh.GetHandle(), false);
		}
	};

	class VehicleInvisibility : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_LastVehicle = 0;

		void Restore(int handle)
		{
			if (handle != 0 && ENTITY::DOES_ENTITY_EXIST(handle))
				ENTITY::SET_ENTITY_VISIBLE(handle, true, false);
		}

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (!veh)
			{
				Restore(m_LastVehicle);
				m_LastVehicle = 0;
				return;
			}

			if (veh.GetHandle() != m_LastVehicle)
			{
				Restore(m_LastVehicle);
				m_LastVehicle = veh.GetHandle();
			}

			ENTITY::SET_ENTITY_VISIBLE(veh.GetHandle(), false, false);
		}

		virtual void OnDisable() override
		{
			Restore(m_LastVehicle);
			m_LastVehicle = 0;
		}
	};

	class VehicleStrong : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (!veh)
				return;

			VEHICLE::SET_VEHICLE_STRONG(veh.GetHandle(), true);
		}
	};

	static DisableSiren _DisableSiren{"disablesiren", "静音警报", "关闭当前载具的警报器"};
	static VehicleInvisibility _VehicleInvisibility{"vehicleinvis", "载具隐形", "当前载具对本地不可见"};
	static VehicleStrong _VehicleStrong{"vehiclestrong", "防撞车身", "载具碰撞不易变形"};
}
