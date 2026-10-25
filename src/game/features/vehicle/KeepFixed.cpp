#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"

namespace YimMenu::Features
{
	class KeepFixed : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (veh)
				veh.Fix();
		}
	};

	static KeepFixed _KeepFixed{"keepfixed", "保持载具完好", "让你的载具始终保持干净并自动修复"};
}
