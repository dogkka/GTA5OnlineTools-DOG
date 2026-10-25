#include "core/commands/IntCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static IntCommand _MentalStateValue{"mentalstateset", "精神值", "-1 关闭；0-100 设定自己的精神值（影响战局匹配与显示）。", -1, 100, -1};

	// 精神值控制：持续写入，保持锁定（Stand 同款原生）
	class MentalStateControl : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			const int value = _MentalStateValue.GetState();
			if (value < 0)
				return;

			NETWORK::NETWORK_SET_PLAYER_MENTAL_STATE(value);
		}
	};

	static MentalStateControl _MentalStateControl{"mentalstate", "精神值控制", "设定并锁定自己的精神值（-1 关闭，0-100 生效）。"};
}
