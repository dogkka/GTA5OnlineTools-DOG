#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Pools.hpp"

namespace YimMenu::Features
{
	// everything nearby bounces like a trampoline world
	class BouncyWorld : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 30)
				return;

			m_Timer = 0;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			const auto center = ped.GetPosition();

			auto bounce = [&](int handle) {
				const auto pos = ENTITY::GET_ENTITY_COORDS(handle, true);
				const float dx = pos.x - center.x;
				const float dy = pos.y - center.y;

				if (dx * dx + dy * dy > 60.0f * 60.0f)
					return;

				if (!ENTITY::IS_ENTITY_IN_AIR(handle))
				{
					const auto vel = ENTITY::GET_ENTITY_VELOCITY(handle);
					ENTITY::SET_ENTITY_VELOCITY(handle, vel.x, vel.y, 11.0f);
				}
			};

			for (auto veh : Pools::GetVehicles())
				bounce(veh.GetHandle());

			for (auto iter : Pools::GetPeds())
				bounce(iter.GetHandle());
		}
	};

	// you personally become a bouncy ball
	class PersonalTrampoline : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto ped = Self::GetPed();
			if (!ped)
				return;

			const int handle = ped.GetHandle();
			if (!ENTITY::IS_ENTITY_IN_AIR(handle))
			{
				const auto vel = ENTITY::GET_ENTITY_VELOCITY(handle);
				ENTITY::SET_ENTITY_VELOCITY(handle, vel.x, vel.y, 10.0f);
			}
		}
	};

	static BouncyWorld _BouncyWorld{"bouncyworld", "跳跳世界", "附近的人和车都会像蹦床一样弹跳"};
	static PersonalTrampoline _PersonalTrampoline{"personaltrampoline", "个人蹦床", "自己落地就被弹起，永不停歇"};
}
