#include "Renderer.hpp"
#include "D3D12Hook.hpp"
#include "core/frontend/manager/styles/Themes.hpp"
#include "game/frontend/Menu.hpp"
#include <backends/imgui_impl_dx12.h>
#include <backends/imgui_impl_win32.h>
#include <imgui.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace YimMenu
{
    void Renderer::InitImpl()
    {
        D3D12Hook::OnPresent([this] {
            if (!m_Initialized)
            {
                if (!InitInternal())
                    return;
            }

            Render();
        });

        D3D12Hook::OnResizeBuffers([this] {
            Resize();
        });

        D3D12Hook::OnDeviceReset([this] {
            Destroy();
        });
    }

    bool Renderer::InitInternal()
    {
        auto device = D3D12Hook::GetDevice();
        auto swapChain = D3D12Hook::GetSwapChain();
        auto queue = D3D12Hook::GetCommandQueue();
        auto window = D3D12Hook::GetWindow();
        if (!device || !swapChain || !queue || !window || !D3D12Hook::IsPlausibleCommandQueue(queue))
        {
			// Rate-limited diagnostics: a silent failure here used to turn into a
			// 30 second watchdog timeout with no clue in the log.
			static DWORD s_LastInitWarn = 0;
			const DWORD now = GetTickCount();
			if (now - s_LastInitWarn >= 5000)
			{
				s_LastInitWarn = now;
				LOGF(WARNING, "Renderer init waiting for valid hooks data (device {}, swapchain {}, queue {}, window {}).",
				    device ? 1 : 0, swapChain ? 1 : 0, queue ? 1 : 0, window ? 1 : 0);
			}
			return false;
        }

        DXGI_SWAP_CHAIN_DESC desc{};
        swapChain->GetDesc(&desc);
        UINT bufferCount = desc.BufferCount > 0 ? desc.BufferCount : 3;

        if (!CreateDeviceObjects(device, queue, bufferCount))
        {
            LOGF(FATAL, "Failed to create device objects.");
            return false;
        }

        CreateRenderTargets(swapChain);

        m_Window = window;
        m_OriginalWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(m_Window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Renderer::WndProc)));

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init(m_Window);

        ImGui_ImplDX12_InitInfo initInfo{};
        initInfo.Device = device;
        initInfo.CommandQueue = queue;
        initInfo.NumFramesInFlight = static_cast<int>(bufferCount);
        // Use the swapchain's actual format: the pipeline state must be built for the
        // RTV format we bind (GTA5 Enhanced uses B8G8R8A8_UNORM on modern builds).
        initInfo.RTVFormat = (desc.BufferDesc.Format != DXGI_FORMAT_UNKNOWN) ? desc.BufferDesc.Format : DXGI_FORMAT_R8G8B8A8_UNORM;
        initInfo.DSVFormat = DXGI_FORMAT_UNKNOWN;
        initInfo.SrvDescriptorHeap = m_SrvHeap.Get();
        initInfo.UserData = &m_SrvAllocator;
        initInfo.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* outCpu, D3D12_GPU_DESCRIPTOR_HANDLE* outGpu) {
            static_cast<SrvHeapAllocator*>(info->UserData)->Alloc(outCpu, outGpu);
        };
        initInfo.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu) {
            static_cast<SrvHeapAllocator*>(info->UserData)->Free(cpu, gpu);
        };

        if (!ImGui_ImplDX12_Init(&initInfo))
        {
			LOGF(FATAL, "ImGui_ImplDX12_Init failed.");

			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();

			if (m_Window && m_OriginalWndProc)
				SetWindowLongPtrA(m_Window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_OriginalWndProc));

			m_Window = nullptr;
			m_OriginalWndProc = nullptr;

			return false;
        }

        Menu::SetupFonts();
		SetupStyle();
		GUI::ToggleMouse();

        m_Initialized = true;
        LOGF(INFO, "Renderer initialized (format {}, buffers {}).", static_cast<int>(desc.BufferDesc.Format), bufferCount);
        return true;
    }

    void Renderer::DestroyImpl()
    {
        if (!m_Initialized && !m_DeviceObjectsValid)
            return;

        ReleaseRenderTargets();

        if (m_Initialized)
        {
            ImGui_ImplDX12_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
        }

        if (m_Window && m_OriginalWndProc)
            SetWindowLongPtrA(m_Window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_OriginalWndProc));

        if (m_FenceEvent)
        {
            CloseHandle(m_FenceEvent);
            m_FenceEvent = nullptr;
        }

        m_FrameContexts.clear();
        m_BackBuffers.clear();
        m_RtvHandles.clear();
        m_CommandList.Reset();
        m_Fence.Reset();
        m_RtvHeap.Reset();
        m_SrvHeap.Reset();

        m_Initialized = false;
        m_DeviceObjectsValid = false;
        m_Window = nullptr;
        m_OriginalWndProc = nullptr;
    }

    void Renderer::Render()
    {
		auto device = D3D12Hook::GetDevice();
		auto swapChain = D3D12Hook::GetSwapChain();
		auto queue = D3D12Hook::GetCommandQueue();
        if (!swapChain || !device || !queue)
            return;

        if (!m_CommandList || !m_Fence || !m_FenceEvent || !m_SrvHeap)
            return;

        // A font/device-object rebuild must happen before any per-frame state
        // (command list, render targets, barriers) is touched. Doing it later
        // made the frame record barriers against released backbuffers.
        if (m_FontsUpdated)
        {
            DXGI_SWAP_CHAIN_DESC desc{};
            swapChain->GetDesc(&desc);
            UINT bufferCount = desc.BufferCount > 0 ? desc.BufferCount : 3;

            InvalidateDeviceObjects();
            if (!CreateDeviceObjects(device, queue, bufferCount))
                return;
            ImGui_ImplDX12_CreateDeviceObjects();
            CreateRenderTargets(swapChain);
            m_FontsUpdated = false;
        }

        // if a resize happened, our cached backbuffer resources are stale, refresh them
        ComPtr<ID3D12Resource> currentAtIndex{};
        auto index = swapChain->GetCurrentBackBufferIndex();

        // The swapchain may have been recreated with a different buffer count.
        if (index >= m_BackBuffers.size() || index >= m_FrameContexts.size() || index >= m_RtvHandles.size())
        {
            DXGI_SWAP_CHAIN_DESC desc{};
            swapChain->GetDesc(&desc);
            UINT bufferCount = desc.BufferCount > 0 ? desc.BufferCount : static_cast<UINT>(index + 1);

            InvalidateDeviceObjects();
            if (!CreateDeviceObjects(device, queue, bufferCount))
                return;
            ImGui_ImplDX12_CreateDeviceObjects();
        }

        // Hard guard: if the vectors still do not cover this index (pathological
        // swapchain state), skip the frame instead of reading out of bounds.
        if (index >= m_BackBuffers.size() || index >= m_FrameContexts.size() || index >= m_RtvHandles.size())
            return;

        if (index >= m_BackBuffers.size() || m_BackBuffers[index] == nullptr)
        {
            CreateRenderTargets(swapChain);
        }
        else
        {
            swapChain->GetBuffer(index, IID_PPV_ARGS(&currentAtIndex));
            if (currentAtIndex.Get() != m_BackBuffers[index].Get())
                CreateRenderTargets(swapChain);
        }

        // Rate-limited diagnostics: records which swapchain/backbuffer/queue the frames
        // are actually rendered with, so an invisible overlay can be traced from the log.
        {
            static DWORD s_LastDiag = 0;
            const DWORD nowTicks = GetTickCount();
            if (nowTicks - s_LastDiag >= 10000)
            {
                s_LastDiag = nowTicks;
                DXGI_SWAP_CHAIN_DESC diagDesc{};
                swapChain->GetDesc(&diagDesc);
                LOGF(INFO, "Render diag: swapchain {:#x} format {} buffers {} index {} backbuffer {:#x} queue {:#x} fence {}.",
                    reinterpret_cast<uintptr_t>(swapChain),
                    static_cast<int>(diagDesc.BufferDesc.Format),
                    diagDesc.BufferCount,
                    index,
                    reinterpret_cast<uintptr_t>(m_BackBuffers[index].Get()),
                    reinterpret_cast<uintptr_t>(queue),
                    m_FenceValue);
            }
        }

        auto& frame = m_FrameContexts[index];
        if (!frame.Allocator)
            return;

        // wait for the GPU to be done with this frame context's allocator before reusing it
        if (m_Fence->GetCompletedValue() < frame.FenceValue)
        {
            m_Fence->SetEventOnCompletion(frame.FenceValue, m_FenceEvent);
            WaitForSingleObject(m_FenceEvent, INFINITE);
        }

        frame.Allocator->Reset();
        m_CommandList->Reset(frame.Allocator.Get(), nullptr);

        if (m_SafeToRender)
        {
			D3D12_RESOURCE_BARRIER toRt{};
			toRt.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			toRt.Transition.pResource = m_BackBuffers[index].Get();
			toRt.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			toRt.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			toRt.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			m_CommandList->ResourceBarrier(1, &toRt);

			m_CommandList->OMSetRenderTargets(1, &m_RtvHandles[index], FALSE, nullptr);
			ID3D12DescriptorHeap* heaps[] = {m_SrvHeap.Get()};
			m_CommandList->SetDescriptorHeaps(1, heaps);


			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			for (const auto& callback : m_RendererCallbacks | std::views::values)
				callback();

			ImGui::Render();
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_CommandList.Get());

			D3D12_RESOURCE_BARRIER toPresent{};
			toPresent.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			toPresent.Transition.pResource = m_BackBuffers[index].Get();
			toPresent.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			toPresent.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			toPresent.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			m_CommandList->ResourceBarrier(1, &toPresent);

			m_CommandList->Close();

			ID3D12CommandList* lists[] = {m_CommandList.Get()};
			D3D12Hook::GetCommandQueue()->ExecuteCommandLists(1, lists);

			++m_FenceValue;
			D3D12Hook::GetCommandQueue()->Signal(m_Fence.Get(), m_FenceValue);
			frame.FenceValue = m_FenceValue;
        }
		else
		{
			m_CommandList->Close();
		}
    }

    void Renderer::Resize()
    {
        if (!m_Initialized)
            return;

        ReleaseRenderTargets();
    }

    bool Renderer::CreateDeviceObjects(ID3D12Device* device, ID3D12CommandQueue* queue, UINT bufferCount)
    {
        m_BufferCount = bufferCount;

        D3D12_DESCRIPTOR_HEAP_DESC rtvDesc{};
        rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvDesc.NumDescriptors = bufferCount;
        rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        if (FAILED(device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&m_RtvHeap))))
            return false;

        D3D12_DESCRIPTOR_HEAP_DESC srvDesc{};
        srvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvDesc.NumDescriptors = 64;
        srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        if (FAILED(device->CreateDescriptorHeap(&srvDesc, IID_PPV_ARGS(&m_SrvHeap))))
            return false;

        m_SrvAllocator.Init(device, m_SrvHeap.Get());

        auto rtvIncrement = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        auto rtvStart = m_RtvHeap->GetCPUDescriptorHandleForHeapStart();

        m_FrameContexts.resize(bufferCount);
        m_RtvHandles.resize(bufferCount);
        m_BackBuffers.resize(bufferCount);

        for (UINT i = 0; i < bufferCount; i++)
        {
            if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_FrameContexts[i].Allocator))))
                return false;

            m_RtvHandles[i] = {rtvStart.ptr + static_cast<SIZE_T>(i) * rtvIncrement};
        }

        if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_FrameContexts[0].Allocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList))))
            return false;

        m_CommandList->Close();

        if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence))))
            return false;

        // Fresh fence: per-frame fence values must be reset with it, otherwise a
        // stale (larger) frame value makes the next wait block on a value that
        // will never be signalled and the game freezes.
        m_FenceValue = 0;
        for (auto& frameContext : m_FrameContexts)
            frameContext.FenceValue = 0;

        // The rebuild path (InvalidateDeviceObjects -> CreateDeviceObjects) used to
        // overwrite this handle without closing the previous one.
        if (m_FenceEvent)
        {
            CloseHandle(m_FenceEvent);
            m_FenceEvent = nullptr;
        }

        m_FenceEvent = CreateEventA(nullptr, FALSE, FALSE, nullptr);
        if (!m_FenceEvent)
            return false;

        m_DeviceObjectsValid = true;
        return true;
    }

    void Renderer::CreateRenderTargets(IDXGISwapChain3* swapChain)
    {
        ComPtr<ID3D12Device> owningDevice{};
        swapChain->GetDevice(IID_PPV_ARGS(&owningDevice));

        for (UINT i = 0; i < m_BufferCount; ++i)
        {
            if (FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&m_BackBuffers[i]))))
                continue;

            owningDevice->CreateRenderTargetView(m_BackBuffers[i].Get(), nullptr, m_RtvHandles[i]);
        }
    }

    void Renderer::ReleaseRenderTargets()
    {
        if (m_Fence && m_FenceEvent)
        {
            auto value = ++m_FenceValue;
            if (SUCCEEDED(m_Fence->Signal(value)) && m_Fence->GetCompletedValue() < value)
            {
                m_Fence->SetEventOnCompletion(value, m_FenceEvent);
                WaitForSingleObject(m_FenceEvent, 2000);
            }
        }

        for (auto& bb : m_BackBuffers)
            bb.Reset();
    }

    LRESULT Renderer::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if (GetInstance().m_Initialized)
        {
            for (const auto& callback : GetInstance().m_WindowProcedureCallbacks)
                callback(hwnd, msg, wparam, lparam);

            ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
        }

        if (GetInstance().m_OriginalWndProc)
            return CallWindowProcA(GetInstance().m_OriginalWndProc, hwnd, msg, wparam, lparam);

        return DefWindowProcA(hwnd, msg, wparam, lparam);
    }

    void Renderer::InvalidateDeviceObjects()
    {
        if (!m_DeviceObjectsValid)
            return;

        ReleaseRenderTargets();

        if (m_Initialized)
            ImGui_ImplDX12_InvalidateDeviceObjects();

        m_DeviceObjectsValid = false;
    }

    void Renderer::SrvHeapAllocator::Init(ID3D12Device* device, ID3D12DescriptorHeap* srvHeap)
    {
        Heap = srvHeap;
        DescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        HeapStartCpu = Heap->GetCPUDescriptorHandleForHeapStart();
        HeapStartGpu = Heap->GetGPUDescriptorHandleForHeapStart();

        FreeIndices.clear();
        FreeIndices.reserve(64);
        for (int i = 64 - 1; i >= 0; --i)
            FreeIndices.push_back(i);
    }

    void Renderer::SrvHeapAllocator::Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* outCpu, D3D12_GPU_DESCRIPTOR_HANDLE* outGpu)
    {
        if (FreeIndices.empty())
        {
            LOGF(FATAL, "SRV heap exhausted.");
            *outCpu = HeapStartCpu;
            *outGpu = HeapStartGpu;
            return;
        }

        int index = FreeIndices.back();
        FreeIndices.pop_back();

        outCpu->ptr = HeapStartCpu.ptr + static_cast<SIZE_T>(index) * DescriptorSize;
        outGpu->ptr = HeapStartGpu.ptr + static_cast<UINT64>(index) * DescriptorSize;
    }

    void Renderer::SrvHeapAllocator::Free(D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu)
    {
        int index = static_cast<int>((cpu.ptr - HeapStartCpu.ptr) / DescriptorSize);
        FreeIndices.push_back(index);
    }
}
