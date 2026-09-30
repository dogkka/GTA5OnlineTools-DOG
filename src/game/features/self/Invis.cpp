#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"

namespace YimMenu::Features
{
	static BoolCommand _LocallyVisible{"localvis", "本地可见", "在对其他玩家隐形的同时，本地仍然可见"};

	class Invis : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (Self::GetPed())
			{
				Self::GetPed().SetVisible(false);
				if (_LocallyVisible.GetState())
					Self::GetPlayer().SetVisibleLocally(true);
				else
					Self::GetPlayer().SetVisibleLocally(false);
			}
		}

		virtual void OnDisable() override
		{
			if (Self::GetPed())
			{
				Self::GetPed().SetVisible(true);
				Self::GetPlayer().SetVisibleLocally(false);
			}
		}
	};

	static Invis _Invis{"invis", "隐形", "让自己隐形"};
}
