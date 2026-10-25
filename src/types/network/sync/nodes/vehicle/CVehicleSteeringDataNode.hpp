#pragma once
#include <cstdint>
#include "types/network/sync/CProjectBaseSyncDataNode.hpp"

class CVehicleSteeringDataNode : CSyncDataNodeFrequent
{
public:
	float m_SteeringHandle; // 0xC0
	char pad[4];            // 0xC4
}; //Size: 0x00C8
static_assert(sizeof(CVehicleSteeringDataNode) == 0xC8);
