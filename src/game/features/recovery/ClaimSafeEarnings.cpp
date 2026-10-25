#include "ClaimSafeEarnings.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/ScriptGlobal.hpp"
#include "game/pointers/Pointers.hpp"
#include "types/script/globals/GPBD_FM.hpp"
#include "types/script/globals/GPBD_FM_2.hpp"

namespace YimMenu::Features
{
	namespace BusinessSafes
	{
		std::optional<BalanceList> GetBalances()
		{
			if (!Pointers.IsSessionStarted || !*Pointers.IsSessionStarted)
				return std::nullopt;

			const auto player = Self::GetPlayer();
			if (!player.IsValid())
				return std::nullopt;

			const int playerId = player.GetId();
			if (playerId < 0 || playerId >= 32)
				return std::nullopt;

			auto gpbdFm = GPBD_FM::Get();
			auto gpbdFm2 = GPBD_FM_2::Get();
			if (!gpbdFm || !gpbdFm2)
				return std::nullopt;

			const auto& propertyData = gpbdFm->Entries[playerId].PropertyData;
			return BalanceList{
			    propertyData.NightclubData.SafeCashValue,
			    propertyData.ArcadeData.SafeCashValue,
			    propertyData.FixerHQData.SafeCashValue,
			    propertyData.SalvageYardData.TotalEarnings,
			    propertyData.BailShopData.SafeCashValue,
			    propertyData.HackerDenData.SafeCashValue,
			    gpbdFm2->Entries[playerId].SYVehSaleData.HOWCData.SafeCashValue,
			};
		}
	}

	static std::vector<std::pair<int, const char*>> businessNames = {
	    {static_cast<int>(BusinessSafes::Business::Nightclub), BusinessSafes::Names[0].data()},
	    {static_cast<int>(BusinessSafes::Business::Arcade), BusinessSafes::Names[1].data()},
	    {static_cast<int>(BusinessSafes::Business::Agency), BusinessSafes::Names[2].data()},
	    {static_cast<int>(BusinessSafes::Business::SalvageYard), BusinessSafes::Names[3].data()},
	    {static_cast<int>(BusinessSafes::Business::BailOffice), BusinessSafes::Names[4].data()},
	    {static_cast<int>(BusinessSafes::Business::GarmentFactory), BusinessSafes::Names[5].data()},
	    {static_cast<int>(BusinessSafes::Business::HandsOnCarWash), BusinessSafes::Names[6].data()},
	};

	static ListCommand _SelectedBusiness{"businesssafe", "产业", "要领取保险箱收益的产业。", businessNames, 0};

	class ClaimSafeEarnings final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!Pointers.IsSessionStarted || !*Pointers.IsSessionStarted)
			{
				Notifications::Show("领取产业保险箱收益", "请先进入 GTA 在线模式。", NotificationType::Error);
				return;
			}

			const int selected = _SelectedBusiness.GetState();
			const auto balances = BusinessSafes::GetBalances();
			if (!balances || selected < 0 || selected >= static_cast<int>(BusinessSafes::Business::Count))
			{
				Notifications::Show("领取产业保险箱收益", "产业余额当前不可访问。", NotificationType::Error);
				return;
			}

			if ((*balances)[selected] <= 0)
			{
				Notifications::Show("领取产业保险箱收益", "所选产业的保险箱当前没有可领取收益。", NotificationType::Warning);
				return;
			}

			static constexpr std::array<int, static_cast<std::size_t>(BusinessSafes::Business::Count)> claimGlobals = {
			    2708943,
			    2708952,
			    2708961,
			    2708970,
			    2708979,
			    2708994,
			    2709001,
			};

			auto claimRequest = ScriptGlobal(claimGlobals[selected]);
			if (!claimRequest.CanAccess())
			{
				Notifications::Show("领取产业保险箱收益", "领取请求全局变量当前不可访问。", NotificationType::Error);
				return;
			}

			*claimRequest.As<BOOL*>() = TRUE;
			LOG(INFO) << "已提交 " << BusinessSafes::Names[selected] << " 保险箱收益领取请求，余额=" << (*balances)[selected];
			Notifications::Show("领取产业保险箱收益", std::format("已提交{}的领取请求。", BusinessSafes::Names[selected]), NotificationType::Success);
		}
	};

	static ClaimSafeEarnings _ClaimSafeEarnings{"claimsafeearnings", "领取产业保险箱收益", "领取所选产业的保险箱收益。"};
}
