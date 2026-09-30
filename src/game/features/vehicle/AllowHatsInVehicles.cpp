#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class AllowHatsInVehicles : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			PED::SET_PED_RESET_FLAG(Self::GetPed().GetHandle(), 337, true);
		}
	};

	static AllowHatsInVehicles _AllowHatsInVehicles{"allowhatsinvehicles", "载具内保留帽子", "允许你在载具内保留帽子或头部饰品。"};
};
