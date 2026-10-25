#include "core/commands/LoopedCommand.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class NightVision : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			GRAPHICS::SET_NIGHTVISION(true);
		}

		virtual void OnDisable() override
		{
			GRAPHICS::SET_NIGHTVISION(false);
		}
	};

	class ThermalVision : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			GRAPHICS::SET_SEETHROUGH(true);
		}

		virtual void OnDisable() override
		{
			GRAPHICS::SET_SEETHROUGH(false);
		}
	};

	static NightVision _NightVision{"nightvision", "夜视仪", "开启夜视模式"};
	static ThermalVision _ThermalVision{"thermalvision", "热成像", "开启热成像视觉"};
}
