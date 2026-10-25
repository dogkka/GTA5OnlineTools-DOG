#include "core/commands/FloatCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	// 载具滑翔：原生 SET_GLIDER_ACTIVE（Enhanced 1.73 原生 6500）
	class VehicleGlide : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_LastVehicle = 0;

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (!veh)
			{
				if (m_LastVehicle && ENTITY::DOES_ENTITY_EXIST(m_LastVehicle))
					VEHICLE::SET_GLIDER_ACTIVE(m_LastVehicle, false);
				m_LastVehicle = 0;
				return;
			}

			if (m_LastVehicle && m_LastVehicle != veh.GetHandle() && ENTITY::DOES_ENTITY_EXIST(m_LastVehicle))
				VEHICLE::SET_GLIDER_ACTIVE(m_LastVehicle, false);

			m_LastVehicle = veh.GetHandle();
			VEHICLE::SET_GLIDER_ACTIVE(m_LastVehicle, true);
		}

		virtual void OnDisable() override
		{
			if (m_LastVehicle && ENTITY::DOES_ENTITY_EXIST(m_LastVehicle))
				VEHICLE::SET_GLIDER_ACTIVE(m_LastVehicle, false);
			m_LastVehicle = 0;
		}
	};

	// 垂直推进：火箭推进激活时持续提供垂直升力（模拟版）
	static FloatCommand _VehVerticalPower{"vehverticalpower", "垂直升力", "垂直推进每帧施加的升力大小。", 0.1f, 10.0f, 1.2f};

	class VehicleVerticalBoost : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (!veh)
				return;

			if (!VEHICLE::IS_ROCKET_BOOST_ACTIVE(veh.GetHandle()))
				return;

			ENTITY::APPLY_FORCE_TO_ENTITY(veh.GetHandle(), 1, 0.0f, 0.0f, _VehVerticalPower.GetState(), 0.0f, 0.0f, 0.0f, 0, true, false, true, false, true);
		}
	};

	static VehicleGlide _VehicleGlide{"vehglide", "载具滑翔", "给当前载具启用滑翔能力（原生实现）。"};
	static VehicleVerticalBoost _VehicleVerticalBoost{"vehvertical", "垂直推进", "火箭推进激活时额外提供垂直升力（模拟）。"};
}
