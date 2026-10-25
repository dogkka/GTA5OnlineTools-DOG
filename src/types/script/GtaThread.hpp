#pragma once
#include "scrThread.hpp"

class CGameScriptHandler;
class CGameScriptHandlerNetComponent;

class GtaThread : public rage::scrThread
{
public:
	CGameScriptHandler* m_ScriptHandler;            // 0x198
	CGameScriptHandlerNetComponent* m_NetComponent; // 0x1A0
	std::uint32_t m_ScriptHash2;                    // 0x1A8
};
// static_assert(sizeof(GtaThread) == 0x1E0); // TODO