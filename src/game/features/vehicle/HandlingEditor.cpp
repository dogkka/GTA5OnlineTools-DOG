#include "core/commands/BoolCommand.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	// verified live on GTA5 Enhanced (2026-09):
	// CVehicle + 0x960 -> CHandlingData (non-legacy offset!)
	static constexpr uintptr_t OFF_VEHICLE_HANDLING = 0x960;

	static constexpr uintptr_t OFF_MASS         = 0x0C;
	static constexpr uintptr_t OFF_ACCELERATION = 0x4C;
	static constexpr uintptr_t OFF_DRIVE_FORCE  = 0x60;
	static constexpr uintptr_t OFF_MAX_FLAT_VEL = 0x64;
	static constexpr uintptr_t OFF_BRAKE_FORCE  = 0x6C;
	static constexpr uintptr_t OFF_TRACTION_MAX = 0x88;
	static constexpr uintptr_t OFF_DEFORMATION  = 0xF8;
	static constexpr uintptr_t OFF_ENGINE_DMG   = 0xFC;

	static BoolCommand _CustomHandling{"handlingedit", "载具调校", "自定义当前载具的操控性能（实时生效）"};
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
			auto veh = Self::GetVehicle();
			if (!veh)
				return nullptr;

			void* veh_ptr = Pointers.HandleToPtr(veh.GetHandle());
			if (!veh_ptr)
				return nullptr;

			return *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(veh_ptr) + OFF_VEHICLE_HANDLING);
		}

		static float ReadFloat(void* base, uintptr_t off)
		{
			return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(base) + off);
		}

		static void WriteFloat(void* base, uintptr_t off, float value)
		{
			*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(base) + off) = value;
		}

		void Restore()
		{
			if (!m_Handling)
				return;

			WriteFloat(m_Handling, OFF_MASS, m_OgMass);
			WriteFloat(m_Handling, OFF_ACCELERATION, m_OgAccel);
			WriteFloat(m_Handling, OFF_DRIVE_FORCE, m_OgForce);
			WriteFloat(m_Handling, OFF_MAX_FLAT_VEL, m_OgMaxFlat);
			WriteFloat(m_Handling, OFF_BRAKE_FORCE, m_OgBrake);
			WriteFloat(m_Handling, OFF_TRACTION_MAX, m_OgTraction);
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

				m_OgMass      = ReadFloat(handling, OFF_MASS);
				m_OgAccel     = ReadFloat(handling, OFF_ACCELERATION);
				m_OgForce     = ReadFloat(handling, OFF_DRIVE_FORCE);
				m_OgMaxFlat   = ReadFloat(handling, OFF_MAX_FLAT_VEL);
				m_OgBrake     = ReadFloat(handling, OFF_BRAKE_FORCE);
				m_OgTraction  = ReadFloat(handling, OFF_TRACTION_MAX);
				m_OgDeform    = ReadFloat(handling, OFF_DEFORMATION);
				m_OgEngineDmg = ReadFloat(handling, OFF_ENGINE_DMG);
			}

			WriteFloat(handling, OFF_MASS, m_OgMass * _MassMult.GetState());
			WriteFloat(handling, OFF_ACCELERATION, m_OgAccel * _AccelMult.GetState());
			WriteFloat(handling, OFF_DRIVE_FORCE, m_OgForce * _AccelMult.GetState());
			const float top_speed_override = _TopSpeedOverride.GetState();
			WriteFloat(handling, OFF_MAX_FLAT_VEL, top_speed_override > 0.0f ? top_speed_override : m_OgMaxFlat * _TopSpeedMult.GetState());
			WriteFloat(handling, OFF_BRAKE_FORCE, m_OgBrake * _BrakeMult.GetState());
			WriteFloat(handling, OFF_TRACTION_MAX, m_OgTraction * _TractionMult.GetState());
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
