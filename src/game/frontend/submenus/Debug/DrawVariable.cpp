#include "DrawVariable.hpp"
#include "core/localization/Localization.hpp"
#include "core/frontend/widgets/imgui_bitfield.hpp"
#include "types/script/scrVector.hpp"

#include <array>

namespace YimMenu
{
	void DrawSavedVariableEdit(const SavedVariableBase& var, void* value)
	{
		if (!value)
			return ImGui::TextDisabled("%s", "无效");

		switch (var.type)
		{
		case VariableType::INT:
		{
			ImGui::SetNextItemWidth(200.f);
			ImGui::InputScalar("值", ImGuiDataType_S32, static_cast<int*>(value));
			break;
		}
		case VariableType::INT64:
		{
			ImGui::SetNextItemWidth(200.f);
			ImGui::InputScalar("值", ImGuiDataType_S64, static_cast<std::int64_t*>(value));
			break;
		}
		case VariableType::BITSET:
		{
			ImGui::SetNextItemWidth(200.f);
			ImGui::Bitfield("值", static_cast<int*>(value));
			break;
		}
		case VariableType::BOOL:
		{
			ImGui::Checkbox("值", static_cast<bool*>(value));
			break;
		}
		case VariableType::FLOAT:
		{
			ImGui::SetNextItemWidth(200.f);
			ImGui::InputScalar("值", ImGuiDataType_Float, static_cast<float*>(value));
			break;
		}
		case VariableType::VECTOR:
		{
			auto vectorPtr = (rage::scrVector*)value;
			ImGui::PushItemWidth(100.f);
			ImGui::InputScalar("X 坐标", ImGuiDataType_Float, &vectorPtr->x);
			ImGui::SameLine();
			ImGui::InputScalar("Y 坐标", ImGuiDataType_Float, &vectorPtr->y);
			ImGui::SameLine();
			ImGui::InputScalar("Z 坐标", ImGuiDataType_Float, &vectorPtr->z);
			ImGui::PopItemWidth();
			break;
		}
		case VariableType::STRING:
		{
			ImGui::SetNextItemWidth(200.f);
			ImGui::InputText("值", static_cast<char*>(value), 255);
			break;
		}
		}
	}

	void DrawSavedVariablePreview(const SavedVariableBase& var, void* value)
	{
		if (!value)
		{
			ImGui::Selectable("无效", false, ImGuiSelectableFlags_Disabled);
			return;
		}

		switch (var.type)
		{
		case VariableType::INT:
		{
			ImGui::Selectable(std::to_string(*static_cast<int*>(value)).c_str(), false, ImGuiSelectableFlags_Disabled);
			break;
		}
		case VariableType::INT64:
		{
			ImGui::Selectable(std::to_string(*static_cast<std::int64_t*>(value)).c_str(), false, ImGuiSelectableFlags_Disabled);
			break;
		}
		case VariableType::BITSET:
		{
			std::ostringstream bitset;
			bitset << HEX(*static_cast<int*>(value));
			ImGui::Selectable(bitset.str().c_str(), false, ImGuiSelectableFlags_Disabled);
			break;
		}
		case VariableType::BOOL:
		{
			ImGui::Selectable(*static_cast<bool*>(value) ? "真" : "假", false, ImGuiSelectableFlags_Disabled);
			break;
		}
		case VariableType::FLOAT:
		{
			ImGui::Selectable(std::to_string(*static_cast<float*>(value)).c_str(), false, ImGuiSelectableFlags_Disabled);
			break;
		}
		case VariableType::VECTOR:
		{
			auto vectorPtr = (rage::scrVector*)value;
			std::ostringstream vector;
			vector << Localization::Translate("X").c_str() << "：" << std::fixed << std::setprecision(2) << vectorPtr->x
			       << " " << Localization::Translate("Y").c_str() << "：" << vectorPtr->y
			       << " " << Localization::Translate("Z").c_str() << "：" << vectorPtr->z;
			ImGui::Selectable(vector.str().c_str(), false, ImGuiSelectableFlags_Disabled);
			break;
		}
		case VariableType::STRING:
		{
			ImGui::Selectable(static_cast<char*>(value), false, ImGuiSelectableFlags_Disabled);
			break;
		}
		}
	}

	void DrawSavedVariable(SavedVariableBase& var)
	{
		uint32_t step = 1;
		uint32_t step_fast = 100;
		static const std::array<const char*, 7> variableTypeKeys = {"整数", "64位整数", "位集", "布尔", "浮点", "向量", "字符串"};

		ImGui::SetNextItemWidth(200.0f);
		ImGui::InputScalar("索引", ImGuiDataType_U32, &var.base, &step, &step_fast);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.0f);
		if (ImGui::BeginCombo("##var_type", variableTypeKeys[static_cast<std::size_t>(var.type)]))
		{
			for (std::size_t i = 0; i < variableTypeKeys.size(); ++i)
			{
				const bool selected = static_cast<std::size_t>(var.type) == i;
				if (ImGui::Selectable(variableTypeKeys[i], selected))
					var.type = static_cast<VariableType>(i);

				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		for (int i = 0; i < var.appendages.size(); i++)
		{
			auto& item = var.appendages[i];
			switch (item.type)
			{
			case VariableAppendage::Type::OFFSET:
				ImGui::SetNextItemWidth(125.f);
				ImGui::InputScalar(std::format("{}##{}{}", "位置", i, (int)item.type).c_str(), ImGuiDataType_S32, &item.offset, &step, &step_fast);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(125.f);
				ImGui::InputScalar(std::format("{}##{}{}", "大小", i, (int)item.type).c_str(), ImGuiDataType_S32, &item.size, &step, &step_fast);
				break;
			case VariableAppendage::Type::PLAYER_ID:
				ImGui::SetNextItemWidth(125.f);
				ImGui::InputScalar(std::format("{}##{}{}", "大小", i, (int)item.type).c_str(), ImGuiDataType_S32, &item.size, &step, &step_fast);
				break;
			}
		}

		if (ImGui::Button("添加偏移"))
			var.appendages.push_back({VariableAppendage::Type::OFFSET, 0LL, 0ULL});
		ImGui::SameLine();
		if (ImGui::Button("添加读取玩家编号"))
			var.appendages.push_back({VariableAppendage::Type::PLAYER_ID, 0LL, 0ULL});

		if (var.appendages.size() > 0)
		{
			ImGui::SameLine();
			if (ImGui::Button("移除偏移"))
			{
				var.appendages.pop_back();
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("清除"))
		{
			var.base = 0;
			var.appendages.clear();
		}
	}
}
