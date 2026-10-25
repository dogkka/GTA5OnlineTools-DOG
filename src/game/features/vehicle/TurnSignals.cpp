#include "core/commands/LoopedCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static ListCommand _TurnSignalsMode{"turnsignalsmode", "转向灯模式", "选择要显示的指示灯", {{0, "关闭"}, {1, "左转"}, {2, "右转"}}, 0};

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

	static TurnSignals _TurnSignals{"turnsignals", "转向灯", "打开左转或右转指示灯"};
}
