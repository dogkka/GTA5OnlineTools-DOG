#include "core/commands/LoopedCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static ListCommand _GravityLevel{"gravitylevel", "Gravity Level", "The world gravity level", {{0, "Normal"}, {1, "Low"}, {2, "Moon"}}, 0};

	class Gravity : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			MISC::SET_GRAVITY_LEVEL(_GravityLevel.GetState());
		}

		virtual void OnDisable() override
		{
			MISC::SET_GRAVITY_LEVEL(0);
		}
	};

	static Gravity _Gravity{"gravity", "Gravity", "Changes the world gravity"};
}
