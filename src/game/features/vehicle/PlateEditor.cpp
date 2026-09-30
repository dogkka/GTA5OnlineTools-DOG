#include "core/commands/Command.hpp"
#include "core/commands/StringCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static StringCommand _PlateText{"platetext", "车牌文字", "要显示在车牌上的文字（最多 8 个字符）"};

	class PlateEditor : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto veh = Self::GetVehicle();
			if (!veh)
				return;

			auto text = _PlateText.GetString();
			if (text.empty())
				return;

			if (text.length() > 8)
				text = text.substr(0, 8);

			VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(veh.GetHandle(), text.c_str());
		}
	};

	static PlateEditor _PlateEditor{"plateeditor", "应用车牌", "把输入的字符应用到当前载具车牌"};
}
