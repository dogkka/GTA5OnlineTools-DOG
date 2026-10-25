#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	class FastRespawn : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			auto ped = Self::GetPed();
			if (!ped || !ped.IsDead())
				return;

			auto pos = ped.GetPosition();
			NETWORK::NETWORK_RESURRECT_LOCAL_PLAYER(pos.x, pos.y, pos.z, 0.0f, false, false, false, 0, 0);
		}
	};

	static FastRespawn _FastRespawn{"fastrespawn", "快速重生", "死亡后在原地立即复活"};
}
