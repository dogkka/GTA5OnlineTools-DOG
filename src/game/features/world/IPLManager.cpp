#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/BoolCommand.hpp"
#include "game/gta/data/IPL.hpp"
#include "game/gta/Natives.hpp"
#include "game/backend/Self.hpp"

namespace YimMenu::Features
{
	class IPLSelectorHelper
	{
	public:
		std::vector<std::pair<int, const char*>> availableIplNames = {};

		IPLSelectorHelper()
		{
			int i = 0;

			for (const auto& ipl : IPLs)
			{
				availableIplNames.push_back({i, ipl.FriendlyName.data()});
				i++;
			}
		}
	};

	static IPLSelectorHelper g_IPLSelectorHelper{};
	static ListCommand _IPLSelector{"iplselector", "要加载的 IPL", "选择要加载的 IPL", g_IPLSelectorHelper.availableIplNames, 0};

	class IPLLoader : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto requestedIpl = _IPLSelector.GetState();

			for (auto& ipl_name : IPLs[requestedIpl].IPLNames)
				if (!STREAMING::IS_IPL_ACTIVE(ipl_name.data()))
					STREAMING::REQUEST_IPL(ipl_name.data());

			for (auto& remove_ipl_name : IPLs[requestedIpl].IPLNamesRemoveWhenLoad)
				STREAMING::REMOVE_IPL(remove_ipl_name.data());
		}
	};

	class IPLUnloader : Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto selectedIpl = _IPLSelector.GetState();

			for (auto& ipl_name : IPLs[selectedIpl].IPLNames)
				if (STREAMING::IS_IPL_ACTIVE(ipl_name.data()))
					STREAMING::REMOVE_IPL(ipl_name.data());

			for (auto& ipl_name : IPLs[selectedIpl].IPLNamesRemove)
				STREAMING::REMOVE_IPL(ipl_name.data());
		}
	};

	class IPLTeleport : Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (auto self = Self::GetPed())
			{
				auto selectedIpl = _IPLSelector.GetState();
				auto iplLocation = IPLs[selectedIpl].Location;

				self.TeleportTo(iplLocation);
			}
		}
	};

	static IPLLoader _IPLLoader{"loadipl", "加载 IPL", "加载所选 IPL"};
	static IPLUnloader _IPLUnloader{"unloadipl", "卸载 IPL", "卸载所选 IPL"};
	static IPLTeleport _IPLTeleport{"ipltp", "传送到 IPL", "传送到所选 IPL"};
};
