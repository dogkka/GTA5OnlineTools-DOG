#include "core/commands/LoopedCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static ListCommand _GravityLevel{"gravitylevel", "重力等级", "世界重力等级", {{0, "正常"}, {1, "低"}, {2, "月球"}}, 0};

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

	static Gravity _Gravity{"gravity", "重力", "修改世界重力"};
}
