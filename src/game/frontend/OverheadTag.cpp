#include "OverheadTag.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <string>

#include "core/commands/BoolCommand.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/StringCommand.hpp"
#include "core/util/Joaat.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/invoker/Invoker.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	static BoolCommand _OverheadTag{"overheadtag", "头顶发光文字", "在自己的头顶显示一条发光文字（本地可见，菜单关闭后依然显示）。"};
	static StringCommand _OverheadTagText{"overheadtagtext", "文字内容", "头顶要显示的文字。", "DOG"};
	static FloatCommand _OverheadTagSize{"overheadtagsize", "文字大小", "头顶文字的字体大小。", 10.0f, 160.0f, 46.0f};
	static ListCommand _OverheadTagColor{"overheadtagcolor", "文字颜色", "头顶文字的主色与辉光颜色。", {{0, "闪耀金"}, {1, "纯白"}, {2, "烈焰红"}, {3, "科技青"}, {4, "少女粉"}, {5, "幻梦紫"}, {6, "流光绿"}}, 0};
	static BoolCommand _OverheadTagGlow{"overheadtagglow", "发光描影", "为文字叠加描边与呼吸辉光。", true};
}

namespace YimMenu
{
	namespace
	{
		struct TagPalette
		{
			ImU32 core;
			ImU32 glow;
		};

		constexpr TagPalette kTagColors[] = {
		    {IM_COL32(255, 216, 96, 255), IM_COL32(255, 158, 32, 255)},  // 闪耀金
		    {IM_COL32(250, 252, 255, 255), IM_COL32(150, 205, 255, 255)}, // 纯白
		    {IM_COL32(255, 86, 72, 255), IM_COL32(255, 36, 24, 255)},     // 烈焰红
		    {IM_COL32(96, 240, 255, 255), IM_COL32(28, 178, 255, 255)},   // 科技青
		    {IM_COL32(255, 128, 214, 255), IM_COL32(255, 62, 174, 255)},  // 少女粉
		    {IM_COL32(196, 138, 255, 255), IM_COL32(142, 74, 255, 255)},  // 幻梦紫
		    {IM_COL32(126, 255, 148, 255), IM_COL32(44, 224, 96, 255)},   // 流光绿
		};

		void DrawOverheadTagImpl()
		{
			auto enabled = Commands::GetCommand<BoolCommand>("overheadtag"_J);
			if (!enabled || !enabled->GetState())
				return;

			auto text_cmd = Commands::GetCommand<StringCommand>("overheadtagtext"_J);
			auto size_cmd = Commands::GetCommand<FloatCommand>("overheadtagsize"_J);
			auto color_cmd = Commands::GetCommand<ListCommand>("overheadtagcolor"_J);
			auto glow_cmd = Commands::GetCommand<BoolCommand>("overheadtagglow"_J);
			if (!text_cmd || !size_cmd || !color_cmd || !glow_cmd)
				return;

			const std::string text = text_cmd->GetString();
			if (text.empty())
				return;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			// 37193 = FB_Brow_Centre_000，鼻梁上方，头顶文字最稳的挂点（与 ESP 一致）
			const auto head = ped.GetBonePosition(37193);
			if (head.x == 0.0f && head.y == 0.0f && head.z == 0.0f)
				return;

			float screen_x{}, screen_y{};
			if (!GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(head.x, head.y, head.z + 0.55f, &screen_x, &screen_y))
				return;

			if (!Pointers.ScreenResX || !Pointers.ScreenResY || !*Pointers.ScreenResX || !*Pointers.ScreenResY)
				return;

			const ImVec2 pos{screen_x * (float)*Pointers.ScreenResX, screen_y * (float)*Pointers.ScreenResY};

			const float size   = size_cmd->GetState();
			const int color_idx = std::max(0, std::min(color_cmd->GetState(), (int)(sizeof(kTagColors) / sizeof(kTagColors[0])) - 1));
			const bool glow    = glow_cmd->GetState();

			ImFont* font = ImGui::GetFont();
			if (!font)
				return;

			const ImVec2 text_size = font->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str());
			const ImVec2 origin(pos.x - text_size.x * 0.5f, pos.y - text_size.y * 0.5f);

			ImDrawList* dl = ImGui::GetBackgroundDrawList();
			if (!dl)
				return;

			const auto& pal = kTagColors[color_idx];

			if (glow)
			{
				// 呼吸脉冲
				const float time  = (float)ImGui::GetTime();
				const float pulse = 0.62f + 0.38f * (0.5f + 0.5f * std::sin(time * 3.4f));
				const float radius = std::max(2.0f, size * 0.075f);

				// 一圈辉光点 + 外圈淡光，模拟发光
				for (int i = 0; i < 12; i++)
				{
					const float angle = (float)i / 12.0f * 6.2831853f;
					const float ox    = std::cos(angle) * radius;
					const float oy    = std::sin(angle) * radius;

					const int inner_a = (int)(130.0f * pulse);
					const int outer_a = (int)(55.0f * pulse);

					dl->AddText(font, size, ImVec2(origin.x + ox, origin.y + oy), (pal.glow & 0x00FFFFFF) | ((ImU32)inner_a << 24), text.c_str());
					dl->AddText(font, size, ImVec2(origin.x + ox * 1.7f, origin.y + oy * 1.7f), (pal.glow & 0x00FFFFFF) | ((ImU32)outer_a << 24), text.c_str());
				}
			}

			// 暗色描影 + 主色文字，保证亮景也能看清
			dl->AddText(font, size, ImVec2(origin.x + 2.0f, origin.y + 2.0f), IM_COL32(0, 0, 0, 190), text.c_str());
			dl->AddText(font, size, origin, pal.core, text.c_str());
		}
	}

	void OverheadTag::Draw()
	{
		if (!NativeInvoker::AreHandlersCached())
			return;
		if (HUD::IS_PAUSE_MENU_ACTIVE() || CAMERA::IS_SCREEN_FADED_OUT())
			return;

		// 渲染线程读取游戏状态有极低概率的竞态，与 ESP / 速度表同样加一层结构化异常兜底
		__try
		{
			DrawOverheadTagImpl();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}
