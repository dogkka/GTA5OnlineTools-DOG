#include "Packet.hpp"

#include "game/pointers/Pointers.hpp"
#include "types/network/CNetworkPlayerMgr.hpp"


namespace YimMenu
{
	Packet::Packet() :
	    m_Buffer(m_Data, sizeof(m_Data))
	{
	}

	void Packet::Send(uint32_t msg_id, bool reliable)
	{
		int flags = 0;
		if (reliable)
			flags |= 1;

		Pointers.QueuePacket((*Pointers.NetworkPlayerMgr)->m_NetConnectionMgr, msg_id, m_Data, (m_Buffer.m_CurBit + 7) >> 3, flags, nullptr);
	}
}