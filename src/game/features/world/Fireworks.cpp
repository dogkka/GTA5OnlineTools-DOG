#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "types/fx/ExplosionType.hpp"

namespace YimMenu::Features
{
	class Fireworks : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 35)
				return;

			m_Timer = 0;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			const auto pos = ped.GetPosition();

			for (int i = 0; i < 3; i++)
			{
				const float x = pos.x + (float)(rand() % 60 - 30);
				const float y = pos.y + (float)(rand() % 60 - 30);
				const float z = pos.z + 40.0f + (float)(rand() % 40);

				FIRE::ADD_EXPLOSION(x, y, z, (int)ExplosionType::FIREWORK, 1.0f, true, false, 1.0f, true);
			}
		}
	};

	static Fireworks _Fireworks{"fireworks", "烟花秀", "在头顶天空持续绽放烟花"};
}
