#include "core/commands/IntCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "types/script/ScriptEvent.hpp"

namespace YimMenu::Features
{
	static IntCommand _FakeMoneyAmount{"fakemoneyamount", "假钱金额", "假通知里显示的金额", 1, 99999999, 1000000};

	static void SendMoneyEvent(Player player, int64_t event_hash)
	{
		const int id   = player.GetId();
		const int bits = 1 << id;

		int64_t args[9]{};
		args[0] = event_hash;
		args[1] = Self::GetPlayer().GetId();
		args[2] = bits;
		args[3] = _FakeMoneyAmount.GetState();

		SCRIPT::_SEND_TU_SCRIPT_EVENT_NEW(1, args, 9, bits, static_cast<Hash>(args[0]));
	}

	class FakeMoneyBanked : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			SendMoneyEvent(player, static_cast<int64_t>(ScriptEventIndex::NotificationMoneyBanked));
		}
	};

	class FakeMoneyRemoved : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			SendMoneyEvent(player, static_cast<int64_t>(ScriptEventIndex::NotificationMoneyRemoved));
		}
	};

	class FakeMoneyStolen : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			SendMoneyEvent(player, static_cast<int64_t>(ScriptEventIndex::NotificationMoneyStolen));
		}
	};

	static FakeMoneyBanked _FakeMoneyBanked{"fakemoneybanked", "假入账通知", "让目标看到一条巨额入账提示"};
	static FakeMoneyRemoved _FakeMoneyRemoved{"fakemoneyremoved", "假扣款通知", "让目标看到一条巨额扣款提示"};
	static FakeMoneyStolen _FakeMoneyStolen{"fakemoneystolen", "假被抢通知", "让目标看到一条被抢钱提示"};
}
