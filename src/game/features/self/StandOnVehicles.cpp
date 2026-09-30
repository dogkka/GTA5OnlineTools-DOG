#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class StandOnVehicles : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			PED::SET_PED_RESET_FLAG(Self::GetPed().GetHandle(), 274, true);
		}
	};

	static StandOnVehicles _StandOnVehicles{"standonvehicles", "站载具不摔", "站在载具顶部时不会进入布娃娃状态。"};
};
