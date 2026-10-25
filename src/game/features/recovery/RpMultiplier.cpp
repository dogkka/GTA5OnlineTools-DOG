#include "core/commands/LoopedCommand.hpp"
#include "core/commands/FloatCommand.hpp"
#include "game/backend/Tunables.hpp"

namespace YimMenu::Features
{
	static FloatCommand _RpMultiplierInput{"rpmultiplierinput", "RP 倍率", "设置获得 RP 时使用的倍率；负数按 0 处理。", std::nullopt, std::nullopt, 1};

	class OverrideRPMultiplier : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		Tunable m_XPMultiplier{"XP_MULTIPLIER"_J};

		virtual void OnTick() override
		{
			auto state = _RpMultiplierInput.GetState();
			if (state < 0)
			{
				state = 0;
			}

			if (m_XPMultiplier.IsReady())
				m_XPMultiplier.Set(state);
		}

		virtual void OnDisable() override
		{
			if (m_XPMultiplier.IsReady())
				m_XPMultiplier.Set(1.0f);
		}
	};

	static OverrideRPMultiplier _OverrideRPMultiplier{"overriderpmultiplier", "自定义 RP 倍率", "按设定倍率修改获得的 RP。"};
}
