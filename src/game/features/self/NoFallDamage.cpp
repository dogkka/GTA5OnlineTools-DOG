#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class NoFallDamage : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto ped = Self::GetPed();
			if (!ped)
				return;

			PED::SET_DISABLE_HIGH_FALL_DEATH(ped.GetHandle(), true);
		}

		virtual void OnDisable() override
		{
			auto ped = Self::GetPed();
			if (!ped)
				return;

			PED::SET_DISABLE_HIGH_FALL_DEATH(ped.GetHandle(), false);
		}
	};

	static NoFallDamage _NoFallDamage{"nofalldamage", "无摔伤", "从高处坠落不会死亡"};
}
