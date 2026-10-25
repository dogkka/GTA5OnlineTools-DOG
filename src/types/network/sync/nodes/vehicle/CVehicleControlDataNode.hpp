#pragma once
#include <cstdint>
#include "types/network/sync/CProjectBaseSyncDataNode.hpp"

class CVehicleControlDataNode : CSyncDataNodeFrequent
{
public:
	uint32_t m_NumWheels;
	uint32_t dwordC4;
	uint32_t m_BrakeControl;
	uint32_t dwordCC;
	uint32_t m_RoadNodeAddress;
	bool m_KersActive;
	bool m_BringingVehicleToHalt;
	float m_HaltDistance;
	bool m_ControlVerticalVelocity;
	bool m_HasSuspensionData;
	bool byteDE;
	float m_SuspensionHeights[10];
	bool byte108;
	bool byte109;
	bool byte10A;
	bool byte10B;
	bool byte10C;
	bool byte10D;
	bool byte10E;
	float float110;
	uint32_t dword114;
	char byte118;
	bool m_IsSubmarineCar;
	char gap11A[2];
	float m_RudderRotationX;
	float m_RudderRotationY;
	float m_RudderRotationZ;
	char byte128;
	char byte129;
	char pad[5];
};
static_assert(sizeof(CVehicleControlDataNode) == 0x130);
