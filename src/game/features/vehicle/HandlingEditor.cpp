#include "core/commands/BoolCommand.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/VehicleHandling.hpp"

namespace YimMenu::Features
{
	// 载具操控字段偏移见 game/gta/VehicleHandling.hpp（CVehicle + 0x960 -> CHandlingData）
	static constexpr uintptr_t OFF_DEFORMATION = 0xF8;
	static constexpr uintptr_t OFF_ENGINE_DMG  = 0xFC;

	static FloatCommand _MassMult{"handlingmass", "重量倍率", "质量倍率，越小越轻快", 0.05f, 5.0f, 0.6f};
	static FloatCommand _AccelMult{"handlingaccel", "动力倍率", "加速与驱动力倍率", 0.5f, 20.0f, 1.5f};
	static FloatCommand _TopSpeedMult{"handlingtopspeed", "极速倍率", "最高速度倍率", 0.5f, 20.0f, 1.5f};
	static FloatCommand _TopSpeedOverride{"handlingtopspeedval", "极速直设", "0=按倍率计算；大于 0 时直接设为该速度（约 150 ≈ 540km/h）", 0.0f, 300.0f, 0.0f};
	static FloatCommand _BrakeMult{"handlingbrake", "刹车倍率", "制动力倍率", 0.5f, 20.0f, 1.5f};
	static FloatCommand _TractionMult{"handlingtraction", "抓地倍率", "轮胎抓地力倍率", 0.5f, 20.0f, 1.3f};
	static BoolCommand _NoDeform{"handlingnodeform", "防变形", "碰撞时车身不变形", false};
	static BoolCommand _EngineImmune{"handlingengineimmune", "引擎免疫", "碰撞不损伤引擎", false};

	class HandlingEditor : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		void* m_Handling = nullptr;
		float m_OgMass = 0.0f, m_OgAccel = 0.0f, m_OgForce = 0.0f;
		float m_OgMaxFlat = 0.0f, m_OgBrake = 0.0f, m_OgTraction = 0.0f;
		float m_OgDeform = 0.0f, m_OgEngineDmg = 0.0f;

		static void* GetHandlingData()
		{
			return VehicleHandling::GetHandlingData(Self::GetVehicle());
		}

		static float ReadFloat(void* base, uintptr_t off)
		{
			return VehicleHandling::ReadFloat(base, off);
		}

		static void WriteFloat(void* base, uintptr_t off, float value)
		{
			VehicleHandling::WriteFloat(base, off, value);
		}

		void Restore()
		{
			if (!m_Handling)
				return;

			WriteFloat(m_Handling, VehicleHandling::kMassOffset, m_OgMass);
			WriteFloat(m_Handling, VehicleHandling::kAccelerationOffset, m_OgAccel);
			WriteFloat(m_Handling, VehicleHandling::kDriveForceOffset, m_OgForce);
			WriteFloat(m_Handling, VehicleHandling::kMaxFlatVelOffset, m_OgMaxFlat);
			WriteFloat(m_Handling, VehicleHandling::kBrakeForceOffset, m_OgBrake);
			WriteFloat(m_Handling, VehicleHandling::kTractionMaxOffset, m_OgTraction);
			WriteFloat(m_Handling, OFF_DEFORMATION, m_OgDeform);
			WriteFloat(m_Handling, OFF_ENGINE_DMG, m_OgEngineDmg);
			m_Handling = nullptr;
		}

		virtual void OnTick() override
		{
			void* handling = GetHandlingData();
			if (!handling)
			{
				Restore();
				return;
			}

			if (handling != m_Handling)
			{
				Restore();
				m_Handling = handling;

				m_OgMass      = ReadFloat(handling, VehicleHandling::kMassOffset);
				m_OgAccel     = ReadFloat(handling, VehicleHandling::kAccelerationOffset);
				m_OgForce     = ReadFloat(handling, VehicleHandling::kDriveForceOffset);
				m_OgMaxFlat   = ReadFloat(handling, VehicleHandling::kMaxFlatVelOffset);
				m_OgBrake     = ReadFloat(handling, VehicleHandling::kBrakeForceOffset);
				m_OgTraction  = ReadFloat(handling, VehicleHandling::kTractionMaxOffset);
				m_OgDeform    = ReadFloat(handling, OFF_DEFORMATION);
				m_OgEngineDmg = ReadFloat(handling, OFF_ENGINE_DMG);
			}

			WriteFloat(handling, VehicleHandling::kMassOffset, m_OgMass * _MassMult.GetState());
			WriteFloat(handling, VehicleHandling::kAccelerationOffset, m_OgAccel * _AccelMult.GetState());
			WriteFloat(handling, VehicleHandling::kDriveForceOffset, m_OgForce * _AccelMult.GetState());
			const float top_speed_override = _TopSpeedOverride.GetState();
			WriteFloat(handling, VehicleHandling::kMaxFlatVelOffset, top_speed_override > 0.0f ? top_speed_override : m_OgMaxFlat * _TopSpeedMult.GetState());
			WriteFloat(handling, VehicleHandling::kBrakeForceOffset, m_OgBrake * _BrakeMult.GetState());
			WriteFloat(handling, VehicleHandling::kTractionMaxOffset, m_OgTraction * _TractionMult.GetState());
			WriteFloat(handling, OFF_DEFORMATION, _NoDeform.GetState() ? 0.0f : m_OgDeform);
			WriteFloat(handling, OFF_ENGINE_DMG, _EngineImmune.GetState() ? 0.0f : m_OgEngineDmg);
		}

		virtual void OnDisable() override
		{
			Restore();
		}
	};

	static HandlingEditor _HandlingEditor{"handlingeditor", "载具调校", "自定义当前载具的操控性能（实时生效）"};
}
