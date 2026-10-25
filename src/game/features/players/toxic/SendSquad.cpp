#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Ped.hpp"

namespace YimMenu::Features
{
	class SendSquad : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto target = player.GetPed();
			if (!target)
				return;

			const auto pos = target.GetPosition();

			static const std::uint32_t models[] = {"s_m_y_swat_01"_J, "s_m_y_marine_01"_J, "s_m_y_ranger_01"_J};

			for (int i = 0; i < 8; i++)
			{
				const auto spawn = rage::fvector3{pos.x + (float)(i % 4) * 2.5f - 5.0f, pos.y + (float)(i / 4) * 2.5f - 1.25f, pos.z + 1.0f};

				auto spawned = Ped::Create(models[i % 3], spawn);
				if (!spawned)
					continue;

				const int handle = spawned.GetHandle();
				PED::SET_PED_AS_ENEMY(handle, true);
				PED::SET_PED_ACCURACY(handle, 25);
				PED::SET_PED_KEEP_TASK(handle, true);
				WEAPON::GIVE_WEAPON_TO_PED(handle, "WEAPON_CARBINERIFLE"_J, 9999, false, true);
				TASK::TASK_COMBAT_PED(handle, target.GetHandle(), 0, 16);
			}

			Notifications::Show("送护卫队", "已给该玩家送上一队武装人员。", NotificationType::Info);
		}
	};

	static SendSquad _SendSquad{"sendsquad", "送护卫队", "在目标玩家周围刷出一队武装 NPC 围攻他"};
}
