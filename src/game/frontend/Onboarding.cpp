#include "Onboarding.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/BoolCommand.hpp"
#include "game/frontend/Menu.hpp"
#include "game/pointers/Pointers.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>

namespace YimMenu
{
	// ============================================================================================
	// [DOG] 注入动效（设计 A：光带启动 · HUD 科技风）
	// --------------------------------------------------------------------------------------------
	// 每次注入（DLL 加载）后自动播放一次，约 8 秒（v2.1.40 加长加量：主字 54px、光带 560px）：
	//   0.00-0.90s  光带从中心向两侧展开（HUD 启动感）
	//   0.60-1.80s  主字「注入成功」从 0.8 倍弹性放大入场（青绿 + 辉光）
	//   1.20-2.20s  署名「D O G」宽字距浮现（金色，两侧细线同步展开）
	//   1.90-2.80s  高光从左向右扫过整组文字
	//   2.80-6.60s  呼吸停留（辉光强度 ±15% 波动）
	//   6.60-8.00s  谢幕：整体上浮渐隐、光带收拢
	//
	// 实现说明：纯 ImDrawList 前景绘制（无窗口、无交互、不写 imgui.ini）；
	// 所有节奏 / 配色 / 尺寸均为下方 constexpr 参数，集中可调。
	// ============================================================================================
	namespace InjectFx
	{
		// ---------- 时间轴（秒） ----------
		constexpr float kDuration = 8.00f;
		constexpr float kBandOpenEnd = 0.90f;
		constexpr float kTitleInStart = 0.60f;
		constexpr float kTitleInEnd = 1.80f;
		constexpr float kDogInStart = 1.20f;
		constexpr float kDogInEnd = 2.20f;
		constexpr float kShimmerStart = 1.90f;
		constexpr float kShimmerEnd = 2.80f;
		constexpr float kHoldEnd = 6.60f;
		constexpr float kFadeEnd = 8.00f;

		// ---------- 配色 ----------
		constexpr ImVec4 kTitleColor{0.36f, 0.95f, 0.76f, 1.0f}; // 主字：青绿
		constexpr ImVec4 kDogColor{1.00f, 0.84f, 0.36f, 1.0f};   // 署名：金色
		constexpr ImVec4 kBandColor{0.30f, 0.90f, 0.76f, 1.0f};  // 光带：青绿

		// ---------- 布局 ----------
		constexpr float kTitleSize = 54.0f;      // 主字号（v2.1.40：34 → 54）
		constexpr float kDogSize = 22.0f;        // 署名字号（15 → 22）
		constexpr float kBandWidth = 560.0f;     // 光带最大宽度（340 → 560）
		constexpr float kBandThickness = 3.0f;   // 光带厚度
		constexpr float kBandGlowHeight = 40.0f; // 光带下方光晕高度
		constexpr float kDogGap = 44.0f;         // 署名字距
		constexpr float kDogLineLen = 96.0f;     // 署名两侧细线长度
		constexpr float kRiseDistance = 18.0f;   // 谢幕上浮距离
		constexpr float kScreenAnchorY = 0.42f;  // 纵向锚点（屏高 42%，中央偏上）

		// ---------- 缓动 ----------
		inline float EaseOutCubic(float t)
		{
			t = std::clamp(t, 0.0f, 1.0f);
			const float u = 1.0f - t;
			return 1.0f - u * u * u;
		}

		inline float EaseOutBack(float t)
		{
			t = std::clamp(t, 0.0f, 1.0f);
			constexpr float c1 = 1.70158f;
			constexpr float c3 = c1 + 1.0f;
			const float u = t - 1.0f;
			return 1.0f + c3 * u * u * u + c1 * u * u;
		}

		inline float EaseInOutCubic(float t)
		{
			t = std::clamp(t, 0.0f, 1.0f);
			return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) * 0.5f;
		}

		inline float Lerp(float a, float b, float t)
		{
			return a + (b - a) * t;
		}

		inline ImU32 Col(const ImVec4& c, float a)
		{
			return ImGui::ColorConvertFloat4ToU32(ImVec4{c.x, c.y, c.z, c.w * std::clamp(a, 0.0f, 1.0f)});
		}

		// 带彩色辉光与黑色软描边的文字（任意背景下可读）
		void AddGlowText(ImDrawList* dl, ImFont* font, float size, const ImVec2& pos, const ImVec4& color, const char* text, float alpha, float glow)
		{
			if (alpha <= 0.001f || font == nullptr)
				return;

			static constexpr float kDirs[8][2] = {
			    {1.0f, 0.0f}, {-1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, -1.0f},
			    {0.7f, 0.7f}, {-0.7f, 0.7f}, {0.7f, -0.7f}, {-0.7f, -0.7f}};

			// 彩色外辉光（两圈）
			if (glow > 0.01f)
			{
				for (int ring = 0; ring < 2; ring++)
				{
					const float dist = 3.0f + ring * 3.0f;
					const float ringAlpha = (ring == 0 ? 0.22f : 0.12f) * glow * alpha;
					for (int i = 0; i < 8; i++)
						dl->AddText(font, size, ImVec2{pos.x + kDirs[i][0] * dist, pos.y + kDirs[i][1] * dist}, Col(color, ringAlpha), text);
				}
			}

			// 黑色软描边
			for (int i = 0; i < 8; i++)
				dl->AddText(font, size, ImVec2{pos.x + kDirs[i][0] * 2.2f, pos.y + kDirs[i][1] * 2.2f}, IM_COL32(0, 0, 0, static_cast<int>(140 * alpha)), text);

			// 本体
			dl->AddText(font, size, pos, Col(color, alpha), text);
		}

		// 逐字绘制的「D O G」署名（金色宽字距 + 两侧细线），并输出其边界（供扫光使用）
		void DrawSignature(ImDrawList* dl, float cx, float y, float alpha, float spread, ImVec2& outMin, ImVec2& outMax)
		{
			ImFont* font = Menu::Font::g_DefaultFont;
			if (font == nullptr)
				return;

			const char* letters[3] = {"D", "O", "G"};
			ImVec2 sizes[3];
			float totalWidth = 0.0f;
			for (int i = 0; i < 3; i++)
			{
				sizes[i] = font->CalcTextSizeA(kDogSize, FLT_MAX, 0.0f, letters[i]);
				totalWidth += sizes[i].x + (i < 2 ? kDogGap : 0.0f);
			}

			float x = cx - totalWidth * 0.5f;
			outMin = ImVec2{x, y};
			outMax = ImVec2{x + totalWidth, y + sizes[0].y};

			for (int i = 0; i < 3; i++)
			{
				AddGlowText(dl, font, kDogSize, ImVec2{x, y}, kDogColor, letters[i], alpha, 0.45f);
				x += sizes[i].x + kDogGap;
			}

			// 两侧细线（随出场进度从内向外展开）
			const float lineLen = kDogLineLen * spread;
			const float lineY = y + sizes[0].y * 0.5f;
			const float lineGap = 18.0f;
			if (lineLen > 1.0f)
			{
				dl->AddRectFilledMultiColor(
				    ImVec2{outMin.x - lineGap - lineLen, lineY - 1.0f}, ImVec2{outMin.x - lineGap, lineY + 1.0f},
				    Col(kDogColor, 0.0f), Col(kDogColor, 0.7f * alpha), Col(kDogColor, 0.7f * alpha), Col(kDogColor, 0.0f));
				dl->AddRectFilledMultiColor(
				    ImVec2{outMax.x + lineGap, lineY - 1.0f}, ImVec2{outMax.x + lineGap + lineLen, lineY + 1.0f},
				    Col(kDogColor, 0.7f * alpha), Col(kDogColor, 0.0f), Col(kDogColor, 0.0f), Col(kDogColor, 0.7f * alpha));
			}
		}

		// 每帧绘制；播放完毕自动停止（DLL 生命周期内只播一次；每次注入自然重播）
		void Draw()
		{
			static const auto start = std::chrono::steady_clock::now();
			static bool done = false;
			if (done)
				return;

			const float t = std::chrono::duration<float>(std::chrono::steady_clock::now() - start).count();
			if (t >= kDuration)
			{
				done = true;
				return;
			}

			if (Pointers.ScreenResX == nullptr || Pointers.ScreenResY == nullptr)
				return;

			const float cx = *Pointers.ScreenResX * 0.5f;
			const float cy = *Pointers.ScreenResY * kScreenAnchorY;

			// ---- 全局时间轴派生量 ----
			float alpha = 1.0f;
			float rise = 0.0f;
			if (t > kHoldEnd)
			{
				const float p = EaseInOutCubic((t - kHoldEnd) / (kFadeEnd - kHoldEnd));
				alpha = 1.0f - p;
				rise = p * kRiseDistance;
			}

			float breath = 1.0f;
			if (t > 2.8f && t < kHoldEnd)
				breath = 1.0f + 0.15f * std::sin((t - 2.8f) * 2.0f * 3.14159265f / 1.2f);

			const float originY = cy - rise;
			ImDrawList* dl = ImGui::GetForegroundDrawList();
			if (dl == nullptr)
				return;

			const float bandY = originY - 92.0f;
			const float titleY = originY - 44.0f;
			const float dogY = originY + 40.0f;

			// ---- ① 光带 ----
			{
				float bandP = EaseOutCubic(t / kBandOpenEnd);
				if (t > kHoldEnd)
					bandP *= 1.0f - EaseInOutCubic((t - kHoldEnd) / (kFadeEnd - kHoldEnd));

				const float halfW = kBandWidth * 0.5f * bandP;
				if (halfW > 1.0f)
				{
					const float a = alpha * 0.95f;
					const ImU32 edge = Col(kBandColor, 0.0f);
					const ImU32 core = Col(kBandColor, a);

					dl->AddRectFilledMultiColor({cx - halfW, bandY}, {cx, bandY + kBandThickness}, edge, core, core, edge);
					dl->AddRectFilledMultiColor({cx, bandY}, {cx + halfW, bandY + kBandThickness}, core, edge, edge, core);

					const ImU32 halo = Col(kBandColor, a * 0.16f);
					dl->AddRectFilledMultiColor(
					    {cx - halfW, bandY + kBandThickness}, {cx + halfW, bandY + kBandThickness + kBandGlowHeight},
					    halo, halo, edge, edge);
				}
			}

			// ---- ② 主字「注入成功」 ----
			ImVec2 titleMin{}, titleMax{};
			{
				ImFont* font = Menu::Font::g_OverlayFont;
				if (font != nullptr && t >= kTitleInStart)
				{
					const float p = EaseOutBack((t - kTitleInStart) / (kTitleInEnd - kTitleInStart));
					const float fadeIn = std::clamp((t - kTitleInStart) / 0.35f, 0.0f, 1.0f);
					const float scale = 0.8f + 0.2f * p;
					const float size = kTitleSize * scale;
					const char* text = "注入成功";
					const ImVec2 ts = font->CalcTextSizeA(size, FLT_MAX, 0.0f, text);

					titleMin = ImVec2{cx - ts.x * 0.5f, titleY - ts.y * 0.5f};
					titleMax = ImVec2{cx + ts.x * 0.5f, titleY + ts.y * 0.5f};

					AddGlowText(dl, font, size, titleMin, kTitleColor, text, fadeIn * alpha, breath);
				}
			}

			// ---- ③ 署名「D O G」 ----
			ImVec2 dogMin{}, dogMax{};
			if (t >= kDogInStart)
			{
				const float p = EaseOutCubic((t - kDogInStart) / (kDogInEnd - kDogInStart));
				DrawSignature(dl, cx, dogY, p * alpha, p, dogMin, dogMax);
			}

			// ---- ④ 扫光（掠过整组文字） ----
			if (t >= kShimmerStart && t <= kShimmerEnd && titleMax.x > titleMin.x)
			{
				const float p = EaseOutCubic((t - kShimmerStart) / (kShimmerEnd - kShimmerStart));
				const float left = titleMin.x - 20.0f;
				const float right = titleMax.x + 20.0f;
				const float top = titleMin.y - 12.0f;
				const float bottom = (dogMax.y > dogMin.y ? dogMax.y : titleMax.y) + 12.0f;
				const float scanX = Lerp(left - 60.0f, right + 60.0f, p);
				constexpr float scanW = 64.0f;

				dl->PushClipRect({left, top}, {right, bottom}, true);
				const ImU32 transparent = IM_COL32(255, 255, 255, 0);
				const ImU32 bright = IM_COL32(255, 255, 255, static_cast<int>(140 * alpha));
				dl->AddRectFilledMultiColor({scanX - scanW, top}, {scanX, bottom}, transparent, bright, bright, transparent);
				dl->AddRectFilledMultiColor({scanX, top}, {scanX + scanW, bottom}, bright, transparent, transparent, bright);
				dl->PopClipRect();
			}
		}
	}

	void ProcessOnboarding()
	{
		// 一次性：默认匹配设置（仅与 YimMenu 用户游玩），之后可在 网络 > 伪装 中调整
		static BoolCommand _OnboardingComplete{"$onboardingcomplete", "", ""};
		if (!_OnboardingComplete.GetState())
		{
			if (auto cmd = Commands::GetCommand<BoolCommand>("cheaterpool"_J))
				cmd->SetState(true);
			_OnboardingComplete.SetState(true);
		}

		// 每次注入：播放「注入成功 · DOG」动效（约 8 秒后自动结束）
		InjectFx::Draw();
	}
}
