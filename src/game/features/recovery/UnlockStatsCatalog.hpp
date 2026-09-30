#pragma once

#include <array>
#include <cstdint>
#include <string_view>

// Stat catalog adapted from lonelybud/YimMenuV2 at fbf441c8 (GPL-2.0).
// The value representation is local: it avoids one heap allocation per catalog entry.

// notes:
// SET_MP_INT_CHARACTER_STAT = STATS::STAT_SET_INT
// MP_INT_STAT_ = MPX_
// SET_MP_BOOL_AWD_CHARACTER_STAT = STATS::STAT_SET_BOOL
// MP_BOOL_AWARD_ = MPX_AWD_
// SET_MP_INT_AWD_CHARACTER_STAT = STATS::STAT_SET_INT
// MP_INT_AWARD_ = MPX_AWD_

// important links:
// https://www.unknowncheats.me/forum/grand-theft-auto-v/578963-packed-stats-int-bool-collection-thread-25.html#post4361972
// https://www.unknowncheats.me/forum/4367678-post551.html
// https://www.unknowncheats.me/forum/grand-theft-auto-v/578963-packed-stats-int-bool-collection-thread-33.html#post4411199
// https://github.com/YimMenu-Lua/UnlockEverything
// https://www.unknowncheats.me/forum/grand-theft-auto-v/701294-gta-online-enhanced-awards-stats-3.html
// https://www.unknowncheats.me/forum/grand-theft-auto-v/578963-packed-stats-int-bool-collection-thread-45.html#post4540660
// https://www.unknowncheats.me/forum/grand-theft-auto-v/707419-lua-scripts-yimmenuv2-collection-thread-12.html#post4539651
// https://www.unknowncheats.me/forum/grand-theft-auto-v/500059-globals-locals-discussion-read-page-1-a-37.html#post4539636
// https://www.unknowncheats.me/forum/grand-theft-auto-v/701294-gta-online-enhanced-awards-stats-7.html#post4539241
// https://www.unknowncheats.me/forum/grand-theft-auto-v/578963-packed-stats-int-bool-collection-thread-55.html#post4741090
// https://www.unknowncheats.me/forum/grand-theft-auto-v/699943-stats-editor-external-enhanced-31.html#post4742522

// not implemented
// https://www.unknowncheats.me/forum/grand-theft-auto-v/707419-lua-scripts-yimmenuv2-collection-thread-21.html#post4551843

namespace YimMenu::UnlockEverything
{
	enum class StatType
	{
		Int,
		Bool,
		Float,
		PackedBool,
		PackedInt,
		PackedBoolRange,
		IntBit,
		IntBits
	};

	struct StatEntry
	{
		StatType Type;
		std::string_view Name;
		int Value;
		int Extra;
		float FloatValue;

		static constexpr StatEntry Int(std::string_view name, int value)
		{
			return {StatType::Int, name, value, 0, 0.0f};
		}

		static constexpr StatEntry Bool(std::string_view name)
		{
			return {StatType::Bool, name, 0, 0, 0.0f};
		}

		static constexpr StatEntry Float(std::string_view name, float value)
		{
			return {StatType::Float, name, 0, 0, value};
		}

		static constexpr StatEntry PackedBool(int index)
		{
			return {StatType::PackedBool, {}, index, 0, 0.0f};
		}

		static constexpr StatEntry PackedInt(int index, int value)
		{
			return {StatType::PackedInt, {}, index, value, 0.0f};
		}

		static constexpr StatEntry PackedBoolRange(int from, int to)
		{
			return {StatType::PackedBoolRange, {}, from, to, 0.0f};
		}

		static constexpr StatEntry IntBit(std::string_view name, int bit)
		{
			return {StatType::IntBit, name, bit, 0, 0.0f};
		}

		static constexpr StatEntry IntBits(std::string_view name, int firstBit, int lastBit)
		{
			const auto upper = lastBit == 31 ? UINT32_MAX : (std::uint32_t{1} << (lastBit + 1)) - 1;
			const auto lower = firstBit == 0 ? 0 : (std::uint32_t{1} << firstBit) - 1;
			return {StatType::IntBits, name, static_cast<int>(upper ^ lower), 0, 0.0f};
		}
	};

	inline constexpr std::array careerStats = {
	    // OSCAR GUZMAN FLIES AGAIN
	    StatEntry::PackedBool(51280),
	    StatEntry::PackedBool(51285),
	    StatEntry::PackedBool(51278),
	    StatEntry::PackedBoolRange(51286, 51291),
	    StatEntry::Int("MPX_PROG_HUB_MFH_EARNINGS", 5000000),
	    StatEntry::PackedBoolRange(51292, 51297),
	    StatEntry::PackedBool(51279),

	    // THE CLUCKIN' BELL FARM RAID
	    StatEntry::PackedBool(28272),
	    StatEntry::PackedBool(28287),
	    StatEntry::PackedBool(28286),
	    StatEntry::PackedBool(28285),
	    StatEntry::PackedBoolRange(28283, 28284),
	    StatEntry::Int("MPX_PROG_HUB_CBR_EARNINGS", 5000000),
	    StatEntry::PackedBool(28282),
	    StatEntry::PackedBoolRange(28273, 28281),

	    // SAN ANDREAS MERCENARIES
	    StatEntry::IntBits("MPX_SUM23_AVOP_PROGRESS", 0, 11),
	    StatEntry::PackedBool(41566),
	    StatEntry::PackedBool(41706),
	    StatEntry::PackedBoolRange(41567, 41572),
	    StatEntry::PackedBoolRange(41573, 41578),
	    StatEntry::PackedBoolRange(41579, 41584),
	    StatEntry::PackedBoolRange(41585, 41590),

	    // OPERATION PAPER TRAIL
	    StatEntry::IntBits("MPX_ULP_MISSION_PROGRESS", 0, 13),
	    StatEntry::PackedBoolRange(41333, 41338),
	    StatEntry::PackedBool(41707),
	    StatEntry::PackedBoolRange(41339, 41344),
	    StatEntry::PackedBoolRange(41345, 41350),
	    StatEntry::PackedBoolRange(41351, 41356),
	    StatEntry::PackedBoolRange(41357, 41362),

	    // A SUPERYATCH LIFE
	    StatEntry::IntBits("MPX_YACHT_MISSION_FLOW", 4, 15),
	    StatEntry::PackedBool(41379),
	    StatEntry::PackedBool(41708),
	    StatEntry::PackedBoolRange(41380, 41385),
	    StatEntry::PackedBoolRange(41386, 41391),
	    StatEntry::PackedBoolRange(41392, 41397),
	    StatEntry::PackedBoolRange(41398, 41403),

	    // GERALD'S LAST PLAY
	    StatEntry::PackedBoolRange(41404, 41409),
	    StatEntry::PackedBool(41410),
	    StatEntry::PackedBool(41709),
	    StatEntry::PackedBoolRange(41411, 41416),
	    StatEntry::PackedBoolRange(41417, 41422),
	    StatEntry::PackedBoolRange(41423, 41428),
	    StatEntry::PackedBoolRange(41429, 41434),

	    // PREMIUM DELUXE REPO WORK
	    StatEntry::PackedBoolRange(41436, 41443),
	    StatEntry::PackedBool(41435),
	    StatEntry::PackedBool(41705),
	    StatEntry::PackedBoolRange(41444, 41451),
	    StatEntry::PackedBoolRange(41452, 41459),
	    StatEntry::PackedBoolRange(41460, 41467),
	    StatEntry::PackedBoolRange(41468, 41475),

	    // MADRAZO DISPATCH SERVICES
	    StatEntry::PackedBoolRange(41476, 41481),
	    StatEntry::PackedBool(41506),
	    StatEntry::PackedBool(41710),
	    StatEntry::PackedBoolRange(41482, 41487),
	    StatEntry::PackedBoolRange(41488, 41493),
	    StatEntry::PackedBoolRange(41494, 41499),
	    StatEntry::PackedBoolRange(41500, 41505),

	    // LOWRIDERS
	    StatEntry::PackedBoolRange(42015, 42022),
	    StatEntry::PackedBool(41539),
	    StatEntry::PackedBool(41711),
	    StatEntry::PackedBoolRange(41507, 41514),
	    StatEntry::PackedBoolRange(41515, 41522),
	    StatEntry::PackedBoolRange(41523, 41530),
	    StatEntry::PackedBoolRange(41531, 41538),

	    // AGENTS OF SABOTAGE
	    // StatEntry::Int("MPX_HACKER_DEN_OWNED", 1),
	    StatEntry::PackedBool(9539),
	    StatEntry::PackedInt(24903, 25),
	    StatEntry::PackedInt(24904, 2),
	    StatEntry::Int("MPX_PROG_HUB_DEN24_SAFEEARNINGS", 100000),
	    StatEntry::IntBits("MPX_HACKER24_GEN_BS", 5, 12),
	    StatEntry::PackedBool(9549),
	    StatEntry::PackedBool(9542),
	    StatEntry::PackedInt(24905, 9),
	    StatEntry::PackedBoolRange(9543, 9546),

	    // BOTTOM DOLLAR BOUNTIES
	    // StatEntry::Int("MPX_BAIL_OFFICE_OWNED", 1),
	    StatEntry::PackedBool(9537),
	    StatEntry::PackedBool(7639),
	    StatEntry::PackedInt(7669, 5),
	    StatEntry::PackedInt(7672, 2),
	    StatEntry::Int("MPX_PROG_HUB_BOUNTY_EARNINGS", 5000000),
	    StatEntry::PackedInt(7670, 10),
	    StatEntry::PackedInt(7674, 3),
	    StatEntry::PackedInt(7671, 25),
	    StatEntry::PackedInt(26809, 10),
	    StatEntry::Int("MPX_PROG_HUB_BOUNTIES_ALIVE_BS", -1),
	    StatEntry::Int("MPX_BAIL_PROPERTY_EARNINGS", 1000000),

	    // THE CHOP SHOP
	    // StatEntry::Int("MPX_SALVAGE_YARD_OWNED", 1),
	    StatEntry::PackedBool(42038),
	    StatEntry::PackedBool(42041),
	    StatEntry::PackedBool(42044),
	    StatEntry::PackedBool(42045),
	    StatEntry::Int("MPX_PROG_HUB_SALV23_EARN_SALV", 2500000),
	    StatEntry::PackedBool(42042),
	    StatEntry::Int("MPX_PROG_HUB_SALV23_EARN_PERF", 5),
	    StatEntry::PackedBool(42046),
	    StatEntry::PackedInt(51052, 10),
	    StatEntry::PackedBoolRange(42047, 42051),
	    StatEntry::Int("MPX_PROG_HUB_SALV23_EARN_SELL", 10000000),

	    // LOS SANTOS DRUG WARS
	    StatEntry::IntBits("MPX_XM22_MISSIONS_SA", 0, 16),
	    StatEntry::IntBits("MPX_XM22_MISSIONS_SA", 6, 10),
	    StatEntry::IntBits("MPX_XM22_MISSIONS_SA", 17, 21),
	    StatEntry::PackedBool(42037),
	    StatEntry::Int("MPX_LFETIME_BIKER_BUY_COMPLET6", 100),
	    StatEntry::IntBits("MPX_XM22_FLOW", 20, 24),
	    StatEntry::Int("MPX_LIFETIME_BKR_SELL_EARNINGS6", 10000000),
	    StatEntry::IntBit("(MPX_XM22_FLOW", 0),
	    StatEntry::Int("MPX_LFETIME_BIKER_SELL_COMPLET6", 10),
	    StatEntry::PackedInt(41241, 5),
	    StatEntry::IntBit("(MPX_XM22_MISSIONS_SA", 5),
	    StatEntry::IntBit("(MPX_XM22_MISSIONS_SA", 10),
	    StatEntry::IntBit("(MPX_XM22_MISSIONS_SA", 26),
	    StatEntry::Int("MPX_PROG_HUB_LSDW_FJ_NO_DEATHS", 25),
	    StatEntry::Int("MPX_PROG_HUB_DAX_CLONE_KILLS", 100),
	    StatEntry::PackedInt(42084, 24),
	    StatEntry::PackedBoolRange(41660, 41670),

	    // THE CONTRACT
	    StatEntry::PackedBool(28257),
	    StatEntry::Int("MPX_FIXER_COUNT", 100),
	    StatEntry::IntBit("(MPX_FIXER_GENERAL_BS", 0),
	    StatEntry::IntBit("(MPX_FIXER_GENERAL_BS", 2),
	    StatEntry::Int("MPX_PROG_HUB_FXER_PAY_HIT_BONUS", 100),
	    StatEntry::PackedInt(42085, 10),
	    StatEntry::Int("MPX_PROG_HUB_FIXER_SEC_CON_SPEC", 100),
	    StatEntry::Int("MPX_FIXER_EARNINGS", 5000000),

	    // AFTER HOURS
	    StatEntry::PackedBool(22067),
	    StatEntry::Int("MPX_NIGHTCLUB_JOBS_DONE", 25),
	    StatEntry::Int("MPX_PROG_HUB_DANCE_DUR", 5),
	    StatEntry::PackedBool(15533),
	    StatEntry::PackedBoolRange(22082, 22083),
	    StatEntry::PackedBool(36868),
	    StatEntry::PackedBool(36944),
	    StatEntry::PackedBool(41989),
	    StatEntry::PackedInt(42089, 8),
	    StatEntry::Int("MPX_NIGHTCLUB_VIP_APPEAR", 25),
	    StatEntry::Int("MPX_PROG_HUB_NCLUB_POP_MAX_TME", 600),
	    StatEntry::Int("MPX_HUB_EARNINGS", 50000000),

	    // SMUGGLER'S RUN
	    StatEntry::PackedBool(15966),
	    StatEntry::Int("MPX_LFETIME_HANGAR_BUY_COMPLET", 100),
	    StatEntry::PackedBool(41676),
	    StatEntry::PackedBool(32398),
	    StatEntry::Int("MPX_LFETIME_HANGAR_SEL_COMPLET", 1),
	    StatEntry::PackedBool(41987),
	    StatEntry::PackedBool(36924),
	    StatEntry::Int("MPX_PROG_HUB_SMUGGLER_CRATES", 1000),
	    StatEntry::PackedBoolRange(36925, 36932),
	    StatEntry::Int("MPX_LFETIME_HANGAR_EARNINGS", 50000000),

	    // GUNRUNNING
	    // StatEntry::Int("MPX_FACTORYSETUP5", 1),
	    StatEntry::Int("MPX_LFETIME_BIKER_BUY_COMPLET5", 25),
	    StatEntry::PackedInt(9359, 25),
	    StatEntry::PackedBool(36870),
	    StatEntry::Int("MPX_WVM_FLOW_BITSET_MISSIONS0", -1),
	    StatEntry::Int("MPX_LIFETIME_BKR_SEL_COMPLETBC5", 1),
	    StatEntry::Int("MPX_LFETIME_BIKER_SELL_COMPLET5", 1),
	    StatEntry::IntBits("MPX_SR_WEAPON_BIT_SET", 1, 17),
	    StatEntry::PackedBoolRange(42002, 42013),
	    StatEntry::PackedBoolRange(36831, 36838),
	    StatEntry::Int("MPX_LIFETIME_BKR_SELL_EARNINGS5", 25000000),

	    // IMPORT / EXPORT
	    // StatEntry::Int("MPX_OWNED_IE_WAREHOUSE", 1),
	    StatEntry::Int("MPX_LFETIME_IE_EXPORT_COMPLETED", 25),
	    StatEntry::PackedBoolRange(42029, 42036),
	    StatEntry::PackedBoolRange(41874, 41883),
	    StatEntry::Int("MPX_PROG_HUB_VEH_CARGO_SELL_PER", 50),
	    StatEntry::Int("MPX_PROG_HUB_VEH_CARGO_SPECIAL", 5),
	    StatEntry::PackedBool(41988),
	    StatEntry::PackedBoolRange(41540, 41547),
	    StatEntry::Int("MPX_LFETIME_IE_MISSION_EARNINGS", 25000000),

	    // BIKERS
	    StatEntry::PackedBool(36871),
	    StatEntry::PackedBool(36872),
	    StatEntry::PackedBoolRange(36875, 36887),
	    StatEntry::PackedBool(36873),
	    StatEntry::PackedBool(42001),
	    StatEntry::PackedBool(36874),
	    StatEntry::Int("MPX_LIFETIME_BKR_SELL_COMPLETBC", 1),
	    StatEntry::Int("MPX_LIFETIME_BKR_SEL_COMPLETBC1", 1),
	    StatEntry::Int("MPX_LIFETIME_BKR_SEL_COMPLETBC2", 1),
	    StatEntry::Int("MPX_LIFETIME_BKR_SEL_COMPLETBC3", 1),
	    StatEntry::Int("MPX_LIFETIME_BKR_SEL_COMPLETBC4", 1),
	    StatEntry::Int("MPX_BAR_RESUPPLY_CR", 10),
	    StatEntry::Int("MPX_LIFETIME_BKR_SELL_EARNINGS0", 25000000),
	    StatEntry::Int("MPX_PROG_HUB_BIK_CUST_DEL_CASH", 2500000),
	    StatEntry::Int("MPX_PROG_HUB_CLBH_BAR_EARNINGS", 500000),
	    StatEntry::Int("MPX_PROG_HUB_BIK_CONTRACT_COUNT", 50),

	    // FURTHER ADVENTURES IN FINANCE AND FELONY
	    StatEntry::PackedBool(36888),
	    StatEntry::PackedBool(36889),
	    StatEntry::PackedBoolRange(36892, 36915),
	    StatEntry::PackedBool(36890),
	    StatEntry::Int("MPX_WARHOUSESLOT0", 1),
	    StatEntry::Int("MPX_LIFETIME_BUY_COMPLETE", 1),
	    StatEntry::PackedBoolRange(7553, 7559),
	    StatEntry::PackedBool(36891),
	    StatEntry::PackedBoolRange(36860, 36865),
	    StatEntry::Int("MPX_PROG_HUB_FAIFAF_CRATES_COL", 250),
	    StatEntry::Int("MPX_LIFETIME_CONTRA_EARNINGS", 50000000),

	    // LOS SANTOS TUNERS
	    // StatEntry::Int("MPX_CAR_CLUB_MEMBERSHIP", 1),
	    StatEntry::PackedBool(31737),
	    StatEntry::PackedBool(41870),
	    StatEntry::PackedBool(31753),
	    StatEntry::IntBits("MPX_TUNER_COMP_BS", 0, 7),
	    StatEntry::PackedBool(32397),
	    StatEntry::PackedInt(30226, 10),
	    StatEntry::Int("MPX_TUNER_COUNT", 25),
	    StatEntry::Int("MPX_PROG_HUB_TUNER_CUS_DEL_CASH", 5000000),

	    // THE DIAMOND CASINO & RESORT
	    StatEntry::PackedBool(27089),
	    StatEntry::PackedBool(27090),
	    StatEntry::IntBit("(MPX_VCM_FLOW_PROGRESS", 6),
	    StatEntry::IntBit("(MPX_VCM_FLOW_PROGRESS", 12),
	    StatEntry::IntBit("(MPX_VCM_FLOW_PROGRESS", 11),
	    StatEntry::IntBit("(MPX_VCM_FLOW_PROGRESS", 17),
	    StatEntry::PackedBool(36916),
	    StatEntry::PackedBoolRange(36844, 36859),
	    StatEntry::PackedBoolRange(41548, 41553),
	    StatEntry::PackedBool(41868),
	    StatEntry::PackedInt(42093, 11),
	    StatEntry::PackedBoolRange(41560, 41565),
	    StatEntry::PackedBoolRange(41554, 41559),

	    // THE CAYO PERICO HEIST
	    StatEntry::PackedBool(30309),
	    StatEntry::PackedBool(30522),
	    StatEntry::IntBit("MPX_H4_MISSIONS", 0),
	    StatEntry::IntBit("(MPX_H4_PROGRESS", 1),
	    StatEntry::IntBits("MPX_H4_PROGRESS", 6, 11),
	    StatEntry::IntBits("MPX_H4_H4_DJ_MISSIONS", 0, 6),
	    StatEntry::PackedBool(41677),
	    StatEntry::Int("MPX_PROG_HUB_CAYO_PRP_NO_DEATHS", 25),
	    StatEntry::PackedInt(42094, 15),
	    StatEntry::Int("MPX_CR_SUBMARINE", 1),
	    StatEntry::Int("MPX_CR_STRATEGIC_BOMBER", 1),
	    StatEntry::Int("MPX_CR_SMUGGLER_PLANE", 1),
	    StatEntry::Int("MPX_CR_STEALTH_HELI", 1),
	    StatEntry::Int("MPX_CR_PATROL_BOAT", 1),
	    StatEntry::Int("MPX_CR_SMUGGLER_BOAT", 1),
	    StatEntry::Int("MPX_PROG_HUB_CAYO_H_EARNINGS", 50000000),

	    // THE DIAMOND CASINO HEIST

	    StatEntry::IntBit("(MPX_CAS_HEIST_FLOW", 1),
	    StatEntry::PackedBool(28270),
	    StatEntry::PackedBool(36842),
	    StatEntry::IntBit("(MPX_CAS_HEIST_FLOW", 10),
	    StatEntry::PackedBool(32399),
	    StatEntry::IntBits("MPX_CAS_HEIST_FLOW", 11, 13),
	    StatEntry::PackedBool(42025),
	    StatEntry::PackedBool(42086),
	    StatEntry::PackedBoolRange(41678, 41681),
	    StatEntry::Int("MPX_PROG_HUB_CASINO_H_EARNINGS", 50000000),

	    // THE DOOMSDAY HEIST
	    StatEntry::PackedBool(18139),
	    StatEntry::PackedBool(36861),
	    StatEntry::IntBits("MPX_GANGOPS_FLOW_BITSET_MISS0", 0, 2),
	    StatEntry::PackedBool(41712),
	    StatEntry::PackedBool(36862),
	    StatEntry::PackedBool(41713),
	    StatEntry::PackedBool(36863),
	    StatEntry::PackedBool(41714),
	    StatEntry::Int("MPX_PROG_HUB_DOOM_PRP_NO_DEATHS", 14),
	    StatEntry::PackedInt(42087, 10),
	    StatEntry::PackedBool(41685),
	    StatEntry::PackedBool(41690),
	    StatEntry::PackedBool(41696),
	    StatEntry::PackedBoolRange(41697, 41699),
	    StatEntry::Int("MPX_PROG_HUB_DOOMSDAY_ACTS", 15),

	    // ORIGINAL HEISTS
	    StatEntry::PackedBool(36867),
	    StatEntry::PackedBool(36933),
	    StatEntry::PackedBool(41700),
	    StatEntry::PackedBool(41715),
	    StatEntry::PackedBool(41716),
	    StatEntry::PackedBool(41717),
	    StatEntry::PackedBool(41718),
	    StatEntry::PackedBool(41719),
	    StatEntry::PackedInt(42100, 5),
	    StatEntry::PackedInt(42090, 15),
	    StatEntry::Int("MPX_PROG_HUB_HEIST_EARNINGS", 50000000),
	    StatEntry::PackedBoolRange(41701, 41704),
	    StatEntry::PackedBool(36917),
	    StatEntry::PackedBool(42000),

	    // ARENA WAR
	    StatEntry::PackedBool(25009),
	    StatEntry::PackedBoolRange(41647, 41655),
	    StatEntry::PackedInt(22063, 20),
	    StatEntry::Int("MPX_ARENAWARS_AP_TIER", 1000),
	    StatEntry::PackedInt(42088, 15),
	    StatEntry::PackedInt(42000, 20),

	    // ADVERSARY MODS
	    StatEntry::PackedBoolRange(41594, 41646),
	    StatEntry::PackedBool(36840),
	    StatEntry::Int("MPX_PROG_HUB_ADV_WINS", 50),
	    StatEntry::PackedBool(36921),

	    // SURVIVALS
	    StatEntry::Int("MPX_PROG_HUB_SURVIVALS_PLAYED", 1),
	    StatEntry::PackedBool(41672),
	    StatEntry::Int("MPX_PROG_HUB_SURV_WAVES", 250),
	    StatEntry::PackedBool(41673),
	    StatEntry::PackedBool(41332),
	    StatEntry::PackedBool(41331),

	    // RACING
	    StatEntry::PackedBool(42023),
	    // These distant endpoints were a single range upstream, which would overwrite 787 unrelated packed stats.
	    StatEntry::PackedBool(41363),
	    StatEntry::PackedBool(42151),
	    StatEntry::Int("MPX_RACES_WON", 50),
	    StatEntry::IntBits("MPX_PROG_HUB_T_TRIAL_PAR_TIME", 0, 25),
	    StatEntry::IntBits("MPX_PROG_HUB_T_TRL_PAR_TIME_RC", 0, 25),
	    StatEntry::IntBits("MPX_PROG_HUB_T_TRL_PAR_TIME_HSW", 0, 25),
	    StatEntry::IntBits("MPX_PROG_HUB_T_TRL_PAR_TIME_BKE", 0, 25),
	    StatEntry::PackedInt(41246, 20),
	    StatEntry::Int("MPX_PROG_HUB_FST_LPS_RSTAR_RAC", 50),


	    // DEATHMATCHES
	    StatEntry::Int("MPX_DM_END", 1),
	    StatEntry::Int("MPX_PROG_HUB_DM_TDM_PLAYS", 1),
	    StatEntry::Int("MPX_PROG_HUB_DM_COMMUNITY_PLAYS", 10),
	    StatEntry::Int("MPX_PROG_HUB_DTHM_KILL_5_WO_DIE", 25),
	    StatEntry::PackedBool(36922),

	    // VEHICLE ENTHUSIAST
	    StatEntry::PackedBool(41864),
	    StatEntry::PackedBool(42014),
	    StatEntry::PackedBool(41865),
	    StatEntry::PackedBool(41863),
	    StatEntry::PackedBool(41840),
	    StatEntry::PackedBool(41839),
	    StatEntry::PackedBool(41841),
	    StatEntry::PackedBool(41838),
	    StatEntry::PackedInt(42092, 5),
	    StatEntry::PackedInt(42091, 10),
	    StatEntry::Int("MPX_HUB_VEH_ENTH_OWNED_VEHS", 99),
	    StatEntry::PackedBool(41866),
	    StatEntry::PackedBoolRange(41842, 41862),
	    StatEntry::Int("MPX_VEHICLES_CUSTOMISED", 50),

	    // WEAPONS EXPERT
	    StatEntry::PackedBool(36934),
	    StatEntry::PackedBool(36935),
	    StatEntry::PackedBool(36936),
	    StatEntry::PackedBool(36937),
	    StatEntry::PackedBool(36938),
	    StatEntry::PackedBool(36920),
	    StatEntry::Int("MPX_PROG_HUB_WEAP_TYPE_ANSR", -1),
	    StatEntry::PackedInt(41242, 8),
	    StatEntry::PackedBool(36942),
	    StatEntry::Int("MPX_PROG_HUB_10_CHAL_ANSR", 10),
	    StatEntry::PackedBool(36941),
	    StatEntry::PackedBoolRange(15456, 15460),

	    // A Safehouse In The Hills
	    StatEntry::PackedBool(54773),
	    StatEntry::PackedBool(54774),
	    StatEntry::PackedBool(54775),
	    StatEntry::PackedBoolRange(54776, 54780),
	    StatEntry::Int("MPX_PROG_HUB_KNOWAY_EARNINGS", 5000000),
	    StatEntry::PackedBoolRange(54781, 54785),
	    StatEntry::PackedBool(54786),

	    // The Kortz Center Heist
	    StatEntry::PackedBool(60011),
	    StatEntry::PackedBool(60021),
	    StatEntry::IntBit("MPX_K26_GENERAL_BS", 2),
	    StatEntry::Int("MPX_AWD_PREPPER", 20),
	    StatEntry::PackedBool(60022),
	    StatEntry::PackedBool(60023),
	    StatEntry::PackedInt(60049, 5),
	    StatEntry::PackedBoolRange(60024, 60027),
	    StatEntry::PackedInt(60050, 15),
	    StatEntry::PackedBool(60028),
	    StatEntry::Int("MPX_PROG_HUB_KORTZ_STOLEN_VALUE", 50000000),
	    StatEntry::IntBits("MPX_K26_TARGETS_OWNED_BS", 1, 26), // New Mansion Paintings
	};

	inline constexpr std::array awardStats = {
	    // Victory
	    StatEntry::Int("MPX_AWD_FM_DM_WINS", 50),
	    StatEntry::Int("MPX_AWD_FM_TDM_WINS", 50),
	    StatEntry::Int("MPX_AWD_FM_TDM_MVP", 50),
	    StatEntry::Int("MPX_AWD_RACES_WON", 50),
	    StatEntry::Int("MPX_AWD_FMWINAIRRACE", 25),
	    StatEntry::Int("MPX_AWD_FMWINSEARACE", 25),
	    StatEntry::Int("MPX_AWD_FM_GTA_RACES_WON", 50),
	    StatEntry::Bool("MPX_AWD_FMKILL3ANDWINGTARACE"),
	    StatEntry::Int("MPX_AWD_FMRALLYWONDRIVE", 25),
	    StatEntry::Int("MPX_AWD_FMRALLYWONNAV", 25),
	    StatEntry::Int("MPX_AWD_FMWINRACETOPOINTS", 25),
	    StatEntry::Bool("MPX_AWD_FMWINCUSTOMRACE"),
	    StatEntry::Int("MPX_AWD_FM_RACE_LAST_FIRST", 25),
	    StatEntry::Bool("MPX_AWD_FMRACEWORLDRECHOLDER"),
	    StatEntry::Int("MPX_AWD_FM_RACES_FASTEST_LAP", 101),
	    StatEntry::Bool("MPX_AWD_FMWINALLRACEMODES"),
	    StatEntry::Int("MPX_AWD_FMHORDWAVESSURVIVE", 10),
	    StatEntry::Int("MPX_MOST_ARM_WRESTLING_WINS", 25),
	    StatEntry::Int("MPX_AWD_WIN_AT_DARTS", 25),
	    StatEntry::Int("MPX_AWD_FM_GOLF_WON", 25),
	    StatEntry::Int("MPX_AWD_FM_TENNIS_WON", 25),
	    StatEntry::Bool("MPX_AWD_FM_TENNIS_5_SET_WINS"),
	    StatEntry::Bool("MPX_AWD_FM_TENNIS_STASETWIN"),
	    StatEntry::Int("MPX_AWD_FM_SHOOTRANG_TG_WON", 25),
	    StatEntry::Int("MPX_AWD_FM_SHOOTRANG_CT_WON", 25),
	    StatEntry::Bool("MPX_AWD_FM_SHOOTRANG_GRAN_WON"),
	    StatEntry::Int("MPX_AWD_FM_SHOOTRANG_RT_WON", 25),
	    StatEntry::Bool("MPX_AWD_FMWINEVERYGAMEMODE"),
	    StatEntry::Int("MPX_AWD_WIN_CAPTURES", 50),
	    StatEntry::Int("MPX_AWD_WIN_CAPTURE_DONT_DYING", 25),
	    StatEntry::Int("MPX_AWD_WIN_LAST_TEAM_STANDINGS", 50),
	    StatEntry::Int("MPX_AWD_ONLY_PLAYER_ALIVE_LTS", 50),
	    StatEntry::Int("MPX_AWD_KILL_TEAM_YOURSELF_LTS", 25),

	    // General
	    StatEntry::Bool("MPX_AWD_FM25DIFFERENTDM"),
	    StatEntry::Bool("MPX_AWD_FM25DIFFERENTRACES"),
	    StatEntry::Int("MPX_AWD_PARACHUTE_JUMPS_20M", 25),
	    StatEntry::Int("MPX_AWD_PARACHUTE_JUMPS_50M", 25),
	    StatEntry::Int("MPX_AWD_FMBASEJMP", 25),
	    StatEntry::Bool("MPX_AWD_FMATTGANGHQ"),
	    StatEntry::Bool("MPX_AWD_FM6DARTCHKOUT"),
	    StatEntry::Int("MPX_AWD_FM_GOLF_BIRDIES", 25),
	    StatEntry::Bool("MPX_AWD_FM_GOLF_HOLE_IN_1"),
	    StatEntry::Int("MPX_AWD_FM_TENNIS_ACE", 25),
	    StatEntry::Int("MPX_AWD_FMBBETWIN", 50000),
	    StatEntry::Int("MPX_AWD_LAPDANCES", 25),
	    StatEntry::Bool("MPX_AWD_FM25DIFITEMSCLOTHES"),
	    StatEntry::Int("MPX_AWD_NO_HAIRCUTS", 25),
	    StatEntry::Bool("MPX_AWD_BUY_EVERY_GUN"),
	    StatEntry::Bool("MPX_AWD_FMTATTOOALLBODYPARTS"),
	    StatEntry::Int("MPPLY_AWD_FM_CR_DM_MADE", 25),
	    StatEntry::Int("MPPLY_AWD_FM_CR_RACES_MADE", 25),
	    StatEntry::Int("MPPLY_AWD_FM_CR_PLAYED_BY_PEEP", 100),
	    StatEntry::Int("MPPLY_AWD_FM_CR_MISSION_SCORE", 100),
	    StatEntry::Int("MPX_AWD_DROPOFF_CAP_PACKAGES", 100),
	    StatEntry::Int("MPX_AWD_PICKUP_CAP_PACKAGES", 100),
	    StatEntry::Int("MPX_AWD_MENTALSTATE_TO_NORMAL", 50),
	    StatEntry::Bool("MPX_AWD_STORE_20_CAR_IN_GARAGES"),
	    StatEntry::Int("MPX_AWD_TRADE_IN_YOUR_PROPERTY", 25),

	    // Crimes
	    StatEntry::Int("MPX_CHAR_WANTED_LEVEL_TIME5STAR", 18000000),
	    StatEntry::Int("MPX_AWD_5STAR_WANTED_AVOIDANCE", 50),
	    StatEntry::Int("MPX_AWD_FMSHOOTDOWNCOPHELI", 25),
	    StatEntry::Int("MPX_PASS_DB_PLAYER_KILLS", 100),
	    StatEntry::Int("MPX_NUMBER_STOLEN_CARS", 521),
	    StatEntry::Int("MPX_AWD_HOLD_UP_SHOPS", 20),

	    // Vehicles
	    StatEntry::Int("MPX_CARS_EXPLODED", 500),
	    StatEntry::Int("MPX_AWD_CARS_EXPORTED", 50),
	    StatEntry::Int("MPX_AWD_FMDRIVEWITHOUTCRASH", 30),
	    StatEntry::Int("MPX_AWD_PASSENGERTIME", 4),
	    StatEntry::Int("MPX_AWD_TIME_IN_HELICOPTER", 4),
	    StatEntry::Bool("MPX_AWD_FMFULLYMODDEDCAR"),
	    StatEntry::Int("MPX_AIR_LAUNCHES_OVER_40M", 25),
	    StatEntry::Int("MPX_MOST_FLIPS_IN_ONE_JUMP", 5),
	    StatEntry::Int("MPX_MOST_SPINS_IN_ONE_JUMP", 5),
	    StatEntry::Bool("MPX_AWD_FMFURTHESTWHEELIE"),

	    // Combat
	    StatEntry::Int("MPX_AWD_100_HEADSHOTS", 501),
	    StatEntry::Int("MPX_KILLS_PLAYERS", 1000),
	    StatEntry::Int("MPX_AWD_FMKILLBOUNTY", 25),
	    StatEntry::Int("MPX_AWD_FMREVENGEKILLSDM", 50),
	    StatEntry::Int("MPX_AWD_FM_DM_KILLSTREAK", 100),
	    StatEntry::Int("MPX_AWD_FM_DM_STOLENKILL", 50),
	    StatEntry::Int("MPX_AWD_FM_DM_TOTALKILLS", 500),
	    StatEntry::Bool("MPX_AWD_FMKILLSTREAKSDM"),
	    StatEntry::Bool("MPX_AWD_FMMOSTKILLSGANGHIDE"),
	    StatEntry::Bool("MPX_AWD_FMMOSTKILLSSURVIVE"),
	    StatEntry::Int("MPX_AWD_FM_DM_3KILLSAMEGUY", 50),
	    StatEntry::Int("MPX_AWD_KILL_CARRIER_CAPTURE", 100),
	    StatEntry::Int("MPX_AWD_NIGHTVISION_KILLS", 100),
	    StatEntry::Int("MPX_AWD_KILL_PSYCHOPATHS", 100),
	    StatEntry::Int("MPX_PISTOL50_ENEMY_KILLS", 500),
	    StatEntry::Int("MPX_AWD_100_KILLS_SMG", 500),
	    StatEntry::Int("MPX_ASLTSHTGN_ENEMY_KILLS", 500),
	    StatEntry::Int("MPX_ASLTRIFLE_ENEMY_KILLS", 500),
	    StatEntry::Int("MPX_SNIPERRFL_ENEMY_KILLS", 501),
	    StatEntry::Int("MPX_CMBTMG_ENEMY_KILLS", 500),
	    StatEntry::Int("MPX_AWD_25_KILLS_STICKYBOMBS", 51),
	    StatEntry::Int("MPX_GRENADE_ENEMY_KILLS", 50),
	    StatEntry::Int("MPX_RPG_ENEMY_KILLS", 500),
	    StatEntry::Int("MPX_UNARMED_ENEMY_KILLS", 50),
	    StatEntry::Int("MPX_AWD_CAR_BOMBS_ENEMY_KILLS", 25),

	    // Heists
	    StatEntry::Int("MPX_AWD_FINISH_HEISTS", 50),
	    StatEntry::Int("MPX_AWD_FINISH_HEIST_SETUP_JOB", 50),
	    StatEntry::Bool("MPX_AWD_FINISH_HEIST_NO_DAMAGE"),
	    StatEntry::Int("MPX_AWD_WIN_GOLD_MEDAL_HEISTS", 25),
	    StatEntry::Int("MPX_AWD_DO_HEIST_AS_THE_LEADER", 25),
	    StatEntry::Bool("MPX_AWD_SPLIT_HEIST_TAKE_EVENLY"),
	    StatEntry::Bool("MPX_AWD_ACTIVATE_2_PERSON_KEY"),
	    StatEntry::Int("MPX_AWD_CONTROL_CROWDS", 25),
	    StatEntry::Bool("MPX_AWD_ALL_ROLES_HEIST"),
	    StatEntry::Bool("MPPLY_AWD_FLEECA_FIN"),
	    StatEntry::Bool("MPPLY_AWD_PRISON_FIN"),
	    StatEntry::Bool("MPPLY_AWD_HUMANE_FIN"),
	    StatEntry::Bool("MPPLY_AWD_SERIESA_FIN"),
	    StatEntry::Bool("MPPLY_AWD_PACIFIC_FIN"),
	    StatEntry::Bool("MPPLY_AWD_HST_ORDER"),
	    StatEntry::Bool("MPPLY_AWD_HST_SAME_TEAM"),
	    StatEntry::Bool("MPPLY_AWD_HST_ULT_CHAL"),
	    StatEntry::Bool("MPPLY_AWD_COMPLET_HEIST_MEM"),
	    StatEntry::Bool("MPPLY_AWD_COMPLET_HEIST_1STPER"),

	    // The Doomsday Heist
	    StatEntry::Bool("MPPLY_AWD_GANGOPS_IAA"),
	    StatEntry::Bool("MPPLY_AWD_GANGOPS_SUBMARINE"),
	    StatEntry::Bool("MPPLY_AWD_GANGOPS_MISSILE"),
	    StatEntry::Bool("MPPLY_AWD_GANGOPS_ALLINORDER"),
	    StatEntry::Bool("MPPLY_AWD_GANGOPS_LOYALTY"),
	    StatEntry::Bool("MPPLY_AWD_GANGOPS_LOYALTY2"),
	    StatEntry::Bool("MPPLY_AWD_GANGOPS_LOYALTY3"),
	    StatEntry::Bool("MPPLY_AWD_GANGOPS_CRIMMASMD"),
	    StatEntry::Bool("MPPLY_AWD_GANGOPS_CRIMMASMD2"),
	    StatEntry::Bool("MPPLY_AWD_GANGOPS_CRIMMASMD3"),
	    StatEntry::Bool("MPPLY_AWD_GANGOPS_SUPPORT"),

	    // After Hours
	    StatEntry::Int("MPX_AWD_CLUB_DRUNK", 200),
	    StatEntry::Int("MPX_DANCEPERFECTOWNCLUB", 100),
	    StatEntry::Int("MPX_NIGHTCLUB_PLAYER_APPEAR", 500),
	    StatEntry::Int("MPX_AWD_DANCE_TO_SOLOMUN", 100),
	    StatEntry::Int("MPX_AWD_DANCE_TO_TALEOFUS", 100),
	    StatEntry::Int("MPX_AWD_DANCE_TO_DIXON", 100),
	    StatEntry::Int("MPX_AWD_DANCE_TO_BLKMAD", 100),
	    StatEntry::Int("MPX_DANCETODIFFDJS", 4),

	    // Arena War --
	    StatEntry::Int("MPX_AWD_CAREER_WINNER", 1000),
	    StatEntry::Int("MPX_AWD_TOP_SCORE", 500000),
	    StatEntry::Int("MPX_AWD_TIME_SERVED", 1000),
	    StatEntry::Int("MPX_AWD_ARENA_WAGEWORKER", 6785341),
	    StatEntry::Int("MPX_AWD_WEVE_GOT_ONE", 60),
	    StatEntry::Int("MPX_AWD_YOURE_OUTTA_HERE", 220),
	    StatEntry::Int("MPX_AWD_MASSIVE_SHUNT", 60),
	    StatEntry::Int("MPX_AWD_KILL_OR_BE_KILLED", 60),
	    StatEntry::Int("MPX_AWD_CROWDPARTICIPATION", 80),
	    StatEntry::Int("MPX_AWD_SITTING_DUCK", 50),
	    StatEntry::Int("MPX_AWD_YOUMEANBOOBYTRAPS", 50),
	    StatEntry::Int("MPX_AWD_MASTER_BANDITO", 50),
	    StatEntry::Int("MPX_AWD_SPINNER", 80),
	    StatEntry::Int("MPX_AWD_THROUGH_A_LENS", 60),
	    StatEntry::Int("MPX_AWD_READY_FOR_WAR", 50),
	    StatEntry::Bool("MPX_AWD_UNSTOPPABLE"),
	    StatEntry::Bool("MPX_AWD_CONTACT_SPORT"),
	    StatEntry::Int("MPX_AWD_TOWER_OFFENSE", 50),
	    StatEntry::Int("MPX_AWD_WATCH_YOUR_STEP", 50),
	    StatEntry::Bool("MPX_AWD_PEGASUS"),
	    StatEntry::Bool("MPX_AWD_BEGINNER"),
	    StatEntry::Bool("MPX_AWD_FIELD_FILLER"),
	    StatEntry::Bool("MPX_AWD_ARMCHAIR_RACER"),
	    StatEntry::Bool("MPX_AWD_LEARNER"),
	    StatEntry::Bool("MPX_AWD_SUNDAY_DRIVER"),
	    StatEntry::Bool("MPX_AWD_THE_ROOKIE"),
	    StatEntry::Bool("MPX_AWD_BUMP_AND_RUN"),
	    StatEntry::Bool("MPX_AWD_GEAR_HEAD"),
	    StatEntry::Bool("MPX_AWD_DOOR_SLAMMER"),
	    StatEntry::Bool("MPX_AWD_HOT_LAP"),
	    StatEntry::Bool("MPX_AWD_ARENA_AMATEUR"),
	    StatEntry::Bool("MPX_AWD_PAINT_TRADER"),
	    StatEntry::Bool("MPX_AWD_SHUNTER"),
	    StatEntry::Bool("MPX_AWD_JOCK"),
	    StatEntry::Bool("MPX_AWD_WARRIOR"),
	    StatEntry::Bool("MPX_AWD_T_BONE"),
	    StatEntry::Bool("MPX_AWD_MAYHEM"),
	    StatEntry::Bool("MPX_AWD_WRECKER"),
	    StatEntry::Bool("MPX_AWD_CRASH_COURSE"),
	    StatEntry::Bool("MPX_AWD_ARENA_LEGEND"),

	    // The Diamond Casino & Resort
	    StatEntry::Bool("MPX_AWD_FIRST_TIME1"),
	    StatEntry::Bool("MPX_AWD_FIRST_TIME2"),
	    StatEntry::Bool("MPX_AWD_FIRST_TIME3"),
	    StatEntry::Bool("MPX_AWD_FIRST_TIME4"),
	    StatEntry::Bool("MPX_AWD_FIRST_TIME5"),
	    StatEntry::Bool("MPX_AWD_FIRST_TIME6"),
	    StatEntry::Bool("MPX_AWD_ALL_IN_ORDER"),
	    StatEntry::Bool("MPX_AWD_SURVIVALIST"),
	    StatEntry::Bool("MPX_AWD_SUPPORTING_ROLE"),
	    StatEntry::Bool("MPX_AWD_LEADER"),
	    StatEntry::Int("MPX_AWD_ODD_JOBS", 100),

	    // Diamond Casino Heist
	    StatEntry::Bool("MPX_AWD_SCOPEOUT"),
	    StatEntry::Int("MPX_AWD_PREPARATION", 40),
	    StatEntry::Bool("MPX_AWD_CREWEDUP"),
	    StatEntry::Bool("MPX_AWD_MOVINGON"),
	    StatEntry::Bool("MPX_AWD_PROMOCAMP"),
	    StatEntry::Bool("MPX_AWD_GUNMAN"),
	    StatEntry::Bool("MPX_AWD_SMASHNGRAB"),
	    StatEntry::Bool("MPX_AWD_INPLAINSI"),
	    StatEntry::Bool("MPX_AWD_UNDETECTED"),
	    StatEntry::Bool("MPX_AWD_ALLROUND"),
	    StatEntry::Bool("MPX_AWD_ELITETHEIF"),
	    StatEntry::Bool("MPX_AWD_PRO"),
	    StatEntry::Bool("MPX_AWD_SUPPORTACT"),
	    StatEntry::Bool("MPX_AWD_SHAFTED"),
	    StatEntry::Int("MPX_AWD_ASLEEPONJOB", 20),
	    StatEntry::Int("MPX_AWD_DAICASHCRAB", 100000),
	    StatEntry::Int("MPX_AWD_BIGBRO", 40),
	    StatEntry::Bool("MPX_AWD_COLLECTOR"),
	    StatEntry::Bool("MPX_AWD_DEADEYE"),
	    StatEntry::Bool("MPX_AWD_PISTOLSATDAWN"),
	    StatEntry::Int("MPX_AWD_SHARPSHOOTER", 40),
	    StatEntry::Int("MPX_AWD_RACECHAMP", 40),
	    StatEntry::Bool("MPX_AWD_TRAFFICAVOI"),
	    StatEntry::Bool("MPX_AWD_CANTCATCHBRA"),
	    StatEntry::Bool("MPX_AWD_WIZHARD"),
	    StatEntry::Int("MPX_AWD_BATSWORD", 1000000),
	    StatEntry::Int("MPX_AWD_COINPURSE", 950000),
	    StatEntry::Bool("MPX_AWD_APEESCAPE"),
	    StatEntry::Bool("MPX_AWD_MONKEYKIND"),
	    StatEntry::Int("MPX_AWD_ASTROCHIMP", 3000000),
	    StatEntry::Bool("MPX_AWD_AQUAAPE"),
	    StatEntry::Bool("MPX_AWD_KEEPFAITH"),
	    StatEntry::Int("MPX_AWD_MASTERFUL", 40000),
	    StatEntry::Bool("MPX_AWD_TRUELOVE"),
	    StatEntry::Bool("MPX_AWD_NEMESIS"),
	    StatEntry::Bool("MPX_AWD_FRIENDZONED"),

	    // Los Santos Summer Special
	    StatEntry::Bool("MPX_AWD_KINGOFQUB3D"),
	    StatEntry::Bool("MPX_AWD_QUBISM"),
	    StatEntry::Bool("MPX_AWD_GODOFQUB3D"),
	    StatEntry::Bool("MPX_AWD_QUIBITS"),
	    StatEntry::Bool("MPX_AWD_ELEVENELEVEN"),
	    StatEntry::Bool("MPX_AWD_GOFOR11TH"),

	    // The Cayo Perico Heist
	    StatEntry::Bool("MPX_AWD_INTELGATHER"),
	    StatEntry::Bool("MPX_AWD_COMPOUNDINFILT"),
	    StatEntry::Int("MPX_AWD_WELL_PREPARED", 50),
	    StatEntry::Bool("MPX_AWD_LOOT_FINDER"),
	    StatEntry::Bool("MPX_AWD_MAX_DISRUPT"),
	    StatEntry::Bool("MPX_AWD_THE_ISLAND_HEIST"),
	    StatEntry::Bool("MPX_AWD_GOING_ALONE"),
	    StatEntry::Bool("MPX_AWD_TEAM_WORK"),
	    StatEntry::Bool("MPX_AWD_MIXING_UP"),
	    StatEntry::Bool("MPX_AWD_PRO_THIEF"),
	    StatEntry::Bool("MPX_AWD_CAT_BURGLAR"),
	    StatEntry::Bool("MPX_AWD_ONE_OF_THEM"),
	    StatEntry::Int("MPX_AWD_FILL_YOUR_BAGS", 20000000),
	    StatEntry::Bool("MPX_AWD_GOLDEN_GUN"),
	    StatEntry::Bool("MPX_AWD_ELITE_THIEF"),
	    StatEntry::Bool("MPX_AWD_PROFESSIONAL"),
	    StatEntry::Int("MPX_AWD_WRECK_DIVING", 1000000),
	    StatEntry::Bool("MPX_AWD_PARTY_VIBES"),
	    StatEntry::Int("MPX_AWD_SUNSET", 1800000),
	    StatEntry::Bool("MPX_AWD_HELPING_HAND"),
	    StatEntry::Int("MPX_AWD_MOODYMANN", 1800000),
	    StatEntry::Int("MPX_AWD_PALMS_TRAX", 1800000),
	    StatEntry::Bool("MPX_AWD_HELPING_OUT"),
	    StatEntry::Int("MPX_AWD_KEINEMUSIK", 1800000),
	    StatEntry::Bool("MPX_AWD_COURIER"),
	    StatEntry::Int("MPX_AWD_TREASURE_HUNTER", 50),

	    // Los Santos Tuners
	    StatEntry::Bool("MPX_AWD_CAR_CLUB"),
	    StatEntry::Int("MPX_AWD_CAR_CLUB_MEM", 1000),
	    StatEntry::Int("MPX_AWD_SPRINTRACER", 250),
	    StatEntry::Int("MPX_AWD_STREETRACER", 250),
	    StatEntry::Int("MPX_AWD_PURSUITRACER", 250),
	    StatEntry::Int("MPX_AWD_TEST_CAR", 1800000),
	    StatEntry::Int("MPX_AWD_AUTO_SHOP", 100),
	    StatEntry::Int("MPX_AWD_CAR_EXPORT", 100),
	    StatEntry::Bool("MPX_AWD_PRO_CAR_EXPORT"),
	    StatEntry::Int("MPX_AWD_GROUNDWORK", 250),
	    StatEntry::Bool("MPX_AWD_UNION_DEPOSITORY"),
	    StatEntry::Bool("MPX_AWD_MILITARY_CONVOY"),
	    StatEntry::Bool("MPX_AWD_FLEECA_BANK"),
	    StatEntry::Bool("MPX_AWD_FREIGHT_TRAIN"),
	    StatEntry::Bool("MPX_AWD_BOLINGBROKE_[removed]"),
	    StatEntry::Bool("MPX_AWD_IAA_RAID"),
	    StatEntry::Bool("MPX_AWD_METH_JOB"),
	    StatEntry::Bool("MPX_AWD_BUNKER_RAID"),
	    StatEntry::Int("MPX_AWD_ROBBERY_CONTRACT", 100),
	    StatEntry::Int("MPX_AWD_FACES_OF_DEATH", 250),
	    StatEntry::Bool("MPX_AWD_STRAIGHT_TO_VIDEO"),
	    StatEntry::Bool("MPX_AWD_MONKEY_C_MONKEY_DO"),
	    StatEntry::Bool("MPX_AWD_TRAINED_TO_KILL"),
	    StatEntry::Bool("MPX_AWD_DIRECTOR"),

	    // The Contract
	    StatEntry::Bool("MPX_AWD_TEEING_OFF"),
	    StatEntry::Bool("MPX_AWD_PARTY_NIGHT"),
	    StatEntry::Bool("MPX_AWD_BILLIONAIRE_GAMES"),
	    StatEntry::Bool("MPX_AWD_HOOD_PASS"),
	    StatEntry::Bool("MPX_AWD_STUDIO_TOUR"),
	    StatEntry::Bool("MPX_AWD_DONT_MESS_DRE"),
	    StatEntry::Bool("MPX_AWD_BACKUP"),
	    StatEntry::Bool("MPX_AWD_SHORTFRANK_1"),
	    StatEntry::Bool("MPX_AWD_SHORTLAMAR_1"),
	    StatEntry::Bool("MPX_AWD_SHORTFRANK_2"),
	    StatEntry::Bool("MPX_AWD_SHORTLAMAR_2"),
	    StatEntry::Bool("MPX_AWD_SHORTFRANK_3"),
	    StatEntry::Bool("MPX_AWD_SHORTLAMAR_3"),
	    StatEntry::Int("MPX_AWD_CONTRACTOR", 50),
	    StatEntry::Int("MPX_AWD_COLD_CALLER", 10),
	    StatEntry::Bool("MPX_AWD_CONTR_KILLER"),
	    StatEntry::Bool("MPX_AWD_DOGS_BEST_FRIEND"),
	    StatEntry::Bool("MPX_AWD_MUSIC_STUDIO"),
	    StatEntry::Int("MPX_AWD_PRODUCER", 60),

	    // Los Santos Drug Wars
	    StatEntry::Bool("MPX_AWD_ACELIQUOR"),
	    StatEntry::Bool("MPX_AWD_TRUCKAMBUSH"),
	    StatEntry::Bool("MPX_AWD_LOSTCAMPREV"),
	    StatEntry::Bool("MPX_AWD_ACIDTRIP"),
	    StatEntry::Int("MPX_AWD_RUNRABBITRUN", 5),
	    StatEntry::Bool("MPX_AWD_HIPPYRIVALS"),
	    StatEntry::Bool("MPX_AWD_TRAINCRASH"),
	    StatEntry::Int("MPX_AWD_CALLME", 50),
	    StatEntry::Bool("MPX_AWD_BACKUPB"),
	    StatEntry::Bool("MPX_AWD_GETSTARTED"),
	    StatEntry::Int("MPX_AWD_CHEMCOMPOUNDS", 50),
	    StatEntry::Bool("MPX_AWD_CHEMREACTION"),
	    StatEntry::Int("MPX_AWD_STASHHORAID", 50),
	    StatEntry::Int("MPX_AWD_DEADDROP", 50),
	    StatEntry::Int("MPX_AWD_GOODSAMARITAN", 50),
	    StatEntry::Bool("MPX_AWD_WAREHODEFEND"),
	    StatEntry::Bool("MPX_AWD_RESCUECOOK"),
	    StatEntry::Bool("MPX_AWD_DRUGTRIPREHAB"),
	    StatEntry::Bool("MPX_AWD_ATTACKINVEST"),
	    StatEntry::Int("MPX_AWD_OWNWORSTENEMY", 60),
	    StatEntry::Bool("MPX_AWD_CARGOPLANE"),
	    StatEntry::Bool("MPX_AWD_BACKUPB2"),
	    StatEntry::Int("MPX_AWD_TAXIDRIVER", 50),
	    StatEntry::Bool("MPX_AWD_TAXISTAR"),

	    // The Chop Shop
	    StatEntry::Bool("MPX_AWD_MAZE_BANK_ROBBERY"),
	    StatEntry::Bool("MPX_AWD_CARGO_SHIP_ROBBERY"),
	    StatEntry::Bool("MPX_AWD_MISSION_ROW_ROBBERY"),
	    StatEntry::Bool("MPX_AWD_PERFECT_RUN"),
	    StatEntry::Bool("MPX_AWD_EXTRA_MILE"),
	    StatEntry::Int("MPX_AWD_VEHICLE_ROBBERIES", 50),
	    StatEntry::Int("MPX_AWD_PREP_WORK", 50),
	    StatEntry::Int("MPX_AWD_CAR_DEALER", 5000000),
	    StatEntry::Int("MPX_AWD_SECOND_HAND_PARTS", 5000000),
	    StatEntry::Int("MPX_AWD_TOW_TRUCK_SERVICE", 50),
	    StatEntry::Bool("MPX_AWD_SUBMARINE_ROBBERY"),
	    StatEntry::Bool("MPX_AWD_DIAMOND_CASINO_ROBBERY"),
	    StatEntry::Bool("MPX_AWD_BOLINGBROKE"),
	    StatEntry::Bool("MPX_AWD_GETTING_SET_UP"),
	    StatEntry::Bool("MPX_AWD_CHICKEN_FACTORY_RAID"),
	    StatEntry::Bool("MPX_AWD_HELPING_HAND2"),
	    StatEntry::Bool("MPX_AWD_SURPRISE_ATTACK"),
	    StatEntry::Bool("MPX_AWD_ALL_OUT_RAID"),
	    StatEntry::Bool("MPX_AWD_WEAPON_ARSENAL"),
	    StatEntry::Bool("MPX_AWD_GETAWAY_VEHICLES"),

	    // Bottom Dollar Bounties
	    StatEntry::Bool("MPX_AWD_HIVALBOUNT1"),
	    StatEntry::Bool("MPX_AWD_HIVALBOUNT2"),
	    StatEntry::Bool("MPX_AWD_HIVALBOUNT3"),
	    StatEntry::Bool("MPX_AWD_HIVALBOUNT4"),
	    StatEntry::Bool("MPX_AWD_HIVALBOUNT5"),
	    StatEntry::Bool("MPX_AWD_HIVALBOUNT6"),
	    StatEntry::Int("MPX_AWD_BOUNTIES", 50),
	    StatEntry::Int("MPX_AWD_STANBOUNTIES", 50),
	    StatEntry::Int("MPX_AWD_BOUNTEARNS", 5000000),
	    StatEntry::Int("MPX_AWD_BAILOFFICSTAFF", 1000000),
	    StatEntry::Int("MPX_AWD_DISPATCHWORK", 50),
	    StatEntry::Int("MPX_AWD_PIZZATHIS", 50),
	    StatEntry::Bool("MPX_AWD_ASSONATTACKSWIN"),
	    StatEntry::Bool("MPX_AWD_ASSONDEFENDWIN"),
	    StatEntry::Int("MPX_AWD_ASSONBONUSOBJ", 20),
	    StatEntry::Int("MPX_AWD_ASSONHARDDRIVE", 50),
	    StatEntry::Int("MPX_AWD_FROSTBITE", 15),
	    StatEntry::Bool("MPX_AWD_DISEASECONTROL"),

	    // Agents of Sabotage
	    StatEntry::Bool("MPX_AWD_FINEART"),
	    StatEntry::Bool("MPX_AWD_BRUTEFORCE"),
	    StatEntry::Bool("MPX_AWD_PROJECTBREAK"),
	    StatEntry::Bool("MPX_AWD_BLACKBOXFILE"),
	    StatEntry::Bool("MPX_AWD_BONUSPOINTS"),
	    StatEntry::Int("MPX_AWD_DARNELLBROSINC", 50),
	    StatEntry::Int("MPX_AWD_GET_READY", 50),
	    StatEntry::Int("MPX_AWD_CASHINHAND", 5000000),
	    StatEntry::Int("MPX_AWD_BROTHERLYLOVE", 50000),
	    StatEntry::Bool("MPX_AWD_UPRUNNING"),
	    StatEntry::Bool("MPX_AWD_MOGUL"),
	    StatEntry::Bool("MPX_AWD_INTEL"),
	    StatEntry::Int("MPX_AWD_NOTOUTDPT", 5),
	    StatEntry::Bool("MPX_AWD_IRONMULE"),
	    StatEntry::Bool("MPX_AWD_AMMUNITION"),
	    StatEntry::Bool("MPX_AWD_DIRDELIVERY"),
	    StatEntry::Int("MPX_AWD_FULSTOCKED", 8),
	    StatEntry::Bool("MPX_AWD_TITANJOB"),
	    StatEntry::Bool("MPX_AWD_PERMANENTCON"),
	    StatEntry::Int("MPX_AWD_ARMSINARMS", 50),

	    // Money Fronts
	    StatEntry::Bool("MPX_AWD_JUNKSEARCH"),
	    StatEntry::Int("MPX_AWD_DOGSDINNER", 4),
	    StatEntry::Bool("MPX_AWD_CASASS"),
	    StatEntry::Bool("MPX_AWD_LOCKUPINT"),
	    StatEntry::Bool("MPX_AWD_ESCORTFLATBED"),
	    StatEntry::Bool("MPX_AWD_LEFT4DEAD"),
	    StatEntry::Bool("MPX_AWD_TRACKER"),
	    StatEntry::Bool("MPX_AWD_CLEARCOMP"),
	    StatEntry::Bool("MPX_AWD_CASHBONUS"),
	    StatEntry::Int("MPX_AWD_HEATAROUDC", 25),
	    StatEntry::Int("MPX_AWD_TYCOON", 2000000),
	    StatEntry::Bool("MPX_AWD_BUSINEXPAND"),
	    StatEntry::Bool("MPX_AWD_LOSTPRODUC"),
	    StatEntry::Int("MPX_AWD_CASHCLEAN", 500000),
	    StatEntry::Int("MPX_AWD_MEDICOURI", 20),
	    StatEntry::Bool("MPX_AWD_EMERGENCYSERV"),
	    StatEntry::Int("MPX_AWD_TRANSPORT", 20),
	    StatEntry::Bool("MPX_AWD_SECUREDEL"),
	    StatEntry::Int("MPX_AWD_SUNBURNED", 15),
	    StatEntry::Bool("MPX_AWD_TWOOFAKIND"),
	    StatEntry::Int("MPX_AWD_UNDEADPARTY", 60),

	    // A Safehouse in the Hills
	    StatEntry::Bool("MPX_AWD_NEGATIVEPRESS"),
	    StatEntry::Bool("MPX_AWD_USESELFDRIVINGVEH"),
	    StatEntry::Bool("MPX_AWD_LIONSDEN"),
	    StatEntry::Bool("MPX_AWD_TAKEOUTLEADEXPLO"),
	    StatEntry::Bool("MPX_AWD_SUBWAYTRAIN"),
	    StatEntry::Bool("MPX_AWD_SUBWAYSTUNTJUMP"),
	    StatEntry::Bool("MPX_AWD_TRASHCOLLECTORS"),
	    StatEntry::Bool("MPX_AWD_ALLTRASHCOLLECTED"),
	    StatEntry::Bool("MPX_AWD_FIBFINALE"),
	    StatEntry::Bool("MPX_AWD_HELPER"),
	    StatEntry::Bool("MPX_AWD_FIBFINALECHALLENGE"),
	    StatEntry::Bool("MPX_AWD_MANSIONDEFEND"),
	    StatEntry::Bool("MPX_AWD_DEFLOWMOTIONKILLS"),
	    StatEntry::Bool("MPX_AWD_YOUVEMADEIT"),
	    StatEntry::Bool("MPX_AWD_ANIMALLOVER"),
	    StatEntry::Int("MPX_AWD_YOGA", 50),
	    StatEntry::Int("MPX_AWD_JUICEDUP", 100),
	    StatEntry::Int("MPX_AWD_MANADVERSARYDEFUSE", 10),
	    StatEntry::Bool("MPX_AWD_MANSADVERSARYWIN"),
	    StatEntry::Int("MPX_AWD_FIREFIGHTER", 20),
	    StatEntry::Int("MPX_AWD_ALPHAMAIL", 20),
	    StatEntry::Int("MPX_AWD_LOSSANTOSMETEOR", 20),
	    StatEntry::Int("MPX_AWD_TAXIDESTRUCTION", 20),
	    StatEntry::Bool("MPX_AWD_ONTHECLOCK"),

	    // The Kortz Center Heist
	    StatEntry::Bool("MPX_AWD_SCOPING"),
	    StatEntry::Int("MPX_AWD_PREPPER", 20),
	    StatEntry::Bool("MPX_AWD_ADAPTABLE"),
	    StatEntry::Bool("MPX_AWD_KORTZCENTERHEIST"),
	    StatEntry::Bool("MPX_AWD_FINDSAWAY"),
	    StatEntry::Bool("MPX_AWD_WHOSTHERE"),
	    StatEntry::Bool("MPX_AWD_ELITETHIEF"),
	    StatEntry::Bool("MPX_AWD_NOLIFER"),
	    StatEntry::Bool("MPX_AWD_SOLITUDE"),
	    StatEntry::Bool("MPX_AWD_COORDINATION"),
	    StatEntry::Bool("MPX_AWD_FLEXIBLETHIEF"),
	    StatEntry::Int("MPX_AWD_REPEATOFFENDER", 20),
	    StatEntry::Int("MPX_AWD_FULLBAGS", 5000000),
	    StatEntry::Bool("MPX_AWD_HIDDENINPLAINSIGHT"),
	    StatEntry::Int("MPX_AWD_CURATOR", 5),
	    StatEntry::Int("MPX_AWD_LAPIDARY", 5),
	    StatEntry::Bool("MPX_AWD_PACIFIST"),
	    StatEntry::Bool("MPX_AWD_PUTRIDPILFERING"),
	    StatEntry::Bool("MPX_AWD_METEORITICS"),
	};
}
