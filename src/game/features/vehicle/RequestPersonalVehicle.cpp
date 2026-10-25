#include "core/commands/Command.hpp"
#include "game/backend/PersonalVehicles.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu
{
	class RequestPersonalVehicle : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			PersonalVehicles::GetCurrent()->Request();
		}
	};

	static RequestPersonalVehicle _RequestPersonalVehicle{"requestpv", "呼叫个人载具", "呼叫当前绑定的个人载具。"};
}
