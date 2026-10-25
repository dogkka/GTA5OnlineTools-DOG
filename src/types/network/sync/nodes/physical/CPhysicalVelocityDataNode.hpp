#pragma once
#include <cstdint>
#include "types/network/sync/CProjectBaseSyncDataNode.hpp"

class CPhysicalVelocityDataNode : CSyncDataNodeFrequent
{
public:
	int32_t m_VelocityX;
	int32_t m_VelocityY;
	int32_t m_VelocityZ;
};
static_assert(sizeof(CPhysicalVelocityDataNode) == 0xD0);