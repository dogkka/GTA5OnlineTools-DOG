#pragma once
#include "../types.hpp"

struct GPBD_MissionName
{
	SCR_ARRAY<TEXT_LABEL_63, 32> MissionNames;

	static GPBD_MissionName* Get();
};
static_assert(sizeof(GPBD_MissionName) == 513 * 8);