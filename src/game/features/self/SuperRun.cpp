#include "core/commands/LoopedCommand.hpp"
#include "core/commands/FloatCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static FloatCommand _SuperRunMoveRateOverride{"moverateoverride", "移动速度倍率", "基础奔跑/冲刺速度倍率", 0.0f, 10.0f, 2.0f};
	static FloatCommand _SuperRunRunSprintMultiplier{"runsprintswimmultiplier", "跑步/冲刺/游泳倍率", "达到最高速度前的跑步、冲刺和游泳加速倍率", 1.0f, 1.49f, 1.0f};

	class SuperRun : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnDisable() override
		{
			if (auto player = Self::GetPlayer())
			{
				auto playerId = player.GetId();
				PLAYER::SET_SWIM_MULTIPLIER_FOR_PLAYER(playerId, 1.0f);
				PLAYER::SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(playerId, 1.0f);
			}
		}

		virtual void OnEnable() override
		{
			if (auto player = Self::GetPlayer())
			{
				auto playerId = player.GetId();
				PLAYER::SET_SWIM_MULTIPLIER_FOR_PLAYER(playerId, _SuperRunRunSprintMultiplier.GetState());
				PLAYER::SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(playerId, _SuperRunRunSprintMultiplier.GetState());
			}
		}

		virtual void OnTick() override
		{
			if (auto ped = Self::GetPed())
				PED::SET_PED_MOVE_RATE_OVERRIDE(ped.GetHandle(), _SuperRunMoveRateOverride.GetState());
		}
	};

	static SuperRun _Superrun{"superrun", "超级奔跑", "跑得比平常更快"};
}
