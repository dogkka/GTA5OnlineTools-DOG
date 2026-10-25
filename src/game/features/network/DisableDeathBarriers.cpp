#include "core/commands/BoolCommand.hpp"
#include "game/backend/ScriptPatches.hpp"

namespace YimMenu::Features
{
	class DisableDeathBarriers : public BoolCommand
	{
		using BoolCommand::BoolCommand;

		ScriptPatch m_DeathBarriersPatch{};

		virtual void OnEnable() override
		{
			if (!m_DeathBarriersPatch)
			{
				m_DeathBarriersPatch = ScriptPatches::AddPatch("freemode"_J, ScriptPointer("DeathBarriersPatch", "2D 01 09 00 00 5D ? ? ? 56 ? ? 3A").Add(5), {0x2E, 0x01, 0x00});
			}
			m_DeathBarriersPatch->Enable();
		}


		virtual void OnDisable() override
		{
			if (m_DeathBarriersPatch)
			{
				m_DeathBarriersPatch->Disable();
			}
		}
	};

	static DisableDeathBarriers _DisableDeathBarriers{"disabledeathbarriers", "禁用死亡屏障", "禁用地图下方的死亡屏障，也可防止旁观他人时随机死亡", true};
}
