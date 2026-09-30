#pragma once

#include <type_traits>

namespace ImGui
{
	template<typename T>
	bool Bitfield(const char* name, T* param)
	{
		using Value = std::remove_cv_t<T>;
		static_assert(std::is_integral_v<Value> && !std::is_same_v<Value, bool>);
		static_assert(sizeof(Value) == 1 || sizeof(Value) == 2 || sizeof(Value) == 4 || sizeof(Value) == 8);
		using UnsignedValue = std::make_unsigned_t<Value>;

		bool ret = false;
		ImGuiDataType type = ImGuiDataType_U8;
		const char* format = "%02X";
		constexpr size_t size = sizeof(Value);
		if constexpr (size == 2)
		{
			type = ImGuiDataType_U16;
			format = "%04X";
		}
		else if constexpr (size == 4)
		{
			type = ImGuiDataType_U32;
			format = "%08X";
		}
		else if constexpr (size == 8)
		{
			type = ImGuiDataType_U64;
			format = "%016llX";
		}

		auto* unsignedParam = reinterpret_cast<UnsignedValue*>(param);
		if (ImGui::InputScalar(name, type, unsignedParam, nullptr, nullptr, format, ImGuiInputTextFlags_CharsHexadecimal))
			ret = true;
		for (size_t i = size * 8; i-- > 0;)
		{
			if (i % 4 == 0)
				ImGui::BeginGroup();
			const auto mask = static_cast<UnsignedValue>(UnsignedValue{1} << i);
			if (*unsignedParam & mask)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32_BLACK_TRANS);
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(41, 134, 204, 255));
				if (ImGui::Button(std::format("1##{}{}", name, i).c_str(), ImVec2(25, 0)))
				{
					ret = true;
					*unsignedParam ^= mask;
				}
				ImGui::PopStyleColor(2);
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32_BLACK_TRANS);
				if (ImGui::Button(std::format("0##{}{}", name, i).c_str(), ImVec2(25, 0)))
				{
					ret = true;
					*unsignedParam ^= mask;
				}
				ImGui::PopStyleColor();
			}
			if (i % 4 == 0)
			{
				ImGui::Text("%llu", static_cast<unsigned long long>(i));
				ImGui::EndGroup();
			}
			if (i % 16 != 0)
				ImGui::SameLine();
		}

		return ret;
	}
}
