#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	// pushes the water away around the player
	class DrainOcean : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		void ApplyWaterGrid(float height)
		{
			auto ped = Self::GetPed();
			if (!ped)
				return;

			const auto pos = ped.GetPosition();

			for (int dx = -2; dx <= 2; dx++)
			{
				for (int dy = -2; dy <= 2; dy++)
				{
					WATER::MODIFY_WATER(pos.x + (float)dx * 100.0f, pos.y + (float)dy * 100.0f, 120.0f, height);
				}
			}
		}

		virtual void OnTick() override
		{
			if (++m_Timer < 90)
				return;

			m_Timer = 0;

			WATER::SET_DEEP_OCEAN_SCALER(0.0f);
			ApplyWaterGrid(-60.0f);
		}

		virtual void OnDisable() override
		{
			WATER::RESET_DEEP_OCEAN_SCALER();
			ApplyWaterGrid(0.0f);
		}
	};

	static DrainOcean _DrainOcean{"drainocean", "海水消失", "把周围的海水推开（效果视区域而定，关闭后自动恢复）"};
}
