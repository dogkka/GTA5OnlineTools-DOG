#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

#include <vector>

namespace YimMenu::Features
{
	// 防笼子：自动清除贴身放置的困人道具（黄金集装箱 / 保险箱 / 围墙 / 集装箱等）
	// 每 2 秒扫描一次，只清除 4.5 米内已知型号的物体，避免误删远处正常场景道具
	class AntiCage : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;
		int m_LastNotify = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 120)
				return;

			m_Timer = 0;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			auto pos = ped.GetPosition();

			static const std::vector<Hash> cageModels = {
			    "prop_gold_cont_01"_J,        // 黄金集装箱（最常见笼子）
			    "prop_gold_cont_01b"_J,
			    "prop_ld_int_safe_01"_J,      // 保险箱
			    "p_v_43_safe_s"_J,            // 保险箱（老版）
			    "h4_prop_h4_safe_01a"_J,      // 保险箱（DLC）
			    "prop_fncsec_03b"_J,          // 大框架 / 围栏板
			    "prop_fncsec_03d"_J,
			    "stt_prop_stunt_tube_l"_J,    // 管道
			    "prop_cons_crate"_J,
			    "prop_mb_crate_01a"_J,
			    "hei_prop_heist_box"_J,
			    "prop_container_01a"_J,       // 集装箱系列
			    "prop_container_01b"_J,
			    "prop_container_01c"_J,
			    "prop_container_01d"_J,
			    "prop_container_01e"_J,
			    "prop_container_01f"_J,
			    "prop_container_01g"_J,
			    "prop_container_01h"_J,
			};

			int removed = 0;
			for (auto model : cageModels)
			{
				// 同型号最多清除 3 个（笼子通常是多块拼装）
				for (int i = 0; i < 3; i++)
				{
					auto obj = OBJECT::GET_CLOSEST_OBJECT_OF_TYPE(pos.x, pos.y, pos.z, 4.5f, model, false, false, false);
					if (!obj)
						break;

					OBJECT::DELETE_OBJECT(&obj);
					ENTITY::SET_OBJECT_AS_NO_LONGER_NEEDED(&obj);
					removed++;
				}
			}

			if (removed > 0)
			{
				m_LastNotify += removed;
				// 通知节流：累计观察到 10 秒才提示一次，避免刷屏
				static int s_NotifyTimer = 0;
				if (++s_NotifyTimer >= 5)
				{
					s_NotifyTimer = 0;
					Notifications::Show("防笼子", "已自动清除贴身的困人道具（本次 " + std::to_string(m_LastNotify) + " 个）。", NotificationType::Warning);
					m_LastNotify = 0;
				}
			}
		}
	};

	static AntiCage _AntiCage{"anticage", "防笼子", "自动清除贴身放置的笼子/保险箱/围墙等困人道具（实验性，每 2 秒扫描）"};
}
