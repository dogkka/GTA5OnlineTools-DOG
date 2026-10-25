#include <vector>

#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Object.hpp"
#include "types/fx/ExplosionType.hpp"

namespace YimMenu::Features
{
	class MeteorShower : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		struct Meteor
		{
			int Handle;
			int Age;
		};

		static const std::uint32_t* Models()
		{
			static const std::uint32_t models[] = {
			    "prop_asteroid_01"_J,
			    "prop_rock_4_cluster"_J,
			    "prop_rock_5_sm"_J,
			    "prop_rock_4_b"_J};
			return models;
		}

		std::vector<Meteor> m_Meteors;
		std::uint32_t m_PendingModel = 0;
		int m_Timer                  = 0;

		virtual void OnTick() override
		{
			// update live meteors: explode when they hit the ground (or after a timeout)
			for (auto it = m_Meteors.begin(); it != m_Meteors.end();)
			{
				if (!ENTITY::DOES_ENTITY_EXIST(it->Handle))
				{
					it = m_Meteors.erase(it);
					continue;
				}

				it->Age++;

				const auto pos = ENTITY::GET_ENTITY_COORDS(it->Handle, true);
				const auto vel = ENTITY::GET_ENTITY_VELOCITY(it->Handle);
				const bool landed = (vel.z > -3.0f && it->Age > 30) || it->Age > 300;

				if (landed)
				{
					FIRE::ADD_EXPLOSION(pos.x, pos.y, pos.z, (int)ExplosionType::SCRIPT_MISSILE_LARGE, 3.0f, true, false, 1.0f, false);
					Object(it->Handle).Delete();
					it = m_Meteors.erase(it);
					continue;
				}

				++it;
			}

			if (++m_Timer < 25)
				return;

			m_Timer = 0;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			// non-blocking model load
			if (m_PendingModel == 0)
			{
				const auto models = Models();
				for (int attempts = 0; attempts < 4; attempts++)
				{
					const auto candidate = models[rand() % 4];
					if (STREAMING::IS_MODEL_IN_CDIMAGE(candidate))
					{
						m_PendingModel = candidate;
						break;
					}
				}

				if (m_PendingModel == 0)
					return;
			}

			if (!STREAMING::HAS_MODEL_LOADED(m_PendingModel))
			{
				STREAMING::REQUEST_MODEL(m_PendingModel);
				return;
			}

			const auto pos = ped.GetPosition();
			const float x  = pos.x + (float)(rand() % 80 - 40);
			const float y  = pos.y + (float)(rand() % 80 - 40);
			const float z  = pos.z + 80.0f + (float)(rand() % 60);

			auto meteor = Object::Create(m_PendingModel, rage::fvector3{x, y, z});
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(m_PendingModel);
			m_PendingModel = 0;

			if (!meteor)
				return;

			ENTITY::SET_ENTITY_VELOCITY(meteor.GetHandle(), (float)(rand() % 20 - 10) * 0.5f, (float)(rand() % 20 - 10) * 0.5f, -70.0f);
			FIRE::START_ENTITY_FIRE(meteor.GetHandle());

			m_Meteors.push_back({meteor.GetHandle(), 0});
		}

		virtual void OnDisable() override
		{
			for (auto& meteor : m_Meteors)
			{
				if (ENTITY::DOES_ENTITY_EXIST(meteor.Handle))
					Object(meteor.Handle).Delete();
			}

			m_Meteors.clear();
			m_PendingModel = 0;
		}
	};

	static MeteorShower _MeteorShower{"meteorshower", "陨石雨", "天降燃烧的陨石砸向周围地面"};
}
