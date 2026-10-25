#include <algorithm>
#include <chrono>
#include <string>

#include "core/backend/FiberPool.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static FloatCommand _BoostSpeed{"instantboostspeed", "瞬间提速目标", "瞬间提速要达到的速度（km/h）", 200.0f, 800.0f, 560.0f};
	static FloatCommand _BoostTime{"instantboosttime", "瞬间提速时长", "提速持续时间（秒）", 0.5f, 3.0f, 1.0f};

	// 瞬间提速（一代版本同款）：行驶中一键把车速直接从当前提到目标速度并保持约 1 秒。
	// 实现要点：
	//  1) 先放开本车物理速度上限（SET_VEHICLE_MAX_SPEED），否则会被“平跑极速”字段掐掉；
	//  2) 提速窗口内每帧把前进速度顶到目标值；
	//  3) 结束后把上限恢复默认（0 = 恢复）。
	class InstantBoost : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto veh = Self::GetVehicle();
			auto ped = Self::GetPed();
			if (!veh || !ped)
			{
				Notifications::Show("瞬间提速", "需要坐在载具中。", NotificationType::Warning);
				return;
			}

			if (VEHICLE::GET_PED_IN_VEHICLE_SEAT(veh.GetHandle(), -1, true) != ped.GetHandle())
			{
				Notifications::Show("瞬间提速", "需要自己驾驶载具（乘客无效）。", NotificationType::Warning);
				return;
			}

			const int handle       = veh.GetHandle();
			const float target_mps = std::clamp(_BoostSpeed.GetState(), 100.0f, 900.0f) / 3.6f;
			const auto duration    = std::chrono::milliseconds(static_cast<int>(std::clamp(_BoostTime.GetState(), 0.2f, 5.0f) * 1000.0f));

			FiberPool::Push([handle, target_mps, duration] {
				if (!ENTITY::DOES_ENTITY_EXIST(handle))
					return;

				// 放开物理速度上限，避免被“平跑极速”字段掐掉
				VEHICLE::SET_VEHICLE_MAX_SPEED(handle, target_mps + 15.0f);

				const auto end = std::chrono::steady_clock::now() + duration;
				while (std::chrono::steady_clock::now() < end)
				{
					if (!ENTITY::DOES_ENTITY_EXIST(handle))
						break;

					if (ENTITY::GET_ENTITY_SPEED(handle) < target_mps)
						VEHICLE::SET_VEHICLE_FORWARD_SPEED(handle, target_mps);

					ScriptMgr::Yield();
				}

				if (ENTITY::DOES_ENTITY_EXIST(handle))
					VEHICLE::SET_VEHICLE_MAX_SPEED(handle, 0.0f); // 0 = 恢复默认
			});

			Notifications::Show("瞬间提速", "已触发：" + std::to_string(static_cast<int>(_BoostSpeed.GetState() + 0.5f)) + " km/h。", NotificationType::Success);
		}
	};

	static InstantBoost _InstantBoost{"instantboost", "瞬间提速", "行驶中一键把车速瞬间提到目标速度并保持约 1 秒；建议在 设置 → 热键 中绑定按键"};
}
