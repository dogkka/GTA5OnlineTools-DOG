#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class SuperJump : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			MISC::SET_SUPER_JUMP_THIS_FRAME(Self::GetPlayer().GetId());
		}
	};

	static SuperJump _SuperJump{"superjump", "超级跳跃", "跳得比平常更高"};
}
