#include <iterator>
#include <vector>

#include "core/commands/ListCommand.hpp"
#include "core/commands/StringCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	// write a custom plate on the target's vehicle
	static StringCommand _TrollPlateText{"trollplatetext", "车牌文字", "要写在目标载具车牌上的文字"};

	class TrollPlate : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto text = _TrollPlateText.GetString();
			if (text.empty())
			{
				Notifications::Show("恶搞车牌", "请先填写车牌文字。", NotificationType::Warning);
				return;
			}

			if (text.length() > 8)
				text = text.substr(0, 8);

			auto veh = player.GetPed().GetVehicle();
			if (!veh)
			{
				Notifications::Show("恶搞车牌", "该玩家不在载具中。", NotificationType::Warning);
				return;
			}

			VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(veh.GetHandle(), text.c_str());
			Notifications::Show("恶搞车牌", "目标车牌已被改写。", NotificationType::Success);
		}
	};

	// force a paint color on the target's vehicle
	static ListCommand _ForceColorSelect{"forcecolorselect", "颜色", "要刷在目标载具上的颜色", {{0, "粉色"}, {1, "亮绿"}, {2, "血红"}, {3, "金色"}, {4, "随机彩虹"}}, 0};

	class ForceVehicleColor : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = player.GetPed().GetVehicle();
			if (!veh)
			{
				Notifications::Show("强制车色", "该玩家不在载具中。", NotificationType::Warning);
				return;
			}

			int r = 255, g = 105, b = 180;
			switch (_ForceColorSelect.GetState())
			{
			case 1: r = 50, g = 255, b = 50; break;
			case 2: r = 180, g = 0, b = 0; break;
			case 3: r = 255, g = 215, b = 0; break;
			case 4: r = rand() % 256, g = rand() % 256, b = rand() % 256; break;
			default: break;
			}

			VEHICLE::SET_VEHICLE_MOD_KIT(veh.GetHandle(), 0);
			VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh.GetHandle(), r, g, b);
			VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh.GetHandle(), r, g, b);

			Notifications::Show("强制车色", "目标载具已被重新喷漆。", NotificationType::Success);
		}
	};

	static TrollPlate _TrollPlate{"trollplate", "恶搞车牌", "把目标载具的车牌改成自定义文字"};
	static ForceVehicleColor _ForceVehicleColor{"forcecolor", "强制车色", "给目标载具喷上指定颜色"};
}
