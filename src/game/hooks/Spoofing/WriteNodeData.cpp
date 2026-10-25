#include "core/commands/BoolCommand.hpp"
#include "core/hooking/DetourHook.hpp"
#include "game/backend/NodeHooks.hpp"
#include "game/backend/Self.hpp"
#include "game/hooks/Hooks.hpp"
#include "game/gta/Nodes.hpp"

#include "types/rage/datBitBuffer.hpp"

namespace YimMenu::Hooks
{
	void Spoofing::WriteNodeData(CProjectBaseSyncDataNode* node, rage::netObject* object, rage::datBitBuffer* buffer, void* logger, bool update)
	{
		auto orig_buffer = *buffer;
		bool node_dirty = false;
		Nodes::Init();

		BaseHook::Get<Spoofing::WriteNodeData, DetourHook<decltype(&Spoofing::WriteNodeData)>>()->Original()(node, object, buffer, logger, update);

		if (NodeHooks::ModifyNodeData(node, object))
			node_dirty = true;

		if (node_dirty)
		{
			*buffer = orig_buffer;
			BaseHook::Get<Spoofing::WriteNodeData, DetourHook<decltype(&Spoofing::WriteNodeData)>>()->Original()(node, object, buffer, logger, false);
		}
	}
}