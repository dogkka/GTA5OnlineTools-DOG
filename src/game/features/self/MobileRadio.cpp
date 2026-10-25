#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class MobileRadio : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;


		virtual void OnTick() override
		{
			AUDIO::SET_MOBILE_PHONE_RADIO_STATE(true);
			AUDIO::SET_MOBILE_RADIO_ENABLED_DURING_GAMEPLAY(true);
		}

		virtual void OnDisable() override
		{
			AUDIO::SET_MOBILE_PHONE_RADIO_STATE(false);
			AUDIO::SET_MOBILE_RADIO_ENABLED_DURING_GAMEPLAY(false);
		}
	};

	static MobileRadio _MobileRadio{"mobileradio", "步行电台", "允许你在步行时通过热键 Q 收听任意电台"};
}
