#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"

namespace YimMenu::Features
{
	class LowerVehicleStance : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (veh)
			{
				veh.LowerStance(true);
			}
		}

		virtual void OnDisable() override
		{
			auto veh = Self::GetVehicle();
			if (veh)
			{
				veh.LowerStance(false);
			}
		}
	};

	static LowerVehicleStance _LowerVehicleStance{"lowervehiclestance", "降低载具姿态", "降低你当前载具的车身姿态，仅部分载具支持"};
}
