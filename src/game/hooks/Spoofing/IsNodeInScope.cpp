#include "core/hooking/DetourHook.hpp"
#include "game/backend/NodeHooks.hpp"
#include "game/hooks/Hooks.hpp"

namespace YimMenu::Hooks
{
	bool Hooks::Spoofing::IsNodeInScope(void* node, void* a2, std::uint8_t playerId, int flags, unsigned int a5, int a6)
	{
		if (NodeHooks::ShouldForceInScope(reinterpret_cast<CProjectBaseSyncDataNode*>(node)))
			return true;

		return BaseHook::Get<Spoofing::IsNodeInScope, DetourHook<decltype(&Spoofing::IsNodeInScope)>>()->Original()(node, a2, playerId, flags, a5, a6);
	}
}