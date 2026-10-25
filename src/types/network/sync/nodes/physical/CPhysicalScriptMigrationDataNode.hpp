#pragma once
#include <cstdint>
#include "types/network/sync/CProjectBaseSyncDataNode.hpp"

class CPhysicalScriptMigrationDataNode : CProjectBaseSyncDataNode
{
public:
	bool m_HasData;           // 0xC0
	int m_ScriptParticipants; // 0xC4
	uint16_t m_HostToken;     // 0xC8
};
static_assert(sizeof(CPhysicalScriptMigrationDataNode) == 0xD0);