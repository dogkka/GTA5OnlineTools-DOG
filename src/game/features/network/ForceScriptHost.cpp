#include "core/commands/LoopedCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Scripts.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	class ForceScriptHost : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (!*Pointers.IsSessionStarted || SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_J) > 0)
				return;

			if (auto freemode = Scripts::FindScriptThread("freemode"_J))
			{
				Scripts::ForceScriptHost(freemode);
			}
		}
	};

	static ForceScriptHost _ForceScriptHost{"forcescripthost", "强制脚本主机", "强制让你成为自由模式脚本主机，可能会导致某些任务异常。"};
}
