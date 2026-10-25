#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"

namespace YimMenu::Features
{
	class KeepPlayerClean : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (!Self::GetPed())
				return;

			Self::GetPed().ClearDamage();
		}
	};

	static KeepPlayerClean _KeepPlayerClean{"keepplayerclean", "保持角色清洁", "让角色始终保持干净，不沾血迹、污渍和水渍。"};
}
