#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class KeepEngineRunning : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (!veh)
				return;

			VEHICLE::SET_VEHICLE_ENGINE_ON(veh.GetHandle(), true, true, false);
		}
	};

	static KeepEngineRunning _KeepEngineRunning{"keepenginerunning", "引擎保持运转", "离开载具后引擎保持运转"};
}
