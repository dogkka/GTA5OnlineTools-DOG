#include "core/hooking/DetourHook.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "game/hooks/Hooks.hpp"
#include "game/gta/Scripts.hpp"

namespace YimMenu::Hooks
{
	bool Script::RunScriptThreads(int ops_to_execute)
	{
		auto res = BaseHook::Get<Script::RunScriptThreads, DetourHook<decltype(&RunScriptThreads)>>()->Original()(ops_to_execute); // let's see what happens if we run our stuff after the game code instead of before
		if (g_Running)
		{
			auto thread = Scripts::FindScriptThread("freemode"_J);
			if (!thread)
				thread = Scripts::FindScriptThread("main_persistent"_J);
			if (!thread)
				thread = Scripts::FindScriptThread("startup"_J);
			if (thread)
			{
				Scripts::RunAsScript(thread, [] {
					ScriptMgr::Tick();
				});
			}
		}
		return res;
	}
}