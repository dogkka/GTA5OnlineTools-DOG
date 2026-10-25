#include "Pools.hpp"

#include "game/pointers/Pointers.hpp"
#include "types/rage/PoolEncryption.hpp"


namespace YimMenu
{
	rage::fwBasePool* GetPedPool()
	{
		if (Pointers.PedPool->m_IsSet)
		{
			uint64_t x = _rotl64(Pointers.PedPool->m_Second, 30);
			return reinterpret_cast<rage::fwBasePool*>(~_rotl64(_rotl64(x ^ Pointers.PedPool->m_First, 32), ((uint8_t)x & 0x1Fu) + 2));
		}

		return nullptr;
	}

	rage::fwBasePool* GetObjectPool()
	{
		if (Pointers.ObjectPool->m_IsSet)
		{
			uint64_t x = _rotl64(Pointers.ObjectPool->m_Second, 30);
			return reinterpret_cast<rage::fwBasePool*>(~_rotl64(_rotl64(x ^ Pointers.ObjectPool->m_First, 32), ((uint8_t)x & 0x1Fu) + 3));
		}

		return nullptr;
	}

	rage::fwVehiclePool* GetVehiclePool()
	{
		return **Pointers.VehiclePool;
	}
}