#include "core/commands/IntCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/ScriptGlobal.hpp"
#include "game/gta/Stats.hpp"
#include "game/pointers/Pointers.hpp"
#include "types/script/globals/GPBD_FM.hpp"

#include <chrono>

namespace YimMenu::Features
{
	// [DOG] 夜总会自动化：移植自 YimMenuV2 PR #1049（作者 Nxbular；上游不收 recovery 类，代码公开可复用）
	// 说明：人气/收益计时沿用本 fork 既有 MPX 统计项写法；保险箱领取复用本 fork 领取命令的现行全局
	namespace Nightclub
	{
		static bool InSession()
		{
			return Pointers.IsSessionStarted && *Pointers.IsSessionStarted;
		}

		static IntCommand _PopularityInterval{"nightclubpopularityinterval", "人气维持间隔（分钟）", "每隔多少分钟补一次夜总会人气（0 = 连续）。", 0, 60, 5};
		static IntCommand _SafeInterval{"nightclubsafeinterval", "保险箱检查间隔（分钟）", "每隔多少分钟检查并领取一次夜总会保险箱（0 = 连续）。", 0, 60, 5};
		static IntCommand _PayTimeInterval{"nightclubpaytimeinterval", "收益计时重置间隔（分钟）", "每隔多少分钟重置一次夜总会收益计时（0 = 连续）。", 0, 60, 5};

		class KeepPopularityLoop : public LoopedCommand
		{
			using LoopedCommand::LoopedCommand;

			virtual void OnTick() override
			{
				if (!InSession())
					return;

				static auto lastRun = std::chrono::steady_clock::now() - std::chrono::hours(1);

				const auto now = std::chrono::steady_clock::now();
				if (std::chrono::duration_cast<std::chrono::minutes>(now - lastRun).count() >= _PopularityInterval.GetState())
				{
					Stats::SetInt("MPX_CLUB_POPULARITY", 1000);
					lastRun = now;
				}
			}
		};

		class AutoCollectSafeLoop : public LoopedCommand
		{
			using LoopedCommand::LoopedCommand;

			virtual void OnTick() override
			{
				if (!InSession())
					return;

				static auto lastRun = std::chrono::steady_clock::now() - std::chrono::hours(1);

				const auto now = std::chrono::steady_clock::now();
				if (std::chrono::duration_cast<std::chrono::minutes>(now - lastRun).count() >= _SafeInterval.GetState())
				{
					const auto player = Self::GetPlayer();
					if (player.IsValid())
					{
						const int playerId = player.GetId();
						if (playerId >= 0 && playerId < 32)
						{
							if (auto gpbdFm = GPBD_FM::Get())
							{
								const auto& nightclub = gpbdFm->Entries[playerId].PropertyData.NightclubData;
								if (nightclub.SafeCashValue >= 250000)
								{
									auto claim = ScriptGlobal(2708943);
									if (claim.CanAccess())
										*claim.As<BOOL*>() = TRUE;
								}
							}
						}
					}
					lastRun = now;
				}
			}
		};

		class AutoResetPayTimeLoop : public LoopedCommand
		{
			using LoopedCommand::LoopedCommand;

			virtual void OnTick() override
			{
				if (!InSession())
					return;

				static auto lastRun = std::chrono::steady_clock::now() - std::chrono::hours(1);

				const auto now = std::chrono::steady_clock::now();
				if (std::chrono::duration_cast<std::chrono::minutes>(now - lastRun).count() >= _PayTimeInterval.GetState())
				{
					Stats::SetInt("MPX_CLUB_PAY_TIME_LEFT", -1);
					lastRun = now;
				}
			}
		};

		static KeepPopularityLoop _KeepPopularity{"keepnightclubpopularity", "持续补满夜总会人气", "按设定间隔持续把夜总会人气设为最高。"};
		static AutoCollectSafeLoop _AutoCollectSafe{"autocollectnightclubsafe", "自动领取夜总会保险箱", "保险箱满（≥25 万）时自动提交领取请求。"};
		static AutoResetPayTimeLoop _AutoResetPayTime{"autoresetnightclubpaytime", "自动重置收益计时", "按设定间隔重置夜总会收益计时。"};
	}
}