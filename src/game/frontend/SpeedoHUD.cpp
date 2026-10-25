#include "SpeedoHUD.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <unordered_map>

#include "core/commands/BoolCommand.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/util/Joaat.hpp"
#include "game/backend/Self.hpp"
#include "game/frontend/GUI.hpp"
#include "game/frontend/Menu.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/VehicleHandling.hpp"
#include "game/gta/invoker/Invoker.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu
{
	namespace
	{
		constexpr float kPi = 3.14159265f;

		constexpr float kGaugeStart = kPi * 0.75f; // 135°
		constexpr float kGaugeSweep = kPi * 1.5f;  // 270°

		// 透明 HUD 配色：不绘制任何实心底板，只有线条 / 刻度 / 文字 / 指针
		constexpr ImU32 kColText    = IM_COL32(244, 248, 254, 255);
		constexpr ImU32 kColTextDim = IM_COL32(168, 178, 194, 225);
		constexpr ImU32 kColTick    = IM_COL32(228, 236, 248, 245);
		constexpr ImU32 kColTickDim = IM_COL32(148, 158, 176, 190);
		constexpr ImU32 kColTrack   = IM_COL32(108, 118, 136, 160);
		constexpr ImU32 kColSpeed0  = IM_COL32(70, 150, 255, 255);
		constexpr ImU32 kColSpeed1  = IM_COL32(130, 218, 255, 255);
		constexpr ImU32 kColRpm0    = IM_COL32(255, 158, 66, 255);
		constexpr ImU32 kColRpm1    = IM_COL32(255, 92, 58, 255);
		constexpr ImU32 kColRedline = IM_COL32(255, 66, 52, 255);
		constexpr ImU32 kColNeon0   = IM_COL32(64, 214, 255, 255);  // 全息青
		constexpr ImU32 kColNeon1   = IM_COL32(182, 110, 255, 255); // 全息紫
		constexpr ImU32 kColShadow  = IM_COL32(0, 0, 0, 140);

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

		// 高转/超速警示：0 = 正常，1 = 满强度闪烁（参考跑车换挡灯：越接近爆表，闪得越快越狠）
		float AlertBlink(float ratio, float warn_at, float crit_at, float t)
		{
			if (ratio < warn_at)
				return 0.0f;

			const float sev = std::clamp((ratio - warn_at) / std::max(crit_at - warn_at, 0.0001f), 0.0f, 1.0f);

			// 预警段：柔性脉冲；临界段：硬频闪
			const float smooth_wave = 0.5f + 0.5f * std::sin(t * (6.0f + 9.0f * sev) * kPi);
			const float strobe      = std::fmod(t * (6.0f + 12.0f * sev), 1.0f) < 0.5f ? 1.0f : 0.0f;
			const float wave        = smooth_wave * (1.0f - sev) + strobe * sev;

			return std::clamp(wave * (0.45f + 0.55f * sev), 0.0f, 1.0f);
		}

		ImU32 BlinkCol(ImU32 base, ImU32 flash, float blink)
		{
			return LerpCol(base, flash, std::clamp(blink * 1.15f, 0.0f, 1.0f));
		}

		// 弧线脉冲：在原进度弧上叠一层闪烁高光
		void AddArcPulse(ImDrawList* dl, ImVec2 c, float r, float a0, float a1, ImU32 flash, float blink, float thick)
		{
			if (blink <= 0.01f || a1 <= a0)
				return;
			AddArcGradientGlow(dl, c, r, a0, a1,
			    WithAlpha(flash, static_cast<int>(170.0f * blink)),
			    WithAlpha(flash, static_cast<int>(220.0f * blink)),
			    thick * (1.0f + 0.35f * blink));
		}

		// 条形脉冲：在原进度条上叠一层闪烁高光
		void AddBarPulse(ImDrawList* dl, ImVec2 b0, ImVec2 b1, float ratio, ImU32 flash, float blink, float rounding)
		{
			if (blink <= 0.01f || ratio <= 0.002f)
				return;
			dl->AddRectFilled(b0, ImVec2(b0.x + (b1.x - b0.x) * ratio, b1.y), WithAlpha(flash, static_cast<int>(215.0f * blink)), rounding);
		}

		// 分段条脉冲（全息样式用）
		void AddSegmentBarPulse(ImDrawList* dl, ImVec2 p0, ImVec2 p1, float ratio, ImU32 flash, float blink, int segs, float s)
		{
			if (blink <= 0.01f || segs < 2)
				return;
			const float total = p1.x - p0.x;
			const float seg_w = total / static_cast<float>(segs);
			for (int i = 0; i < segs; i++)
			{
				const float t = static_cast<float>(i) / static_cast<float>(segs - 1);
				if (t > ratio)
					break;
				const ImVec2 a2(p0.x + seg_w * i, p0.y);
				const ImVec2 b2(a2.x + seg_w * 0.62f, p1.y);
				dl->AddRectFilled(a2, b2, WithAlpha(flash, static_cast<int>(215.0f * blink)), 1.5f * s);
			}
		}

		// 文字（暗色描影提升亮景可读性，但不绘制任何底板）
		void AddTextCentered(ImDrawList* dl, ImFont* font, float size, ImVec2 c, ImU32 col, const char* text, float s)
		{
			if (!font || !text)
				return;
			const ImVec2 ts = font->CalcTextSizeA(size, FLT_MAX, 0.0f, text);
			const ImVec2 pos(c.x - ts.x * 0.5f, c.y - ts.y * 0.5f);
			dl->AddText(font, size, ImVec2(pos.x + 1.6f * s, pos.y + 1.6f * s), kColShadow, text);
			dl->AddText(font, size, pos, col, text);
		}

		void AddTextLeft(ImDrawList* dl, ImFont* font, float size, ImVec2 pos, ImU32 col, const char* text, float s)
		{
			if (!font || !text)
				return;
			dl->AddText(font, size, ImVec2(pos.x + 1.6f * s, pos.y + 1.6f * s), kColShadow, text);
			dl->AddText(font, size, pos, col, text);
		}

		// 档位徽章：低透明底色 + 描边
		void DrawGearChip(ImDrawList* dl, ImFont* cjk, ImVec2 c, const char* text, ImU32 accent, float s)
		{
			const ImVec2 p0(c.x - 30.0f * s, c.y - 14.0f * s);
			const ImVec2 p1(c.x + 30.0f * s, c.y + 14.0f * s);
			dl->AddRectFilled(p0, p1, WithAlpha(accent, 38), 7.0f * s);
			dl->AddRect(p0, p1, WithAlpha(accent, 190), 7.0f * s, 0, 1.4f * s);
			AddTextCentered(dl, cjk, 18.0f * s, c, IM_COL32(232, 240, 250, 255), text, s);
		}

		// 表针：细长锥形针体 + 尾部配重 + 中心轴（真实仪表风格）
		void DrawNeedle(ImDrawList* dl, ImVec2 c, float len, float angle, ImU32 col, float s)
		{
			const ImVec2 dir(std::cos(angle), std::sin(angle));
			const ImVec2 perp(-dir.y, dir.x);
			const ImVec2 tip(c.x + dir.x * len, c.y + dir.y * len);
			const float half_w = 2.8f * s;
			const ImVec2 b1(c.x + dir.x * 16.0f * s + perp.x * half_w, c.y + dir.y * 16.0f * s + perp.y * half_w);
			const ImVec2 b2(c.x + dir.x * 16.0f * s - perp.x * half_w, c.y + dir.y * 16.0f * s - perp.y * half_w);
			const ImVec2 tail(c.x - dir.x * 24.0f * s, c.y - dir.y * 24.0f * s);
			dl->AddQuadFilled(tip, b1, b2, tail, col);
			dl->AddCircleFilled(c, 8.5f * s, IM_COL32(22, 26, 34, 235), 24);
			dl->AddCircle(c, 8.5f * s, WithAlpha(col, 230), 24, 1.6f * s);
			dl->AddCircleFilled(c, 3.2f * s, col, 16);
		}

		// 透明圆盘：仅表圈 / 刻度 / 轨道 / 发光进度 / 指针，无实心底
		void DrawDialTransparent(ImDrawList* dl, ImVec2 c, float r, float ratio, ImU32 col0, ImU32 col1, float red_zone, float s, float alert)
		{
			ratio = std::clamp(ratio, 0.0f, 1.0f);

			dl->AddCircle(c, r + 6.0f * s, WithAlpha(col0, 60), 56, 1.4f * s);
			dl->AddCircle(c, r, WithAlpha(kColTick, 46), 56, 1.1f * s);

			AddArc(dl, c, r - 7.0f * s, kGaugeStart, kGaugeStart + kGaugeSweep, kColTrack, 5.0f * s);

			if (red_zone > 0.0f)
				AddArc(dl, c, r - 7.0f * s, kGaugeStart + kGaugeSweep * red_zone, kGaugeStart + kGaugeSweep, WithAlpha(kColRedline, 120), 5.0f * s);

			for (int i = 0; i <= 20; i++)
			{
				const float a    = kGaugeStart + kGaugeSweep * static_cast<float>(i) / 20.0f;
				const bool major = (i % 5 == 0);
				const float r0   = major ? r - 17.0f * s : r - 11.0f * s;
				dl->AddLine(ImVec2(c.x + std::cos(a) * r0, c.y + std::sin(a) * r0),
				    ImVec2(c.x + std::cos(a) * (r - 3.0f * s), c.y + std::sin(a) * (r - 3.0f * s)),
				    major ? kColTick : kColTickDim,
				    major ? 2.0f * s : 1.0f * s);
			}

			if (ratio > 0.002f)
				AddArcGradientGlow(dl, c, r - 7.0f * s, kGaugeStart, kGaugeStart + kGaugeSweep * ratio, col0, col1, 5.0f * s);

			// 爆表警示脉冲（参考跑车换挡灯）
			AddArcPulse(dl, c, r - 7.0f * s, kGaugeStart, kGaugeStart + kGaugeSweep * ratio, IM_COL32(255, 255, 255, 255), alert, 5.0f * s);
			DrawNeedle(dl, c, r - 14.0f * s, kGaugeStart + kGaugeSweep * ratio, BlinkCol(LerpCol(col0, col1, ratio), IM_COL32(255, 255, 255, 255), alert * 0.85f), s);
		}

		// 环形轨道（透明）：底环 + 红区 + 发光进度环 + 端点圆帽
		void DrawRing(ImDrawList* dl, ImVec2 c, float r, float ratio, ImU32 col0, ImU32 col1, float thick, float red_zone, float s, float alert)
		{
			ratio = std::clamp(ratio, 0.0f, 1.0f);

			AddArc(dl, c, r, kGaugeStart, kGaugeStart + kGaugeSweep, kColTrack, thick);
			if (red_zone > 0.0f)
				AddArc(dl, c, r, kGaugeStart + kGaugeSweep * red_zone, kGaugeStart + kGaugeSweep, WithAlpha(kColRedline, 120), thick);

			if (ratio > 0.002f)
			{
				const float a1 = kGaugeStart + kGaugeSweep * ratio;
				AddArcGradientGlow(dl, c, r, kGaugeStart, a1, col0, col1, thick);
				dl->AddCircleFilled(ImVec2(c.x + std::cos(a1) * r, c.y + std::sin(a1) * r), thick * 0.55f, LerpCol(col0, col1, ratio), 16);
			}

			AddArcPulse(dl, c, r, kGaugeStart, kGaugeStart + kGaugeSweep * ratio, IM_COL32(255, 255, 255, 255), alert, thick);
		}

		// 分段进度条（段间留缝），ratio 控制点亮比例
		void DrawSegmentBar(ImDrawList* dl, ImVec2 p0, ImVec2 p1, float ratio, ImU32 col0, ImU32 col1, int segs, float s)
		{
			if (segs < 2)
				return;
			const float total = p1.x - p0.x;
			const float seg_w = total / static_cast<float>(segs);
			for (int i = 0; i < segs; i++)
			{
				const float t   = static_cast<float>(i) / static_cast<float>(segs - 1);
				const ImVec2 a2(p0.x + seg_w * i, p0.y);
				const ImVec2 b2(a2.x + seg_w * 0.62f, p1.y);
				const ImU32 col = (t <= ratio) ? LerpCol(col0, col1, t) : WithAlpha(kColTrack, 110);
				dl->AddRectFilled(a2, b2, col, 1.5f * s);
			}
		}


		// ------------------------------------------------ 动态量程（2026-10-06）

		struct SpeedoDynamics
		{
			std::unordered_map<std::uint32_t, float> observed_mps; // 车模 -> 实测峰值（m/s）
			float scale = 0.0f;                                    // 平滑后的满刻度（km/h）
			std::chrono::steady_clock::time_point last_tick{};
		};

		SpeedoDynamics& GetDynamics()
		{
			static SpeedoDynamics s;
			return s;
		}

		float CmdFloat(const char* name, float fallback)
		{
			auto cmd = PosCmd(name);
			return cmd ? cmd->GetState() : fallback;
		}

		int CmdList(const char* name, int fallback)
		{
			auto cmd = Commands::GetCommand<ListCommand>(Joaat(name));
			return cmd ? cmd->GetState() : fallback;
		}

		bool CmdBool(const char* name, bool fallback)
		{
			auto cmd = Commands::GetCommand<BoolCommand>(Joaat(name));
			return cmd ? cmd->GetState() : fallback;
		}

		float RoundUpNice(float value)
		{
			static constexpr float kSteps[] = {120.0f, 160.0f, 200.0f, 240.0f, 280.0f, 320.0f, 360.0f, 400.0f, 450.0f, 500.0f, 550.0f, 600.0f, 700.0f, 800.0f};
			for (const float step : kSteps)
			{
				if (value <= step)
					return step;
			}
			return 900.0f;
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

			const float unit_factor = metric ? 3.6f : 2.23694f;
			const float speed_mps   = veh.GetSpeed();
			const float disp_speed  = speed_mps * unit_factor;
			const float rpm_ratio   = std::clamp(veh.GetRevRatio(), 0.0f, 1.0f);
			const int gear          = veh.GetGear();

			// —— 动态量程：按本车实际能力估算极速（调教字段 + 官方估算 + 实测峰值），留 30~50 余量 ——
			const float warn_frac = std::clamp(CmdFloat("speedowarn", 0.88f), 0.50f, 0.95f);
			const float rpm_warn  = std::clamp(CmdFloat("rpmwarn", 0.88f), 0.50f, 0.99f);
			const bool gear_mode  = CmdBool("rpmgearmode", true);

			int max_gear = VEHICLE::_GET_VEHICLE_MAX_DRIVE_GEAR_COUNT(veh.GetHandle());
			if (max_gear < 1 || max_gear > 12)
				max_gear = std::max(gear, 1);
			const bool top_gear = gear >= max_gear;

			float est_kmh   = 0.0f;
			float range_kmh = 0.0f;
			if (!SpeedoHUD::ComputeRange(veh, speed_mps, est_kmh, range_kmh))
			{
				range_kmh = std::max(veh.GetMaxSpeed() * 3.6f * 1.05f, 1.0f);
				est_kmh   = range_kmh;
			}

			const float max_speed  = std::max(range_kmh * (metric ? 1.0f : 0.621371f), 1.0f);
			const float zone_start = std::clamp(est_kmh * warn_frac / std::max(range_kmh, 1.0f), 0.20f, 0.995f);
			const float crit_start = std::clamp(est_kmh * 0.985f / std::max(range_kmh, 1.0f), zone_start + 0.01f, 1.0f);
			const float rpm_zone   = (gear_mode && !top_gear) ? 0.97f : rpm_warn;

			const float speed_ratio = std::clamp(disp_speed / max_speed, 0.0f, 1.0f);
			const char* unit        = metric ? "公里/时" : "英里/时";

			// 超转 / 超速警示闪烁（越接近本车极限，闪得越快越狠；非顶档只在接近断油时硬闪）
			const float alert_time = static_cast<float>(ImGui::GetTime());
			float rpm_blink;
			if (gear_mode && !top_gear)
				rpm_blink = std::min(1.0f, AlertBlink(rpm_ratio, 0.97f, 0.995f, alert_time) + AlertBlink(rpm_ratio, rpm_warn, 0.97f, alert_time) * 0.45f);
			else
				rpm_blink = AlertBlink(rpm_ratio, rpm_warn, 0.985f, alert_time);
			const float speed_blink = AlertBlink(speed_ratio, zone_start, crit_start, alert_time);

			char speed_buf[16]{};
			std::snprintf(speed_buf, sizeof(speed_buf), "%d", static_cast<int>(disp_speed + 0.5f));

			char rpm_buf[16]{};
			std::snprintf(rpm_buf, sizeof(rpm_buf), "%.1fk", rpm_ratio * CmdFloat("rpmredline", 8.0f));

			char gear_buf[16]{};
			if (gear <= 0)
				std::snprintf(gear_buf, sizeof(gear_buf), "N");
			else
				std::snprintf(gear_buf, sizeof(gear_buf), "%d挡", gear);

			const float sw = *Pointers.ScreenResX;
			const float sh = *Pointers.ScreenResY;

			// 整体缩放（0.5x ~ 2.0x）
			const float s = std::clamp(PosCmd("speedoscale") ? PosCmd("speedoscale")->GetState() : 1.0f, 0.5f, 2.0f);

			float w = 380.0f, h = 210.0f;
			switch (style)
			{
			case 2: w = h = 265.0f; break;
			case 3: w = 335.0f; h = 128.0f; break;
			case 4: w = h = 300.0f; break;
			case 5: w = 360.0f; h = 180.0f; break;
			default: break;
			}
			w *= s;
			h *= s;

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
				// ============ 双圆盘（透明） ============
				const float r1 = h * 0.325f;
				const float r2 = h * 0.405f;
				const ImVec2 c1(origin.x + h * 0.40f, origin.y + h * 0.54f);
				const ImVec2 c2(origin.x + h * 1.16f, origin.y + h * 0.52f);

				DrawDialTransparent(dl, c1, r1, rpm_ratio, kColRpm0, kColRpm1, rpm_zone, s, rpm_blink);
				AddTextCentered(dl, over, 24.0f * s, ImVec2(c1.x, c1.y + r1 * 0.40f), IM_COL32(255, 196, 130, 255), rpm_buf, s);
				AddTextCentered(dl, cjk, 14.0f * s, ImVec2(c1.x, c1.y + r1 * 0.72f), kColTextDim, "转速", s);

				DrawDialTransparent(dl, c2, r2, speed_ratio, kColSpeed0, kColSpeed1, zone_start, s, speed_blink);
				AddTextCentered(dl, big, std::clamp(r2 * 0.62f, 34.0f * s, 60.0f * s), ImVec2(c2.x, c2.y + r2 * 0.34f), kColText, speed_buf, s);
				AddTextCentered(dl, cjk, 15.0f * s, ImVec2(c2.x, c2.y + r2 * 0.66f), kColTextDim, unit, s);
				DrawGearChip(dl, cjk, ImVec2(c2.x, c2.y - r2 * 0.62f), gear_buf, kColSpeed0, s);
			}
			else if (style == 2)
			{
				// ============ 环形组合（透明） ============
				const ImVec2 c(origin.x + w * 0.5f, origin.y + h * 0.5f);
				const float r_out = w * 0.435f;
				const float r_in  = w * 0.335f;
				const float r_mid = (r_out + r_in) * 0.5f;

				for (int i = 0; i <= 36; i++)
				{
					const float a     = kGaugeStart + kGaugeSweep * static_cast<float>(i) / 36.0f;
					const float dot_r = (i % 9 == 0) ? 2.2f * s : 1.2f * s;
					dl->AddCircleFilled(ImVec2(c.x + std::cos(a) * r_mid, c.y + std::sin(a) * r_mid), dot_r,
					    (i % 9 == 0) ? IM_COL32(190, 200, 214, 225) : IM_COL32(120, 128, 144, 165), 12);
				}

				DrawRing(dl, c, r_out, rpm_ratio, kColRpm0, kColRpm1, 10.0f * s, rpm_zone, s, rpm_blink);
				DrawRing(dl, c, r_in, speed_ratio, kColSpeed0, kColSpeed1, 10.0f * s, zone_start, s, speed_blink);

				DrawGearChip(dl, cjk, ImVec2(c.x, c.y - 62.0f * s), gear_buf, kColSpeed0, s);
				AddTextCentered(dl, big, 50.0f * s, ImVec2(c.x, c.y - 6.0f * s), kColText, speed_buf, s);
				AddTextCentered(dl, cjk, 15.0f * s, ImVec2(c.x, c.y + 28.0f * s), kColTextDim, unit, s);
				AddTextCentered(dl, over, 20.0f * s, ImVec2(c.x, c.y + 54.0f * s), IM_COL32(255, 180, 110, 255), rpm_buf, s);
			}
			else if (style == 3)
			{
				// ============ 数字极简（透明） ============
				const float p_x = origin.x;
				const float p_y = origin.y;

				// 左侧状态条（随转速变色）
				const ImU32 strip = BlinkCol(LerpCol(kColRpm0, kColRedline, std::clamp((rpm_ratio - 0.7f) / 0.3f, 0.0f, 1.0f)), IM_COL32(255, 210, 170, 255), rpm_blink * 0.8f);
				dl->AddRectFilled(ImVec2(p_x + 6.0f * s, p_y + 10.0f * s), ImVec2(p_x + 10.0f * s, p_y + h - 10.0f * s), WithAlpha(strip, 200), 2.0f * s);

				AddTextLeft(dl, cjk, 14.0f * s, ImVec2(p_x + 22.0f * s, p_y + 12.0f * s), kColTextDim, "转速", s);
				AddTextLeft(dl, over, 21.0f * s, ImVec2(p_x + 62.0f * s, p_y + 8.0f * s), IM_COL32(255, 186, 116, 255), rpm_buf, s);
				DrawGearChip(dl, cjk, ImVec2(p_x + w - 46.0f * s, p_y + 26.0f * s), gear_buf, kColSpeed0, s);

				AddTextLeft(dl, big, 58.0f * s, ImVec2(p_x + 22.0f * s, p_y + 36.0f * s), kColText, speed_buf, s);
				AddTextLeft(dl, cjk, 16.0f * s, ImVec2(p_x + 26.0f * s, p_y + 102.0f * s), kColTextDim, unit, s);

				const ImVec2 b0(p_x + 22.0f * s, p_y + h - 24.0f * s);
				const ImVec2 b1(p_x + w - 22.0f * s, p_y + h - 14.0f * s);
				dl->AddRectFilled(b0, b1, WithAlpha(kColTrack, 120), 5.0f * s);
				if (rpm_ratio > 0.002f)
					dl->AddRectFilledMultiColor(b0, ImVec2(b0.x + (b1.x - b0.x) * rpm_ratio, b1.y),
					    kColRpm0, kColRpm1, kColRpm1, kColRpm0);
				dl->AddRect(b0, b1, WithAlpha(kColRpm0, 160), 5.0f * s, 0, 1.2f * s);
				if (rpm_ratio > rpm_zone)
					dl->AddRectFilled(ImVec2(b1.x - (b1.x - b0.x) * 0.15f, b0.y - 1.0f * s), ImVec2(b1.x, b1.y + 1.0f * s), WithAlpha(kColRedline, 100), 5.0f * s);
				AddBarPulse(dl, b0, b1, rpm_ratio, IM_COL32(255, 240, 220, 255), rpm_blink, 5.0f * s);
			}
			else if (style == 4)
			{
				// ============ 科技圆盘（模拟指针 · 真实仪表风） ============
				const ImVec2 c(origin.x + w * 0.5f, origin.y + h * 0.5f);
				const float R = w * 0.44f;

				// 表圈：双层细环 + 外沿微光
				dl->AddCircle(c, R + 6.0f * s, WithAlpha(kColSpeed0, 55), 72, 1.4f * s);
				dl->AddCircle(c, R, WithAlpha(kColTick, 205), 72, 1.6f * s);
				dl->AddCircle(c, R - 3.0f * s, WithAlpha(kColTick, 70), 72, 1.0f * s);

				// 速度上限取整到 20 的倍数
				const int max_num = std::max(20, static_cast<int>(std::ceil(max_speed / 20.0f)) * 20);

				// 刻度：主刻度（带数字）/ 中刻度 / 细刻度
				for (int i = 0; i <= 40; i++)
				{
					const float a2   = kGaugeStart + kGaugeSweep * static_cast<float>(i) / 40.0f;
					const bool major = (i % 8 == 0);
					const bool mid   = (i % 4 == 0);
					const float r0   = R - (major ? 19.0f : (mid ? 13.0f : 8.0f)) * s;
					dl->AddLine(ImVec2(c.x + std::cos(a2) * r0, c.y + std::sin(a2) * r0),
					    ImVec2(c.x + std::cos(a2) * (R - 5.0f * s), c.y + std::sin(a2) * (R - 5.0f * s)),
					    major ? kColTick : (mid ? kColTickDim : WithAlpha(kColTickDim, 120)),
					    major ? 2.2f * s : 1.2f * s);
				}

				// 数字（0 ~ max_num，共 6 个）
				for (int i = 0; i <= 5; i++)
				{
					const float a2 = kGaugeStart + kGaugeSweep * static_cast<float>(i) / 5.0f;
					char num_buf[8]{};
					std::snprintf(num_buf, sizeof(num_buf), "%d", max_num * i / 5);
					const ImVec2 np(c.x + std::cos(a2) * (R - 36.0f * s), c.y + std::sin(a2) * (R - 36.0f * s));
					AddTextCentered(dl, cjk, 15.0f * s, np, kColTextDim, num_buf, s);
				}

				// 红区
				AddArc(dl, c, R - 10.0f * s, kGaugeStart + kGaugeSweep * zone_start, kGaugeStart + kGaugeSweep, WithAlpha(kColRedline, 120), 8.0f * s);
				AddArcPulse(dl, c, R - 10.0f * s, kGaugeStart + kGaugeSweep * zone_start, kGaugeStart + kGaugeSweep, IM_COL32(255, 90, 70, 255), speed_blink, 8.0f * s);

				// 速度进度弧（轨道 + 发光 + 端点）
				const float r_track = R * 0.62f;
				AddArc(dl, c, r_track, kGaugeStart, kGaugeStart + kGaugeSweep, kColTrack, 8.0f * s);
				if (speed_ratio > 0.002f)
				{
					const float a1 = kGaugeStart + kGaugeSweep * speed_ratio;
					AddArcGradientGlow(dl, c, r_track, kGaugeStart, a1, kColSpeed0, kColSpeed1, 8.0f * s);
					dl->AddCircleFilled(ImVec2(c.x + std::cos(a1) * r_track, c.y + std::sin(a1) * r_track), 4.4f * s, LerpCol(kColSpeed0, kColSpeed1, speed_ratio), 16);
				}
				AddArcPulse(dl, c, r_track, kGaugeStart, kGaugeStart + kGaugeSweep * speed_ratio, IM_COL32(255, 255, 255, 255), speed_blink, 8.0f * s);

				// 指针
				DrawNeedle(dl, c, R * 0.56f, kGaugeStart + kGaugeSweep * speed_ratio, BlinkCol(LerpCol(kColSpeed0, kColSpeed1, speed_ratio), IM_COL32(255, 255, 255, 255), speed_blink * 0.85f), s);

				// 中心读数：速度 / 单位 / 档位 / 转速迷你条
				AddTextCentered(dl, big, 44.0f * s, ImVec2(c.x, c.y + 28.0f * s), kColText, speed_buf, s);
				AddTextCentered(dl, cjk, 14.0f * s, ImVec2(c.x, c.y + 56.0f * s), kColTextDim, unit, s);
				DrawGearChip(dl, cjk, ImVec2(c.x, c.y + 80.0f * s), gear_buf, kColSpeed0, s);

				const float bw = 110.0f * s;
				const ImVec2 b0(c.x - bw * 0.5f, c.y + 100.0f * s);
				const ImVec2 b1(c.x + bw * 0.5f, b0.y + 7.0f * s);
				dl->AddRectFilled(b0, b1, WithAlpha(kColTrack, 120), 3.5f * s);
				if (rpm_ratio > 0.002f)
					dl->AddRectFilledMultiColor(b0, ImVec2(b0.x + bw * rpm_ratio, b1.y), kColRpm0, kColRpm1, kColRpm1, kColRpm0);
				dl->AddRect(b0, b1, WithAlpha(kColRpm0, 150), 3.5f * s, 0, 1.2f * s);
				AddBarPulse(dl, b0, b1, rpm_ratio, IM_COL32(255, 240, 220, 255), rpm_blink, 3.5f * s);
				AddTextCentered(dl, over, 15.0f * s, ImVec2(c.x, b1.y + 14.0f * s), IM_COL32(255, 190, 120, 255), rpm_buf, s);
			}
			else if (style == 5)
			{
				// ============ 全息 HUD（科技风） ============
				const ImVec2 p0(origin.x + 10.0f * s, origin.y + 10.0f * s);
				const ImVec2 p1(origin.x + w - 10.0f * s, origin.y + h - 10.0f * s);

				// 四角框线
				const float len = 18.0f * s;
				const ImU32 gc  = WithAlpha(kColNeon0, 170);
				dl->AddLine(ImVec2(p0.x, p0.y), ImVec2(p0.x + len, p0.y), gc, 2.0f * s);
				dl->AddLine(ImVec2(p0.x, p0.y), ImVec2(p0.x, p0.y + len), gc, 2.0f * s);
				dl->AddLine(ImVec2(p1.x - len, p0.y), ImVec2(p1.x, p0.y), gc, 2.0f * s);
				dl->AddLine(ImVec2(p1.x, p0.y), ImVec2(p1.x, p0.y + len), gc, 2.0f * s);
				dl->AddLine(ImVec2(p0.x, p1.y - len), ImVec2(p0.x, p1.y), gc, 2.0f * s);
				dl->AddLine(ImVec2(p0.x, p1.y), ImVec2(p0.x + len, p1.y), gc, 2.0f * s);
				dl->AddLine(ImVec2(p1.x - len, p1.y), ImVec2(p1.x, p1.y), gc, 2.0f * s);
				dl->AddLine(ImVec2(p1.x, p1.y - len), ImVec2(p1.x, p1.y), gc, 2.0f * s);

				// 顶部装饰线
				dl->AddLine(ImVec2(p0.x + len + 6.0f * s, p0.y), ImVec2(p1.x - len - 6.0f * s, p0.y), WithAlpha(kColNeon1, 90), 1.2f * s);

				// 主读数：大号速度 + 单位
				AddTextLeft(dl, big, 64.0f * s, ImVec2(p0.x + 14.0f * s, origin.y + h * 0.5f - 44.0f * s), kColText, speed_buf, s);
				AddTextLeft(dl, cjk, 15.0f * s, ImVec2(p0.x + 18.0f * s, origin.y + h * 0.5f + 24.0f * s), kColTextDim, unit, s);

				// 速度分段条（青→紫）
				const ImVec2 sb0(p0.x + 16.0f * s, origin.y + h - 28.0f * s);
				const ImVec2 sb1(origin.x + w * 0.64f, sb0.y + 8.0f * s);
				DrawSegmentBar(dl, sb0, sb1, speed_ratio, kColNeon0, kColNeon1, 22, s);
				AddSegmentBarPulse(dl, sb0, sb1, speed_ratio, IM_COL32(255, 255, 255, 255), speed_blink, 22, s);

				// 右侧：转速梯形条（从底部向上点亮）
				const float cx     = origin.x + w - 40.0f * s;
				const int segs     = 10;
				const float seg_h  = 9.0f * s;
				const float gap    = 3.0f * s;
				const float base_y = origin.y + h - 22.0f * s;
				for (int i = 0; i < segs; i++)
				{
					const float t    = static_cast<float>(i) / static_cast<float>(segs - 1);
					const float y1   = base_y - (seg_h + gap) * i;
					const float y0   = y1 - seg_h;
					const float half = (7.0f + 5.0f * t) * s;
					const ImU32 col  = (t <= rpm_ratio) ? BlinkCol(LerpCol(kColRpm0, kColRedline, t), IM_COL32(255, 255, 255, 255), rpm_blink * 0.85f) : WithAlpha(kColTrack, 120);
					dl->AddRectFilled(ImVec2(cx - half, y0), ImVec2(cx + half, y1), col, 2.0f * s);
				}

				// 档位
				DrawGearChip(dl, cjk, ImVec2(cx, origin.y + 24.0f * s), gear_buf, kColNeon0, s);
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

	bool SpeedoHUD::ComputeRange(Vehicle veh, float speed_mps, float& est_kmh, float& range_kmh)
	{
		if (!veh)
			return false;

		// 手动模式：满刻度直接取设置值
		if (CmdList("speedorange", 0) == 1)
		{
			range_kmh = std::clamp(CmdFloat("speedorangemanual", 300.0f), 60.0f, 999.0f);
			est_kmh   = range_kmh;
			return true;
		}

		auto& dyn = GetDynamics();
		const auto now = std::chrono::steady_clock::now();
		float dt = std::chrono::duration<float>(now - dyn.last_tick).count();
		dyn.last_tick = now;
		if (!(dt > 0.0f) || dt > 1.0f)
			dt = 1.0f / 60.0f;

		auto sane_mps = [](float v) { return (v > 2.0f && v < 400.0f) ? v : 0.0f; };

		float seed_mps = sane_mps(veh.GetMaxSpeed()); // 官方估算（不含改装）
		const std::uint32_t model = ENTITY::GET_ENTITY_MODEL(veh.GetHandle());
		if (const float model_mps = sane_mps(VEHICLE::GET_VEHICLE_MODEL_ESTIMATED_MAX_SPEED(model)); model_mps > seed_mps)
			seed_mps = model_mps;
		if (void* handling = VehicleHandling::GetHandlingData(veh))
		{
			const float flat  = sane_mps(VehicleHandling::ReadFloat(handling, VehicleHandling::kMaxFlatVelOffset));
			const float flat2 = sane_mps(VehicleHandling::ReadFloat(handling, VehicleHandling::kMaxFlatVel2Offset));
			const float best  = std::max(flat, flat2);
			if (best > seed_mps)
				seed_mps = best;
		}
		seed_mps = std::max(seed_mps, 120.0f / 3.6f); // 下限 120 km/h

		float& observed = dyn.observed_mps[model];
		observed = std::max(observed - observed * 0.01f * dt, 0.0f); // 1%/秒 缓慢衰减
		observed = std::max(observed, speed_mps);
		observed = std::max(observed, seed_mps);

		const float est_mps = std::max(seed_mps, observed);
		est_kmh = est_mps * 3.6f;

		const float margin = std::clamp(CmdFloat("speedorangeadd", 40.0f), 10.0f, 120.0f);
		const float target = RoundUpNice(est_kmh + margin);

		if (!(dyn.scale > 1.0f))
			dyn.scale = target;
		else if (target >= dyn.scale)
			dyn.scale = target; // 上升即时
		else
			dyn.scale = std::max(target, dyn.scale - 60.0f * dt); // 下降限速 60 km/h 每秒

		range_kmh = dyn.scale;
		return true;
	}
}
