#pragma once
#include "scriptIdBase.hpp"

namespace rage
{
	class scriptId : public scriptIdBase
	{
	public:
		std::uint32_t m_Hash; // 0x08
		char m_Name[32];      // 0x0C
	};
	static_assert(sizeof(scriptId) == 0x30);
}