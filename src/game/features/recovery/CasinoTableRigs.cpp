#include "core/commands/IntCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/util/Joaat.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/ScriptLocal.hpp"
#include "game/gta/Scripts.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	static IntCommand _RouletteValue{"rigroulettevalue", "轮盘结果", "-1 关闭；0-36 指定数字；37 = 00。", -1, 37, -1};

	// 轮盘控制：夺取 casinoroulette 脚本主机后写入六个转盘的结果局部变量
	// 局部索引照抄 Stand 参考实现：LOCAL_CR_BASE(153) + LOCAL_CR_SPINRESULT(1357+153) + 1 + i
	class RigRoulette : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		virtual void OnTick() override
		{
			const int value = _RouletteValue.GetState();
			if (value < 0)
				return;

			if (++m_Timer < 30)
				return;
			m_Timer = 0;

			if (!Pointers.IsSessionStarted || !*Pointers.IsSessionStarted)
				return;

			auto thread = Scripts::FindScriptThread("casinoroulette"_J);
			if (!thread || !thread->m_Stack)
				return;

			Scripts::ForceScriptHost(thread);

			constexpr int spin_result_local = 153 + (1357 + 153) + 1;
			for (int i = 0; i < 6; i++)
			{
				auto* spin = ScriptLocal(thread, spin_result_local + i).As<int*>();
				if (spin)
					*spin = value;
			}
		}
	};

	// 黑杰克控制：夺取 blackjack 脚本主机后把庄家所有暗牌的“牌值”改写为 47（爆牌）
	class RigBlackjack : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 30)
				return;
			m_Timer = 0;

			if (!Pointers.IsSessionStarted || !*Pointers.IsSessionStarted)
				return;

			auto thread = Scripts::FindScriptThread("blackjack"_J);
			if (!thread || !thread->m_Stack)
				return;

			Scripts::ForceScriptHost(thread);

			constexpr int dealer_cards_local = 145 + 846; // Stand 参考值
			constexpr int dealer_size        = 13;

			auto* dealer_count = ScriptLocal(thread, dealer_cards_local).As<int*>();
			if (!dealer_count || *dealer_count <= 0 || *dealer_count > 32)
				return;

			for (int dealer = 0; dealer < *dealer_count; dealer++)
			{
				auto* card_count = ScriptLocal(thread, dealer_cards_local + 1 + dealer * dealer_size).As<int*>();
				if (!card_count || *card_count < 0 || *card_count > 64)
					continue;

				for (int card = 0; card < *card_count; card++)
				{
					auto* card_ptr = ScriptLocal(thread, dealer_cards_local + 1 + dealer * dealer_size + 1 + card).As<int*>();
					if (card_ptr && *card_ptr != 0)
						*card_ptr = 47; // 47 映射为“8”→庄家爆牌
				}
			}
		}
	};

	static RigRoulette _RigRoulette{"rigroulette", "轮盘控制", "把赌场轮盘结果锁定为指定数字（实验性：需在轮盘桌旁使用）。"};
	static RigBlackjack _RigBlackjack{"rigblackjack", "黑杰克控制", "让庄家永远爆牌（实验性：需在 21 点牌桌旁使用）。"};
}
