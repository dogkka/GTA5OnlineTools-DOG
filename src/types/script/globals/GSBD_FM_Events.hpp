#pragma once
#include "../types.hpp"

// TODO
struct GSBD_FM_Events
{
	uint64_t PAD_0000[8];
	SCR_INT AntiReplayValue1;
	SCR_INT AntiReplayValue2;

	static GSBD_FM_Events* Get();
};
//static_assert(sizeof(GSBD) == 1498 * 8);