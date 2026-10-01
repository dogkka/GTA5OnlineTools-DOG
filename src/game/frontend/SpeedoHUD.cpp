#include "SpeedoHUD.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "core/commands/Commands.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/util/Joaat.hpp"
#include "game/backend/Self.hpp"
#include "game/frontend/Menu.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/invoker/Invoker.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu
{
	namespace
	{
		constexpr float kPi = 3.14159265f;

		constexpr float kGaugeStart = kPi * 0.75f;  // 135°
		constexpr float kGaugeSweep = kPi * 1.5f;   // 270°

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

		void AddTextCentered(ImDrawList* dl, ImFont* font, float size, ImVec2 c, ImU32 col, const char* text)
		{
			if (!font || !text)
				return;
			const ImVec2 ts = font->CalcTextSizeA(size, FLT_MAX, 0.0f, text);
			dl->AddText(font, size, ImVec2(c.x - ts.x * 0.5f, c.y - ts.y * 0.5f), col, text);
		}

		// 圆形表盘：刻度 + 进度弧 + 指针 + 中心数字
		void DrawDial(ImDrawList* dl, ImFont* font, ImVec2 c, float r, float ratio, const char* big, const char* small, ImU32 accent)
		{
			ratio = std::clamp(ratio, 0.0f, 1.0f);

			dl->AddCircleFilled(c, r + 6.0f, IM_COL32(10, 12, 18, 185), 48);
			AddArc(dl, c, r, kGaugeStart, kGaugeStart + kGaugeSweep, IM_COL32(70, 78, 92, 255), 3.0f);

			for (int i = 0; i <= 10; i++)
			{
				const float a  = kGaugeStart + kGaugeSweep * static_cast<float>(i) / 10.0f;
				const float r0 = (i % 5 == 0) ? r - 12.0f : r - 7.0f;
				dl->AddLine(ImVec2(c.x + std::cos(a) * r0, c.y + std::sin(a) * r0),
				    ImVec2(c.x + std::cos(a) * (r - 2.0f), c.y + std::sin(a) * (r - 2.0f)),
				    IM_COL32(120, 130, 148, 255), 1.5f);
			}

			if (ratio > 0.001f)
				AddArc(dl, c, r, kGaugeStart, kGaugeStart + kGaugeSweep * ratio, accent, 4.0f);

			const float a = kGaugeStart + kGaugeSweep * ratio;
			dl->AddLine(c, ImVec2(c.x + std::cos(a) * (r - 14.0f), c.y + std::sin(a) * (r - 14.0f)), accent, 3.0f);
			dl->AddCircleFilled(c, 5.0f, IM_COL32(220, 226, 236, 255), 16);

			if (big)
				AddTextCentered(dl, font, r * 0.40f, ImVec2(c.x, c.y + r * 0.38f), IM_COL32(240, 244, 250, 255), big);
			if (small)
				AddTextCentered(dl, font, r * 0.19f, ImVec2(c.x, c.y + r * 0.68f), IM_COL32(150, 158, 172, 255), small);
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

			const int units = UnitsCmd() ? UnitsCmd()->GetState() : 0;
			const bool metric = UseMetric(units);

			const float speed_mps    = veh.GetSpeed();
			const float disp_speed   = metric ? speed_mps * 3.6f : speed_mps * 2.23694f;
			const float max_speed    = std::max(metric ? veh.GetMaxSpeed() * 3.6f : veh.GetMaxSpeed() * 2.23694f, 1.0f);
			const float rpm_ratio    = std::clamp(veh.GetRevRatio(), 0.0f, 1.0f);
			const int gear           = veh.GetGear();
			const float speed_ratio  = std::clamp(disp_speed / (max_speed * 1.05f), 0.0f, 1.0f);
			const char* unit         = metric ? "公里/时" : "英里/时";

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

			float w = 330.0f, h = 185.0f;
			if (style == 2)
			{
				w = 235.0f;
				h = 235.0f;
			}
			else if (style == 3)
			{
				w = 290.0f;
				h = 112.0f;
			}

			const float px = PosCmd("speedox") ? PosCmd("speedox")->GetState() : 1.0f;
			const float py = PosCmd("speedoy") ? PosCmd("speedoy")->GetState() : 0.85f;
			const ImVec2 origin(px * (sw - w), py * (sh - h));

			if (auto dl = ImGui::GetBackgroundDrawList())
			{
				ImFont* font = Menu::Font::g_DefaultFont ? Menu::Font::g_DefaultFont : ImGui::GetFont();

				if (style == 1)
				{
					// 双圆盘：左转速 + 右速度
					const float r1 = h * 0.32f;
					const float r2 = h * 0.40f;
					const ImVec2 c1(origin.x + h * 0.38f, origin.y + h * 0.56f);
					const ImVec2 c2(origin.x + h * 1.13f, origin.y + h * 0.52f);

					DrawDial(dl, font, c1, r1, rpm_ratio, nullptr, nullptr, IM_COL32(255, 150, 60, 255));
					AddTextCentered(dl, font, r1 * 0.30f, ImVec2(c1.x, c1.y + r1 * 0.40f), IM_COL32(255, 176, 96, 255), rpm_buf);
					AddTextCentered(dl, font, r1 * 0.22f, ImVec2(c1.x, c1.y + r1 * 0.72f), IM_COL32(150, 158, 172, 255), "转速");

					DrawDial(dl, font, c2, r2, speed_ratio, speed_buf, unit, IM_COL32(80, 180, 255, 255));
					AddTextCentered(dl, font, r2 * 0.22f, ImVec2(c2.x, c2.y - r2 * 0.68f), IM_COL32(200, 208, 220, 255), gear_buf);
				}
				else if (style == 2)
				{
					// 环形组合：外圈转速 + 内圈速度
					const ImVec2 c(origin.x + w * 0.5f, origin.y + w * 0.5f);
					const float r_out = w * 0.44f;
					const float r_in  = w * 0.335f;

					dl->AddCircleFilled(c, r_out + 10.0f, IM_COL32(10, 12, 18, 185), 64);

					AddArc(dl, c, r_out, kGaugeStart, kGaugeStart + kGaugeSweep, IM_COL32(66, 72, 86, 255), 9.0f);
					if (rpm_ratio > 0.001f)
						AddArc(dl, c, r_out, kGaugeStart, kGaugeStart + kGaugeSweep * rpm_ratio, IM_COL32(255, 122, 50, 255), 9.0f);

					AddArc(dl, c, r_in, kGaugeStart, kGaugeStart + kGaugeSweep, IM_COL32(40, 52, 70, 255), 9.0f);
					if (speed_ratio > 0.001f)
						AddArc(dl, c, r_in, kGaugeStart, kGaugeStart + kGaugeSweep * speed_ratio, IM_COL32(70, 170, 255, 255), 9.0f);

					AddTextCentered(dl, font, 46.0f, ImVec2(c.x, c.y - 4.0f), IM_COL32(240, 244, 250, 255), speed_buf);
					AddTextCentered(dl, font, 15.0f, ImVec2(c.x, c.y + 30.0f), IM_COL32(150, 158, 172, 255), unit);
					AddTextCentered(dl, font, 15.0f, ImVec2(c.x, c.y - 48.0f), IM_COL32(255, 176, 96, 255), gear_buf);
					AddTextCentered(dl, font, 13.0f, ImVec2(c.x, c.y + 52.0f), IM_COL32(140, 148, 162, 255), rpm_buf);
				}
				else // style == 3 数字极简
				{
					dl->AddRectFilled(origin, ImVec2(origin.x + w, origin.y + h), IM_COL32(10, 12, 18, 180), 10.0f);
					dl->AddRect(origin, ImVec2(origin.x + w, origin.y + h), IM_COL32(60, 68, 82, 230), 10.0f);

					// 大号速度数字
					if (font)
					{
						const ImVec2 ts = font->CalcTextSizeA(54.0f, FLT_MAX, 0.0f, speed_buf);
						dl->AddText(font, 54.0f, ImVec2(origin.x + 16.0f, origin.y + 16.0f), IM_COL32(240, 244, 250, 255), speed_buf);
						dl->AddText(font, 16.0f, ImVec2(origin.x + 20.0f + ts.x, origin.y + 48.0f), IM_COL32(150, 158, 172, 255), unit);
					}

					// 右上档位
					AddTextCentered(dl, font, 22.0f, ImVec2(origin.x + w - 52.0f, origin.y + 36.0f), IM_COL32(80, 180, 255, 255), gear_buf);
					AddTextCentered(dl, font, 13.0f, ImVec2(origin.x + w - 52.0f, origin.y + 62.0f), IM_COL32(140, 148, 162, 255), rpm_buf);

					// 底部转速条
					const ImVec2 b0(origin.x + 16.0f, origin.y + h - 24.0f);
					const ImVec2 b1(origin.x + w - 16.0f, origin.y + h - 14.0f);
					dl->AddRectFilled(b0, b1, IM_COL32(34, 38, 48, 255), 5.0f);
					dl->AddRectFilled(b0, ImVec2(b0.x + (b1.x - b0.x) * rpm_ratio, b1.y),
					    rpm_ratio > 0.85f ? IM_COL32(255, 90, 70, 255) : IM_COL32(255, 150, 60, 255), 5.0f);
					dl->AddRect(b0, b1, IM_COL32(70, 78, 92, 255), 5.0f);
				}
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
