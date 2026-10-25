#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/util/Joaat.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/ScriptLocal.hpp"
#include "game/gta/Scripts.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	// 自由模式挑战（Freemode Challenge）——战局里轮换出现的“低空飞行 / 无碰撞行驶 / 擦身而过”等 19 项小挑战。
	//
	// 机制（按 Enhanced 1.73 反编译脚本 am_challenges.c 核验；脚本槽位已用 blackjack(147/145)、
	// casinoroulette(155/153) 两处广播注册点校准：反编译 uLocal_N 与实际槽位相差 2）：
	//   - 共享数据（主机广播，88 词，槽 671）：+3 = 挑战类型（0-18）；+37 = 阶段（3 = 挑战进行中）
	//     +4[32] = 排名列表（主机按分数排序，f_4[0] 即第一名）
	//   - 玩家数据（玩家广播，32 × 12 词，槽 2638）：+4 = 我的分数
	//   - 分数写入玩家广播块后会同步给主机，主机重排排名；脚本只在数值变“更好”时才会覆写玩家的值，
	//     因此在挑战进行中按拍锁分即可稳定拿到第一名。
	//   - 比较规则：类型 6（最低开伞）越小越好（0 视作无穷大），其余类型越大越好。
	class FreemodeChallengeWin : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		static constexpr int kSharedDataLocal = 671;  // am_challenges 共享数据（主机广播）
		static constexpr int kPlayerDataLocal = 2638; // am_challenges 玩家数据（玩家广播）
		static constexpr int kPlayerStride = 12;      // 每个玩家 12 词
		static constexpr int kTypeOffset = 3;         // 挑战类型 0-18
		static constexpr int kStateOffset = 37;       // 阶段：3 = 挑战进行中
		static constexpr int kScoreOffset = 4;        // 我的分数
		static constexpr int kActiveState = 3;
		static constexpr int kMaxBetterScore = 1000000; // “越大越好”类型锁定的分数

		static constexpr const char* kNames[19] = {
		    "最远跳跃距离",
		    "最长自由落体",
		    "最高速度",
		    "最长前轮着地（Stoppie）",
		    "最长后轮离地（Wheelie）",
		    "无碰撞行驶",
		    "最低开伞",
		    "偷取载具数量",
		    "擦身而过次数",
		    "最长倒车距离",
		    "最高坠落生还",
		    "低空飞行",
		    "倒飞飞行",
		    "最长甩出距离（Bail）",
		    "飞越桥下次数",
		    "爆头击杀",
		    "车载击杀",
		    "近战击杀",
		    "狙击击杀",
		};

		int m_Timer = 0;
		int m_LastType = -1;
		DWORD m_LastLayoutWarn = 0;

	public:
		virtual void OnTick() override
		{
			// LoopedCommand 每个脚本帧跑一次，这里降到约 0.2 秒一次，锁分足够快且不浪费。
			if (++m_Timer < 10)
				return;
			m_Timer = 0;

			if (!Pointers.IsSessionStarted || !*Pointers.IsSessionStarted)
				return;

			auto thread = Scripts::FindScriptThread("am_challenges"_J);
			if (!thread || !thread->m_Stack)
				return;

			auto state = ScriptLocal(thread, kSharedDataLocal + kStateOffset).As<int*>();
			auto type = ScriptLocal(thread, kSharedDataLocal + kTypeOffset).As<int*>();

			if (!state || !type)
				return;

			// 结构校验：游戏更新导致槽位变化时宁可不动作，也不写坏脚本数据。
			if (*type < 0 || *type > 18 || *state < 0 || *state > 8)
			{
				const DWORD now = GetTickCount();
				if (now - m_LastLayoutWarn >= 60000)
				{
					m_LastLayoutWarn = now;
					LOGF(VERBOSE, "Freemode challenge layout check failed (type {}, state {}); no writes performed.", *type, *state);
				}
				return;
			}

			if (*state != kActiveState)
				return;

			const int my_slot = NETWORK::PARTICIPANT_ID_TO_INT();
			if (my_slot < 0 || my_slot >= 32)
				return;

			// 类型 6（最低开伞）是“越小越好”，且 0 会被脚本当作无穷大 → 锁定 1；其余类型锁一个大数值。
			const int value = (*type == 6) ? 1 : kMaxBetterScore;

			auto score = ScriptLocal(thread, kPlayerDataLocal + my_slot * kPlayerStride + kScoreOffset).As<int*>();
			if (!score || *score == value)
				return;

			*score = value;

			if (m_LastType != *type)
			{
				m_LastType = *type;
				Notifications::Show("战局挑战", std::string("已锁定第一名：") + kNames[*type], NotificationType::Success);
			}
		}
	};

	static FreemodeChallengeWin _FreemodeChallengeWin{"freemodechallengewin", "自由模式挑战·必胜", "战局挑战（Low Flying 低空飞行 / No Crashes 无碰撞行驶等 19 项）进行时自动锁定第一名，按正常规则结算奖励。"};
}
