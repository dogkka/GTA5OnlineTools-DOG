#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Players.hpp"
#include "game/gta/Natives.hpp"
#include "types/fx/ExplosionType.hpp"

namespace YimMenu::Features
{
	// harmless fireworks constantly going off on the selected player
	class TargetFireworks : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 30)
				return;

			m_Timer = 0;

			auto player = Players::GetSelected();
			if (!player.IsValid() || player.IsLocal())
				return;

			auto target = player.GetPed();
			if (!target)
				return;

			const auto pos = target.GetPosition();

			for (int i = 0; i < 2; i++)
			{
				const float x = pos.x + (float)(rand() % 7 - 3);
				const float y = pos.y + (float)(rand() % 7 - 3);
				const float z = pos.z + 2.0f + (float)(rand() % 5);

				FIRE::ADD_EXPLOSION(x, y, z, (int)ExplosionType::FIREWORK, 1.0f, true, false, 1.0f, true);
			}
		}
	};

	static TargetFireworks _TargetFireworks{"targetfireworks", "烟花绑身", "让烟花在选中的玩家身上持续绽放（不伤人）"};
}
