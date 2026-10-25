#include "Invoker.hpp"

#include "Crossmap.hpp"
#include "core/hooking/DetourHook.hpp"
#include "game/hooks/Hooks.hpp"

#include "types/script/scrProgram.hpp"

namespace YimMenu
{
	void NativeInvoker::DefaultHandler(rage::scrNativeCallContext* ctx)
	{
		LOG(FATAL) << "Native handler not registered";
		ctx->SetReturnValue(0);
	}

	void NativeInvoker::CacheHandlers()
	{
		if (m_AreHandlersCached)
			return;

		memcpy(m_Handlers.data(), g_Crossmap.data(), sizeof(g_Crossmap));
		auto program = reinterpret_cast<rage::scrProgram*>(calloc(1, sizeof(rage::scrProgram)));
		program->m_NativeCount = m_Handlers.size();
		program->m_NativeEntrypoints = m_Handlers.data();
		BaseHook::Get<Hooks::Script::InitNativeTables, DetourHook<decltype(&Hooks::Script::InitNativeTables)>>()->Original()(program);
		free(program);
		m_AreHandlersCached = true;
	}
}