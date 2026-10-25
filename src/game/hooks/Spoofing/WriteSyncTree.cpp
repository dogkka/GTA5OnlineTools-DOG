#include "core/hooking/DetourHook.hpp"
#include "game/backend/NodeHooks.hpp"
#include "game/hooks/Hooks.hpp"

namespace YimMenu::Hooks
{
	bool Hooks::Spoofing::WriteSyncTree(void* tree, int type, int flags, rage::netObject* object, rage::datBitBuffer* buffer, int unk, void* a6, std::uint8_t player_id, void* a8)
	{
		NodeHooks::SetSyncingObject(object);
		NodeHooks::SetSyncingPlayer(Player(player_id));

		auto ret = BaseHook::Get<Spoofing::WriteSyncTree, DetourHook<decltype(&Spoofing::WriteSyncTree)>>()->Original()(tree, type, flags, object, buffer, unk, a6, player_id, a8);

		NodeHooks::SetSyncingObject(nullptr);
		NodeHooks::SetSyncingPlayer(nullptr);

		return ret;
	}
}