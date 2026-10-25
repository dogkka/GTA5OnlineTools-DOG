#include <algorithm>
#include <iterator>
#include <vector>

#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Ped.hpp"

namespace YimMenu::Features
{
	class Bodyguards : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		static constexpr size_t s_MaxBodyguards = 3;

		static const std::uint32_t* Models()
		{
			static const std::uint32_t models[] = {"s_m_m_security_01"_J, "s_m_m_highsec_01"_J, "s_m_y_blackops_01"_J};
			return models;
		}

		std::vector<int> m_Bodyguards;
		int m_CheckTimer = 0;

		void SpawnBodyguard()
		{
			auto ped = Self::GetPed();
			if (!ped)
				return;

			const auto pos = ped.GetPosition();

			const auto spawn = rage::fvector3{pos.x + (float)(rand() % 6 - 3), pos.y + (float)(rand() % 6 - 3), pos.z + 0.5f};

			auto spawned = Ped::Create(Models()[rand() % 3], spawn);
			if (!spawned)
				return;

			const int handle = spawned.GetHandle();
			PED::SET_PED_AS_GROUP_MEMBER(handle, PLAYER::GET_PLAYER_GROUP(PLAYER::PLAYER_ID()));
			PED::SET_PED_KEEP_TASK(handle, true);
			PED::SET_PED_COMBAT_ABILITY(handle, 2);
			PED::SET_PED_ACCURACY(handle, 50);
			WEAPON::GIVE_WEAPON_TO_PED(handle, "WEAPON_CARBINERIFLE"_J, 9999, false, true);
			TASK::TASK_FOLLOW_TO_OFFSET_OF_ENTITY(handle, ped.GetHandle(), 1.5f, 1.5f, 0.0f, 3.0f, -1, 2.0f, true);

			m_Bodyguards.push_back(handle);
		}

		virtual void OnEnable() override
		{
			for (size_t i = 0; i < s_MaxBodyguards; i++)
				SpawnBodyguard();

			Notifications::Show("保镖小队", "保镖已到位。", NotificationType::Success);
		}

		virtual void OnTick() override
		{
			if (++m_CheckTimer < 120)
				return;

			m_CheckTimer = 0;

			// drop dead bodyguards
			std::erase_if(m_Bodyguards, [](int handle) { return !ENTITY::DOES_ENTITY_EXIST(handle) || PED::IS_PED_DEAD_OR_DYING(handle, true); });

			while (m_Bodyguards.size() < s_MaxBodyguards)
				SpawnBodyguard();
		}

		virtual void OnDisable() override
		{
			for (const int handle : m_Bodyguards)
			{
				if (ENTITY::DOES_ENTITY_EXIST(handle))
					Ped(handle).Delete();
			}

			m_Bodyguards.clear();
		}
	};

	static Bodyguards _Bodyguards{"bodyguards", "保镖小队", "生成 3 名武装保镖跟随并保护你"};
}
