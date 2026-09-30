#include "core/commands/Command.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static IntCommand _NetworkTimeHour{"networktimehour", "小时", "设置小时（0-23）", 0, 23, 12};
	static IntCommand _NetworkTimeMinute{"networktimeminute", "分钟", "设置分钟（0-59）", 0, 59, 0};
	static IntCommand _NetworkTimeSecond{"networktimesecond", "秒", "设置秒数（0-59）", 0, 59, 0};


	class SetNetworkTime : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			NETWORK::NETWORK_OVERRIDE_CLOCK_TIME(_NetworkTimeHour.GetState(),
			    _NetworkTimeMinute.GetState(),
			    _NetworkTimeSecond.GetState());
		}
	};

	class FreezeNetworkTime : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			NETWORK::NETWORK_OVERRIDE_CLOCK_TIME(_NetworkTimeHour.GetState(),
			    _NetworkTimeMinute.GetState(),
			    _NetworkTimeSecond.GetState());
		}

		virtual void OnDisable() override
		{
			NETWORK::NETWORK_CLEAR_CLOCK_TIME_OVERRIDE();
		}
	};

	static SetNetworkTime _SetTime{"setnetworktime", "设置时间", "设置当前在线时间"};
	static FreezeNetworkTime _FreezeTime{"freezenetworktime", "冻结时间", "将在线时钟冻结在所选时间"};
}
