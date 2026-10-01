#include <vector>

#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Ped.hpp"

namespace YimMenu::Features
{
	// herds of wild animals rampaging around the player
	class AnimalMigration : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		static constexpr size_t s_MaxAnimals = 25;

		static const std::uint32_t* Models()
		{
			static const std::uint32_t models[] = {
			    "a_c_deer"_J,
			    "a_c_cow"_J,
			    "a_c_pig"_J,
			    "a_c_boar"_J,
			    "a_c_coyote"_J,
			};
			return models;
		}

		std::vector<int> m_Animals;
		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 60)
				return;

			m_Timer = 0;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			// drop dead / recycle oldest to stay within the cap
			std::erase_if(m_Animals, [](int handle) { return !ENTITY::DOES_ENTITY_EXIST(handle); });

			while (m_Animals.size() >= s_MaxAnimals)
			{
				const int oldest = m_Animals.front();
				m_Animals.erase(m_Animals.begin());

				if (ENTITY::DOES_ENTITY_EXIST(oldest))
					Ped(oldest).Delete();
			}

			const auto pos   = ped.GetPosition();
			const auto models = Models();

			// spawn two animals per burst
			for (int i = 0; i < 2; i++)
			{
				std::uint32_t model = 0;
				for (int attempts = 0; attempts < 5; attempts++)
				{
					const auto candidate = models[rand() % 5];
					if (STREAMING::IS_MODEL_IN_CDIMAGE(candidate))
					{
						model = candidate;
						break;
					}
				}

				if (model == 0)
					continue;

				const float x = pos.x + (float)(rand() % 60 - 30);
				const float y = pos.y + (float)(rand() % 60 - 30);

				auto animal = Ped::Create(model, rage::fvector3{x, y, pos.z + 0.5f}, (float)(rand() % 360));
				if (!animal)
					continue;

				TASK::TASK_WANDER_IN_AREA(animal.GetHandle(), x, y, pos.z, 80.0f, 5.0f, 3.0f);
				PED::SET_PED_KEEP_TASK(animal.GetHandle(), true);

				m_Animals.push_back(animal.GetHandle());
			}
		}

		virtual void OnDisable() override
		{
			for (const int handle : m_Animals)
			{
				if (ENTITY::DOES_ENTITY_EXIST(handle))
					Ped(handle).Delete();
			}

			m_Animals.clear();
		}
	};

	static AnimalMigration _AnimalMigration{"animalmigration", "动物大迁徙", "周围不断出现满街游荡的野生动物（鹿/牛/猪/野猪/郊狼）"};
}
