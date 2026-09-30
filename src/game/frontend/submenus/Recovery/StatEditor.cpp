#include "StatEditor.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/frontend/widgets/imgui_bitfield.hpp"
#include "core/localization/Localization.hpp"
#include "game/backend/AnticheatBypass.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Stats.hpp"
#include "game/pointers/Pointers.hpp"
#include "types/stats/CStatsMgr.hpp"

#include <array>
#include <bit>
#include <charconv>
#include <cstdio>
#include <cstring>

namespace YimMenu::Submenus
{
	struct StatInfo
	{
		std::string m_Name;
		std::uint32_t m_NameHash = 0;
		bool m_Normalized = false;
		sStatData* m_Data = nullptr;

		bool IsValid() const
		{
			return m_Data != nullptr;
		}
	};

	struct PackedStatInfo
	{
		int m_Index;
		bool m_IsBoolStat;
		bool m_IsValid;

		bool IsValid() const
		{
			return m_IsValid;
		}
	};

	union StatValue {
		float m_AsFloat[3];
		int m_AsInt;
		bool m_AsBool;
		std::int64_t m_AsI64;
		std::uint64_t m_AsU64;
		char m_AsString[21];
		Date m_Date;
	};

	constexpr std::size_t kMaxClipboardBytes = 1024 * 1024;
	constexpr std::size_t kMaxClipboardLines = 4096;
	constexpr std::size_t kMaxPackedRangeWrites = 4096;

	// https://stackoverflow.com/questions/66897068/can-trim-of-a-string-be-done-inplace-with-c20-ranges
	static std::string_view TrimString(std::string_view string)
	{
		return std::string_view{
		    std::ranges::find_if_not(
		        string,
		        [](auto c) {
			        return std::isspace(static_cast<unsigned char>(c));
		        }),
		    std::ranges::find_if_not(
		        string | std::views::reverse,
		        [](auto c) {
			        return std::isspace(static_cast<unsigned char>(c));
		        }).base()};
	}

	static StatInfo GetStatInfo(std::string_view name_str)
	{
		StatInfo name{};
		auto len = name_str.length();

		// not sure why people do this
		if (len > 1 && name_str[0] == '$')
		{
			auto it = name_str.begin();
			std::advance(it, 1);
			name_str = std::string_view{it, name_str.end()};
			len--;
			name.m_Normalized = true;
		}

		name.m_Name = name_str;

		if (len > 3 && std::tolower(static_cast<unsigned char>(name_str[0])) == 'm' && std::tolower(static_cast<unsigned char>(name_str[1])) == 'p' && std::tolower(static_cast<unsigned char>(name_str[2])) == 'x')
		{
			if (auto last_char = Pointers.StatsMgr->GetStat("MPPLY_LAST_MP_CHAR"_J))
			{
				name.m_Name[2] = '0' + last_char->GetInt();
				name.m_Normalized = true;
			}
		}

		name.m_NameHash = Joaat(name.m_Name);
		name.m_Data = Pointers.StatsMgr->GetStat(name.m_NameHash);

		if (name.m_Data == nullptr && len > 3 && (std::tolower(static_cast<unsigned char>(name_str[0])) != 'm' || std::tolower(static_cast<unsigned char>(name_str[1])) != 'p' || !(std::tolower(static_cast<unsigned char>(name_str[2])) == '0' || std::tolower(static_cast<unsigned char>(name_str[2])) == '1')))
		{
			// stat names without a character prefix
			auto last_char = Pointers.StatsMgr->GetStat("MPPLY_LAST_MP_CHAR"_J);
			auto char_index = last_char ? last_char->GetInt() : 0;
			auto char_prefix = char_index == 0 ? "MP0_" : "MP1_";
			auto new_hash = Joaat(char_prefix + name.m_Name);
			auto new_stat = Pointers.StatsMgr->GetStat(new_hash);

			if (new_stat)
			{
				name.m_Name = char_prefix + name.m_Name;
				name.m_NameHash = new_hash;
				name.m_Data = new_stat;
				name.m_Normalized = true;
			}
		}

		return name;
	}

	static void ReadStat(std::uint32_t hash, StatValue& value, sStatData* data)
	{
		memset(&value, 0, sizeof(StatValue));

		switch (data->GetType())
		{
		case sStatData::Type::_BOOL:
			value.m_AsBool = data->GetBool();
			return;
		case sStatData::Type::FLOAT:
			value.m_AsFloat[0] = data->GetFloat();
			return;
		case sStatData::Type::INT:
		case sStatData::Type::UINT32:
		case sStatData::Type::UINT16:
		case sStatData::Type::UINT8:
			value.m_AsInt = data->GetInt();
			return;
		case sStatData::Type::INT64:
			value.m_AsI64 = data->GetInt64();
			return;
		case sStatData::Type::UINT64:
		case sStatData::Type::PACKED:
			value.m_AsU64 = data->GetUInt64();
			return;
		case sStatData::Type::STRING:
			if (const auto text = data->GetString())
				std::snprintf(value.m_AsString, sizeof(value.m_AsString), "%s", text);
			return;
		case sStatData::Type::POS:
			STATS::STAT_GET_POS(hash, &value.m_AsFloat[0], &value.m_AsFloat[1], &value.m_AsFloat[2], -1);
			return;
		case sStatData::Type::DATE:
			STATS::STAT_GET_DATE(hash, &value.m_Date, SCR_SIZEOF(Date), -1);
			return;
		case sStatData::Type::USERID:
		{
			char userId[21]{};
			if (data->GetUserID(userId, sizeof(userId)))
				value.m_AsU64 = std::strtoull(userId, nullptr, 10);
			return;
		}
		case sStatData::Type::PROFILE_SETTING:
		case sStatData::Type::TEXTLABEL:
		default:
			return; // data type not supported
		}
	}

	static void WriteInt64Stat(std::uint32_t hash, std::int64_t value, sStatData* data)
	{
		// There is no 64-bit setter Native. Seed the local value one step behind,
		// then increment through the Native so the game queues the stat for persistence.
		const auto previousBits = std::bit_cast<std::uint64_t>(value) - 1;
		data->SetInt64(std::bit_cast<std::int64_t>(previousBits));
		STATS::STAT_INCREMENT(hash, 1.0f);
	}

	static void WriteUInt64Stat(std::uint32_t hash, std::uint64_t value, sStatData* data)
	{
		data->SetUInt64(value - 1);
		STATS::STAT_INCREMENT(hash, 1.0f);
	}

	static void WriteStat(std::uint32_t hash, StatValue& value, sStatData* data)
	{
		switch (data->GetType())
		{
		case sStatData::Type::_BOOL:
			STATS::STAT_SET_BOOL(hash, value.m_AsBool, true);
			return;
		case sStatData::Type::FLOAT:
			STATS::STAT_SET_FLOAT(hash, value.m_AsFloat[0], true);
			return;
		case sStatData::Type::INT:
		case sStatData::Type::UINT32:
		case sStatData::Type::UINT16:
		case sStatData::Type::UINT8:
			STATS::STAT_SET_INT(hash, value.m_AsInt, true);
			return;
		case sStatData::Type::INT64:
			WriteInt64Stat(hash, value.m_AsI64, data);
			return;
		case sStatData::Type::UINT64:
			WriteUInt64Stat(hash, value.m_AsU64, data);
			return;
		case sStatData::Type::STRING:
			STATS::STAT_SET_STRING(hash, value.m_AsString, true);
			return;
		case sStatData::Type::USERID:
		{
			const auto userId = std::to_string(value.m_AsU64);
			STATS::STAT_SET_USER_ID(hash, userId.c_str(), true);
			return;
		}
		case sStatData::Type::PACKED:
			Stats::SetMaskedUInt64(hash, value.m_AsU64);
			return;
		case sStatData::Type::POS:
			STATS::STAT_SET_POS(hash, value.m_AsFloat[0], value.m_AsFloat[1], value.m_AsFloat[2], true);
			return;
		case sStatData::Type::DATE:
			STATS::STAT_SET_DATE(hash, &value.m_Date, SCR_SIZEOF(Date), true);
			return;
		case sStatData::Type::PROFILE_SETTING:
		case sStatData::Type::TEXTLABEL:
		default:
			return; // data type not supported
		}
	}

	static bool CheckDate(const Date& date)
	{
		if (date.Year < 0 || date.Month < 1 || date.Month > 12 || date.Day < 1 || date.Hour < 0 || date.Hour > 23 || date.Minute < 0 || date.Minute > 59 || date.Second < 0 || date.Second > 59 || date.Millisecond < 0 || date.Millisecond > 999)
			return false;

		constexpr std::array daysPerMonth{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		auto maxDay = daysPerMonth[date.Month - 1];
		const bool leapYear = (date.Year % 4 == 0 && date.Year % 100 != 0) || date.Year % 400 == 0;
		if (date.Month == 2 && leapYear)
			maxDay = 29;

		return date.Day <= maxDay;
	}

	template<typename T, std::size_t Size>
	static bool ParseCommaSeparated(std::string_view text, std::array<T, Size>& output)
	{
		std::size_t index = 0;
		for (auto part : text | std::views::split(','))
		{
			if (index == output.size())
				return false;

			const auto token = TrimString(std::string_view{part.begin(), part.end()});
			auto [ptr, error] = std::from_chars(token.data(), token.data() + token.size(), output[index]);
			if (error != std::errc() || ptr != token.data() + token.size())
				return false;
			++index;
		}

		return index == output.size();
	}

	static void WriteStatWithStringValue(std::uint32_t hash, std::string_view value, sStatData* data)
	{
		const auto text = std::string(value);
		switch (data->GetType())
		{
		case sStatData::Type::_BOOL:
		{
			bool _bool = false;
			std::string as_string(text);
			std::transform(as_string.begin(), as_string.end(), as_string.begin(), [](char c) {
				return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
			});

			if (as_string == "true" || as_string == "1")
			{
				_bool = true;
			}

			STATS::STAT_SET_BOOL(hash, _bool, true);
			return;
		}
		case sStatData::Type::FLOAT:
		{
			auto _float = std::strtof(text.c_str(), nullptr);
			STATS::STAT_SET_FLOAT(hash, _float, true);
			return;
		}
		case sStatData::Type::INT:
		case sStatData::Type::UINT32:
		case sStatData::Type::UINT16:
		case sStatData::Type::UINT8:
		{
			auto _int = std::strtol(text.c_str(), nullptr, 10);
			STATS::STAT_SET_INT(hash, _int, true);
			return;
		}
		case sStatData::Type::INT64:
		{
			auto int64_ = std::strtoll(text.c_str(), nullptr, 10);
			WriteInt64Stat(hash, int64_, data);
			return;
		}
		case sStatData::Type::UINT64:
		{
			auto uint64_ = std::strtoull(text.c_str(), nullptr, 10);
			WriteUInt64Stat(hash, uint64_, data);
			return;
		}
		case sStatData::Type::STRING:
			STATS::STAT_SET_STRING(hash, text.c_str(), true);
			return;
		case sStatData::Type::PACKED:
		{
			auto uint64_ = std::strtoull(text.c_str(), nullptr, 10);
			Stats::SetMaskedUInt64(hash, uint64_);
			return;
		}
		case sStatData::Type::USERID:
			if (!text.empty() && text.find_first_not_of("0123456789") == std::string::npos)
				STATS::STAT_SET_USER_ID(hash, text.c_str(), true);
			return;
		case sStatData::Type::DATE:
		{
			std::array<int, 7> fields{};
			if (!ParseCommaSeparated(text, fields))
				return;

			Date date{fields[0], fields[1], fields[2], fields[3], fields[4], fields[5], fields[6]};
			if (CheckDate(date))
				STATS::STAT_SET_DATE(hash, &date, SCR_SIZEOF(Date), true);
			return;
		}
		case sStatData::Type::POS:
		{
			std::array<float, 3> position{};
			if (ParseCommaSeparated(text, position))
				STATS::STAT_SET_POS(hash, position[0], position[1], position[2], true);
			return;
		}
		case sStatData::Type::PROFILE_SETTING:
		case sStatData::Type::TEXTLABEL:
		default:
			return; // data type not supported
		}
	}

	static bool RenderStatEditor(StatValue& value, sStatData* data)
	{
		ImGui::SetNextItemWidth(150.f);
		switch (data->GetType())
		{
		case sStatData::Type::_BOOL:
			ImGui::Checkbox("值", &value.m_AsBool);
			return true;
		case sStatData::Type::FLOAT:
			ImGui::InputFloat("值", &value.m_AsFloat[0]);
			return true;
		case sStatData::Type::INT:
			ImGui::InputInt("值", &value.m_AsInt);
			return true;
		case sStatData::Type::UINT32:
			ImGui::InputScalar("值", ImGuiDataType_U32, &value.m_AsInt);
			return true;
		case sStatData::Type::UINT16:
			ImGui::InputScalar("值", ImGuiDataType_U16, &value.m_AsInt);
			return true;
		case sStatData::Type::UINT8:
			ImGui::InputScalar("值", ImGuiDataType_U8, &value.m_AsInt);
			return true;
		case sStatData::Type::INT64:
			ImGui::InputScalar("值", ImGuiDataType_S64, &value.m_AsI64);
			return true;
		case sStatData::Type::UINT64:
		case sStatData::Type::USERID:
			ImGui::InputScalar("值", ImGuiDataType_U64, &value.m_AsU64);
			return true;
		case sStatData::Type::STRING:
			ImGui::InputText("值", value.m_AsString, sizeof(value.m_AsString));
			return true;
		case sStatData::Type::PACKED:
			ImGui::Bitfield("值", &value.m_AsU64);
			return true;
		case sStatData::Type::POS:
			ImGui::PushItemWidth(70.0f);
			ImGui::InputFloat("X", &value.m_AsFloat[0]);
			ImGui::SameLine();
			ImGui::InputFloat("Y", &value.m_AsFloat[1]);
			ImGui::SameLine();
			ImGui::InputFloat("Z", &value.m_AsFloat[2]);
			ImGui::PopItemWidth();
			return true;
		case sStatData::Type::DATE:
			ImGui::PushItemWidth(70.0f);
			ImGui::InputScalar("年", ImGuiDataType_S32, &value.m_Date.Year);
			ImGui::SameLine();
			ImGui::InputScalar("月", ImGuiDataType_S32, &value.m_Date.Month);
			ImGui::SameLine();
			ImGui::InputScalar("日", ImGuiDataType_S32, &value.m_Date.Day);
			ImGui::InputScalar("时", ImGuiDataType_S32, &value.m_Date.Hour);
			ImGui::SameLine();
			ImGui::InputScalar("分", ImGuiDataType_S32, &value.m_Date.Minute);
			ImGui::SameLine();
			ImGui::InputScalar("秒", ImGuiDataType_S32, &value.m_Date.Second);
			ImGui::SameLine();
			ImGui::InputScalar("毫秒", ImGuiDataType_S32, &value.m_Date.Millisecond);
			ImGui::PopItemWidth();
			if (!CheckDate(value.m_Date))
			{
				ImGui::TextColored(ImVec4(0.957f, 0.643f, 0.376f, 1.0f), "日期或时间无效，请检查输入。");
				return false;
			}
			return true;
		case sStatData::Type::PROFILE_SETTING:
		case sStatData::Type::TEXTLABEL:
		default:
			ImGui::BeginDisabled();
			ImGui::Text("%s", "不支持该数据类型");
			ImGui::EndDisabled();
			return false; // data type not supported
		}
	}

	static PackedStatInfo GetPackedStatInfo(int index)
	{
		PackedStatInfo info{};
		int row;
		bool unk;

		info.m_Index = index;
		Pointers.GetPackedStatData(index, &row, &info.m_IsBoolStat, &unk);

		if (row != 0 || index <= 191)
			info.m_IsValid = true;

		return info;
	}

	static void ReadPackedStat(StatValue& value, const PackedStatInfo& info)
	{
		if (info.m_IsBoolStat)
			value.m_AsBool = STATS::GET_PACKED_STAT_BOOL_CODE(info.m_Index, -1);
		else
			value.m_AsInt = STATS::GET_PACKED_STAT_INT_CODE(info.m_Index, -1);
	}

	static void WritePackedStat(const StatValue& value, const PackedStatInfo& info)
	{
		if (info.m_IsBoolStat)
			STATS::SET_PACKED_STAT_BOOL_CODE(info.m_Index, value.m_AsBool, -1);
		else
			STATS::SET_PACKED_STAT_INT_CODE(info.m_Index, value.m_AsInt, -1);
	}

	static void WritePackedStatRange(int start, int end, int value)
	{
		const auto itemCount = static_cast<std::int64_t>(end) - static_cast<std::int64_t>(start) + 1;
		if (itemCount <= 0 || itemCount > static_cast<std::int64_t>(kMaxPackedRangeWrites))
		{
			LOG(WARNING) << "Packed Stat 批量写入范围无效或超过 4096 项。";
			return;
		}

		for (int i = start;; ++i)
		{
			auto info = GetPackedStatInfo(i);
			if (!info.m_IsValid)
				break; // the rest are probably not valid, either

			if (info.m_IsBoolStat)
				STATS::SET_PACKED_STAT_BOOL_CODE(info.m_Index, static_cast<bool>(value), -1);

			if (i == end)
				break;
		}
	}

	static bool RenderPackedStatEditor(StatValue& value, const PackedStatInfo& info)
	{
		ImGui::SetNextItemWidth(150.f);
		if (info.m_IsBoolStat)
			return ImGui::Checkbox("值##packed", &value.m_AsBool);
		else
			return ImGui::InputScalar("值##packed", ImGuiDataType_U8, &value.m_AsInt);
	}

	std::shared_ptr<Category> BuildStatEditorMenu()
	{
		auto menu = std::make_shared<Category>("数据编辑器");
		auto normal = std::make_shared<Group>("常规");
		auto packed = std::make_shared<Group>("打包");
		auto packed_range = std::make_shared<Group>("打包范围");
		auto from_clipboard = std::make_shared<Group>("从剪贴板");

		normal->AddItem(std::make_unique<ImGuiItem>([] {
			if (!NativeInvoker::AreHandlersCached())
				return ImGui::TextDisabled("%s", Localization::Translate("Natives not cached yet.").c_str());

			static StatInfo current_info;
			static char stat_buf[48]{};
			static StatValue value{};

			ImGui::SetNextItemWidth(300.f);
			if (ImGui::InputText("名称", stat_buf, sizeof(stat_buf)))
			{
				current_info = GetStatInfo(stat_buf);
				if (current_info.IsValid())
					ReadStat(current_info.m_NameHash, value, current_info.m_Data);
			}

			if (!current_info.IsValid())
				return ImGui::TextDisabled("%s", Localization::Translate("Stat not found").c_str());
			else if (current_info.m_Normalized)
			{
				ImGui::Text("名称规范化为：%s", current_info.m_Name.data());
			}

			const bool supported = RenderStatEditor(value, current_info.m_Data);
			const bool can_edit = supported && !current_info.m_Data->IsControlledByNetshop();

			if (ImGui::Button("刷新"))
				ReadStat(current_info.m_NameHash, value, current_info.m_Data);
			ImGui::SameLine();
			ImGui::BeginDisabled(!can_edit);
			if (ImGui::Button("写入"))
				FiberPool::Push([] {
					WriteStat(current_info.m_NameHash, value, current_info.m_Data);
				});
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
				FiberPool::Push([] {
					WriteStat(current_info.m_NameHash, value, current_info.m_Data);
				});
			if (!can_edit && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
				ImGui::SetTooltip("%s", "该数据不应由客户端修改。若仍要写入，请右键强制执行。");
			ImGui::EndDisabled();
		}));

		packed->AddItem(std::make_unique<ImGuiItem>([] {
			if (!NativeInvoker::AreHandlersCached())
				return ImGui::TextDisabled("%s", Localization::Translate("Natives not cached yet.").c_str());

			// TODO: improve packed stat editor
			static PackedStatInfo current_info{0, false, true};
			static StatValue value{};

			ImGui::SetNextItemWidth(200.f);
			if (ImGui::InputInt("索引", &current_info.m_Index))
			{
				current_info = GetPackedStatInfo(current_info.m_Index);
				if (current_info.IsValid())
					ReadPackedStat(value, current_info);
			}

			if (!current_info.IsValid())
				return ImGui::TextDisabled("%s", Localization::Translate("Invalid index").c_str());

			RenderPackedStatEditor(value, current_info);

			if (ImGui::Button("刷新##packed"))
				ReadPackedStat(value, current_info);
			ImGui::SameLine();
			if (ImGui::Button("写入##packed"))
				FiberPool::Push([] {
					WritePackedStat(value, current_info);
				});
		}));

		packed_range->AddItem(std::make_unique<ImGuiItem>([] {
			if (!NativeInvoker::AreHandlersCached())
				return ImGui::TextDisabled("%s", Localization::Translate("Natives not cached yet.").c_str());

			static int start{}, end{}, value{};

			ImGui::SetNextItemWidth(150.f);
			ImGui::InputInt("起始", &start);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(150.f);
			ImGui::InputInt("结束", &end);
			ImGui::SetNextItemWidth(150.f);
			ImGui::InputScalar("值##packed_range", ImGuiDataType_U8, &value);
			ImGui::SameLine();
			if (ImGui::Button("写入##packed_range"))
				FiberPool::Push([] {
					WritePackedStatRange(start, end, value);
				});
		}));

		from_clipboard->AddItem(std::make_unique<ImGuiItem>([] {
			if (!NativeInvoker::AreHandlersCached())
				return ImGui::TextDisabled("%s", Localization::Translate("Natives not cached yet.").c_str());

			if (ImGui::Button("从剪贴板加载"))
			{
				const auto clipboard = ImGui::GetClipboardText();
				if (!clipboard)
					return;

				const auto clipboardSize = std::strlen(clipboard);
				if (clipboardSize > kMaxClipboardBytes)
				{
					LOG(WARNING) << "剪贴板数据超过 1 MiB，已拒绝导入。";
					return;
				}

				auto clipText = std::string(clipboard, clipboardSize);
				FiberPool::Push([clipText = std::move(clipText)] {
					std::size_t lineCount = 0;
					for (auto line : clipText | std::ranges::views::split('\n'))
					{
						if (++lineCount > kMaxClipboardLines)
						{
							LOG(WARNING) << "剪贴板数据超过 4096 行，已停止导入。";
							break;
						}

						const auto lineText = TrimString(std::string_view{line.begin(), line.end()});
						const auto separator = lineText.find('=');

						if (separator == std::string_view::npos || lineText.find('=', separator + 1) != std::string_view::npos)
						{
							LOGF(WARNING, "Load From Clipboard: line \"{}\" is malformed", lineText);
							continue;
						}

						auto info = GetStatInfo(TrimString(lineText.substr(0, separator)));
						if (!info.IsValid())
						{
							LOGF(WARNING, "Load From Clipboard: cannot find stat {}", lineText.substr(0, separator));
							continue;
						}

						WriteStatWithStringValue(info.m_NameHash, TrimString(lineText.substr(separator + 1)), info.m_Data);
					}
				});
			}
		}));

		menu->AddItem(std::move(normal));
		menu->AddItem(std::move(packed));
		menu->AddItem(std::move(packed_range));
		menu->AddItem(std::move(from_clipboard));
		return menu;
	}
}
