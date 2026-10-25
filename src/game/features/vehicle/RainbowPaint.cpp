#include <algorithm>

#include "core/commands/IntCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static IntCommand _RainbowPaintSpeed{"rainbowspeed", "彩虹速度", "彩虹变色的速度", 1, 25, 5};

	class RainbowPaint : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		float m_R = 255.0f;
		float m_G = 0.0f;
		float m_B = 0.0f;

		int m_LastVehicle = 0;

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (!veh)
				return;

			if (veh.GetHandle() != m_LastVehicle)
			{
				m_LastVehicle = veh.GetHandle();
				VEHICLE::SET_VEHICLE_MOD_KIT(veh.GetHandle(), 0);
			}

			const float speed = static_cast<float>(_RainbowPaintSpeed.GetState());

			if (m_R > 0.0f && m_B == 0.0f)
			{
				m_G += speed;
				m_R -= speed;
			}
			if (m_G > 0.0f && m_R == 0.0f)
			{
				m_B += speed;
				m_G -= speed;
			}
			if (m_B > 0.0f && m_G == 0.0f)
			{
				m_R += speed;
				m_B -= speed;
			}

			m_R = std::clamp(m_R, 0.0f, 255.0f);
			m_G = std::clamp(m_G, 0.0f, 255.0f);
			m_B = std::clamp(m_B, 0.0f, 255.0f);

			VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh.GetHandle(), (int)m_R, (int)m_G, (int)m_B);
			VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh.GetHandle(), (int)m_R, (int)m_G, (int)m_B);
		}
	};

	static RainbowPaint _RainbowPaint{"rainbowpaint", "彩虹车漆", "让载具车漆循环彩虹色"};
}
