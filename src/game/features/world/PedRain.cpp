#include <iterator>

#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Ped.hpp"

namespace YimMenu::Features
{
	static const std::uint32_t ped_models[] = {
	    "a_m_y_skater_01"_J,
	    "a_m_m_business_01"_J,
	    "a_f_y_hipster_01"_J,
	    "s_m_y_cop_01"_J,
	    "a_m_y_breakdance_01"_J,
	    "a_m_y_beach_01"_J,
	    "a_m_y_musclbeac_01"_J,
	    "a_f_y_beach_01"_J};

	class PedRain : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 10)
				return;

			m_Timer = 0;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			auto pos   = ped.GetPosition();
			auto model = ped_models[rand() % std::size(ped_models)];

			Ped::Create(model,
			    rage::fvector3(pos.x + (float)(rand() % 20 - 10), pos.y + (float)(rand() % 20 - 10), pos.z + 15.0f),
			    0.0f);
		}
	};

	static PedRain _PedRain{"pedrain", "人形雨", "在你头顶下起人形雨"};
}
