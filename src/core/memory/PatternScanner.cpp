#include "PatternScanner.hpp"

#include "Module.hpp"
#include "ModuleMgr.hpp"
#include "core/backend/PatternCache.hpp"

#include <algorithm>
#include <future>
#include <limits>

namespace
{
	bool IsReadableProtection(DWORD protection)
	{
		if (protection & (PAGE_GUARD | PAGE_NOACCESS))
			return false;

		switch (protection & 0xFF)
		{
		case PAGE_READONLY:
		case PAGE_READWRITE:
		case PAGE_WRITECOPY:
		case PAGE_EXECUTE:
		case PAGE_EXECUTE_READ:
		case PAGE_EXECUTE_READWRITE:
		case PAGE_EXECUTE_WRITECOPY: return true;
		default: return false;
		}
	}

	bool MatchesSignature(std::uintptr_t address, std::span<const std::optional<std::uint8_t>> signature)
	{
		const auto bytes = reinterpret_cast<const std::uint8_t*>(address);
		for (std::size_t index = 0; index < signature.size(); ++index)
		{
			if (signature[index] && signature[index].value() != bytes[index])
				return false;
		}
		return true;
	}
}

namespace YimMenu
{
	PatternScanner::PatternScanner(const Module* module) :
	    m_Module(module),
	    m_Patterns()
	{
	}

	bool PatternScanner::Scan()
	{
		if (!m_Module || !m_Module->Valid())
			return false;

		bool scanSuccess = true;

		if (!ModuleMgr.IsManualMapped())
		{
			std::vector<std::future<bool>> jobs;
			for (const auto& [pattern, func] : m_Patterns)
			{
				jobs.emplace_back(std::async(&PatternScanner::ScanInternal, this, pattern, func));
			}

			for (auto& job : jobs)
			{
				scanSuccess = job.get() && scanSuccess;
			}
		}
		else
		{
			// spawning threads seems to throw STATUS_THREADPOOL_FREE_LIBRARY_ON_COMPLETION_FAILED when manual mapping
			for (const auto& [pattern, func] : m_Patterns)
				scanSuccess = PatternScanner::ScanInternal(pattern, func) && scanSuccess;
		}

		return scanSuccess;
	}

	bool PatternScanner::ScanInternal(const IPattern* pattern, PatternFunc func) const
	{
		const auto signature = pattern->Signature();
		if (signature.empty())
		{
			LOG(WARNING) << "Cannot scan empty pattern [" << pattern->Name() << "]";
			return false;
		}

		if (PatternCache::IsInitialized())
		{
			auto offset = PatternCache::GetCachedOffset(pattern->Hash().Update(m_Module->Size()));
			if (offset.has_value() && offset.value() >= 0 && static_cast<std::uintptr_t>(offset.value()) < m_Module->Size())
			{
				const auto address = m_Module->Base() + offset.value();
				const auto moduleBytesRemaining = m_Module->End() - address;
				MEMORY_BASIC_INFORMATION memoryInfo{};
				const bool queried = VirtualQuery(reinterpret_cast<void*>(address), &memoryInfo, sizeof(memoryInfo)) == sizeof(memoryInfo);
				const auto regionStart = reinterpret_cast<std::uintptr_t>(memoryInfo.BaseAddress);
				const bool addressInRegion = queried && address >= regionStart && address - regionStart < memoryInfo.RegionSize;
				if (addressInRegion
				    && memoryInfo.State == MEM_COMMIT && IsReadableProtection(memoryInfo.Protect)
				    && signature.size() <= moduleBytesRemaining
				    && signature.size() <= memoryInfo.RegionSize - (address - regionStart)
				    && MatchesSignature(address, signature))
				{
					LOGF(INFO, "Using cached pattern [{}] : [{:X}] [Hash(): {:X}]", pattern->Name(), address, pattern->Hash().Update(m_Module->Size()).m_Hash);
					std::invoke(func, address);
					return true;
				}
			}
		}

		auto regionStart = m_Module->Base();
		while (regionStart < m_Module->End())
		{
			MEMORY_BASIC_INFORMATION memoryInfo{};
			if (VirtualQuery(reinterpret_cast<void*>(regionStart), &memoryInfo, sizeof(memoryInfo)) != sizeof(memoryInfo))
				break;

			const auto queriedStart = reinterpret_cast<std::uintptr_t>(memoryInfo.BaseAddress);
			const auto queriedEnd = memoryInfo.RegionSize <= std::numeric_limits<std::uintptr_t>::max() - queriedStart
			    ? queriedStart + memoryInfo.RegionSize
			    : std::numeric_limits<std::uintptr_t>::max();
			const auto scanStart = std::max(regionStart, queriedStart);
			const auto scanEnd = std::min(m_Module->End(), queriedEnd);

			if (scanStart < scanEnd && memoryInfo.State == MEM_COMMIT && IsReadableProtection(memoryInfo.Protect) && signature.size() <= scanEnd - scanStart)
			{
				const auto lastAddress = scanEnd - signature.size();
				for (auto address = scanStart; address <= lastAddress; ++address)
				{
					if (!MatchesSignature(address, signature))
						continue;

					LOG(INFO) << "Found pattern [" << pattern->Name() << "] : [" << HEX(address) << "]";

					std::invoke(func, address);

					if (PatternCache::IsInitialized())
						PatternCache::UpdateCachedOffset(pattern->Hash().Update(m_Module->Size()), address - m_Module->Base());

					return true;
				}
			}

			if (scanEnd <= regionStart)
				break;
			regionStart = scanEnd;
		}

		LOG(WARNING) << "Failed to find pattern [" << pattern->Name() << "]";
		return false;
	}
}
