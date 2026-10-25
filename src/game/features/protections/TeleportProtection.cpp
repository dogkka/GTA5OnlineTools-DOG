#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	// 传送回滚（实验性）：检测非事件类的强制位移并拉回原位；过场/任务/暂停中不生效。
	class TeleportProtection : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		rage::fvector3 m_LastPos{};
		bool m_HavePos = false;
		int m_NotifyCooldown = 0;

		virtual void OnTick() override
		{
			auto ped = Self::GetPed();
			if (!ped)
			{
				m_HavePos = false;
				return;
			}

			if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("freemode"_J) == 0
			    || HUD::IS_PAUSE_MENU_ACTIVE()
			    || CAMERA::IS_SCREEN_FADING_OUT()
			    || CAMERA::IS_SCREEN_FADED_OUT())
			{
				m_HavePos = false;
				return;
			}

			const auto pos = ped.GetPosition();

			if (m_NotifyCooldown > 0)
				m_NotifyCooldown--;

			if (!m_HavePos)
			{
				m_LastPos  = pos;
				m_HavePos  = true;
				return;
			}

			if (pos.GetDistance(m_LastPos) > 250.0f)
			{
				// 突兀瞬移 → 拉回
				ENTITY::SET_ENTITY_COORDS(ped.GetHandle(), m_LastPos.x, m_LastPos.y, m_LastPos.z, false, false, false, true);

				if (m_NotifyCooldown == 0)
				{
					m_NotifyCooldown = 300;
					Notifications::Show("传送防护", "检测到异常的强制位移，已将你拉回原位。", NotificationType::Warning);
				}

				return;
			}

			m_LastPos = pos;
		}

		virtual void OnDisable() override
		{
			m_HavePos = false;
		}
	};

	static TeleportProtection _TeleportProtection{"teleportprotection", "传送回滚（实验性）", "检测非事件类的强制位移并自动拉回；过场/任务中不生效"};
}
