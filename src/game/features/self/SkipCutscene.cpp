#include "core/commands/Command.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class SkipCutscene : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			CUTSCENE::STOP_CUTSCENE_IMMEDIATELY();
		}
	};

	static SkipCutscene _SkipCutscene{"skipcutscene", "跳过过场动画", "跳过当前正在播放的过场动画"};
}
