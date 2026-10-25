#include "core/hooking/DetourHook.hpp"
#include "game/backend/Players.hpp"
#include "game/hooks/Hooks.hpp"

namespace YimMenu::Hooks
{
	void Info::NetworkPlayerMgrShutdown(CNetworkPlayerMgr* mgr)
	{
		if (!g_Running)
			return BaseHook::Get<Info::NetworkPlayerMgrShutdown, DetourHook<decltype(&Info::NetworkPlayerMgrShutdown)>>()->Original()(mgr);

		Players::Shutdown();
		BaseHook::Get<Info::NetworkPlayerMgrShutdown, DetourHook<decltype(&Info::NetworkPlayerMgrShutdown)>>()->Original()(mgr);
	}
}