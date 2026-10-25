#include "core/commands/Command.hpp"
#include "core/commands/HotkeySystem.hpp"
#include "core/util/Joaat.hpp"
#include "core/frontend/manager/UIManager.hpp"
#include "GUI.hpp"
#include "Menu.hpp"
#include "ESP.hpp"
#include "SpeedoHUD.hpp"
#include "Overlay.hpp"
#include "OverheadTag.hpp"
#include "PlayerBar.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/renderer/Renderer.hpp"
#include "core/scripting/LuaManager.hpp"
#include "core/scripting/LuaScript.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/frontend/ChatDisplay.hpp"
#include "game/gta/Natives.hpp"
#include "types/pad/ControllerInputs.hpp"

namespace YimMenu
{
	void GUI::InitImpl()
	{
		Menu::Init();

		Renderer::AddWindowProcedureCallback([this](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
			GUI::WndProc(hwnd, msg, wparam, lparam);
		});

		Renderer::AddRendererCallback(
		    [&] {
			    Notifications::Draw();
		    },
		    -2);
		Renderer::AddRendererCallback(
		    [&] {
			    ESP::Draw();
		    },
		    -3);
		Renderer::AddRendererCallback(
		    [&] {
			    ChatDisplay::Draw();
		    },
		    -5);
		Renderer::AddRendererCallback(
		    [&] {
			    Overlay::Draw();
		    },
		    -6);
		Renderer::AddRendererCallback(
		    [] {
			    SpeedoHUD::Draw();
		    },
		    -8);
		Renderer::AddRendererCallback(
		    [] {
			    OverheadTag::Draw();
		    },
		    -9);
		Renderer::AddRendererCallback(
		    [] {
			    PlayerBar::Draw();
		    },
		    -10);
		Renderer::AddRendererCallback(
		    [] {
			    LuaManager::ForAllLoadedScripts([](std::shared_ptr<LuaScript>& script) {
				    script->GetUserInterface().DrawAlwaysDrawImGuiCallbacks();
			    });
		    },
		    -7);
		Renderer::AddRendererCallback(
		    [] {
			    if (!GUI::IsOpen())
				    return;
			    LuaManager::ForAllLoadedScripts([](std::shared_ptr<LuaScript>& script) {
				    script->GetUserInterface().DrawImGuiCallbacks();
			    });
			    },
			    -4);

			Renderer::SetSafeToRender();
	}

	void GUI::ToggleMouse()
	{
		auto& io = ImGui::GetIO();
		io.MouseDrawCursor = GUI::IsOpen() || GUI::IsOnboarding();
		GUI::IsOpen() || GUI::IsOnboarding() 
			? io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse :
			  io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
	}

	void GUI::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	bool shouldToggle = false;

	if (msg == WM_KEYUP && !HotkeySystem::IsBeingModified())
	{
		// [DOG] 固定备用组合：Ctrl + \（自定义键被清空后仍可呼出）
		if (wparam == VK_OEM_5 && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			shouldToggle = true;
		}
		// [DOG] 自定义呼出键（默认 F7，见 设置 → 热键 →「呼出菜单」）：
		// 松开链中任一键时，其余键仍按住则触发
		else if (auto it = g_HotkeySystem.m_CommandHotkeys.find("menutoggle"_J);
		         it != g_HotkeySystem.m_CommandHotkeys.end() && !it->second.m_Chain.empty())
		{
			bool in_chain = false;
			bool others_down = true;

			for (auto key : it->second.m_Chain)
			{
				if (key == static_cast<int>(wparam))
					in_chain = true;
				else if (!(GetAsyncKeyState(key) & 0x8000))
					others_down = false;
			}

			shouldToggle = in_chain && others_down;
		}
	}

	if (shouldToggle)
	{
		// Persist and restore the cursor position between menu instances
		static POINT CursorCoords{};
		if (m_IsOpen)
		{
			GetCursorPos(&CursorCoords);
		}
		else if (CursorCoords.x + CursorCoords.y)
		{
			SetCursorPos(CursorCoords.x, CursorCoords.y);
		}
		if (!GUI::IsOnboarding())
			Toggle();
		ToggleMouse();
	}
}
	
	void GUI::SetOnboardingImpl(bool state)
	{
		m_Onboarding = state;
		ToggleMouse();
	}

	void GUI::RunScriptImpl()
	{
		while (g_Running)
		{
			if (Renderer::IsInitialized() && GUI::IsOpen())
			{
				if (UIManager::ShowingContentWindow())
				{
					if (GUI::IsUsingKeyboard() && PAD::IS_USING_KEYBOARD_AND_MOUSE(0))
					{
						PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
					}
					else
					{
						static constexpr ControllerInputs controls[] = {ControllerInputs::INPUT_LOOK_LR, ControllerInputs::INPUT_LOOK_UD, ControllerInputs::INPUT_ATTACK, ControllerInputs::INPUT_AIM, ControllerInputs::INPUT_DUCK, ControllerInputs::INPUT_SELECT_WEAPON, ControllerInputs::INPUT_VEH_AIM, ControllerInputs::INPUT_VEH_ATTACK, ControllerInputs::INPUT_VEH_ATTACK2, ControllerInputs::INPUT_VEH_NEXT_RADIO, ControllerInputs::INPUT_VEH_PASSENGER_AIM, ControllerInputs::INPUT_VEH_PASSENGER_ATTACK, ControllerInputs::INPUT_VEH_SELECT_NEXT_WEAPON, ControllerInputs::INPUT_VEH_SELECT_PREV_WEAPON, ControllerInputs::INPUT_VEH_MOUSE_CONTROL_OVERRIDE, ControllerInputs::INPUT_MELEE_ATTACK_ALTERNATE, ControllerInputs::INPUT_FRONTEND_Y, ControllerInputs::INPUT_ATTACK2, ControllerInputs::INPUT_PREV_WEAPON, ControllerInputs::INPUT_NEXT_WEAPON, ControllerInputs::INPUT_VEH_DRIVE_LOOK, ControllerInputs::INPUT_VEH_DRIVE_LOOK2};

						for (const auto& control : controls)
							PAD::DISABLE_CONTROL_ACTION(0, static_cast<int>(control), true);
					}
				}
			}

			ScriptMgr::Yield();
		}
	}

	// [DOG] 菜单呼出键命令：占位注册，用于在 设置 → 热键 中显示并提供自定义绑定；
	// 实际触发由 GUI::WndProc 处理（见上方），HotkeySystem 轮询循环会跳过它，避免双重触发。
	class MenuToggleCommand : public Command
	{
	public:
		MenuToggleCommand() :
		    Command("menutoggle", "呼出菜单", "自定义呼出 / 关闭菜单的快捷键（默认 F7；固定备用 Ctrl + \\）")
		{
		}

		virtual void OnCall() override
		{
			// 兜底：若将来改由 HotkeySystem 直接驱动，也可正常工作
			GUI::Toggle();
			GUI::ToggleMouse();
		}
	};

	static MenuToggleCommand _MenuToggleCommand;
}
