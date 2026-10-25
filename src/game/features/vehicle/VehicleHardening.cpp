#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class NoCollision : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_LastVehicle = 0;

		void Restore(int handle)
		{
			if (handle != 0 && ENTITY::DOES_ENTITY_EXIST(handle))
				ENTITY::SET_ENTITY_COLLISION(handle, true, true);
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

			ENTITY::SET_ENTITY_COLLISION(veh.GetHandle(), false, false);
		}

		virtual void OnDisable() override
		{
			Restore(m_LastVehicle);
			m_LastVehicle = 0;
		}
	};

	class BulletproofTyres : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (!veh)
				return;

			VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh.GetHandle(), false);
			VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(veh.GetHandle(), false);
		}
	};

	static NoCollision _NoCollision{"vehnocol", "无碰撞", "载具穿墙模式（本地效果）"};
	static BulletproofTyres _BulletproofTyres{"bulletprooftyres", "防爆胎", "车轮不会被打破或脱落"};
}
