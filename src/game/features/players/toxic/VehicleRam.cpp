#include "core/backend/FiberPool.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/util/Joaat.hpp"
#include "game/backend/Players.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Vehicle.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>

namespace YimMenu::Features
{
	static const char* kRamModels[] = {"dominator3", "phantom", "bulldozer", "firetruk", "taxi"};

	static ListCommand _RamModel{"rammodel", "冲撞车型", "用来撞目标的车。", {{0, "公路霸者"}, {1, "大卡车"}, {2, "推土机"}, {3, "消防车"}, {4, "出租车"}}, 0};
	static FloatCommand _RamSpeed{"ramspeed", "冲撞速度", "冲撞载具的初速度。", 20.0f, 200.0f, 60.0f};
	static BoolCommand _RamInvisible{"raminvis", "隐身冲撞车", "让冲撞载具不可见（只看到目标被撞飞）。", false};
	static BoolCommand _RamCleanup{"ramcleanup", "自动清除冲撞车", "撞完约 8 秒后自动删除冲撞载具。", true};

	// 载具 RAM：在目标背后生成一辆车，瞄准目标全速撞过去（Stand 同款思路）
	class VehicleRam : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			if (!player.IsValid() || player.IsLocal())
				return;

			auto ped = player.GetPed();
			if (!ped)
				return;

			const auto target = ped.GetPosition();
			const int idx     = std::clamp(_RamModel.GetState(), 0, (int)(sizeof(kRamModels) / sizeof(kRamModels[0])) - 1);
			const auto model  = Joaat(kRamModels[idx]);
			const float speed = _RamSpeed.GetState();
			const bool invisible = _RamInvisible.GetState();
			const bool cleanup   = _RamCleanup.GetState();

			// 生成在目标正后方 25 米，车头朝向目标
			const auto native_pos = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped.GetHandle(), 0.0f, -25.0f, 0.5f);
			const rage::fvector3 spawn_pos{native_pos.x, native_pos.y, native_pos.z};
			const float heading  = std::atan2(target.x - spawn_pos.x, target.y - spawn_pos.y) * 57.2957795f;

			FiberPool::Push([spawn_pos, heading, model, speed, invisible, cleanup] {
				auto veh = Vehicle::Create(model, spawn_pos, heading, false);
				if (!veh)
					return;

				const int handle = veh.GetHandle();
				VEHICLE::SET_VEHICLE_ENGINE_ON(handle, true, true, false);
				VEHICLE::SET_VEHICLE_GRAVITY(handle, false);
				VEHICLE::SET_DISABLE_MAP_COLLISION(handle);
				ENTITY::SET_ENTITY_VISIBLE(handle, !invisible, false);
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(handle, speed);

				if (cleanup)
				{
					ScriptMgr::Yield(std::chrono::milliseconds(8000));
					if (ENTITY::DOES_ENTITY_EXIST(handle))
						Vehicle(handle).Delete();
				}
			});

			Notifications::Show("载具冲撞", "撞击车已出动。", NotificationType::Success);
		}
	};

	static VehicleRam _VehicleRam{"ramveh", "载具冲撞", "在目标背后生成一辆车全速撞过去（车型/速度/隐身可调）。"};
}
