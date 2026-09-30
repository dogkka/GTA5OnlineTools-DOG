#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class DriveOnWater : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (!veh)
				return;

			auto pos          = veh.GetPosition();
			float waterHeight = 0.0f;

			if (!WATER::GET_WATER_HEIGHT_NO_WAVES(pos.x, pos.y, pos.z, &waterHeight))
				return;

			VEHICLE::SET_VEHICLE_ENGINE_ON(veh.GetHandle(), true, true, false);

			if (pos.z < waterHeight + 0.2f)
				ENTITY::SET_ENTITY_COORDS(veh.GetHandle(), pos.x, pos.y, waterHeight + 0.2f, false, false, false, false);
		}
	};

	static DriveOnWater _DriveOnWater{"driveonwater", "Drive On Water", "Keeps your vehicle floating on the water surface"};
}
