#pragma once
#include <functional>

#include "core/backend/FiberPool.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	// 载具整蛊通用工具（2026-10-06）
	//
	// 背景：对“正在被玩家驾驶”的载具，本地调用 SET_VEHICLE_* 只会改本机副本，车主端的同步会立刻覆盖，
	// 所以旧实现看起来“不生效”。这里统一处理：
	//   1) 先在后台上抢控制权（无人载具一般 1~2 帧就能拿到，拿到后写入会经同步生效）；
	//   2) 拿不到（目标正在驾驶）时走伤害事件兜底（爆炸伤害事件，对未开防护的目标有效）。
	//
	// 伤害事件参数照抄 Entity::Kill 的已验证组合（WEAPON_EXPLOSION + 相同 event bits）。
	inline void BlastRemoteVehicle(int handle, float damage)
	{
		auto self = Self::GetPed();
		void* veh_ptr = Pointers.HandleToPtr(handle);
		if (!self || !veh_ptr)
			return;

		const auto coords = ENTITY::GET_ENTITY_COORDS(handle, true);
		rage::fvector3 pos{coords.x, coords.y, coords.z};
		Pointers.TriggerWeaponDamageEvent(self.GetPointer<CEntity*>(),
		    static_cast<CEntity*>(veh_ptr),
		    &pos,
		    0,
		    true,
		    "WEAPON_EXPLOSION"_J,
		    damage,
		    2,
		    0,
		    (1 << 4) | 0x80000,
		    0,
		    0,
		    0,
		    false,
		    false,
		    true,
		    true,
		    nullptr);
	}

	// on_control(handle)：拿到控制权后要执行的操作；blast_damage > 0 时作为拿不到控制权的兜底
	inline void ApplyVehicleTroll(int handle, std::function<void(int)> on_control, float blast_damage = 0.0f)
	{
		if (handle == 0)
			return;

		FiberPool::Push([handle, on_control = std::move(on_control), blast_damage] {
			bool controlled = false;
			for (int i = 0; i < 90; i++) // 约 1.5 秒
			{
				if (!ENTITY::DOES_ENTITY_EXIST(handle))
					return;

				if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(handle))
				{
					controlled = true;
					break;
				}

				NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(handle);
				ScriptMgr::Yield();
			}

			if (controlled)
			{
				if (on_control)
					on_control(handle);
			}
			else if (blast_damage > 0.0f)
			{
				BlastRemoteVehicle(handle, blast_damage);
			}
		});
	}
}
