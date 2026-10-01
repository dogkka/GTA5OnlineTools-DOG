#include <cmath>
#include <vector>

#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Players.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Ped.hpp"

namespace YimMenu::Features
{
	// a swarm of clowns that cling to the selected player forever
	class ClownArmy : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		static constexpr size_t s_MaxClowns = 8;

		static const std::uint32_t* Models()
		{
			static const std::uint32_t models[] = {"u_m_y_pogo_01"_J, "s_m_m_clown_01"_J, "u_m_y_clown_01"_J, "u_m_y_mime_01"_J};
			return models;
		}

		std::vector<int> m_Clowns;
		int m_Timer = 0;

		void Cleanup()
		{
			for (const int handle : m_Clowns)
			{
				if (ENTITY::DOES_ENTITY_EXIST(handle))
					Ped(handle).Delete();
			}

			m_Clowns.clear();
		}

		virtual void OnTick() override
		{
			auto player = Players::GetSelected();
			if (!player.IsValid() || player.IsLocal())
			{
				Cleanup();
				return;
			}

			if (++m_Timer < 120)
				return;

			m_Timer = 0;

			auto target = player.GetPed();
			if (!target)
				return;

			// drop dead clowns
			std::erase_if(m_Clowns, [](int handle) {
				return !ENTITY::DOES_ENTITY_EXIST(handle) || PED::IS_PED_DEAD_OR_DYING(handle, true);
			});

			if (m_Clowns.size() >= s_MaxClowns)
				return;

			// pick an available clown model
			std::uint32_t model = 0;
			const auto models   = Models();

			for (auto candidate : {models[0], models[1], models[2], models[3]})
			{
				if (STREAMING::IS_MODEL_IN_CDIMAGE(candidate))
				{
					model = candidate;
					break;
				}
			}

			if (model == 0)
				return;

			const auto pos = target.GetPosition();

			auto clown = Ped::Create(model, rage::fvector3{pos.x + (float)(rand() % 12 - 6), pos.y + (float)(rand() % 12 - 6), pos.z + 0.5f});
			if (!clown)
				return;

			// surround the target at evenly spaced angles
			const float angle = (float)m_Clowns.size() / (float)s_MaxClowns * 6.28318f;

			TASK::TASK_FOLLOW_TO_OFFSET_OF_ENTITY(clown.GetHandle(),
			    target.GetHandle(),
			    cosf(angle) * 2.5f,
			    sinf(angle) * 2.5f,
			    0.0f,
			    2.0f,
			    -1,
			    1.5f,
			    true);
			PED::SET_PED_KEEP_TASK(clown.GetHandle(), true);

			m_Clowns.push_back(clown.GetHandle());
		}

		virtual void OnDisable() override
		{
			Cleanup();
		}
	};

	static ClownArmy _ClownArmy{"clownarmy", "小丑军团", "让一群小丑阴魂不散地围着选中的玩家"};
}
