#pragma once
#include <cstdint>
#include "types/network/sync/CProjectBaseSyncDataNode.hpp"

class CPhysicalMigrationDataNode : CProjectBaseSyncDataNode
{
public:
	bool m_Unk;
};
static_assert(sizeof(CPhysicalMigrationDataNode) == 0xC8);