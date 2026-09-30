#include "core/commands/Command.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/commands/IntCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static IntCommand _WantedSlider{"wantedslider", "通缉等级", "要设置/冻结的通缉等级", 0, 5, 0};

	class ClearWanted : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			Self::GetPlayer().SetWantedLevel(0);
		}
	};

	class SetWanted : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			Self::GetPlayer().SetWantedLevel(_WantedSlider.GetState());
		}
	};

	class NeverWanted : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			Self::GetPlayer().SetWantedLevel(0);
			PLAYER::SET_MAX_WANTED_LEVEL(0);
		}

		virtual void OnDisable() override
		{
			PLAYER::SET_MAX_WANTED_LEVEL(6);
		}
	};

	class FreezeWanted : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (Self::GetPlayer().GetWantedLevel() != _WantedSlider.GetState())
				Self::GetPlayer().SetWantedLevel(_WantedSlider.GetState());
			PLAYER::SET_MAX_WANTED_LEVEL(_WantedSlider.GetState());
		}
	};

	static ClearWanted _ClearWanted{"clearwanted", "清除通缉", "清除你的通缉等级"};
	static SetWanted _SetWanted{"setwanted", "设置通缉", "将你的通缉等级设置为指定数值"};
	static NeverWanted _NeverWanted{"neverwanted", "永不通缉", "永远不会获得通缉等级"};
	static FreezeWanted _FreezeWanted{"freezewanted", "冻结通缉", "将你的通缉等级冻结在指定数值"};
}
