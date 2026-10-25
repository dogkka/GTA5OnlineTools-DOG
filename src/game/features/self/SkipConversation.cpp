#include "core/commands/Command.hpp"
#include "game/backend/Self.hpp"
#include <game/gta/Natives.hpp>

namespace YimMenu::Features
{
	class SkipConversation : Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			AUDIO::SKIP_TO_NEXT_SCRIPTED_CONVERSATION_LINE();
		}
	};

	static SkipConversation _SkipConversation{"skipconversation", "跳过对话", "跳过当前手机对话。"};
}
