#include "SpeedoHUD.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "core/commands/Commands.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/util/Joaat.hpp"
#include "game/backend/Self.hpp"
#include "game/frontend/GUI.hpp"
#include "game/frontend/Menu.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/invoker/Invoker.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu
{
	namespace
	{
		constexpr float kPi = 3.14159265f;

		constexpr float kGaugeStart = kPi * 0.75f; // 135°
		constexpr float kGaugeSweep = kPi * 1.5f;  // 270°

		// 现代仪表配色
		constexpr ImU32 kColText    = IM_COL32(240, 244, 250, 255);
		constexpr ImU32 kColTextDim = IM_COL32(150, 158, 172, 255);
		constexpr ImU32 kColPanel   = IM_COL32(20, 24, 33, 208);
		constexpr ImU32 kColBorder  = IM_COL32(92, 102, 124, 230);
		constexpr ImU32 kColFace    = IM_COL32(15, 18, 25, 235);
		constexpr ImU32 kColTrack   = IM_COL32(52, 58, 72, 255);
		constexpr ImU32 kColSpeed0  = IM_COL32(64, 148, 255, 255);
		constexpr ImU32 kColSpeed1  = IM_COL32(122, 214, 255, 255);
		constexpr ImU32 kColRpm0    = IM_COL32(255, 156, 64, 255);
		constexpr ImU32 kColRpm1    = IM_COL32(255, 92, 58, 255);
		constexpr ImU32 kColRedline = IM_COL32(255, 64, 48, 255);

		ListCommand* StyleCmd()
		{
			return Commands::GetCommand<ListCommand>("speedostyle"_J);
		}

		ListCommand* UnitsCmd()
		{
			return Commands::GetCommand<ListCommand>("speedounits"_J);
		}

		FloatCommand* PosCmd(const char* name)
		{
			return Commands::GetCommand<FloatCommand>(Joaat(name));
		}

		bool UseMetric(int units)
		{
			if (units == 1)
				return true;
			if (units == 2)
				return false;
			return MISC::SHOULD_USE_METRIC_MEASUREMENTS();
		}

		// ------------------------------------------------ 绘制工具

		ImU32 WithAlpha(ImU32 col, int alpha)
		{
			return (col & 0x00FFFFFFu) | (static_cast<ImU32>(alpha & 0xFF) << 24);
		}

		ImU32 LerpCol(ImU32 a, ImU32 b, float t)
		{
			t = std::clamp(t, 0.0f, 1.0f);
			auto lerp = [t](int x, int y) { return static_cast<int>(x + (y - x) * t); };
			return IM_COL32(lerp(a & 0xFF, b & 0xFF),
			    lerp((a >> 8) & 0xFF, (b >> 8) & 0xFF),
			    lerp((a >> 16) & 0xFF, (b >> 16) & 0xFF),
			    lerp((a >> 24) & 0xFF, (b >> 24) & 0xFF));
		}

		void AddArc(ImDrawList* dl, ImVec2 c, float r, float a0, float a1, ImU32 col, float thick)
		{
			const int seg = std::max(2, static_cast<int>(std::fabs(a1 - a0) / (kPi / 40.0f)));
			dl->PathClear();
			for (int i = 0; i <= seg; i++)
			{
				const float a = a0 + (a1 - a0) * static_cast<float>(i) / static_cast<float>(seg);
				dl->PathLineTo(ImVec2(c.x + std::cos(a) * r, c.y + std::sin(a) * r));
			}
			dl->PathStroke(col, 0, thick);
		}

		void AddArcGradient(ImDrawList* dl, ImVec2 c, float r, float a0, float a1, ImU32 col0, ImU32 col1, float thick)
		{
			const int seg = std::max(6, static_cast<int>(std::fabs(a1 - a0) / (kPi / 36.0f)));
			for (int i = 0; i < seg; i++)
			{
				const float t0 = static_cast<float>(i) / static_cast<float>(seg);
				const float t1 = static_cast<float>(i + 1) / static_cast<float>(seg);
				const float fa = a0 + (a1 - a0) * t0;
				const float fb = a0 + (a1 - a0) * t1;
				dl->AddLine(ImVec2(c.x + std::cos(fa) * r, c.y + std::sin(fa) * r),
				    ImVec2(c.x + std::cos(fb) * r, c.y + std::sin(fb) * r),
				    LerpCol(col0, col1, (t0 + t1) * 0.5f), thick);
			}
		}

		// 发光进度弧：三层（宽弱 → 中 → 实心）
		void AddArcGradientGlow(ImDrawList* dl, ImVec2 c, float r, float a0, float a1, ImU32 col0, ImU32 col1, float thick)
		{
			AddArcGradient(dl, c, r, a0, a1, WithAlpha(col0, 40), WithAlpha(col1, 40), thick * 2.6f);
			AddArcGradient(dl, c, r, a0, a1, WithAlpha(col0, 90), WithAlpha(col1, 90), thick * 1.5f);
			AddArcGradient(dl, c, r, a0, a1, col0, col1, thick);
		}

		void AddTextCentered(ImDrawList* dl, ImFont* font, float size, ImVec2 c, ImU32 col, const char* text)
		{
			if (!font || !text)
				return;
			const ImVec2 ts = font->CalcTextSizeA(size, FLT_MAX, 0.0f, text);
			dl->AddText(font, size, ImVec2(c.x - ts.x * 0.5f, c.y - ts.y * 0.5f), col, text);
		}

		void AddTextLeft(ImDrawList* dl, ImFont* font, float size, ImVec2 pos, ImU32 col, const char* text)
		{
			if (!font || !text)
				return;
			dl->AddText(font, size, pos, col, text);
		}

		// 卡片面板：阴影 + 渐变填充 + 描边
		void DrawPanel(ImDrawList* dl, ImVec2 p0, ImVec2 p1, float rounding)
		{
			dl->AddRectFilled(ImVec2(p0.x + 3.0f, p0.y + 4.0f), ImVec2(p1.x + 3.0f, p1.y + 4.0f), IM_COL32(0, 0, 0, 110), rounding);
			dl->AddRectFilledMultiColor(p0, p1, WithAlpha(kColPanel, 235), WithAlpha(kColPanel, 235), kColPanel, kColPanel);
			dl->AddRect(p0, p1, kColBorder, rounding);
		}

		// 档位徽章：圆角底 + 居中文字
		void DrawGearChip(ImDrawList* dl, ImFont* cjk, ImVec2 c, const char* text, ImU32 accent)
		{
			const ImVec2 p0(c.x - 30.0f, c.y - 14.0f);
			const ImVec2 p1(c.x + 30.0f, c.y + 14.0f);
			dl->AddRectFilled(p0, p1, WithAlpha(accent, 46), 7.0f);
			dl->AddRect(p0, p1, WithAlpha(accent, 200), 7.0f);
			AddTextCentered(dl, cjk, 18.0f, c, IM_COL32(230, 238, 248, 255), text);
		}

		// 表针：三角 + 中心轴
		void DrawNeedle(ImDrawList* dl, ImVec2 c, float r, float angle, ImU32 col)
		{
			const ImVec2 tip(c.x + std::cos(angle) * (r - 12.0f), c.y + std::sin(angle) * (r - 12.0f));
			const ImVec2 left(c.x + std::cos(angle + kPi * 0.5f) * 5.5f, c.y + std::sin(angle + kPi * 0.5f) * 5.5f);
			const ImVec2 right(c.x + std::cos(angle - kPi * 0.5f) * 5.5f, c.y + std::sin(angle - kPi * 0.5f) * 5.5f);
			dl->AddTriangleFilled(tip, left, right, col);
			dl->AddCircleFilled(c, 7.0f, IM_COL32(30, 34, 44, 255), 24);
			dl->AddCircle(c, 7.0f, IM_COL32(205, 214, 228, 255), 24, 2.0f);
			dl->AddCircleFilled(c, 2.8f, col, 16);
		}

		// 大表盘底盘（表圈 + 刻度 + 轨道 + 红区 + 发光进度 + 指针）
		void DrawDialBase(ImDrawList* dl, ImVec2 c, float r, float ratio, ImU32 col0, ImU32 col1, bool redline)
		{
			ratio = std::clamp(ratio, 0.0f, 1.0f);

			dl->AddCircleFilled(ImVec2(c.x + 3.0f, c.y + 4.0f), r + 8.0f, IM_COL32(0, 0, 0, 90), 56);
			dl->AddCircleFilled(c, r + 7.0f, IM_COL32(32, 36, 46, 255), 56);
			dl->AddCircle(c, r + 7.0f, WithAlpha(col0, 110), 56, 2.0f);
			dl->AddCircleFilled(c, r, kColFace, 56);

			// 轨道
			AddArc(dl, c, r - 7.0f, kGaugeStart, kGaugeStart + kGaugeSweep, kColTrack, 6.0f);

			// 红区
			if (redline)
				AddArc(dl, c, r - 7.0f, kGaugeStart + kGaugeSweep * 0.85f, kGaugeStart + kGaugeSweep, WithAlpha(kColRedline, 130), 6.0f);

			// 刻度
			for (int i = 0; i <= 20; i++)
			{
				const float a    = kGaugeStart + kGaugeSweep * static_cast<float>(i) / 20.0f;
				const bool major = (i % 5 == 0);
				const float r0   = major ? r - 18.0f : r - 12.0f;
				dl->AddLine(ImVec2(c.x + std::cos(a) * r0, c.y + std::sin(a) * r0),
				    ImVec2(c.x + std::cos(a) * (r - 4.0f), c.y + std::sin(a) * (r - 4.0f)),
				    major ? IM_COL32(170, 180, 196, 255) : IM_COL32(96, 104, 120, 255),
				    major ? 2.0f : 1.0f);
			}

			// 进度
			if (ratio > 0.002f)
				AddArcGradientGlow(dl, c, r - 7.0f, kGaugeStart, kGaugeStart + kGaugeSweep * ratio, col0, col1, 6.0f);

			DrawNeedle(dl, c, r, kGaugeStart + kGaugeSweep * ratio, LerpCol(col0, col1, ratio));
		}

		// 圆环轨道（样式 2 用）：底环 + 红区 + 发光进度环 + 端点圆帽
		void DrawRing(ImDrawList* dl, ImVec2 c, float r, float ratio, ImU32 col0, ImU32 col1, float thick, bool redline)
		{
			ratio = std::clamp(ratio, 0.0f, 1.0f);

			AddArc(dl, c, r, kGaugeStart, kGaugeStart + kGaugeSweep, kColTrack, thick);
			if (redline)
				AddArc(dl, c, r, kGaugeStart + kGaugeSweep * 0.85f, kGaugeStart + kGaugeSweep, WithAlpha(kColRedline, 130), thick);

			if (ratio > 0.002f)
			{
				const float a1 = kGaugeStart + kGaugeSweep * ratio;
				AddArcGradientGlow(dl, c, r, kGaugeStart, a1, col0, col1, thick);
				dl->AddCircleFilled(ImVec2(c.x + std::cos(a1) * r, c.y + std::sin(a1) * r), thick * 0.55f, LerpCol(col0, col1, ratio), 16);
			}
		}

		void DrawImpl()
		{
			auto style_cmd = StyleCmd();
			if (!style_cmd)
				return;

			const int style = style_cmd->GetState();
			if (style == 0) // 原版 scaleform 由 Features::Speedometer 绘制
				return;

			auto veh = Self::GetVehicle();
			if (!veh)
				return;

			const int units   = UnitsCmd() ? UnitsCmd()->GetState() : 0;
			const bool metric = UseMetric(units);

			const float speed_mps   = veh.GetSpeed();
			const float disp_speed  = metric ? speed_mps * 3.6f : speed_mps * 2.23694f;
			const float max_speed   = std::max(metric ? veh.GetMaxSpeed() * 3.6f : veh.GetMaxSpeed() * 2.23694f, 1.0f);
			const float rpm_ratio   = std::clamp(veh.GetRevRatio(), 0.0f, 1.0f);
			const int gear          = veh.GetGear();
			const float speed_ratio = std::clamp(disp_speed / (max_speed * 1.05f), 0.0f, 1.0f);
			const char* unit        = metric ? "公里/时" : "英里/时";

			char speed_buf[16]{};
			std::snprintf(speed_buf, sizeof(speed_buf), "%d", static_cast<int>(disp_speed + 0.5f));

			char rpm_buf[16]{};
			std::snprintf(rpm_buf, sizeof(rpm_buf), "%.1fk", rpm_ratio * 8.0f);

			char gear_buf[16]{};
			if (gear <= 0)
				std::snprintf(gear_buf, sizeof(gear_buf), "N");
			else
				std::snprintf(gear_buf, sizeof(gear_buf), "%d挡", gear);

			const float sw = *Pointers.ScreenResX;
			const float sh = *Pointers.ScreenResY;

			float w = 380.0f, h = 210.0f;
			if (style == 2)
			{
				w = 265.0f;
				h = 265.0f;
			}
			else if (style == 3)
			{
				w = 335.0f;
				h = 128.0f;
			}

			const float px = PosCmd("speedox") ? PosCmd("speedox")->GetState() : 1.0f;
			const float py = PosCmd("speedoy") ? PosCmd("speedoy")->GetState() : 0.85f;
			const ImVec2 origin(px * (sw - w), py * (sh - h));

			auto dl = ImGui::GetBackgroundDrawList();
			if (!dl)
				return;

			ImFont* cjk  = Menu::Font::g_DefaultFont ? Menu::Font::g_DefaultFont : ImGui::GetFont();
			ImFont* big  = Menu::Font::g_HudBigFont ? Menu::Font::g_HudBigFont : cjk;
			ImFont* over = Menu::Font::g_OverlayFont ? Menu::Font::g_OverlayFont : cjk;

			if (style == 1)
			{
				// ============ 双圆盘 ============
				const float r1 = h * 0.325f;
				const float r2 = h * 0.405f;
				const ImVec2 c1(origin.x + h * 0.40f, origin.y + h * 0.54f);
				const ImVec2 c2(origin.x + h * 1.16f, origin.y + h * 0.52f);

				// 转速盘
				DrawDialBase(dl, c1, r1, rpm_ratio, kColRpm0, kColRpm1, true);
				AddTextCentered(dl, over, 24.0f, ImVec2(c1.x, c1.y + r1 * 0.40f), IM_COL32(255, 196, 130, 255), rpm_buf);
				AddTextCentered(dl, cjk, 14.0f, ImVec2(c1.x, c1.y + r1 * 0.72f), kColTextDim, "转速");

				// 速度盘
				DrawDialBase(dl, c2, r2, speed_ratio, kColSpeed0, kColSpeed1, false);
				AddTextCentered(dl, big, std::clamp(r2 * 0.62f, 34.0f, 60.0f), ImVec2(c2.x, c2.y + r2 * 0.34f), kColText, speed_buf);
				AddTextCentered(dl, cjk, 15.0f, ImVec2(c2.x, c2.y + r2 * 0.66f), kColTextDim, unit);
				DrawGearChip(dl, cjk, ImVec2(c2.x, c2.y - r2 * 0.62f), gear_buf, kColSpeed0);
			}
			else if (style == 2)
			{
				// ============ 环形组合 ============
				const ImVec2 p0 = origin;
				const ImVec2 p1(origin.x + w, origin.y + h);
				DrawPanel(dl, p0, p1, 12.0f);

				const ImVec2 c(origin.x + w * 0.5f, origin.y + h * 0.5f);
				const float r_out = w * 0.435f;
				const float r_in  = w * 0.335f;
				const float r_mid = (r_out + r_in) * 0.5f;

				// 环间点刻度
				for (int i = 0; i <= 36; i++)
				{
					const float a     = kGaugeStart + kGaugeSweep * static_cast<float>(i) / 36.0f;
					const float dot_r = (i % 9 == 0) ? 2.2f : 1.2f;
					dl->AddCircleFilled(ImVec2(c.x + std::cos(a) * r_mid, c.y + std::sin(a) * r_mid), dot_r,
					    (i % 9 == 0) ? IM_COL32(180, 190, 205, 220) : IM_COL32(110, 118, 134, 160), 12);
				}

				DrawRing(dl, c, r_out, rpm_ratio, kColRpm0, kColRpm1, 10.0f, true);
				DrawRing(dl, c, r_in, speed_ratio, kColSpeed0, kColSpeed1, 10.0f, false);

				DrawGearChip(dl, cjk, ImVec2(c.x, c.y - 62.0f), gear_buf, kColSpeed0);
				AddTextCentered(dl, big, 50.0f, ImVec2(c.x, c.y - 6.0f), kColText, speed_buf);
				AddTextCentered(dl, cjk, 15.0f, ImVec2(c.x, c.y + 28.0f), kColTextDim, unit);
				AddTextCentered(dl, over, 20.0f, ImVec2(c.x, c.y + 54.0f), IM_COL32(255, 180, 110, 255), rpm_buf);
			}
			else // style == 3 数字极简
			{
				// ============ 数字极简 ============
				const ImVec2 p0 = origin;
				const ImVec2 p1(origin.x + w, origin.y + h);
				DrawPanel(dl, p0, p1, 12.0f);

				// 左侧状态条（随转速变色）
				const ImU32 strip = LerpCol(kColRpm0, kColRedline, std::clamp((rpm_ratio - 0.7f) / 0.3f, 0.0f, 1.0f));
				dl->AddRectFilled(ImVec2(p0.x + 6.0f, p0.y + 10.0f), ImVec2(p0.x + 10.0f, p1.y - 10.0f), strip, 2.0f);

				// 顶部：转速 + 档位
				AddTextLeft(dl, cjk, 14.0f, ImVec2(p0.x + 20.0f, p0.y + 12.0f), kColTextDim, "转速");
				AddTextLeft(dl, over, 21.0f, ImVec2(p0.x + 58.0f, p0.y + 8.0f), IM_COL32(255, 186, 116, 255), rpm_buf);
				DrawGearChip(dl, cjk, ImVec2(p1.x - 46.0f, p0.y + 26.0f), gear_buf, kColSpeed0);

				// 大号速度
				AddTextLeft(dl, big, 58.0f, ImVec2(p0.x + 20.0f, p0.y + 38.0f), kColText, speed_buf);
				AddTextLeft(dl, cjk, 16.0f, ImVec2(p0.x + 24.0f, p0.y + 100.0f), kColTextDim, unit);

				// 底部转速条
				const ImVec2 b0(p0.x + 20.0f, p1.y - 22.0f);
				const ImVec2 b1(p1.x - 20.0f, p1.y - 12.0f);
				dl->AddRectFilled(b0, b1, IM_COL32(36, 40, 52, 255), 5.0f);
				if (rpm_ratio > 0.002f)
					dl->AddRectFilledMultiColor(b0, ImVec2(b0.x + (b1.x - b0.x) * rpm_ratio, b1.y),
					    kColRpm0, kColRpm1, kColRpm1, kColRpm0);
				dl->AddRect(b0, b1, IM_COL32(78, 86, 102, 255), 5.0f);
				if (rpm_ratio > 0.85f)
					dl->AddRectFilled(ImVec2(b1.x - (b1.x - b0.x) * 0.15f, b0.y - 1.0f), ImVec2(b1.x, b1.y + 1.0f), WithAlpha(kColRedline, 90), 5.0f);
			}

			// 菜单位置编辑参考框（仅菜单打开时显示，方便对齐）
			if (GUI::IsOpen())
			{
				const ImVec2 g0(origin.x - 8.0f, origin.y - 8.0f);
				const ImVec2 g1(origin.x + w + 8.0f, origin.y + h + 8.0f);
				const ImU32 gc  = IM_COL32(110, 170, 255, 110);
				const float len = 20.0f;
				dl->AddLine(ImVec2(g0.x, g0.y), ImVec2(g0.x + len, g0.y), gc, 2.0f);
				dl->AddLine(ImVec2(g0.x, g0.y), ImVec2(g0.x, g0.y + len), gc, 2.0f);
				dl->AddLine(ImVec2(g1.x - len, g0.y), ImVec2(g1.x, g0.y), gc, 2.0f);
				dl->AddLine(ImVec2(g1.x, g0.y), ImVec2(g1.x, g0.y + len), gc, 2.0f);
				dl->AddLine(ImVec2(g0.x, g1.y - len), ImVec2(g0.x, g1.y), gc, 2.0f);
				dl->AddLine(ImVec2(g0.x, g1.y), ImVec2(g0.x + len, g1.y), gc, 2.0f);
				dl->AddLine(ImVec2(g1.x - len, g1.y), ImVec2(g1.x, g1.y), gc, 2.0f);
				dl->AddLine(ImVec2(g1.x, g1.y - len), ImVec2(g1.x, g1.y), gc, 2.0f);
			}
		}
	}

	void SpeedoHUD::Draw()
	{
		if (!NativeInvoker::AreHandlersCached())
			return;
		if (HUD::IS_PAUSE_MENU_ACTIVE() || CAMERA::IS_SCREEN_FADED_OUT())
			return;

		// 与 ESP 相同：渲染线程读取游戏状态有极低概率的竞态，加一层结构化异常兜底
		__try
		{
			DrawImpl();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}
