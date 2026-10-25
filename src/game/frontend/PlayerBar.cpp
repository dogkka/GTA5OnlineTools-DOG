#include "PlayerBar.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "core/commands/BoolCommand.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/util/Joaat.hpp"
#include "game/backend/Players.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/invoker/Invoker.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	static BoolCommand _PlayerBar{"playerbar", "玩家面板", "在屏幕上显示玩家列表（名字/距离/状态标签），Stand 同款思路。", true};
	static FloatCommand _PlayerBarX{"playerbarx", "面板位置 X", "面板横向位置（0-1）。", 0.0f, 1.0f, 0.84f};
	static FloatCommand _PlayerBarY{"playerbary", "面板位置 Y", "面板纵向位置（0-1）。", 0.0f, 1.0f, 0.10f};
	static FloatCommand _PlayerBarScale{"playerbarscale", "面板大小", "面板整体缩放。", 0.5f, 2.0f, 1.0f};
	static BoolCommand _PlayerBarTags{"playerbartags", "显示状态标签", "显示 主机/作弊者/载具/死亡 等标签。", true};
}

namespace YimMenu
{
	namespace
	{
		struct BarRow
		{
			std::string name;
			std::string tags;
			float dist;
			ImU32 color;
		};

		void DrawPlayerBarImpl()
		{
			auto enabled = Commands::GetCommand<BoolCommand>("playerbar"_J);
			if (!enabled || !enabled->GetState())
				return;

			auto x_cmd = Commands::GetCommand<FloatCommand>("playerbarx"_J);
			auto y_cmd = Commands::GetCommand<FloatCommand>("playerbary"_J);
			auto scale_cmd = Commands::GetCommand<FloatCommand>("playerbarscale"_J);
			auto tags_cmd = Commands::GetCommand<BoolCommand>("playerbartags"_J);
			if (!x_cmd || !y_cmd || !scale_cmd || !tags_cmd)
				return;

			if (!Pointers.ScreenResX || !Pointers.ScreenResY || !*Pointers.ScreenResX || !*Pointers.ScreenResY)
				return;

			auto self = Self::GetPlayer();
			if (!self.IsValid())
				return;

			auto selfPed = Self::GetPed();
			if (!selfPed)
				return;

			const auto selfPos = selfPed.GetPosition();
			const float scale  = scale_cmd->GetState();
			const bool showTags = tags_cmd->GetState();

			std::vector<BarRow> rows;
			for (auto& [id, player] : Players::GetPlayers())
			{
				if (!player.IsValid() || player.GetId() == self.GetId())
					continue;

				auto ped = player.GetPed();
				if (!ped)
					continue;

				const auto pos = ped.GetPosition();
				const float dx = pos.x - selfPos.x;
				const float dy = pos.y - selfPos.y;
				const float dz = pos.z - selfPos.z;
				const float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

				std::string tags;
				if (showTags)
				{
					if (player.IsHost())
						tags += "主机 ";
					if (player.IsModder())
						tags += "作弊者 ";
					if (ped.IsDead())
						tags += "死亡 ";
					if (ped.GetVehicle())
						tags += "载具 ";
				}

				ImU32 color = IM_COL32(235, 240, 250, 255);
				if (player.IsModder())
					color = IM_COL32(255, 118, 118, 255);
				else if (player.IsHost())
					color = IM_COL32(120, 220, 255, 255);

				rows.push_back({player.GetName(), tags, dist, color});
			}

			std::sort(rows.begin(), rows.end(), [](const BarRow& a, const BarRow& b) { return a.dist < b.dist; });

			ImFont* font = ImGui::GetFont();
			ImDrawList* dl = ImGui::GetBackgroundDrawList();
			if (!font || !dl)
				return;

			const float font_size  = 16.0f * scale;
			const float line_h     = 22.0f * scale;
			const float pad        = 10.0f * scale;
			const float title_size = 17.0f * scale;
			const float width      = 240.0f * scale;
			const float height     = line_h * (float)(rows.size() + 1) + pad * 2.0f;

			const ImVec2 origin{*Pointers.ScreenResX * x_cmd->GetState(), *Pointers.ScreenResY * y_cmd->GetState()};
			const ImVec2 p0 = origin;
			const ImVec2 p1 = ImVec2(origin.x + width, origin.y + height);

			dl->AddRectFilled(ImVec2(p0.x + 3.0f, p0.y + 3.0f), ImVec2(p1.x + 3.0f, p1.y + 3.0f), IM_COL32(0, 0, 0, 90), 8.0f * scale);
			dl->AddRectFilled(p0, p1, IM_COL32(16, 20, 28, 175), 8.0f * scale);
			dl->AddRect(p0, p1, IM_COL32(96, 150, 220, 190), 8.0f * scale, 0, 1.4f * scale);

			char title[64];
			snprintf(title, sizeof(title), "玩家在线 %d", (int)rows.size());
			dl->AddText(font, title_size, ImVec2(p0.x + pad, p0.y + pad * 0.7f), IM_COL32(140, 200, 255, 255), title);

			float y = p0.y + pad * 0.7f + line_h;
			for (const auto& row : rows)
			{
				char right[48];
				snprintf(right, sizeof(right), "%.0fm", row.dist);
				const ImVec2 right_size = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, right);

				dl->AddText(font, font_size, ImVec2(p0.x + pad, y), row.color, row.name.c_str());

				if (!row.tags.empty())
				{
					const float name_w = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, row.name.c_str()).x;
					dl->AddText(font, font_size, ImVec2(p0.x + pad + name_w + 6.0f * scale, y), IM_COL32(255, 200, 120, 235), row.tags.c_str());
				}

				dl->AddText(font, font_size, ImVec2(p1.x - pad - right_size.x, y), IM_COL32(180, 190, 205, 235), right);
				y += line_h;
			}
		}
	}

	void PlayerBar::Draw()
	{
		if (!NativeInvoker::AreHandlersCached())
			return;
		if (HUD::IS_PAUSE_MENU_ACTIVE() || CAMERA::IS_SCREEN_FADED_OUT())
			return;

		__try
		{
			DrawPlayerBarImpl();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}
