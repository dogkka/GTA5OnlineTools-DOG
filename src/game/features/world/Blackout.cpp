#include "core/commands/LoopedCommand.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class Blackout : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			GRAPHICS::SET_ARTIFICIAL_LIGHTS_STATE(false);
		}

		virtual void OnDisable() override
		{
			GRAPHICS::SET_ARTIFICIAL_LIGHTS_STATE(true);
		}
	};

	static Blackout _Blackout{"blackout", "全城断电", "关闭世界中所有人工照明"};
}
