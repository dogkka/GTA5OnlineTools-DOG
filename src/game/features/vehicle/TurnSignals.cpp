#include "core/commands/LoopedCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static ListCommand _TurnSignalsMode{"turnsignalsmode", "Turn Signals Mode", "Which indicator to show", {{0, "Off"}, {1, "Left"}, {2, "Right"}}, 0};

	class TurnSignals : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (!veh)
				return;

			switch (_TurnSignalsMode.GetState())
			{
			case 1:
				VEHICLE::SET_VEHICLE_INDICATOR_LIGHTS(veh.GetHandle(), 0, true);
				VEHICLE::SET_VEHICLE_INDICATOR_LIGHTS(veh.GetHandle(), 1, false);
				break;
			case 2:
				VEHICLE::SET_VEHICLE_INDICATOR_LIGHTS(veh.GetHandle(), 1, true);
				VEHICLE::SET_VEHICLE_INDICATOR_LIGHTS(veh.GetHandle(), 0, false);
				break;
			default:
				VEHICLE::SET_VEHICLE_INDICATOR_LIGHTS(veh.GetHandle(), 0, false);
				VEHICLE::SET_VEHICLE_INDICATOR_LIGHTS(veh.GetHandle(), 1, false);
				break;
			}
		}

		virtual void OnDisable() override
		{
			auto veh = Self::GetVehicle();
			if (!veh)
				return;

			VEHICLE::SET_VEHICLE_INDICATOR_LIGHTS(veh.GetHandle(), 0, false);
			VEHICLE::SET_VEHICLE_INDICATOR_LIGHTS(veh.GetHandle(), 1, false);
		}
	};

	static TurnSignals _TurnSignals{"turnsignals", "Turn Signals", "Turns on the left or right indicator"};
}
