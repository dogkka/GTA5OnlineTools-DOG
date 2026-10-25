#include "../rage/datBitBuffer.hpp"
#include "rlGamerHandle.hpp"

namespace rage
{
	void rlGamerHandle::Serialize(rage::datBitBuffer& buffer) const
	{
		buffer.Write<uint8_t>(m_Platform, 8);
		buffer.WriteInt64(m_RockstarId, 64);
		buffer.Write<uint8_t>(m_ProfileIndex, 8);
	}

	void rlGamerHandle::Deserialize(rage::datBitBuffer& buffer)
	{
		m_Platform = buffer.Read<uint8_t>(8);
		buffer.ReadInt64(&m_RockstarId, 64);
		m_ProfileIndex = buffer.Read<uint8_t>(8);
	}
}