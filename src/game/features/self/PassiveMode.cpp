#include "core/commands/LoopedCommand.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class PassiveMode : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			NETWORK::NETWORK_SET_PLAYER_IS_PASSIVE(true);
		}

		virtual void OnDisable() override
		{
			NETWORK::NETWORK_SET_PLAYER_IS_PASSIVE(false);
		}
	};

	static PassiveMode _PassiveMode{"passivemode", "被动模式", "开启自己的被动模式"};
}
