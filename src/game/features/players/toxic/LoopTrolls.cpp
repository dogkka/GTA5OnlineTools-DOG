#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/backend/Players.hpp"
#include "game/gta/Natives.hpp"
#include "types/fx/ExplosionType.hpp"
#include "types/script/ScriptEvent.hpp"

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

	class LoopKill : public LoopedCommand
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

			if (auto ped = player.GetPed(); ped && !ped.IsDead())
				ped.Kill();
		}
	};

	static LoopExplode _LoopExplode{"loopexplode", "循环爆炸", "持续引爆选中的玩家（每 2 秒一次）"};
	static LoopRagdoll _LoopRagdoll{"loopragdoll", "循环跌倒", "让选中的玩家反复摔倒"};
	static LoopKill _LoopKill{"loopkill", "循环击杀", "持续击杀选中的玩家（每 2 秒一次）"};

	class LoopBounty : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 300)
				return;

			m_Timer = 0;

			auto player = Players::GetSelected();
			if (!player.IsValid() || player.IsLocal())
				return;

			const int id   = player.GetId();
			const int bits = 1 << id;

			int64_t args[23]{};
			args[0] = static_cast<int64_t>(ScriptEventIndex::Bounty);
			args[1] = Self::GetPlayer().GetId();
			args[2] = bits;
			args[3] = id;
			args[5] = 10000;

			SCRIPT::_SEND_TU_SCRIPT_EVENT_NEW(1, args, 23, bits, static_cast<Hash>(args[0]));
		}
	};

	static LoopBounty _LoopBounty{"loopbounty", "循环赏金", "持续给选中的玩家挂赏金（每 5 秒）"};
}
