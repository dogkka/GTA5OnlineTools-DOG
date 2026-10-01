#include "ScriptEventSender.hpp"

#include <algorithm>
#include <cstdlib>

#include "core/backend/FiberPool.hpp"
#include "core/util/Joaat.hpp"
#include "game/backend/Players.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "types/script/ScriptEvent.hpp"

namespace YimMenu::Submenus
{
	std::shared_ptr<Category> BuildScriptEventSenderMenu()
	{
		auto menu = std::make_shared<Category>("脚本事件发送器");

		auto sender = std::make_shared<Group>("发送", 1);
		sender->AddItem(std::make_shared<ImGuiItem>([] {
			static char hash_buf[32]{};
			static int64_t params[8]{};
			static int param_count = 1;
			static std::string last_result;

			ImGui::InputTextWithHint("##evhash", "事件哈希（十进制 / 0x十六进制 / 名称）", hash_buf, sizeof(hash_buf));

			ImGui::SetNextItemWidth(120.f);
			ImGui::InputInt("参数个数", &param_count);
			param_count = std::clamp(param_count, 0, 8);

			for (int i = 0; i < param_count; i++)
			{
				ImGui::SetNextItemWidth(200.f);
				ImGui::InputScalar(("参数 " + std::to_string(i)).c_str(), ImGuiDataType_S64, &params[i]);
			}

			auto selected = Players::GetSelected();
			if (selected.IsValid())
				ImGui::Text("目标：%s（ID %d）", selected.GetName(), selected.GetId());
			else
				ImGui::TextColored(ImVec4(1.0f, 0.55f, 0.4f, 1.0f), "目标：未选中（请先在玩家页选择目标）");

			if (ImGui::Button("发送"))
			{
				if (!selected.IsValid())
				{
					last_result = "失败：未选中玩家";
				}
				else if (hash_buf[0] == '\0')
				{
					last_result = "失败：事件哈希为空";
				}
				else
				{
					const std::string hash_str = hash_buf;
					int64_t values[8]{};
					if (param_count > 0)
						std::copy(params, params + param_count, values);
					const int count = param_count;
					auto target     = selected;

					FiberPool::Push([hash_str, values, count, target] {
						char* end               = nullptr;
						const long long parsed  = std::strtoll(hash_str.c_str(), &end, 0);
						const int64_t event_hash = (end && *end == '\0' && end != hash_str.c_str())
						    ? parsed
						    : static_cast<int64_t>(static_cast<int32_t>(Joaat(hash_str)));

						const int id   = target.GetId();
						const int bits = 1 << id;

						int64_t args[32]{};
						args[0] = event_hash;
						args[1] = Self::GetPlayer().GetId();
						args[2] = bits;
						for (int i = 0; i < count; i++)
							args[3 + i] = values[i];

						const int size = 3 + (count > 0 ? count : 1);
						SCRIPT::_SEND_TU_SCRIPT_EVENT_NEW(1, args, size, bits, static_cast<Hash>(event_hash));
					});

					last_result = "已发送到玩家 ID " + std::to_string(selected.GetId());
				}
			}

			if (!last_result.empty())
				ImGui::Text("%s", last_result.c_str());

			ImGui::Separator();
			ImGui::TextWrapped("%s", "用法：向选中玩家发送任意脚本事件。哈希支持十进制、0x 十六进制或名称（自动 Joaat）。"
			                        "字符串参数请用网页控制台的 s: 前缀语法（本页仅支持整数参数）。");
		}));

		menu->AddItem(sender);
		return menu;
	}
}
