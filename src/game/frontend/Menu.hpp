#pragma once

namespace YimMenu::Menu
{
	extern void Init();
	extern void SetupFonts();

	namespace Font
	{
		inline ImFont* g_DefaultFont = nullptr;
		inline float g_DefaultFontScale = 1.0f; // keep the original menu font size (1.65 caused oversized text and broken modern-theme layouts)
		inline float g_DefaultFontSize = 19.0f;

		inline ImFont* g_OptionsFont = nullptr;
		inline float g_OptionsFontSize = 17.0f;

		inline ImFont* g_ChildTitleFont = nullptr;
		inline float g_ChildTitleFontSize = 14.5f;

		inline ImFont* g_ChatFont = nullptr;
		inline float g_ChatFontSize = 24.0f;

		inline ImFont* g_OverlayFont = nullptr;
		inline float g_OverlayFontSize = 30.0f;

		inline ImFont* g_AwesomeFont = nullptr;
		inline float g_AwesomeFontSize = 30.0f;

		inline ImFont* g_HudBigFont = nullptr; // 速度表等 HUD 专用大号数字字体（仅拉丁字形，清晰锐利）
		inline float g_HudBigFontSize = 56.0f;
	}
}
