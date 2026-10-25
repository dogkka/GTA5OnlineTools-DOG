#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Players.hpp"
#include "game/gta/Natives.hpp"

#include <cstdlib>

namespace YimMenu::Features
{
	// 蹦蹦车：目标载具不断被小幅度弹起（本地/实验性）
	class BouncyVehicle : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;
		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 45)
				return;
			m_Timer = 0;

			auto player = Players::GetSelected();
			if (!player.IsValid() || player.IsLocal())
				return;

			auto target = player.GetPed();
			if (!target)
				return;

			if (auto veh = target.GetVehicle())
				ENTITY::SET_ENTITY_VELOCITY(veh.GetHandle(), (float)(rand() % 3 - 1), (float)(rand() % 3 - 1), 3.0f + (float)(rand() % 3));
		}
	};

	// 喇叭狂魔：目标载具持续鸣笛（本地可闻）
	class HonkVehicle : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;
		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 60)
				return;
			m_Timer = 0;

			auto player = Players::GetSelected();
			if (!player.IsValid() || player.IsLocal())
				return;

			auto target = player.GetPed();
			if (!target)
				return;

			if (auto veh = target.GetVehicle())
				VEHICLE::START_VEHICLE_HORN(veh.GetHandle(), 1000, 0, true);
		}
	};

	// 漂移模式：降低目标载具抓地力（实验性）
	class DriftVehicle : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;
		int m_Timer   = 0;
		int m_LastVeh = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 30)
				return;
			m_Timer = 0;

			auto player = Players::GetSelected();
			if (!player.IsValid() || player.IsLocal())
				return;

			auto target = player.GetPed();
			if (!target)
				return;

			auto veh = target.GetVehicle();
			if (!veh)
				return;

			m_LastVeh = veh.GetHandle();
			VEHICLE::SET_VEHICLE_REDUCE_GRIP(veh.GetHandle(), true);
			VEHICLE::SET_VEHICLE_REDUCE_GRIP_LEVEL(veh.GetHandle(), 10);
		}

		virtual void OnDisable() override
		{
			if (m_LastVeh && ENTITY::DOES_ENTITY_EXIST(m_LastVeh))
				VEHICLE::SET_VEHICLE_REDUCE_GRIP(m_LastVeh, false);
			m_LastVeh = 0;
		}
	};

	static BouncyVehicle _BouncyVehicle{"bouncyvehicle", "蹦蹦车", "目标载具不断被弹起（实验性）"};
	static HonkVehicle _HonkVehicle{"honkvehicle", "喇叭狂魔", "目标载具持续鸣笛（本地可闻）"};
	static DriftVehicle _DriftVehicle{"driftvehicle", "漂移模式", "降低目标载具抓地力（实验性）"};
}
