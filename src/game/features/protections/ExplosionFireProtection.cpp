#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	// 防炸防火：自身与载具免疫爆炸/火焰伤害（持续修正 proofs），自动扑灭附身火焰。
	// 近身爆炸事件在 HandleNetGameEvent 中额外拦截（双保险）。
	class ExplosionFireProtection : public LoopedCommand
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

			ENTITY::SET_ENTITY_PROOFS(ped.GetHandle(), false, true, true, false, false, false, true, false);

			if (FIRE::IS_ENTITY_ON_FIRE(ped.GetHandle()))
			{
				FIRE::STOP_ENTITY_FIRE(ped.GetHandle());
				auto pos = ped.GetPosition();
				FIRE::STOP_FIRE_IN_RANGE(pos.x, pos.y, pos.z, 6.0f);
			}

			if (auto veh = ped.GetVehicle())
			{
				ENTITY::SET_ENTITY_PROOFS(veh.GetHandle(), false, true, true, false, false, false, true, false);

				if (FIRE::IS_ENTITY_ON_FIRE(veh.GetHandle()))
				{
					FIRE::STOP_ENTITY_FIRE(veh.GetHandle());
					auto pos = veh.GetPosition();
					FIRE::STOP_FIRE_IN_RANGE(pos.x, pos.y, pos.z, 8.0f);
				}
			}
		}
	};

	static ExplosionFireProtection _ExplosionFireProtection{"explosionfireprotection", "防炸防火", "自身与载具免疫爆炸/火焰伤害，自动扑灭附身火焰（近身爆炸事件双保险拦截）"};
}
