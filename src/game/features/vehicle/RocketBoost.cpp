#include "core/commands/LoopedCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/Self.hpp"

namespace YimMenu::Features
{
	static std::vector<std::pair<int, const char*>> g_BoostBehavior = {
	    {0, "无限"},
	    {1, "瞬间充能"}};

	static ListCommand _BoostBehavior{"boostbehavior", "推进器行为", "火箭推进器的目标行为", g_BoostBehavior, 0};

	class ModifyRocketBoost : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (auto vehicle = Self::GetVehicle())
			{
				if (vehicle.SupportsBoost())
				{
					auto desiredBehavior = _BoostBehavior.GetState();

					/* Infinite */
					if (desiredBehavior == 0)
						vehicle.SetBoostCharge();

					/* Instant charge */
					else if (desiredBehavior == 1)
						if (!vehicle.IsBoostActive())
							vehicle.SetBoostCharge();
				}
			}
		}
	};

	static ModifyRocketBoost _ModifyRocketBoost{"modifyboostbehavior", "修改推进器行为", "修改支持火箭推进器的载具行为"};
};
