#include "core/commands/Command.hpp"
#include "game/backend/Self.hpp"

namespace YimMenu::Features
{
	class RepairVehicle : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (Self::GetVehicle())
				Self::GetVehicle().Fix();
		}
	};

	static RepairVehicle _RepairVehicle{"repairvehicle", "修复载具", "修复你当前载具的所有损坏"};
}
