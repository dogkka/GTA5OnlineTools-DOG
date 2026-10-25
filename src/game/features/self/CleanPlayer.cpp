#include "core/commands/Command.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class CleanPlayer : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto ped = Self::GetPed();
			if (!ped)
				return;

			PED::CLEAR_PED_BLOOD_DAMAGE(ped.GetHandle());
			PED::CLEAR_PED_WETNESS(ped.GetHandle());
			PED::CLEAR_PED_ENV_DIRT(ped.GetHandle());
			PED::CLEAR_PED_DECORATIONS(ped.GetHandle());
		}
	};

	static CleanPlayer _CleanPlayer{"cleanplayer", "清洁角色", "清除身上的血迹、污渍和装饰"};
}
