#pragma once
#include "fwExtensionContainer.hpp"
#include "fwRefAwareBase.hpp"

namespace rage
{
	class fwExtensibleBase : public fwRefAwareBase
	{
	public:
		fwExtensionContainer* m_ExtensionContainer; // 0x0010
		void* m_ExtensibleUnk;                      // 0x0018
	}; //Size: 0x0020
	static_assert(sizeof(fwExtensibleBase) == 0x20);

}