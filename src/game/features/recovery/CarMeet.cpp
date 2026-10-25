#include "core/commands/Command.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/util/Joaat.hpp"
#include "game/gta/Natives.hpp"

#include <string>

namespace YimMenu::Features
{
	static IntCommand _CarClubLevel{"carclublevel", "车友会等级", "要设置的车友会声望等级（1-1000）。", 1, 1000, 100};

	// 声望需求公式来自 Stand 参考实现（get_car_club_rep_required_for_level）
	static int RepForLevel(int rank)
	{
		const float tier = (float)rank;
		const float inc  = 2.5f;
		const int rep    = (int)((tier * 100.0f) - 100.0f + (tier - 3.0f) * ((inc * (tier - 3.0f)) + inc));
		return rep < 0 ? 0 : rep;
	}

	// 车友会声望：直接写两个角色槽的 CAR_CLUB_REP 统计
	class SetCarClubRep : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			const int rank = _CarClubLevel.GetState();
			const int rep  = RepForLevel(rank);

			STATS::STAT_SET_INT("MP0_CAR_CLUB_REP"_J, rep, true);
			STATS::STAT_SET_INT("MP1_CAR_CLUB_REP"_J, rep, true);

			Notifications::Show("车友会", "已写入等级 " + std::to_string(rank) + " 的声望（" + std::to_string(rep) + "）。进车友会或再赚一次声望后生效。", NotificationType::Success);
		}
	};

	// 奖品车解锁：参考实现的 CARMEET_PV_CHLLGE_CMPLT 统计
	class UnlockPrizeRide : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			STATS::STAT_SET_BOOL("MP0_CARMEET_PV_CHLLGE_CMPLT"_J, true, true);
			STATS::STAT_SET_BOOL("MP1_CARMEET_PV_CHLLGE_CMPLT"_J, true, true);
			Notifications::Show("车友会", "已写入奖品车挑战完成标记（重新进车友会查看）。", NotificationType::Success);
		}
	};

	static SetCarClubRep _SetCarClubRep{"setcarclublelevel", "设置车友会等级", "直接写入目标等级对应的车友会声望（两个角色槽）。"};
	static UnlockPrizeRide _UnlockPrizeRide{"unlockprizeride", "解锁奖品车", "写入车友会每周奖品车挑战完成标记。"};
}
