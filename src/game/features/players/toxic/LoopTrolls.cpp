#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Players.hpp"
#include "game/gta/Natives.hpp"
#include "types/fx/ExplosionType.hpp"

namespace YimMenu::Features
{
	class LoopExplode : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 120)
				return;

			m_Timer = 0;

			auto player = Players::GetSelected();
			if (!player.IsValid() || player.IsLocal())
				return;

			if (auto ped = player.GetPed())
				ped.Explode(ExplosionType::BLIMP, 900.f);
		}
	};

	class LoopRagdoll : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 90)
				return;

			m_Timer = 0;

			auto player = Players::GetSelected();
			if (!player.IsValid() || player.IsLocal())
				return;

			if (auto ped = player.GetPed(); ped && !ped.IsDead())
				PED::SET_PED_TO_RAGDOLL(ped.GetHandle(), 1000, 1000, 0, false, false, false);
		}
	};

	static LoopExplode _LoopExplode{"loopexplode", "循环爆炸", "持续引爆选中的玩家（每 2 秒一次）"};
	static LoopRagdoll _LoopRagdoll{"loopragdoll", "循环跌倒", "让选中的玩家反复摔倒"};
}
