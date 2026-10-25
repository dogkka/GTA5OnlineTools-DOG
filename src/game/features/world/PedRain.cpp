#include <iterator>
#include <vector>

#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Ped.hpp"
#include "game/gta/Natives.hpp"

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

		static constexpr size_t s_MaxPeds = 60;

		int m_Timer = 0;
		std::vector<int> m_SpawnedPeds;

		virtual void OnTick() override
		{
			if (++m_Timer < 10)
				return;

			m_Timer = 0;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			// recycle the oldest peds to avoid performance degradation
			if (m_SpawnedPeds.size() >= s_MaxPeds)
			{
				const int oldest = m_SpawnedPeds.front();
				m_SpawnedPeds.erase(m_SpawnedPeds.begin());

				if (ENTITY::DOES_ENTITY_EXIST(oldest))
					Ped(oldest).Delete();
			}

			auto pos   = ped.GetPosition();
			auto model = ped_models[rand() % std::size(ped_models)];

			auto spawned = Ped::Create(model,
			    rage::fvector3(pos.x + (float)(rand() % 20 - 10), pos.y + (float)(rand() % 20 - 10), pos.z + 15.0f),
			    0.0f);

			if (spawned)
				m_SpawnedPeds.push_back(spawned.GetHandle());
		}

		virtual void OnDisable() override
		{
			for (const int handle : m_SpawnedPeds)
			{
				if (ENTITY::DOES_ENTITY_EXIST(handle))
					Ped(handle).Delete();
			}

			m_SpawnedPeds.clear();
		}
	};

	static PedRain _PedRain{"pedrain", "人形雨", "在你头顶下起人形雨"};
}
