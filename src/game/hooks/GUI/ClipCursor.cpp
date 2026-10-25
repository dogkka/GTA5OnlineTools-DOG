#include "core/hooking/DetourHook.hpp"
#include "game/hooks/Hooks.hpp"
#include "game/frontend/GUI.hpp"

namespace YimMenu::Hooks
{
	// [DOG] 修复部分用户"呼出菜单后鼠标被锁在屏幕中心"（上游 issue #883 / #760 同类问题）：
	// 游戏在 Windows 鼠标输入模式下会持续调用 ClipCursor 把光标钳制到窗口内；
	// 菜单打开期间拦截该调用并解除钳制（传 nullptr），关闭菜单后完全恢复原行为。
	BOOL Window::ClipCursor(const RECT* lpRect)
	{
		const auto original = BaseHook::Get<Window::ClipCursor, DetourHook<decltype(&Window::ClipCursor)>>()->Original();

		if (YimMenu::GUI::IsOpen() || YimMenu::GUI::IsOnboarding())
			return original(nullptr);

		return original(lpRect);
	}
}
