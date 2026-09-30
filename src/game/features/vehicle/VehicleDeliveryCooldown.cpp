#include "VehicleDeliveryCooldown.hpp"
#include "game/gta/ScriptGlobal.hpp"
#include "types/script/Timer.hpp"

namespace YimMenu::Features
{
	bool ResetVehicleDeliveryCooldown()
	{
		auto timer = ScriptGlobal(2686124).At(4373).At(260).At(7, 2);
		if (!timer.CanAccess())
			return false;

		timer.As<TIMER*>()->Destroy();
		return true;
	}
}
