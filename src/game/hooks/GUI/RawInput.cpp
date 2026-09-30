#include "core/hooking/DetourHook.hpp"
#include "core/renderer/Renderer.hpp"
#include "game/hooks/Hooks.hpp"

namespace YimMenu::Hooks
{
	UINT RawInput::GetRawInputData(HRAWINPUT hRawInput, UINT uiCommand, LPVOID pData, PUINT pcbSize, UINT cbSizeHeader)
	{
		auto result = BaseHook::Get<RawInput::GetRawInputData, DetourHook<decltype(&RawInput::GetRawInputData)>>()->Original()(hRawInput, uiCommand, pData, pcbSize, cbSizeHeader);

		constexpr auto failure = static_cast<UINT>(-1);
		if (result != failure && result > 0 && pData && uiCommand == RID_INPUT && Renderer::IsInitialized() && ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureMouse)
		{
			auto& raw = *static_cast<RAWINPUT*>(pData);
			if (raw.header.dwType == RIM_TYPEMOUSE)
				raw.data.mouse.usButtonFlags = 0;
		}

		return result;
	}
}
