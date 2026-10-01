#include <cmath>

#include "core/commands/BoolCommand.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Players.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Object.hpp"

namespace YimMenu::Features
{
	static int g_BowlingBall = 0;

	class SpawnBowlingBall : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto target = player.GetPed();
			if (!target)
				return;

			if (g_BowlingBall != 0 && ENTITY::DOES_ENTITY_EXIST(g_BowlingBall))
			{
				Object(g_BowlingBall).Delete();
				g_BowlingBall = 0;
			}

			std::uint32_t model = 0;
			for (auto candidate : {"stt_prop_stunt_bowling_ball"_J, "stt_prop_stunt_bowling_pin"_J, "prop_bowling_ball"_J})
			{
				if (STREAMING::IS_MODEL_IN_CDIMAGE(candidate))
				{
					model = candidate;
					break;
				}
			}

			if (model == 0)
			{
				Notifications::Show("死亡保龄球", "当前游戏版本没有保龄球模型。", NotificationType::Warning);
				return;
			}

			const auto pos = target.GetPosition();

			auto ball = Object::Create(model, rage::fvector3{pos.x + 30.0f, pos.y + 30.0f, pos.z + 2.0f});
			if (!ball)
			{
				Notifications::Show("死亡保龄球", "生成失败，请重试。", NotificationType::Error);
				return;
			}

			g_BowlingBall = ball.GetHandle();
			ENTITY::SET_ENTITY_INVINCIBLE(g_BowlingBall, true, false);

			if (auto cmd = Commands::GetCommand<BoolCommand>("bowlingballhunt"_J))
				cmd->SetState(true);

			Notifications::Show("死亡保龄球", "一颗死亡保龄球开始向目标滚动。", NotificationType::Success);
		}
	};

	class BowlingBallHunt : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (g_BowlingBall == 0 || !ENTITY::DOES_ENTITY_EXIST(g_BowlingBall))
				return;

			auto target = Players::GetSelected().GetPed();
			if (!target)
				return;

			const auto ball_pos   = ENTITY::GET_ENTITY_COORDS(g_BowlingBall, true);
			const auto target_pos = target.GetPosition();

			const float dx   = target_pos.x - ball_pos.x;
			const float dy   = target_pos.y - ball_pos.y;
			const float dist = sqrtf(dx * dx + dy * dy) + 0.1f;

			constexpr float speed = 28.0f;
			const float vx        = dx / dist * speed;
			const float vy        = dy / dist * speed;

			ENTITY::SET_ENTITY_VELOCITY(g_BowlingBall, vx, vy, 0.0f);

			// roll it: spin around the horizontal axis perpendicular to travel
			ENTITY::SET_ENTITY_ANGULAR_VELOCITY(g_BowlingBall, -vy * 0.6f, vx * 0.6f, 0.0f);
		}
	};

	static SpawnBowlingBall _SpawnBowlingBall{"spawnbowlingball", "死亡保龄球", "生成一颗巨型保龄球并开始滚动追猎目标"};
	static BowlingBallHunt _BowlingBallHunt{"bowlingballhunt", "保龄球追猎", "让死亡保龄球持续追撞选中的玩家"};
}
