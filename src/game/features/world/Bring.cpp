#include "core/commands/Command.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Pools.hpp"

namespace YimMenu::Features
{
	class BringAllObjs : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto pos = Self::GetPed().GetPosition();
			for (auto obj : Pools::GetObjects())
			{
				if (obj)
				{
					obj.ForceControl();
					obj.SetPosition(pos);
				}
			}
		}
	};

	class BringAllPeds : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto pos = Self::GetPed().GetPosition();
			for (auto ped : Pools::GetPeds())
			{
				if (!ped.IsPlayer())
				{
					ped.ForceControl();
					ped.SetPosition(pos);
				}
			}
		}
	};

	class BringAllVehs : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto pos = Self::GetPed().GetPosition();
			for (auto veh : Pools::GetVehicles())
			{
				veh.ForceControl();
				veh.SetPosition(pos);
			}
		}
	};

	static BringAllObjs _BringAllObjs{"bringobjs", "传送全部物体到身边", "将游戏中的全部物体传送到你身边。"};
	static BringAllPeds _BringAllPeds{"bringpeds", "传送全部行人到身边", "将游戏中的全部行人传送到你身边。"};
	static BringAllVehs _BringAllVehs{"bringvehs", "传送全部载具到身边", "将游戏中的全部载具传送到你身边。"};
}
