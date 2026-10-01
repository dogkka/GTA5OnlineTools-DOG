#include <vector>

#include "core/commands/BoolCommand.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Players.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Ped.hpp"

namespace YimMenu::Features
{
	// spawns hostile animals around the selected player
	class AnimalAttack : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto target = player.GetPed();
			if (!target)
				return;

			const auto pos = target.GetPosition();

			static const std::uint32_t models[] = {"a_c_mtlion"_J, "a_c_panther"_J, "a_c_cougar"_J};

			int spawned = 0;
			for (int i = 0; i < 4; i++)
			{
				auto model = models[rand() % 3];
				if (!STREAMING::IS_MODEL_IN_CDIMAGE(model))
					continue;

				auto beast = Ped::Create(model, rage::fvector3{pos.x + (float)(rand() % 16 - 8), pos.y + (float)(rand() % 16 - 8), pos.z + 0.5f});
				if (!beast)
					continue;

				PED::SET_PED_AS_ENEMY(beast.GetHandle(), true);
				PED::SET_PED_KEEP_TASK(beast.GetHandle(), true);
				TASK::TASK_COMBAT_PED(beast.GetHandle(), target.GetHandle(), 0, 16);
				spawned++;
			}

			Notifications::Show("猛兽袭击", std::to_string(spawned) + " 只猛兽已扑向目标。", NotificationType::Success);
		}
	};

	// spawns a zombie horde around the selected player
	class ZombieHorde : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto target = player.GetPed();
			if (!target)
				return;

			const auto pos = target.GetPosition();

			static const std::uint32_t models[] = {"u_m_y_zombie_01"_J, "u_m_o_zombie_01"_J, "u_m_y_zombie_01"_J};

			int spawned = 0;
			for (int i = 0; i < 8; i++)
			{
				auto model = models[i % 3];
				if (!STREAMING::IS_MODEL_IN_CDIMAGE(model))
					continue;

				auto zombie = Ped::Create(model, rage::fvector3{pos.x + (float)(rand() % 24 - 12), pos.y + (float)(rand() % 24 - 12), pos.z + 0.5f});
				if (!zombie)
					continue;

				const int handle = zombie.GetHandle();
				PED::SET_PED_AS_ENEMY(handle, true);
				PED::SET_PED_KEEP_TASK(handle, true);
				PED::SET_PED_MOVE_RATE_OVERRIDE(handle, 1.4f);
				PED::SET_PED_COMBAT_ATTRIBUTES(handle, 5, true); // always fight
				TASK::TASK_COMBAT_PED(handle, target.GetHandle(), 0, 16);
				spawned++;
			}

			if (spawned > 0)
				Notifications::Show("尸潮", std::to_string(spawned) + " 只丧尸包围了目标。", NotificationType::Success);
			else
				Notifications::Show("尸潮", "丧尸模型不可用（当前游戏版本）。", NotificationType::Warning);
		}
	};

	// spawns a driverless car that relentlessly hunts the selected player
	class GhostCar : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		static int g_GhostCar;

	public:
		static int GetGhostCar()
		{
			return g_GhostCar;
		}

		virtual void OnCall(Player player) override
		{
			auto target = player.GetPed();
			if (!target)
				return;

			const auto pos = target.GetPosition();

			if (g_GhostCar != 0 && ENTITY::DOES_ENTITY_EXIST(g_GhostCar))
				Vehicle(g_GhostCar).Delete();

			auto ghost = Vehicle::Create("zentorno"_J, rage::fvector3{pos.x + 25.0f, pos.y + 25.0f, pos.z + 1.0f});
			if (!ghost)
				return;

			g_GhostCar = ghost.GetHandle();
			ENTITY::SET_ENTITY_INVINCIBLE(g_GhostCar, true, false);
			VEHICLE::SET_VEHICLE_ENGINE_ON(g_GhostCar, true, true, false);
			VEHICLE::SET_VEHICLE_DOORS_LOCKED(g_GhostCar, 4);
			VEHICLE::SET_VEHICLE_STRONG(g_GhostCar, true);

			if (auto cmd = Commands::GetCommand<BoolCommand>("ghostcarhunt"_J))
				cmd->SetState(true);

			Notifications::Show("幽灵猎车", "一辆不会停下的幽灵车开始追猎目标。", NotificationType::Success);
		}
	};

	int GhostCar::g_GhostCar = 0;

	class GhostCarHunt : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			const int handle = GhostCar::GetGhostCar();
			if (handle == 0 || !ENTITY::DOES_ENTITY_EXIST(handle))
				return;

			auto target = Players::GetSelected().GetPed();
			if (!target)
				return;

			const auto car_pos    = ENTITY::GET_ENTITY_COORDS(handle, true);
			const auto target_pos = target.GetPosition();
			const float dx        = target_pos.x - car_pos.x;
			const float dy        = target_pos.y - car_pos.y;
			const float heading   = atan2f(-dx, dy) * 180.0f / 3.14159265f;

			ENTITY::SET_ENTITY_HEADING(handle, heading);
			VEHICLE::SET_VEHICLE_FORWARD_SPEED(handle, 45.0f);
		}
	};

	static GhostCarHunt _GhostCarHunt{"ghostcarhunt", "幽灵追猎", "让幽灵车持续追击选中的玩家"};

	static AnimalAttack _AnimalAttack{"animalattack", "猛兽袭击", "在目标周围刷出 4 只猛兽围攻"};
	static ZombieHorde _ZombieHorde{"zombiehorde", "尸潮", "在目标周围刷出 8 只丧尸围攻"};
	static GhostCar _GhostCar{"ghostcar", "幽灵猎车", "刷出一辆无敌的无人车追撞目标"};
}
