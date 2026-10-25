#include "core/commands/Command.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/gta/Stats.hpp"

namespace YimMenu::Features
{
	// [DOG] 设置等级（实验性）：思路与声望表移植自 UltimateMenu V3（社区方案，GPL-2.0）
	namespace SetRankFeature
	{
		// 每级所需累计声望（索引 = 等级 - 1，1~98 级；来源：UltimateMenu V3 社区数据）
		static const int kRankRp[] = {
		0, 800, 2100, 3800, 6100, 9500, 12500, 16000, 19800, 24000, 28500, 33400,
		38700, 44200, 50200, 56400, 63000, 69900, 77100, 84700, 92500, 100700, 109200, 118000,
		127100, 136500, 146200, 156200, 166500, 177100, 188000, 199200, 210700, 222400, 234500, 246800,
		259400, 272300, 285500, 299000, 312700, 326800, 341000, 355600, 370500, 385600, 401000, 416600,
		432600, 448800, 465200, 482000, 499000, 516300, 533800, 551600, 569600, 588000, 606500, 625400,
		644500, 663800, 683400, 703300, 723400, 743800, 764500, 785400, 806500, 827900, 849600, 871500,
		893600, 916000, 938700, 961600, 984700, 1008100, 1031800, 1055700, 1079800, 1104200, 1128800, 1153700,
		1178800, 1204200, 1229800, 1255600, 1281700, 1308100, 1334600, 1361400, 1388500, 1415800, 1443300, 1471100,
		1499100, 1527300,
	};

		static int GetRpForLevel(int level)
		{
			constexpr int tableSize = static_cast<int>(sizeof(kRankRp) / sizeof(kRankRp[0]));

			if (level >= 1 && level <= tableSize)
				return kRankRp[level - 1];

			// 社区公式（与官方曲线在 98 级以上一致，100 级 = 1,584,350 已验证）
			return 25 * level * level + 23575 * level - 1023150;
		}

		// 默认给 120 级而非 1 级：防止误点“设置等级”把等级清回 1
		static IntCommand _SetRankLevel{"setranklevel", "目标等级", "要设置的账号等级（1-8000）。", 1, 8000, 120};

		class SetRankApply : public Command
		{
		public:
			using Command::Command;

			virtual void OnCall() override
			{
				const int level = _SetRankLevel.GetState();
				if (level < 1 || level > 8000)
				{
					Notifications::Show("等级设置", "等级需在 1-8000 之间。", NotificationType::Error);
					return;
				}

				const int rp = GetRpForLevel(level);
				Stats::SetInt("MPX_CHAR_SET_RP_GIFT_ADMIN", rp);
				Stats::SetInt("MPPLY_GLOBALXP", rp);

				Notifications::Show("等级设置",
				    "已写入等级 " + std::to_string(level) + " 所需的声望（实验性功能）。切换一次战局后生效；若未变化请反馈。",
				    NotificationType::Success);
			}
		};

		static SetRankApply _SetRankApply{"setrankapply", "设置等级（实验性）", "直接写入目标等级所需声望（社区方案移植）；切换一次战局后生效。"};
	}
}
