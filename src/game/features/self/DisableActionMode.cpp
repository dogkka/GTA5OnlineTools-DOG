#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class DisableActionMode : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			PED::SET_PED_RESET_FLAG(Self::GetPed().GetHandle(), 200, true);
		}
	};

	static DisableActionMode _DisableActionMode{"disableactionmode", "禁用动作模式", "禁用战斗时那种别扭的角色动作模式。"};
};
