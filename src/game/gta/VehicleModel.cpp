#include "VehicleModel.hpp"

#include "core/localization/Localization.hpp"
#include "core/util/Joaat.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/data/VehicleValues.hpp"

namespace YimMenu
{
	namespace
	{
		std::string LocalizeModLabel(const char* label)
		{
			if (!label || label[0] == 0)
				return {};

			return Localization::Translate(label);
		}

		std::string LocalizeHudLabel(const char* label)
		{
			if (!label || label[0] == 0)
				return {};

			return Localization::Translate(HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION(label));
		}
	}

	std::string VehicleModel::GetModSlotName(Hash model, int vehicle, int mod_slot)
	{
		switch ((VehicleModType)mod_slot)
		{
		case VehicleModType::MOD_HOOD: return LocalizeHudLabel("CMOD_MOD_HOD");
		case VehicleModType::MOD_ARMOR: return LocalizeHudLabel("CMOD_MOD_ARM");
		case VehicleModType::MOD_BRAKES: return LocalizeHudLabel("CMOD_MOD_BRA");
		case VehicleModType::MOD_ENGINE: return LocalizeHudLabel("CMOD_MOD_ENG");
		case VehicleModType::MOD_SUSPENSION: return LocalizeHudLabel("CMOD_MOD_SUS");
		case VehicleModType::MOD_TRANSMISSION: return LocalizeHudLabel("CMOD_MOD_TRN");
		case VehicleModType::MOD_HORNS: return LocalizeHudLabel("CMOD_MOD_HRN");
		case VehicleModType::MOD_FRONTWHEEL:
			if (!VEHICLE::IS_THIS_MODEL_A_BIKE(model) && VEHICLE::IS_THIS_MODEL_A_BICYCLE(model))
				return LocalizeHudLabel("CMOD_MOD_WHEM");
			else
				return LocalizeHudLabel("CMOD_WHE0_0");
		case VehicleModType::MOD_REARWHEEL:
			return LocalizeHudLabel("CMOD_WHE0_1");
			//Bennys
		case VehicleModType::MOD_PLATEHOLDER: return LocalizeHudLabel("CMM_MOD_S0");
		case VehicleModType::MOD_VANITYPLATES: return LocalizeHudLabel("CMM_MOD_S1");
		case VehicleModType::MOD_TRIMDESIGN:
			if (model == "SULTANRS"_J)
				return LocalizeHudLabel("CMM_MOD_S2b");
			else
				return LocalizeHudLabel("CMM_MOD_S2");
		case VehicleModType::MOD_ORNAMENTS: return LocalizeHudLabel("CMM_MOD_S3");
		case VehicleModType::MOD_DASHBOARD: return LocalizeHudLabel("CMM_MOD_S4");
		case VehicleModType::MOD_DIALDESIGN: return LocalizeHudLabel("CMM_MOD_S5");
		case VehicleModType::MOD_DOORSPEAKERS: return LocalizeHudLabel("CMM_MOD_S6");
		case VehicleModType::MOD_SEATS: return LocalizeHudLabel("CMM_MOD_S7");
		case VehicleModType::MOD_STEERINGWHEELS: return LocalizeHudLabel("CMM_MOD_S8");
		case VehicleModType::MOD_COLUMNSHIFTERLEVERS: return LocalizeHudLabel("CMM_MOD_S9");
		case VehicleModType::MOD_PLAQUES: return LocalizeHudLabel("CMM_MOD_S10");
		case VehicleModType::MOD_SPEAKERS: return LocalizeHudLabel("CMM_MOD_S11");
		case VehicleModType::MOD_TRUNK: return LocalizeHudLabel("CMM_MOD_S12");
		case VehicleModType::MOD_HYDRO: return LocalizeHudLabel("CMM_MOD_S13");
		case VehicleModType::MOD_ENGINEBLOCK: return LocalizeHudLabel("CMM_MOD_S14");
		case VehicleModType::MOD_AIRFILTER:
			if (model == "SULTANRS"_J)
				return LocalizeHudLabel("CMM_MOD_S15b");
			else
				return LocalizeHudLabel("CMM_MOD_S15");
		case VehicleModType::MOD_STRUTS:
			if (model == "SULTANRS"_J || model == "BANSHEE2"_J)
				return LocalizeHudLabel("CMM_MOD_S16b");
			else
				return LocalizeHudLabel("CMM_MOD_S16");
		case VehicleModType::MOD_ARCHCOVER:
			if (model == "SULTANRS"_J)
				return LocalizeHudLabel("CMM_MOD_S17b");
			else
				return LocalizeHudLabel("CMM_MOD_S17");
		case VehicleModType::MOD_AERIALS:
			if (model == "SULTANRS"_J)
				return LocalizeHudLabel("CMM_MOD_S18b");
			else if (model == "BTYPE3"_J)
				return LocalizeHudLabel("CMM_MOD_S18c");
			else
				return LocalizeHudLabel("CMM_MOD_S18");
		case VehicleModType::MOD_TRIM:
			if (model == "SULTANRS"_J)
				return LocalizeHudLabel("CMM_MOD_S19b");
			else if (model == "BTYPE3"_J)
				return LocalizeHudLabel("CMM_MOD_S19c");
			else if (model == "VIRGO2"_J)
				return LocalizeHudLabel("CMM_MOD_S19d");
			else if ("TOP_IMANI_TECH"_J == MISC::GET_HASH_KEY(VEHICLE::GET_MOD_SLOT_NAME(vehicle, mod_slot)))
				return Localization::Translate("Imani Tech");
			else
				return LocalizeHudLabel("CMM_MOD_S19");
		case VehicleModType::MOD_TANK:
			if (model == "SLAMVAN3"_J)
				return LocalizeHudLabel("CMM_MOD_S27");
			else
				return LocalizeHudLabel("CMM_MOD_S20");
		case VehicleModType::MOD_WINDOWS:
			if (model == "BTYPE3"_J)
				return LocalizeHudLabel("CMM_MOD_S21b");
			else
				return LocalizeHudLabel("CMM_MOD_S21");
		case VehicleModType::MOD_DOORS:
			if (model == "SLAMVAN3"_J)
				return LocalizeHudLabel("SLVAN3_RDOOR");
			else
				return LocalizeHudLabel("CMM_MOD_S22");
		case VehicleModType::MOD_LIVERY: return LocalizeHudLabel("CMM_MOD_S23");
		default:
			auto name = VEHICLE::GET_MOD_SLOT_NAME(vehicle, mod_slot);
			if (name == nullptr)
				return {};
			if (strstr(name, "_"))
				return LocalizeHudLabel(name);
			return LocalizeModLabel(name);
		}
	}

	static const std::map<int, const char*> horn_map = {{-1, "CMOD_HRN_0"},
	    {0, "CMOD_HRN_TRK"},
	    {1, "CMOD_HRN_COP"},
	    {2, "CMOD_HRN_CLO"},
	    {3, "CMOD_HRN_MUS1"},
	    {4, "CMOD_HRN_MUS2"},
	    {5, "CMOD_HRN_MUS3"},
	    {6, "CMOD_HRN_MUS4"},
	    {7, "CMOD_HRN_MUS5"},
	    {8, "CMOD_HRN_SAD"},
	    {9, "HORN_CLAS1"},
	    {10, "HORN_CLAS2"},
	    {11, "HORN_CLAS3"},
	    {12, "HORN_CLAS4"},
	    {13, "HORN_CLAS5"},
	    {14, "HORN_CLAS6"},
	    {15, "HORN_CLAS7"},
	    {16, "HORN_CNOTE_C0"},
	    {17, "HORN_CNOTE_D0"},
	    {18, "HORN_CNOTE_E0"},
	    {19, "HORN_CNOTE_F0"},
	    {20, "HORN_CNOTE_G0"},
	    {21, "HORN_CNOTE_A0"},
	    {22, "HORN_CNOTE_B0"},
	    {23, "HORN_CNOTE_C1"},
	    {24, "HORN_HIPS1"},
	    {25, "HORN_HIPS2"},
	    {26, "HORN_HIPS3"},
	    {27, "HORN_HIPS4"},
	    {28, "HORN_INDI_1"},
	    {29, "HORN_INDI_2"},
	    {30, "HORN_INDI_3"},
	    {31, "HORN_INDI_4"},
	    {32, "HORN_LUXE2"},
	    {33, "HORN_LUXE1"},
	    {34, "HORN_LUXE3"}, /*{35, "HORN_LUXE2"},
		{36, "HORN_LUXE1"}, {37, "HORN_LUXE3"},*/
	    {38, "HORN_HWEEN1"},
	    /*{39, "HORN_HWEEN1"},*/ {40, "HORN_HWEEN2"},
	    /*{41, "HORN_HWEEN2"},*/ {42, "HORN_LOWRDER1"},
	    /*{43, "HORN_LOWRDER1"},*/ {44, "HORN_LOWRDER2"}, /*{45, "HORN_LOWRDER2"},*/
	    {46, "HORN_XM15_1"},
	    {47, "HORN_XM15_2"},
	    {48, "HORN_XM15_3"}};

	std::string VehicleModel::GetModName(Hash model, int vehicle, int mod_slot, int mod, int mod_count)
	{
		if (mod_count == 0)
			return "";
		if (mod < -1 || mod >= mod_count)
			return "";
		if (mod_slot == (int)VehicleModType::MOD_HORNS)
		{
			if (horn_map.find(mod) != horn_map.end())
			{
				return LocalizeHudLabel(horn_map.find(mod)->second);
			}
			return "";
		}

		if (mod_slot == (int)VehicleModType::MOD_FRONTWHEEL || mod_slot == (int)VehicleModType::MOD_REARWHEEL)
		{
			if (mod == -1)
			{
				if (!VEHICLE::IS_THIS_MODEL_A_BIKE(model) && VEHICLE::IS_THIS_MODEL_A_BICYCLE(model))
					return LocalizeHudLabel("CMOD_WHE_0");
				else
					return LocalizeHudLabel("CMOD_WHE_B_0");
			}
			if (mod >= mod_count / 2)
				return std::format("{} {}", Localization::Translate("Chrome"), LocalizeHudLabel(VEHICLE::GET_MOD_TEXT_LABEL(vehicle, mod_slot, mod)));
			else
				return LocalizeHudLabel(VEHICLE::GET_MOD_TEXT_LABEL(vehicle, mod_slot, mod));
		}

		switch (mod_slot)
		{
		case (int)VehicleModType::MOD_ARMOR:
			return LocalizeHudLabel(std::format("CMOD_ARM_{}", (mod + 1)).c_str());
		case (int)VehicleModType::MOD_BRAKES:
			return LocalizeHudLabel(std::format("CMOD_BRA_{}", (mod + 1)).c_str());
		case (int)VehicleModType::MOD_ENGINE:
			if (mod == -1)
				return LocalizeHudLabel("CMOD_ARM_0");
			return LocalizeHudLabel(std::format("CMOD_ENG_{}", (mod + 1)).c_str());
		case (int)VehicleModType::MOD_SUSPENSION:
			return LocalizeHudLabel(std::format("CMOD_SUS_{}", (mod + 1)).c_str());
		case (int)VehicleModType::MOD_TRANSMISSION:
			return LocalizeHudLabel(std::format("CMOD_GBX_{}", (mod + 1)).c_str());
		}


		if (mod > -1)
		{
			if (mod_slot == (int)VehicleModType::MOD_SIDESKIRT && VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, (int)VehicleModType::MOD_SIDESKIRT) < 2)
			{
				return LocalizeHudLabel("CMOD_SKI_1");
			}
			auto label = VEHICLE::GET_MOD_TEXT_LABEL(vehicle, mod_slot, mod);
			if (label == nullptr || strlen(label) == 0)
				return Localization::Translate("MISSING_LABEL");
			return LocalizeHudLabel(label);
		}
		else
		{
			switch (mod_slot)
			{
			case (int)VehicleModType::MOD_AIRFILTER: break;
			case (int)VehicleModType::MOD_STRUTS:
				switch (model)
				{
				case "BANSHEE"_J:
				case "BANSHEE2"_J:
				case "SULTANRS"_J: return LocalizeHudLabel("CMOD_COL5_41");
				}
				break;
			}
			return LocalizeHudLabel("CMOD_DEF_0");
		}
	}

	static const std::map<Hash, std::map<int, std::vector<int32_t>>> mod_blacklists = {
	    {"BANSHEE"_J, {{(int)VehicleModType::MOD_SPOILERS, {3, 4}}, {(int)VehicleModType::MOD_COLUMNSHIFTERLEVERS, {0, 1, 2, 3}}, {(int)VehicleModType::MOD_SPEAKERS, {0}}, {(int)VehicleModType::MOD_LIVERY, {15, 16}}}},
	    {"SENTINEL"_J, {{(int)VehicleModType::MOD_SPOILERS, {4, 5}}, {(int)VehicleModType::MOD_COLUMNSHIFTERLEVERS, {0, 1, 2, 3}}, {(int)VehicleModType::MOD_SPEAKERS, {0}}, {(int)VehicleModType::MOD_LIVERY, {0, 1}}}},
	};

	bool VehicleModel::CheckModBlacklist(Hash model, int mod_slot, int mod)
	{
		if (mod_blacklists.find(model) == mod_blacklists.end())
		{
			return false;
		}

		auto veh_slot_blacklist = mod_blacklists.find(model)->second;
		if (veh_slot_blacklist.find(mod_slot) == veh_slot_blacklist.end())
		{
			return false;
		}

		auto veh_mod_blacklist = veh_slot_blacklist.find(mod_slot)->second;
		if (std::find(veh_mod_blacklist.begin(), veh_mod_blacklist.end(), mod) != veh_mod_blacklist.end())
		{
			return true;
		}

		return false;
	}

	bool VehicleModel::IsBennys(int veh)
	{
		auto v = (WheelTypes)VEHICLE::GET_VEHICLE_WHEEL_TYPE(veh);
		return v == WheelTypes::WHEEL_TYPE_BENNYS_ORIGINAL || v == WheelTypes::WHEEL_TYPE_BENNYS_BESPOKE || v == WheelTypes::WHEEL_TYPE_OPEN_WHEEL || v == WheelTypes::WHEEL_TYPE_STREET || v == WheelTypes::WHEEL_TYPE_TRACK;
	}
}
