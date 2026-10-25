#include "D3D12Hook.hpp"

#include <intrin.h>

namespace YimMenu
{
    static thread_local bool g_InsideD3d12Hook = false;

    namespace
    {
        // The Present hook is re-entered once per layer of the hook chain
        // (us -> overlay -> dxgi). A healthy chain is only a handful of frames
        // deep; anything deeper is a cycle and must be broken before the
        // thread stack overflows. Without this guard a tangled chain with the
        // Steam overlay recursed ~3000 frames deep and crashed the game about
        // one minute after injection.
        constexpr int kMaxPresentRecursionDepth = 32;

        struct PresentRecursionGuard
        {
            static thread_local int s_Depth;

            PresentRecursionGuard()
            {
                ++s_Depth;
            }

            ~PresentRecursionGuard()
            {
                --s_Depth;
            }
        };

        thread_local int PresentRecursionGuard::s_Depth = 0;

        // A swapchain that does not belong to the game itself: overlay render
        // targets use a non-opaque alpha mode and/or a different window, and
        // hooking them serves no purpose (the game presents through its own
        // chain). While no window is known yet we cannot judge, so only the
        // alpha mode is checked.
        bool IsForeignSwapchainForMenu(IDXGISwapChain* swapChain, HWND menuWindow)
        {
            if (!swapChain)
                return true;

            Microsoft::WRL::ComPtr<IDXGISwapChain1> swapchain1{};
            if (FAILED(swapChain->QueryInterface(IID_PPV_ARGS(&swapchain1))))
                return false;

            DXGI_SWAP_CHAIN_DESC1 desc1{};
            if (FAILED(swapchain1->GetDesc1(&desc1)))
                return false;

            // Composition-style overlay swapchains are never the game main chain.
            if (desc1.AlphaMode == DXGI_ALPHA_MODE_PREMULTIPLIED || desc1.AlphaMode == DXGI_ALPHA_MODE_STRAIGHT)
                return true;

            HWND swapchainWindow{};
            if (FAILED(swapchain1->GetHwnd(&swapchainWindow)))
                return false;

            if (menuWindow && swapchainWindow != nullptr && swapchainWindow != menuWindow)
                return true;

            return false;
        }

        // A candidate pointer must be committed, readable memory before we are
        // willing to dereference it. The old code only checked for null, so a
        // stale/garbage slot in a recreated swapchain slipped through.
        bool IsReadableMemory(const void* ptr, size_t size = sizeof(void*))
        {
            if (!ptr)
                return false;

            MEMORY_BASIC_INFORMATION mbi{};
            if (VirtualQuery(ptr, &mbi, sizeof(mbi)) != sizeof(mbi))
                return false;
            if (mbi.State != MEM_COMMIT)
                return false;
            if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))
                return false;
            if (mbi.RegionSize < size)
                return false;

            return true;
        }

        bool IsExecutableMemory(const void* ptr)
        {
            if (!IsReadableMemory(ptr))
                return false;

            MEMORY_BASIC_INFORMATION mbi{};
            if (VirtualQuery(ptr, &mbi, sizeof(mbi)) != sizeof(mbi))
                return false;

            return (mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) != 0;
        }
    }

    bool D3D12Hook::RawVtablePatch::Install(void** vtable, size_t index, void* newFn)
    {
        if (!vtable)
            return false;

        auto slot = &vtable[index];

        // Idempotency + self-hook guard: if the slot already points to our own
        // detour we must never capture it as the "original" - that would make
        // the detour call itself through its own chain and recurse until the
        // thread stack is gone. This is exactly how the old re-hook path blew
        // up while the Steam overlay sat in the same vtable slot.
        if (*slot == newFn)
        {
            if (Installed && Slot == slot)
            {
                return true; // our patch is already active, nothing to do
            }

            LOGF(WARNING, "Refusing to install vtable patch at {:#x}: the slot already contains our detour.", reinterpret_cast<uintptr_t>(slot));
            return false;
        }

        if (Installed && Slot != slot)
        {
            LOGF(WARNING, "Vtable patch re-install on a different slot; restoring the previous slot first.");
            Uninstall();
        }

        Slot = slot;
        OriginalFn = *slot;

        DWORD oldProtect{};
        if (!VirtualProtect(Slot, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect)) // we still need this in case the menu is injected very early
            return false;

        *Slot = newFn;

        VirtualProtect(Slot, sizeof(void*), oldProtect, &oldProtect);

        Installed = true;
        return true;
    }

    void D3D12Hook::RawVtablePatch::Uninstall()
    {
        if (!Installed || !Slot)
            return;

        DWORD oldProtect{};
        if (VirtualProtect(Slot, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect))
        {
            *Slot = OriginalFn;
            VirtualProtect(Slot, sizeof(void*), oldProtect, &oldProtect);
        }

        Installed = false;
        Slot = nullptr;
        OriginalFn = nullptr;
    }

    bool D3D12Hook::InitImpl()
    {
		std::scoped_lock hookLock{m_HookMutex};

        g_InsideD3d12Hook = true;
        struct Restore
        {
            ~Restore()
            {
                g_InsideD3d12Hook = false;
            }
        } restore{};

        // if this is a rehook after a swapchain recreation
        if (m_CommandQueueOffset != 0 && m_SwapchainVtable && m_FactoryVtable)
        {
            LOGF(INFO, "Re-initializing via previously discovered vtables.");

            if (m_Hooked && !m_IsPhase1 && m_SwapchainVmt)
            {
                // The per-instance hook is already live. Touching the shared
                // vtable again would tangle our chain with other overlays'
                // hooks (Steam/NVIDIA) - the exact thing this fix removes.
                LOGF(INFO, "Per-instance swapchain hook already active; skipping re-install.");
                return true;
            }

            InstallHooks();
            return m_Hooked;
        }

        IDXGISwapChain1* swapChain1 = nullptr;
        IDXGISwapChain3* swapChain = nullptr;
        ID3D12Device* device = nullptr;

        const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc1{};
        swapChainDesc1.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapChainDesc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        swapChainDesc1.BufferCount = 2;
        swapChainDesc1.SampleDesc.Count = 1;
        swapChainDesc1.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
        swapChainDesc1.Width = 1;
        swapChainDesc1.Height = 1;

        auto d3d12Module = LoadLibraryA("d3d12.dll");
        if (!d3d12Module)
        {
            LOGF(FATAL, "Failed to load d3d12.dll.");
            return false;
        }

        auto d3d12CreateDevice = reinterpret_cast<decltype(D3D12CreateDevice)*>(GetProcAddress(d3d12Module, "D3D12CreateDevice"));
        if (!d3d12CreateDevice)
        {
            LOGF(FATAL, "Failed to resolve D3D12CreateDevice.");
            return false;
        }

        bool deviceCreated = !FAILED(d3d12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&device)));
        if (!deviceCreated)
        {
            LOGF(FATAL, "Failed to create dummy D3D12 device.");
            return false;
        }

        auto dxgiModule = LoadLibraryA("dxgi.dll");
        if (!dxgiModule)
        {
            LOGF(FATAL, "Failed to load dxgi.dll.");
            device->Release();
            return false;
        }

        auto createDxgiFactory = reinterpret_cast<decltype(CreateDXGIFactory)*>(GetProcAddress(dxgiModule, "CreateDXGIFactory"));
        if (!createDxgiFactory)
        {
            LOGF(FATAL, "Failed to resolve CreateDXGIFactory.");
            device->Release();
            return false;
        }

        IDXGIFactory4* factory = nullptr;
        if (FAILED(createDxgiFactory(IID_PPV_ARGS(&factory))))
        {
            LOGF(FATAL, "Failed to create the dummy DXGI factory.");
            device->Release();
            return false;
        }

        D3D12_COMMAND_QUEUE_DESC queueDesc{};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        ID3D12CommandQueue* commandQueue = nullptr;
        if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue))))
        {
            LOGF(FATAL, "Failed to create the dummy command queue.");
            factory->Release();
            device->Release();
            return false;
        }

        HWND hwnd = nullptr;
        WNDCLASSEXA wc{};

        auto initDummyWindow = [&]() {
            wc.cbSize = sizeof(WNDCLASSEXA);
            wc.style = CS_HREDRAW | CS_VREDRAW;
            wc.lpfnWndProc = DefWindowProcA;
            wc.hInstance = GetModuleHandleA(nullptr);
            wc.lpszClassName = "YimMenuDummyWindow";

            RegisterClassExA(&wc);
            hwnd = CreateWindowA(wc.lpszClassName, "YimMenu Dummy Window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, nullptr, nullptr, wc.hInstance, nullptr);

            swapChainDesc1.BufferCount = 3;
            swapChainDesc1.Width = 0;
            swapChainDesc1.Height = 0;
            swapChainDesc1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swapChainDesc1.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
            swapChainDesc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc1.SampleDesc.Count = 1;
            swapChainDesc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            swapChainDesc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
            swapChainDesc1.Scaling = DXGI_SCALING_STRETCH;
        };

        const std::vector<std::function<bool()>> swapchainAttempts{
            [&]() {
                return !FAILED(factory->CreateSwapChainForComposition(commandQueue, &swapChainDesc1, nullptr, &swapChain1));
            },
            [&]() {
                initDummyWindow();
                return !FAILED(factory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc1, nullptr, nullptr, &swapChain1));
            },
            [&]() {
                return !FAILED(factory->CreateSwapChainForHwnd(commandQueue, GetDesktopWindow(), &swapChainDesc1, nullptr, nullptr, &swapChain1));
            },
        };

        bool anySucceeded = false;
        for (size_t i = 0; i < swapchainAttempts.size(); i++)
        {
            try
            {
                if (swapchainAttempts[i]())
                {
                    LOGF(INFO, "Created dummy swapchain via attempt {}.", i);
                    anySucceeded = true;
                    break;
                }
            }
            catch (const std::exception& e)
            {
                LOGF(FATAL, "Dummy swapchain attempt {} threw: {}", i, e.what());
            }
            catch (...)
            {
                LOGF(FATAL, "Dummy swapchain attempt {} threw an unknown exception.", i);
            }
        }

        auto cleanupDummyWindow = [&]() {
            if (hwnd)
                DestroyWindow(hwnd);
            if (wc.lpszClassName)
                UnregisterClassA(wc.lpszClassName, wc.hInstance);
        };

        if (!anySucceeded)
        {
            LOGF(FATAL, "Failed to create a dummy swapchain via any method.");
            cleanupDummyWindow();
            commandQueue->Release();
            factory->Release();
            device->Release();
            return false;
        }

        if (FAILED(swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain))))
        {
            LOGF(FATAL, "Failed to query IDXGISwapChain3 from the dummy swapchain.");
            cleanupDummyWindow();
            swapChain1->Release();
            commandQueue->Release();
            factory->Release();
            device->Release();
            return false;
        }

        m_CommandQueueOffset = 0;

        for (uint32_t i = 0; i < 512 * sizeof(void*); i += sizeof(void*))
        {
            auto base = reinterpret_cast<uintptr_t>(swapChain1) + i;
            if (IsBadReadPtr(reinterpret_cast<void*>(base), sizeof(void*)))
                break;

            if (*reinterpret_cast<ID3D12CommandQueue**>(base) == commandQueue)
            {
                m_CommandQueueOffset = i;
                LOGF(INFO, "Found command queue offset at {:#x}.", i);
                break;
            }
        }

        auto targetSwapchain = swapChain;

        if (m_CommandQueueOffset == 0)
		{
			bool shouldBreak = false;
			for (uint32_t base = 0; base < 512 * sizeof(void*) && !shouldBreak; base += sizeof(void*))
			{
				auto preScanBase = reinterpret_cast<uintptr_t>(swapChain1) + base;
				if (IsBadReadPtr(reinterpret_cast<void*>(preScanBase), sizeof(void*)))
					break;

				auto scanBase = *reinterpret_cast<uintptr_t*>(preScanBase);
				if (scanBase == 0 || IsBadReadPtr(reinterpret_cast<void*>(scanBase), sizeof(void*)))
					continue;

				for (uint32_t i = 0; i < 512 * sizeof(void*); i += sizeof(void*))
				{
					auto preData = scanBase + i;
					if (IsBadReadPtr(reinterpret_cast<void*>(preData), sizeof(void*)))
						break;

					if (*reinterpret_cast<ID3D12CommandQueue**>(preData) == commandQueue)
					{
						targetSwapchain = reinterpret_cast<IDXGISwapChain3*>(scanBase);
						m_CommandQueueOffset = i;
						shouldBreak = true;

						LOGF(INFO, "Found nested command queue offset at {:#x}.", i);
						break;
					}
				}
			}
		}

        if (m_CommandQueueOffset == 0)
        {
            LOGF(FATAL, "Failed to find the command queue offset.");
            cleanupDummyWindow();
            swapChain->Release();
            swapChain1->Release();
            commandQueue->Release();
            factory->Release();
            device->Release();
            return false;
        }

        m_SwapchainVtable = *reinterpret_cast<void***>(targetSwapchain);
        m_FactoryVtable = *reinterpret_cast<void***>(factory);
        InstallHooks();

        commandQueue->Release();
        swapChain1->Release();
        swapChain->Release();
        device->Release();
        factory->Release();
        cleanupDummyWindow();

        if (!m_Hooked)
            return false;

        LOGF(INFO, "D3D12 Hook initialized.");
        return true;
    }

    bool D3D12Hook::IsPlausibleCommandQueue(ID3D12CommandQueue* queue)
    {
        // Cheap sanity check for the primary (cached-offset) path: the object
        // and its vtable pointer must both live in committed, readable memory.
        // This catches garbage values like -1 without rejecting valid queues.
        if (!IsReadableMemory(queue))
            return false;

        void** vtable = *reinterpret_cast<void***>(queue);
        return IsReadableMemory(vtable);
    }

    bool D3D12Hook::ValidateCommandQueue(ID3D12CommandQueue* queue)
    {
        // Strict verification, used for candidates found by scanning: the
        // vtable is read-only data (.rdata is NOT executable), so only its
        // entries must point at executable code, then do a real QueryInterface.
        if (!IsPlausibleCommandQueue(queue))
            return false;

        void** vtable = *reinterpret_cast<void***>(queue);
        if (!IsExecutableMemory(vtable[0]))
            return false;

        __try
        {
            ID3D12CommandQueue* verified = nullptr;
            const HRESULT hr = queue->QueryInterface(__uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(&verified));
            if (FAILED(hr) || verified == nullptr)
                return false;

            const bool isDirect = verified->GetDesc().Type == D3D12_COMMAND_LIST_TYPE_DIRECT;
            verified->Release();
            return isDirect;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            return false;
        }
    }

    void D3D12Hook::RefreshCommandQueue(IDXGISwapChain3* swapChain)
    {
        if (!swapChain)
            return;

        // Fast path: already resolved for this exact swapchain object and still valid.
        if (m_ResolvedChain == swapChain && IsPlausibleCommandQueue(m_ResolvedQueue))
        {
            m_CommandQueue = m_ResolvedQueue;
            return;
        }

        m_ResolvedChain = nullptr;
        m_ResolvedQueue = nullptr;

        // Throttle: a failed resolution used to rescan the whole object on every
        // Present (hundreds of pointer checks plus QueryInterface calls per frame).
        // While nothing can be resolved, retry at most twice a second - but always
        // try immediately for a chain we have not failed on yet.
        const DWORD nowTicks = GetTickCount();
        if (swapChain == m_LastFailedChain && nowTicks < m_NextResolveTick)
            return;

        m_LastFailedChain = nullptr;
        m_NextResolveTick = 0;

        // Preferred: the queue captured in our CreateSwapChainForHwnd hook. The
        // presenter was created with this queue, so overlay draws submitted on it are
        // ordered with its presents - including with frame-generation present paths.
        if (m_CreationQueue && ValidateCommandQueue(m_CreationQueue))
        {
            m_CommandQueue = m_CreationQueue;
            m_ResolvedChain = swapChain;
            m_ResolvedQueue = m_CreationQueue;
            LOGF(INFO, "Command queue resolved from creation-time capture {:#x}.", reinterpret_cast<uintptr_t>(m_CreationQueue));
            return;
        }

        // Second: the offset discovered during initialisation. Strictly verified so a
        // stale/garbage slot can never be adopted silently (it used to be).
        if (m_CommandQueueOffset != 0)
        {
            auto candidate = *reinterpret_cast<ID3D12CommandQueue**>(reinterpret_cast<uintptr_t>(swapChain) + m_CommandQueueOffset);
            if (ValidateCommandQueue(candidate))
            {
                m_CommandQueue = candidate;
                m_ResolvedChain = swapChain;
                m_ResolvedQueue = candidate;
                LOGF(INFO, "Command queue verified at cached offset {:#x}.", m_CommandQueueOffset);
                return;
            }
        }

        // The chain was recreated and the cached offset no longer points at a
        // valid queue. Rescan the live object for a candidate that verifies.
        for (uint32_t i = 0; i < 0x1000; i += sizeof(void*))
        {
            auto base = reinterpret_cast<uintptr_t>(swapChain) + i;
            if (!IsReadableMemory(reinterpret_cast<void*>(base)))
                break;

            auto candidate = *reinterpret_cast<ID3D12CommandQueue**>(base);
            if (ValidateCommandQueue(candidate))
            {
                m_CommandQueueOffset = i;
                m_CommandQueue = candidate;
                m_ResolvedChain = swapChain;
                m_ResolvedQueue = candidate;
                LOGF(INFO, "Re-resolved command queue after swapchain recreation (offset {:#x}).", i);
                return;
            }
        }

        // One level of nesting (matches the layout found during init).
        for (uint32_t base = 0; base < 0x1000; base += sizeof(void*))
        {
            auto wrapperPtr = reinterpret_cast<uintptr_t>(swapChain) + base;
            if (!IsReadableMemory(reinterpret_cast<void*>(wrapperPtr)))
                break;

            auto wrapper = *reinterpret_cast<uintptr_t*>(wrapperPtr);
            if (!wrapper || !IsReadableMemory(reinterpret_cast<void*>(wrapper)))
                continue;

            for (uint32_t i = 0; i < 0x1000; i += sizeof(void*))
            {
                auto candidatePtr = wrapper + i;
                if (!IsReadableMemory(reinterpret_cast<void*>(candidatePtr)))
                    break;

                auto candidate = *reinterpret_cast<ID3D12CommandQueue**>(candidatePtr);
                if (ValidateCommandQueue(candidate))
                {
                    m_CommandQueue = candidate;
                    m_ResolvedChain = swapChain;
                    m_ResolvedQueue = candidate;
                    LOGF(INFO, "Re-resolved nested command queue after swapchain recreation (nested offset {:#x}).", i);
                    return;
                }
            }
        }

        // Last resort: accept the cached slot on the cheap plausibility check only.
        // Keeps rendering alive on layouts where strict verification cannot run, but
        // logged loudly: an unverified queue is exactly how the overlay turns
        // invisible (draws execute, but not on the queue the presents are ordered on).
        if (m_CommandQueueOffset != 0)
        {
            auto candidate = *reinterpret_cast<ID3D12CommandQueue**>(reinterpret_cast<uintptr_t>(swapChain) + m_CommandQueueOffset);
            if (IsPlausibleCommandQueue(candidate))
            {
                m_CommandQueue = candidate;
                m_ResolvedChain = swapChain;
                m_ResolvedQueue = candidate;
                LOGF(WARNING, "Command queue {:#x} accepted on plausibility only (cached offset {:#x}); strict verification failed.", reinterpret_cast<uintptr_t>(candidate), m_CommandQueueOffset);
                return;
            }
        }

        if (!IsPlausibleCommandQueue(m_CommandQueue))
            m_CommandQueue = nullptr;

        // Nothing resolved: schedule the next attempt instead of rescanning on
        // every single Present.
        m_LastFailedChain = swapChain;
        m_NextResolveTick = nowTicks + 500;

        static DWORD s_LastWarn = 0;
        if (nowTicks - s_LastWarn >= 5000)
        {
            s_LastWarn = nowTicks;
            LOGF(WARNING, "No valid command queue for the current swapchain; menu rendering is paused until one can be resolved.");
        }
    }

    bool D3D12Hook::DestroyImpl(bool unloading)
    {
        std::scoped_lock hookLock{m_HookMutex};

        if (!m_Hooked)
            return true;

        m_PresentPatch.Uninstall();
        m_SwapchainVmt.reset();

		if (unloading)
			m_CreateSwapchainPatch.Uninstall();

        // The captured queue holds a reference of its own; drop it so an unload does
        // not keep the command queue (and its device) alive forever.
        if (m_CreationQueue)
        {
            m_CreationQueue->Release();
            m_CreationQueue = nullptr;
        }

        m_CommandQueue = nullptr;
        m_ResolvedChain = nullptr;
        m_ResolvedQueue = nullptr;
        m_SwapChain = nullptr;
        m_Device = nullptr;
        m_Window = nullptr;

        m_Hooked = false;
        m_IsPhase1 = true;

        return true;
    }

    void D3D12Hook::InstallHooks()
    {
        // Never re-install while the per-instance swapchain hook is active:
        // the shared-vtable phase-1 patch is retired after the first Present,
        // and re-adding it on top of another overlay's hook is what used to
        // create a recursive hook chain (Steam overlay <-> our Present).
        if (m_Hooked && !m_IsPhase1 && m_SwapchainVmt)
        {
            LOGF(INFO, "D3D12 hooks already active; skipping re-install to keep the hook chain intact.");
            return;
        }

        m_PresentPatch.Uninstall();
        m_SwapchainVmt.reset();

        m_IsPhase1 = true;

        if (!m_PresentPatch.Install(m_SwapchainVtable, 8, reinterpret_cast<void*>(&D3D12Hook::Present)))
        {
            LOGF(FATAL, "Failed to install the phase 1 Present vtable patch.");
            m_Hooked = false;
            return;
        }

        if (!m_CreateSwapchainPatch.Installed)
        {
            if (!m_CreateSwapchainPatch.Install(m_FactoryVtable, 15, reinterpret_cast<void*>(&D3D12Hook::CreateSwapchain)))
                LOGF(FATAL, "Failed to install the CreateSwapChain patch.");
        }

        m_Hooked = true;
    }

    HRESULT D3D12Hook::Present(IDXGISwapChain3* swapChain, UINT syncInterval, UINT flags, void* r9)
    {
        // Last line of defence: if our Present is re-entered through another
        // overlay's hook chain, break the cycle instead of blowing the 1 MiB
        // thread stack (crash ~1 minute after inject without this).
        PresentRecursionGuard recursionGuard{};
        if (PresentRecursionGuard::s_Depth > kMaxPresentRecursionDepth)
        {
            static thread_local DWORD s_LastRecursionLog = 0;
            const DWORD now = GetTickCount();
            if (now - s_LastRecursionLog >= 5000)
            {
                s_LastRecursionLog = now;
                LOGF(WARNING, "Present hook recursion depth exceeded ({}); breaking the chain to protect the thread stack.", PresentRecursionGuard::s_Depth);
            }
            return S_OK;
        }

        std::scoped_lock hookLock{GetInstance().m_HookMutex};

        auto& self = GetInstance();

        auto presentFn = self.m_IsPhase1
            ? self.m_PresentPatch.Original<decltype(&D3D12Hook::Present)>()
            : (self.m_SwapchainVmt ? self.m_SwapchainVmt->Original<decltype(&D3D12Hook::Present)>(8) : nullptr);

        if (!presentFn)
        {
            LOGF(WARNING, "Present hook has no valid original function to forward to; dropping the frame.");
            return S_OK;
        }

        HWND swapchainWnd = nullptr;
        {
            DXGI_SWAP_CHAIN_DESC swapchainDesc{};
            if (SUCCEEDED(swapChain->GetDesc(&swapchainDesc)))
                swapchainWnd = swapchainDesc.OutputWindow;
        }

        if (!self.m_IsPhase1 && swapChain != self.m_SwapChain)
            return presentFn(swapChain, syncInterval, flags, r9);

        if (self.m_IsPhase1) // caught the real present, switch to per instance vmt hook
        {
            // Never migrate onto an overlay swapchain - hook only the game chain.
            if (IsForeignSwapchainForMenu(swapChain, nullptr))
            {
                return presentFn(swapChain, syncInterval, flags, r9);
            }

            self.m_PresentPatch.Uninstall();

            self.m_SwapchainVmt = std::make_unique<VMTHook>("D3D12SwapChain", swapChain, 64); // should be enough
            self.m_SwapchainVmt->Hook(8, reinterpret_cast<void*>(&D3D12Hook::Present));
            self.m_SwapchainVmt->Hook(13, reinterpret_cast<void*>(&D3D12Hook::ResizeBuffers));
            self.m_SwapchainVmt->Enable();

            self.m_IsPhase1 = false;
            presentFn = self.m_SwapchainVmt->Original<decltype(&D3D12Hook::Present)>(8);
            LOGF(INFO, "Migrated to a per-instance swapchain hook (window {:#x}); the phase-1 shared patch is retired.", reinterpret_cast<uintptr_t>(swapchainWnd));
        }

        self.m_Window = swapchainWnd;
        self.m_SwapChain = swapChain;

        {
            Microsoft::WRL::ComPtr<ID3D12Device4> tempDevice{};
            swapChain->GetDevice(IID_PPV_ARGS(&tempDevice));
            self.m_Device = tempDevice.Get();
        }

        // Re-resolve and verify the command queue for the current chain. After a
        // swapchain recreation the cached offset can hold garbage (observed as
        // 0xFFFFFFFFFFFFFFFF); using it crashed D3D12 during texture uploads.
        self.RefreshCommandQueue(swapChain);

        // Without a verified queue we cannot safely render (or even initialise
        // the renderer's ImGui backend); just forward the frame.
        if (!self.m_CommandQueue)
            return presentFn(swapChain, syncInterval, flags, r9);

        if (self.m_OnPresent)
            self.m_OnPresent();

        return presentFn(swapChain, syncInterval, flags, r9);
    }

    HRESULT D3D12Hook::ResizeBuffers(IDXGISwapChain3* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags)
    {
        std::scoped_lock hookLock{GetInstance().m_HookMutex};

        auto& self = GetInstance();

        if (!self.m_SwapchainVmt)
        {
            // Should never happen: the hook lives together with the per-instance
            // VMT. Fail the call instead of recursing into our own hook.
            LOGF(WARNING, "ResizeBuffers hook fired without an active swapchain hook.");
            return E_FAIL;
        }

        LOGF(INFO, "ResizeBuffers called: buffers {}, {}x{}, format {}.", bufferCount, width, height, static_cast<int>(newFormat));

        auto resizeBuffersFn = self.m_SwapchainVmt->Original<decltype(&D3D12Hook::ResizeBuffers)>(13);

        if (self.m_OnResizeBuffers)
            self.m_OnResizeBuffers();

        return resizeBuffersFn(swapChain, bufferCount, width, height, newFormat, swapChainFlags);
    }

    HRESULT D3D12Hook::CreateSwapchain(IDXGIFactory4* factory, IUnknown* device, HWND hwnd, const DXGI_SWAP_CHAIN_DESC1* desc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain** swapChain)
    {
        auto& self = GetInstance();

        auto createSwapChainFn = self.m_CreateSwapchainPatch.Original<decltype(&D3D12Hook::CreateSwapchain)>();
        if (!createSwapChainFn)
            return E_FAIL;

        if (g_InsideD3d12Hook)
            return createSwapChainFn(factory, device, hwnd, desc, pFullscreenDesc, pRestrictToOutput, swapChain);

        // Diagnostics: record who is asking for a new swapchain and with what parameters.
        const void* callerAddress = _ReturnAddress();
        char callerModuleName[MAX_PATH]{};
        DWORD callerModuleRva = 0;
        {
            HMODULE callerModule = nullptr;
            if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(callerAddress), &callerModule) && callerModule)
            {
                GetModuleFileNameA(callerModule, callerModuleName, MAX_PATH);
                callerModuleRva = static_cast<DWORD>(reinterpret_cast<uintptr_t>(callerAddress) - reinterpret_cast<uintptr_t>(callerModule));
            }
        }

        const auto* desc1Info = reinterpret_cast<const DXGI_SWAP_CHAIN_DESC1*>(desc);
        LOGF(INFO, "CreateSwapChain called (tid {}): hwnd {:#x}, menu window {:#x}, caller {}+{:#x}, buffers {}, {}x{}, format {}, alpha {}, flags {:#x}.",
            GetCurrentThreadId(),
            reinterpret_cast<uintptr_t>(hwnd),
            reinterpret_cast<uintptr_t>(GetInstance().m_Window),
            callerModuleName[0] ? callerModuleName : "unknown",
            callerModuleRva,
            desc1Info ? desc1Info->BufferCount : 0u,
            desc1Info ? desc1Info->Width : 0u,
            desc1Info ? desc1Info->Height : 0u,
            desc1Info ? static_cast<int>(desc1Info->Format) : -1,
            desc1Info ? static_cast<int>(desc1Info->AlphaMode) : -1,
            desc1Info ? desc1Info->Flags : 0u);

        // Run the real creation first and hook afterwards. The old
        // implementation destroyed + re-installed our hooks from inside this
        // callback; once an external overlay (Steam/NVIDIA) held hooks in the
        // same vtable slots that produced a cyclic hook chain and a stack
        // overflow crash ~1 minute after injection. Root fix: never re-tangle
        // existing hooks here, only attach to the freshly created swapchain.

        // Pre-pass order matters:
        // 1) retire the per-instance hook on the OLD chain while it is still
        //    alive - the VMTHook destructor writes the original vtable pointer
        //    back into the object, which would corrupt the heap if the object
        //    is already gone;
        // 2) then drop our backbuffer references (this is what lets the game
        //    replace the chain at all; without it DXGI fails the creation with
        //    E_ACCESSDENIED / 0x80070005 and the game raises ERR_GFX_D3D_DEFERRED_MEM).
        if (self.m_SwapchainVmt)
        {
            LOGF(INFO, "CreateSwapchain pre-pass: retiring hook on the old chain {:#x}.", reinterpret_cast<uintptr_t>(self.m_SwapChain));
            self.m_SwapchainVmt.reset();
        }

        if (self.m_OnResizeBuffers)
        {
            LOGF(INFO, "CreateSwapchain pre-pass: releasing render targets (old chain {:#x}) before creation.", reinterpret_cast<uintptr_t>(self.m_SwapChain));
            self.m_OnResizeBuffers();
        }
        const HRESULT result = createSwapChainFn(factory, device, hwnd, desc, pFullscreenDesc, pRestrictToOutput, swapChain);
        if (FAILED(result))
        {
            LOGF(WARNING, "CreateSwapChainForHwnd FAILED with {:#x} (tid {}); restoring hook on the current chain.", static_cast<unsigned>(result), GetCurrentThreadId());

            // The game keeps using its current chain when creation fails; put
            // our per-instance hook back so the menu keeps rendering.
            if (self.m_SwapChain && !self.m_SwapchainVmt)
            {
                self.m_SwapchainVmt = std::make_unique<VMTHook>("D3D12SwapChain", self.m_SwapChain, 64);
                self.m_SwapchainVmt->Hook(8, reinterpret_cast<void*>(&D3D12Hook::Present));
                self.m_SwapchainVmt->Hook(13, reinterpret_cast<void*>(&D3D12Hook::ResizeBuffers));
                self.m_SwapchainVmt->Enable();
            }

            return result;
        }

        if (swapChain == nullptr || *swapChain == nullptr)
            return result;

        LOGF(INFO, "CreateSwapChainForHwnd succeeded ({:#x}): new swapchain {:#x} (tid {}).", static_cast<unsigned>(result), reinterpret_cast<uintptr_t>(*swapChain), GetCurrentThreadId());

        if (!self.m_Hooked)
            return result;

        std::scoped_lock hookLock{self.m_HookMutex};

        auto* newSwapchain = *swapChain;

        // Only hook the game chain; ignore overlay/external swapchains.
        if (IsForeignSwapchainForMenu(newSwapchain, self.m_Window))
        {
            LOGF(INFO, "Ignored foreign swapchain created by an overlay/external component.");
            return result;
        }

        // Capture the queue the caller creates the swapchain with. This is the
        // authoritative queue for present ordering; the offset-based scan below can
        // land on a different (internal/proxy) queue after a swapchain recreation,
        // which silently renders the overlay invisibly instead of crashing.
        {
            std::scoped_lock hookLock{self.m_HookMutex};

            ID3D12CommandQueue* creationQueue = nullptr;
            if (device && SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&creationQueue))) && creationQueue)
            {
                if (creationQueue->GetDesc().Type == D3D12_COMMAND_LIST_TYPE_DIRECT)
                {
                    if (creationQueue != self.m_CreationQueue)
                    {
                        if (self.m_CreationQueue)
                            self.m_CreationQueue->Release();

                        self.m_CreationQueue = creationQueue; // QueryInterface handed us a reference
                        LOGF(INFO, "Captured creation-time command queue {:#x} (scan result {:#x}).",
                            reinterpret_cast<uintptr_t>(creationQueue), reinterpret_cast<uintptr_t>(self.m_CommandQueue));
                    }
                    else
                    {
                        creationQueue->Release();
                    }
                }
                else
                {
                    creationQueue->Release();
                }
            }
        }

        if (self.m_IsPhase1)
        {
            // The shared-vtable Present patch is still armed and will migrate
            // to a per-instance hook on the first Present of this swapchain.
            LOGF(INFO, "Swapchain created while phase-1 hook is active; the shared Present patch will pick it up.");
            return result;
        }

        {
            Microsoft::WRL::ComPtr<ID3D12Device> newDevice{};
            newSwapchain->GetDevice(IID_PPV_ARGS(&newDevice));
            LOGF(INFO, "Re-hooking new swapchain (per-instance, overlay-safe): old {:#x} -> new {:#x}, device {:#x} (previous {:#x}).",
                reinterpret_cast<uintptr_t>(self.m_SwapChain),
                reinterpret_cast<uintptr_t>(newSwapchain),
                reinterpret_cast<uintptr_t>(newDevice.Get()),
                reinterpret_cast<uintptr_t>(self.m_Device));
        }

        // The renderer has to drop everything bound to the old swapchain.
        if (self.m_OnDeviceReset)
            self.m_OnDeviceReset();

        // Clone the new object vtable and patch our detours into the clone
        // only - the shared class vtable is never touched here, so a foreign
        // hook chain (Steam overlay, NVIDIA) stays intact and cannot loop
        // back into us.
        self.m_SwapchainVmt = std::make_unique<VMTHook>("D3D12SwapChain", newSwapchain, 64); // should be enough
        self.m_SwapchainVmt->Hook(8, reinterpret_cast<void*>(&D3D12Hook::Present));
        self.m_SwapchainVmt->Hook(13, reinterpret_cast<void*>(&D3D12Hook::ResizeBuffers));
        self.m_SwapchainVmt->Enable();

        self.m_SwapChain = reinterpret_cast<IDXGISwapChain3*>(newSwapchain);

        return result;
    }
}
