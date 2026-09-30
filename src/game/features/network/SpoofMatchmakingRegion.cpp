#include "core/commands/LoopedCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	static std::vector<std::pair<int, const char*>> g_RegionCodes = {
	    {0, "独联体"},
	    {1, "南美"},
	    {2, "美国东部"},
	    {3, "欧洲"},
	    {4, "中国"},
	    {5, "澳大利亚"},
	    {6, "美国西部"},
	    {7, "日本"},
	    {8, "未知"},
	};

	static ListCommand _SpoofedRegion{"mmregion", "匹配区域", "要伪装成的匹配区域", g_RegionCodes, 0};

	class SpoofMatchmakingRegion : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_OldRegion;

		virtual void OnEnable() override
		{
			m_OldRegion = *Pointers.RegionCode;
		}

		virtual void OnTick() override
		{
			*Pointers.RegionCode = _SpoofedRegion.GetState();
		}

		virtual void OnDisable() override
		{
			*Pointers.RegionCode = m_OldRegion;
		}
	};

	static SpoofMatchmakingRegion _SpoofMatchmakingRegion{"spoofmmregion", "伪装匹配区域", "将当前匹配区域伪装为指定值"};
}
