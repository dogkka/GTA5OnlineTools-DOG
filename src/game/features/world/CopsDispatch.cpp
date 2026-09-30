#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Pools.hpp"

#include <game/gta/Natives.hpp>

namespace YimMenu::Features
{
	class CopsDispatch : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;
		virtual void OnTick() override
		{
			PLAYER::SET_DISPATCH_COPS_FOR_PLAYER(Self::GetPlayer().GetId(), FALSE);
		}

		virtual void OnDisable() override
		{
			PLAYER::SET_DISPATCH_COPS_FOR_PLAYER(Self::GetPlayer().GetId(), TRUE);
		}
	};

	static CopsDispatch _CopsDispatch{"CopsDispatch", "禁用警察调度", "当你被通缉时，不再生成警察。"};

}
