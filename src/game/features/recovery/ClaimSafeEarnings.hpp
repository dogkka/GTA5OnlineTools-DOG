#pragma once

#include <array>
#include <optional>
#include <string_view>

namespace YimMenu::Features::BusinessSafes
{
	enum class Business
	{
		Nightclub,
		Arcade,
		Agency,
		SalvageYard,
		BailOffice,
		GarmentFactory,
		HandsOnCarWash,
		Count
	};

	inline constexpr std::array<std::string_view, static_cast<std::size_t>(Business::Count)> Names = {
	    "夜总会",
	    "游戏厅",
	    "事务所",
	    "废车回收场",
	    "保释事务所",
	    "制衣厂",
	    "亲力亲为洗车行",
	};

	using BalanceList = std::array<int, static_cast<std::size_t>(Business::Count)>;

	std::optional<BalanceList> GetBalances();
}
