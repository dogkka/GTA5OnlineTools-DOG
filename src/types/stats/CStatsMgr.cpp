#include "CStatsMgr.hpp"

sStatData* CStatsMgr::GetStat(std::uint32_t stat)
{
	auto res = bsearch(&stat, m_Stats.begin(), m_Stats.size(), sizeof(sStatMap), [](const void* a1, const void* a2) {
		auto v2 = *(std::uint32_t*)a1 < *(std::uint32_t*)a2;
		auto v3 = *(std::uint32_t*)a1 != *(std::uint32_t*)a2;
		if (!v2)
			return (int)v3;
		return -1;
	});

	if (res)
		return reinterpret_cast<sStatMap*>(res)->m_Data;

	return nullptr;
}
