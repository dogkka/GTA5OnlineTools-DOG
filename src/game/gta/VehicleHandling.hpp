#pragma once
#include <cstdint>

#include "game/gta/Vehicle.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::VehicleHandling
{
	// 实机验证（GTA5 Enhanced 2026-09 指针 / 2026-10 字段指纹）：
	//   CVehicle + 0x960 -> CHandlingData
	//   字段偏移以“调教六件套同时命中”的内存指纹确认（质量/加速/驱动力/极速/刹车/抓地）
	static constexpr uintptr_t kVehicleHandlingOffset = 0x960;
	static constexpr uintptr_t kMassOffset = 0x0C;
	static constexpr uintptr_t kAccelerationOffset = 0x4C;
	static constexpr uintptr_t kDriveForceOffset = 0x60;
	static constexpr uintptr_t kMaxFlatVelOffset = 0x64;
	static constexpr uintptr_t kMaxFlatVel2Offset = 0x68; // 相邻的第二速度字段（原厂值保留；估算时两者取大）
	static constexpr uintptr_t kBrakeForceOffset = 0x6C;
	static constexpr uintptr_t kTractionMaxOffset = 0x88;

	inline void* GetHandlingData(Vehicle veh)
	{
		if (!veh)
			return nullptr;

		void* veh_ptr = Pointers.HandleToPtr(veh.GetHandle());
		if (!veh_ptr)
			return nullptr;

		return *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(veh_ptr) + kVehicleHandlingOffset);
	}

	inline float ReadFloat(void* handling, uintptr_t offset)
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(handling) + offset);
	}

	inline void WriteFloat(void* handling, uintptr_t offset, float value)
	{
		*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(handling) + offset) = value;
	}
}
