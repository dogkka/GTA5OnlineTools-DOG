#pragma once
#include "CEntity.hpp"
#include <cstddef>
namespace rage
{
	class netObject;
}

class CDynamicEntity : public CEntity
{
public:
	class rage::netObject* m_NetObject; //0x00D0
	char gapD8[16];
	uint64_t qwordE8;
};
static_assert(sizeof(CDynamicEntity) == 0xF0);