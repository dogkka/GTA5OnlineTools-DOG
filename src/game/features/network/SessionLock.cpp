#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	// 锁战局：阻止新玩家加入当前战局
	// 原理：向会话层设置"阻止加入请求"标志，并周期性重申，防止被战局脚本重置
	class SessionLock : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		void Apply()
		{
			NETWORK::NETWORK_SESSION_BLOCK_JOIN_REQUESTS(true);
			NETWORK::NETWORK_TRANSITION_BLOCK_JOIN_REQUESTS(true);
		}

		virtual void OnEnable() override
		{
			Apply();
			Notifications::Show("锁战局", "已开启：新玩家将无法加入当前战局。", NotificationType::Success);
		}

		virtual void OnDisable() override
		{
			NETWORK::NETWORK_SESSION_BLOCK_JOIN_REQUESTS(false);
			NETWORK::NETWORK_TRANSITION_BLOCK_JOIN_REQUESTS(false);
			Notifications::Show("锁战局", "已关闭：战局重新开放加入。", NotificationType::Warning);
		}

		virtual void OnTick() override
		{
			if (++m_Timer < 300)
				return;

			m_Timer = 0;
			Apply();
		}
	};

	static SessionLock _SessionLock{"locklobby", "锁战局", "阻止新玩家加入当前战局（关闭本项即重新开放）"};
}
