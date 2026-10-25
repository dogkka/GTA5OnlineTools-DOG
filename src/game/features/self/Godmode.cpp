#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"

namespace YimMenu::Features
{
	class Godmode : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (!Self::GetPed())
				return;

			if (Self::GetPed().IsDead())
				Self::GetPed().SetInvincible(false);
			else
				Self::GetPed().SetInvincible(true);
		}

		virtual void OnDisable() override
		{
			if (!Self::GetPed())
				return;

			Self::GetPed().SetInvincible(false);
		}
	};

	static Godmode _Godmode{"godmode", "无敌模式", "阻止一切传入伤害"};
}
