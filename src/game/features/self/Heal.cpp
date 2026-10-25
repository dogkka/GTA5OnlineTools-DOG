#include "core/commands/Command.hpp"
#include "game/backend/Self.hpp"

namespace YimMenu::Features
{
	class Heal : Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			int maxHealth = Self::GetPed().GetMaxHealth();
			int maxArmour = Self::GetPlayer().GetMaxArmour();
			Self::GetPed().SetHealth(maxHealth);
			Self::GetPed().SetArmour(maxArmour);
		}
	};

	static Heal _Heal{"heal", "治疗", "恢复你的生命值与护甲。"};
}
