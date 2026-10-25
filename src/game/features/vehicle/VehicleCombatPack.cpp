#include "core/commands/BoolCommand.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static FloatCommand _VehRampImpulse{"vehrampimpulse", "撞飞力度", "斜坡/撞角把目标弹飞的力度倍数。", 0.1f, 10.0f, 1.5f};
	static BoolCommand _VehRampDamage{"vehrampdamage", "撞击伤害", "让斜坡/撞角直接对撞到的载具造成伤害。", true};

	// 载具特技对抗包：把当前载具改造成撞角/斜坡车（Impulse 同款原生）
	class VehicleCombatPack : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_LastVehicle = 0;

		void ApplyTo(int veh, bool enable)
		{
			if (!veh || !ENTITY::DOES_ENTITY_EXIST(veh))
				return;

			VEHICLE::SET_ALLOW_RAMMING_SOOP_OR_RAMP(veh, enable);
			VEHICLE::VEHICLE_SET_RAMP_AND_RAMMING_CARS_TAKE_DAMAGE(veh, enable);
			VEHICLE::VEHICLE_SET_ENABLE_RAMP_CAR_SIDE_IMPULSE(veh, enable);
			VEHICLE::VEHICLE_SET_ENABLE_NORMALISE_RAMP_CAR_VERTICAL_VELOCTIY(veh, enable);
			VEHICLE::SET_SCRIPT_RAMP_IMPULSE_SCALE(veh, enable ? _VehRampImpulse.GetState() : 1.0f);
		}

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (!veh)
			{
				if (m_LastVehicle)
				{
					ApplyTo(m_LastVehicle, false);
					m_LastVehicle = 0;
				}
				return;
			}

			if (veh.GetHandle() != m_LastVehicle)
			{
				if (m_LastVehicle)
					ApplyTo(m_LastVehicle, false);
				m_LastVehicle = veh.GetHandle();
			}

			ApplyTo(m_LastVehicle, true);
			VEHICLE::VEHICLE_SET_RAMP_AND_RAMMING_CARS_TAKE_DAMAGE(m_LastVehicle, _VehRampDamage.GetState());
			VEHICLE::SET_SCRIPT_RAMP_IMPULSE_SCALE(m_LastVehicle, _VehRampImpulse.GetState());
		}

		virtual void OnDisable() override
		{
			if (m_LastVehicle)
			{
				ApplyTo(m_LastVehicle, false);
				m_LastVehicle = 0;
			}
		}
	};

	static VehicleCombatPack _VehicleCombatPack{"vehcombatpack", "撞角/斜坡改造", "把当前载具改造成撞角车：推开/弹飞障碍与其他载具，可调力度与伤害。"};
}
