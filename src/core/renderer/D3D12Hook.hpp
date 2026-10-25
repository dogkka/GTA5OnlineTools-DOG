// Credits to:
// https://github.com/praydog/REFramework/blob/master/src/D3D12Hook.cpp
// https://github.com/praydog/REFramework/blob/master/src/D3D12Hook.hpp
#pragma once
#include "core/hooking/VMTHook.hpp"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

namespace YimMenu
{
    class D3D12Hook
    {
    public:
        static bool Init()
        {
            return GetInstance().InitImpl();
        }

        static bool Destroy(bool unloading = false)
        {
            return GetInstance().DestroyImpl(unloading);
        }

        static void OnPresent(std::function<void()> fn)
        {
            GetInstance().m_OnPresent = std::move(fn);
        }

        static void OnResizeBuffers(std::function<void()> fn)
        {
            GetInstance().m_OnResizeBuffers = std::move(fn);
        }

        static void OnDeviceReset(std::function<void()> fn)
        {
            GetInstance().m_OnDeviceReset = std::move(fn);
        }

        static ID3D12Device4* GetDevice()
        {
            return GetInstance().m_Device;
        }

        static IDXGISwapChain3* GetSwapChain()
        {
            return GetInstance().m_SwapChain;
        }

        static ID3D12CommandQueue* GetCommandQueue()
        {
            return GetInstance().m_CommandQueue;
        }

        // Verifies that a pointer is really an ID3D12CommandQueue (and a direct
        // queue). The swapchain-recreation path used to only check for nullptr,
        // so a garbage slot value (-1 etc.) slipped through and crashed D3D12.
        static bool IsPlausibleCommandQueue(ID3D12CommandQueue* queue);
        static bool ValidateCommandQueue(ID3D12CommandQueue* queue);

        static HWND GetWindow()
        {
            return GetInstance().m_Window;
        }

    private:
        // VMTHook clones the vtable per-instance (so hooking one object never
        // affects another), which is exactly the opposite of what phase 1 needs, so
        // this one raw patch is done by hand.
        struct RawVtablePatch
        {
            bool Install(void** vtable, size_t index, void* newFn);
            void Uninstall();

            template <typename T>
            T Original() const
            {
                return reinterpret_cast<T>(OriginalFn);
            }

            void** Slot = nullptr;
            void* OriginalFn = nullptr;
            bool Installed = false;
        };

        static D3D12Hook& GetInstance()
        {
            static D3D12Hook instance;
            return instance;
        }

        bool InitImpl();
        bool DestroyImpl(bool unloading);
        void InstallHooks();
        void RefreshCommandQueue(IDXGISwapChain3* swapChain);

        static HRESULT Present(IDXGISwapChain3* swapChain, UINT syncInterval, UINT flags, void* r9);
        static HRESULT ResizeBuffers(IDXGISwapChain3* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
        static HRESULT CreateSwapchain(IDXGIFactory4* factory, IUnknown* device, HWND hwnd, const DXGI_SWAP_CHAIN_DESC1* desc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain** swapChain);

        std::function<void()> m_OnPresent = nullptr;
        std::function<void()> m_OnResizeBuffers = nullptr;
        std::function<void()> m_OnDeviceReset = nullptr;
        ID3D12Device4* m_Device = nullptr;
        ID3D12CommandQueue* m_CommandQueue = nullptr;
        // Authoritative queue captured from the CreateSwapChainForHwnd call: the
        // offset-based scan can latch onto a different (internal/proxy) queue after a
        // swapchain recreation, which makes the overlay invisible instead of crashing.
        ID3D12CommandQueue* m_CreationQueue = nullptr;
        IDXGISwapChain3* m_SwapChain = nullptr;
        HWND m_Window = nullptr;
        bool m_Hooked = false;
        bool m_IsPhase1 = true;
        uint32_t m_CommandQueueOffset = 0;
        DWORD m_NextResolveTick = 0; // throttles repeated queue scans after a failed resolution
        IDXGISwapChain3* m_LastFailedChain = nullptr; // chain the throttle above belongs to

        IDXGISwapChain3* m_ResolvedChain = nullptr; // cached queue resolution, per swapchain object
        ID3D12CommandQueue* m_ResolvedQueue = nullptr;
        void** m_FactoryVtable = nullptr;
        void** m_SwapchainVtable = nullptr;
        RawVtablePatch m_CreateSwapchainPatch = {};
        RawVtablePatch m_PresentPatch = {};
		std::unique_ptr<VMTHook> m_SwapchainVmt = {};
        std::recursive_mutex m_HookMutex = {};
    };
}
