#pragma once
#include "Player.hpp"

#include "types/rage/datBitBuffer.hpp"
#include "types/network/netMessage.hpp"


namespace YimMenu
{
	class Packet
	{
		char m_Data[0x400]{};
		rage::datBitBuffer m_Buffer;

	public:
		Packet();
		void Send(uint32_t msg_id, bool reliable = true);

		inline operator rage::datBitBuffer&()
		{
			return m_Buffer;
		}

		inline rage::datBitBuffer& GetBuffer()
		{
			return m_Buffer;
		}

		inline void WriteMessageHeader(rage::netMessage::Type message)
		{
			GetBuffer().Write<int>(0x3246, 14);
			if ((int)message > 0xFF)
			{
				GetBuffer().Write<bool>(true, 1);
				GetBuffer().Write<rage::netMessage::Type>(message, 16);
			}
			else
			{
				GetBuffer().Write<bool>(false, 1);
				GetBuffer().Write<rage::netMessage::Type>(message, 8);
			}
		}
	};

}