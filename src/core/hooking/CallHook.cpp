#include "CallHook.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace
{
	constexpr std::size_t CallHookMemorySize = 1024;
	constexpr std::size_t JumpSequenceSize = 12;
	constexpr std::uintptr_t MaximumRel32BackwardDistance = 2147483643ULL;
	constexpr std::uintptr_t MaximumRel32ForwardDistance = 2147483652ULL;

	bool FitsRel32(void* location, void* destination)
	{
		const auto displacement = static_cast<std::int64_t>(reinterpret_cast<std::uintptr_t>(destination))
		    - static_cast<std::int64_t>(reinterpret_cast<std::uintptr_t>(location)) - 5;
		return displacement >= std::numeric_limits<std::int32_t>::min() && displacement <= std::numeric_limits<std::int32_t>::max();
	}

	void* AllocateNear(void* location)
	{
		SYSTEM_INFO systemInfo{};
		GetSystemInfo(&systemInfo);

		const auto origin = reinterpret_cast<std::uintptr_t>(location);
		const auto minimumApplicationAddress = reinterpret_cast<std::uintptr_t>(systemInfo.lpMinimumApplicationAddress);
		const auto maximumApplicationAddress = reinterpret_cast<std::uintptr_t>(systemInfo.lpMaximumApplicationAddress);
		const auto lowerCandidate = origin > MaximumRel32BackwardDistance ? origin - MaximumRel32BackwardDistance : std::uintptr_t{0};
		const auto lowerBound = std::max(minimumApplicationAddress, lowerCandidate);
		const auto upperCandidate = origin <= std::numeric_limits<std::uintptr_t>::max() - MaximumRel32ForwardDistance
		    ? origin + MaximumRel32ForwardDistance
		    : std::numeric_limits<std::uintptr_t>::max();
		const auto upperBound = std::min(maximumApplicationAddress, upperCandidate - static_cast<std::uintptr_t>(CallHookMemorySize));
		const auto granularity = static_cast<std::uintptr_t>(systemInfo.dwAllocationGranularity);

		auto cursor = lowerBound;
		while (cursor < upperBound)
		{
			MEMORY_BASIC_INFORMATION memoryInfo{};
			if (VirtualQuery(reinterpret_cast<void*>(cursor), &memoryInfo, sizeof(memoryInfo)) != sizeof(memoryInfo))
				break;

			const auto regionStart = std::max(cursor, reinterpret_cast<std::uintptr_t>(memoryInfo.BaseAddress));
			const auto regionEnd = std::min(upperBound, reinterpret_cast<std::uintptr_t>(memoryInfo.BaseAddress) + memoryInfo.RegionSize);
			if (memoryInfo.State == MEM_FREE)
			{
				const auto candidate = (regionStart + granularity - 1) & ~(granularity - 1);
				if (candidate <= regionEnd && CallHookMemorySize <= regionEnd - candidate)
				{
					if (auto memory = VirtualAlloc(reinterpret_cast<void*>(candidate), CallHookMemorySize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))
					{
						if (FitsRel32(location, memory))
							return memory;
						VirtualFree(memory, 0, MEM_RELEASE);
					}
				}
			}

			if (regionEnd <= cursor)
				break;
			cursor = regionEnd;
		}

		return nullptr;
	}
}

namespace YimMenu
{
	CallHookMemory::CallHookMemory() :
	    m_Memory(nullptr),
	    m_Offset(0)
	{
	}

	void* CallHookMemory::AllocateJumpSequenceImpl(void* func, void* location)
	{
		if (!m_Memory)
		{
			m_Memory = AllocateNear(location);
			if (!m_Memory)
			{
				LOGF(FATAL, "CallHookMemory: Failed to allocate jump sequence memory within rel32 range");
				throw std::runtime_error("Failed to allocate call hook memory within rel32 range");
			}
		}

		const auto alignedOffset = m_Offset + ((16 - (m_Offset % 16)) % 16);
		if (alignedOffset + JumpSequenceSize > CallHookMemorySize)
			throw std::runtime_error("Call hook jump sequence memory exhausted");

		m_Offset = alignedOffset;
		auto sequence = m_Memory.Add(m_Offset).As<void*>();
		if (!FitsRel32(location, sequence))
			throw std::runtime_error("Call hook jump sequence is outside rel32 range");

		*m_Memory.Add(m_Offset).As<int16_t*>() = 0xB848;
		*m_Memory.Add(m_Offset).Add(2).As<void**>() = func;
		*m_Memory.Add(m_Offset).Add(10).As<int16_t*>() = 0xE0FF;

		m_Offset += JumpSequenceSize;
		FlushInstructionCache(GetCurrentProcess(), sequence, JumpSequenceSize);

		return sequence;
	}

	void CallHookMemory::DestroyImpl()
	{
		if (m_Memory)
		{
			if (!VirtualFree(m_Memory.As<void*>(), 0, MEM_RELEASE))
				LOGF(FATAL, "Failed to release call hook memory: {}", GetLastError());
			m_Memory = nullptr;
			m_Offset = 0;
		}
	}

	CallSiteHook::Hook::Hook(void* location, void* hook) :
	    m_Location(location),
	    m_Hook(hook),
	    m_Enabled(false)
	{
		if (!location || !hook)
			throw std::invalid_argument("Call hook location and target must be valid");

		auto seq = CallHookMemory::AllocateJumpSequence(hook, location);
		m_PatchedBytes[0] = 0xE8;
		const auto displacement = static_cast<std::int64_t>(reinterpret_cast<std::uintptr_t>(seq))
		    - static_cast<std::int64_t>(reinterpret_cast<std::uintptr_t>(location)) - 5;
		const auto relativeDisplacement = static_cast<std::int32_t>(displacement);
		memcpy(&m_PatchedBytes[1], &relativeDisplacement, sizeof(relativeDisplacement));
		memcpy(m_OriginalBytes, location, 5);
		m_OriginalFunction = PointerCalculator(location).Add(1).Rip().As<void*>();
	}

	void CallSiteHook::Hook::Enable()
	{
		if (!m_Enabled)
		{
			DWORD oldProtect{};
			if (!VirtualProtect(m_Location, 5, PAGE_EXECUTE_READWRITE, &oldProtect))
			{
				LOGF(FATAL, "Failed to make call site writable: {}", GetLastError());
				return;
			}

			memcpy(m_Location, m_PatchedBytes, 5);
			m_Enabled = true;

			FlushInstructionCache(GetCurrentProcess(), m_Location, 5);
			DWORD currentProtect{};
			if (!VirtualProtect(m_Location, 5, oldProtect, &currentProtect))
				LOGF(FATAL, "Failed to restore call site protection: {}", GetLastError());
		}
	}

	void CallSiteHook::Hook::Disable()
	{
		if (m_Enabled)
		{
			DWORD oldProtect{};
			if (!VirtualProtect(m_Location, 5, PAGE_EXECUTE_READWRITE, &oldProtect))
			{
				LOGF(FATAL, "Failed to make call site writable while disabling hook: {}", GetLastError());
				return;
			}

			memcpy(m_Location, m_OriginalBytes, 5);
			m_Enabled = false;

			FlushInstructionCache(GetCurrentProcess(), m_Location, 5);
			DWORD currentProtect{};
			if (!VirtualProtect(m_Location, 5, oldProtect, &currentProtect))
				LOGF(FATAL, "Failed to restore call site protection while disabling hook: {}", GetLastError());
		}
	}

	std::shared_ptr<CallSiteHook::Hook> CallSiteHook::AddHookImpl(void* location, void* hook)
	{
		auto hookPtr = std::make_shared<Hook>(location, hook);

		m_Hooks.push_back(hookPtr);
		return hookPtr;
	}

	void CallSiteHook::DestroyImpl()
	{
		for (auto& hook : m_Hooks)
		{
			if (hook)
				hook->Disable();
		}

		m_Hooks.clear();
		CallHookMemory::Destroy();
	}
}
