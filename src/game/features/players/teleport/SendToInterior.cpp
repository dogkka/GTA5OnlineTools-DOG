#include "game/backend/Self.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "types/script/globals/GlobalPlayerBD.hpp"
#include "types/script/ScriptEvent.hpp"

namespace YimMenu::Features
{
	static std::vector<std::pair<int, const char*>> g_Interiors = {
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_ARMORY_TRUCK_1, "机动作战中心"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_CASINO, "赌场"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_CASINO_APT, "赌场顶层豪宅"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_ARCADE_DAVIS, "游戏厅（戴维斯）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_ARCADE_GRAPESEED, "游戏厅（葡萄籽）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_ARCADE_LA_MESA, "游戏厅（拉梅萨）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_ARCADE_PALETO_BAY, "游戏厅（佩立托湾）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_ARCADE_ROCKFORD_HILLS, "游戏厅（罗克福德山）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_ARCADE_WEST_VINEWOOD, "游戏厅（西好麦坞）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_CASINO_NIGHTCLUB, "音乐柜"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_AUTO_SHOP_BURTON, "改车铺（伯顿）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_AUTO_SHOP_LA_MESA, "改车铺（拉梅萨）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_AUTO_SHOP_MISSION_ROW, "改车铺（教堂区）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_AUTO_SHOP_RANCHO, "改车铺（兰乔）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_AUTO_SHOP_STRAWBERRY, "改车铺（草莓）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_CAR_MEET, "车友会"}, // requires players to have been there at least once on their own
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_FIXER_HQ_HAWICK, "事务所（霍伊克）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_FIXER_HQ_ROCKFORD, "事务所（罗克福德）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_FIXER_HQ_SEOUL, "事务所（小首尔）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_FIXER_HQ_VESPUCCI, "事务所（维斯普奇）"},
	 // {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_ACID_LAB, "Acid Lab"}, stuck in black screen
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_JUGGALO_HIDEOUT, "怪咖屋"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_SALVAGE_YARD_LA_PUERTA, "废车回收场（拉普尔塔）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_SALVAGE_YARD_MURIETTA_HEIGHTS, "废车回收场（穆列塔高地）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_SALVAGE_YARD_PALETO_BAY, "废车回收场（佩立托湾）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_SALVAGE_YARD_SANDY_SHORES, "废车回收场（沙滩海岸）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_SALVAGE_YARD_STRAWBERRY, "废车回收场（草莓）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_BAIL_OFFICE_DAVIS, "保释事务所（戴维斯）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_BAIL_OFFICE_DEL_PERRO, "保释事务所（德佩罗）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_BAIL_OFFICE_MISSION_ROW, "保释事务所（教堂区）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_BAIL_OFFICE_PALETO_BAY, "保释事务所（佩立托湾）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_BAIL_OFFICE_WEST_VINEWOOD, "保释事务所（西好麦坞）"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_HACKER_DEN, "制衣厂"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_FIELD_HANGAR, "田野机库"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_SMALL_BUSINESS_CAR_WASH, "亲力亲为洗车行"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_SMALL_BUSINESS_WEED_SHOP, "喷云吐雾馆"},
	    {(int)eSimpleInteriorIndex::SIMPLE_INTERIOR_SMALL_BUSINESS_HELI_TOURS, "希金斯直升机旅行社"},

	};

	static ListCommand _SelectedInterior{"sendtointeriorindex", "室内地点", "要将玩家送往的室内地点", g_Interiors, (int)eSimpleInteriorIndex::SIMPLE_INTERIOR_ARMORY_TRUCK_1};

	class SendToInterior : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			OnCall(std::vector{player});
		}

		virtual void OnCall(const std::vector<Player>& players) override
		{
			SCRIPT_EVENT_SEND_TO_INTERIOR message;
			message.Interior = _SelectedInterior.GetState();
			message.EnterOwnerInterior = true;
			message.GoonsOnly = false;
			message.InstanceId = 0;
			message.SubInstanceId = -1;
			message.Owner = Self::GetPlayer().GetId();
			message.Distance = 99999;
			message.Position = {0, 0, 0};
			for (auto& player : players)
				message.SetPlayer(player.GetId());
			message.Send();
		}
	};

	static SendToInterior _SendToInterior{"sendtointerior", "送往室内地点", "将玩家送往所选室内地点。"};
}
