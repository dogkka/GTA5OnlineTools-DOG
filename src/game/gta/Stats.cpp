#include "Stats.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Stats
{
	constexpr int kMaskedChunkBits = 16;
	constexpr int kMaskedValueBits = 64;
	constexpr std::uint64_t kMaskedChunkMask = (std::uint64_t{1} << kMaskedChunkBits) - 1;

	static void ConvertMPX(std::string& statName)
	{
		std::transform(statName.begin(), statName.end(), statName.begin(), ::tolower);
		if (statName.substr(0, 3) == "mpx")
			statName[2] = GetCharIndex() + '0';
	}

	int GetCharIndex()
	{
		int index{};
		STATS::STAT_GET_INT("MPPLY_LAST_MP_CHAR"_J, &index, -1);
		return index;
	}

	bool SaveStats()
	{
		return STATS::STAT_SAVE(0, FALSE, 3, FALSE);
	}

	void SetInt(std::string statName, int value)
	{
		ConvertMPX(statName);
		STATS::STAT_SET_INT(Joaat(statName), value, TRUE);
	}

	void SetBool(std::string statName, bool value)
	{
		ConvertMPX(statName);
		STATS::STAT_SET_BOOL(Joaat(statName), value, TRUE);
	}

	void SetFloat(std::string statName, float value)
	{
		ConvertMPX(statName);
		STATS::STAT_SET_FLOAT(Joaat(statName), value, TRUE);
	}

	void SetDate(std::string statName, Date* value)
	{
		ConvertMPX(statName);
		STATS::STAT_SET_DATE(Joaat(statName), value, SCR_SIZEOF(Date), TRUE);
	}

	void SetString(std::string statName, const char* value)
	{
		ConvertMPX(statName);
		STATS::STAT_SET_STRING(Joaat(statName), value, TRUE);
	}

	int GetInt(std::string statName)
	{
		int value{};
		ConvertMPX(statName);
		STATS::STAT_GET_INT(Joaat(statName), &value, -1);
		return value;
	}

	bool GetBool(std::string statName)
	{
		BOOL value{};
		ConvertMPX(statName);
		STATS::STAT_GET_BOOL(Joaat(statName), &value, -1);
		return value;
	}

	float GetFloat(std::string statName)
	{
		float value{};
		ConvertMPX(statName);
		STATS::STAT_GET_FLOAT(Joaat(statName), &value, -1);
		return value;
	}

	Date GetDate(std::string statName)
	{
		Date value{};
		ConvertMPX(statName);
		STATS::STAT_GET_DATE(Joaat(statName), &value, SCR_SIZEOF(Date), -1);
		return value;
	}

	const char* GetString(std::string statName)
	{
		ConvertMPX(statName);
		return STATS::STAT_GET_STRING(Joaat(statName), -1); // The native is null-safe
	}

	void SetPackedBool(int index, bool value)
	{
		STATS::SET_PACKED_STAT_BOOL_CODE(index, value, -1);
	}

	void SetPackedInt(int index, int value)
	{
		STATS::SET_PACKED_STAT_INT_CODE(index, value, -1);
	}

	bool GetPackedBool(int index)
	{
		return STATS::GET_PACKED_STAT_BOOL_CODE(index, -1);
	}

	int GetPackedInt(int index)
	{
		return STATS::GET_PACKED_STAT_INT_CODE(index, -1);
	}

	void SetMaskedBool(std::string statName, int bitIndex, bool value)
	{
		ConvertMPX(statName);
		STATS::STAT_SET_MASKED_INT(Joaat(statName), value, bitIndex, 1, true);
	}

	void SetMaskedInt(std::string statName, int bitStart, int bitSize, int value)
	{
		ConvertMPX(statName);
		STATS::STAT_SET_MASKED_INT(Joaat(statName), value, bitStart, bitSize, true);
	}

	bool GetMaskedBool(std::string statName, int bitIndex)
	{
		int value{};
		ConvertMPX(statName);
		STATS::STAT_GET_MASKED_INT(Joaat(statName), &value, bitIndex, 1, -1);
		return value != 0;
	}

	int GetMaskedInt(std::string statName, int bitIndex, int bitSize)
	{
		int value{};
		ConvertMPX(statName);
		STATS::STAT_GET_MASKED_INT(Joaat(statName), &value, bitIndex, bitSize, -1);
		return value;
	}

	void SetMaskedUInt64(Hash hash, std::uint64_t value)
	{
		for (int bitIndex = 0; bitIndex < kMaskedValueBits; bitIndex += kMaskedChunkBits)
		{
			const auto chunk = static_cast<int>((value >> bitIndex) & kMaskedChunkMask);
			STATS::STAT_SET_MASKED_INT(hash, chunk, bitIndex, kMaskedChunkBits, true);
		}
	}

	void SetMaskedUInt64(std::string statName, std::uint64_t value)
	{
		ConvertMPX(statName);
		SetMaskedUInt64(Joaat(statName), value);
	}

	std::uint64_t GetMaskedUInt64(Hash hash, int playerIndex)
	{
		std::uint64_t value = 0;
		for (int bitIndex = 0; bitIndex < kMaskedValueBits; bitIndex += kMaskedChunkBits)
		{
			int chunk = 0;
			STATS::STAT_GET_MASKED_INT(hash, &chunk, bitIndex, kMaskedChunkBits, playerIndex);
			value |= (static_cast<std::uint64_t>(chunk) & kMaskedChunkMask) << bitIndex;
		}
		return value;
	}

	std::uint64_t GetMaskedUInt64(std::string statName, int playerIndex)
	{
		ConvertMPX(statName);
		return GetMaskedUInt64(Joaat(statName), playerIndex);
	}
}
