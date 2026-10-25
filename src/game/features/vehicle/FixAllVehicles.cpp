#include "core/commands/Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/localization/Localization.hpp"
#include "game/backend/PersonalVehicles.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	class FixAllVehicles : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			int count = 0;
			for (const auto& it : PersonalVehicles::GetPersonalVehicles())
			{
				const auto& personalVeh = it.second;
				if (personalVeh->Repair())
					count++;
			}

			if (count > 0)
				Notifications::Show(Localization::Translate("修复所有载具"), std::format("已修复 {} 辆载具。", count), NotificationType::Success);
			else
				Notifications::Show(Localization::Translate("修复所有载具"), Localization::Translate("没有可修复的载具。"));
		}
	};

	static FixAllVehicles _FixAllVehicles{"fixallvehicles", "修复全部个人载具", "修复所有已损毁的个人载具。"};
}
