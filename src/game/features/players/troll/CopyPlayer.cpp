#include "core/backend/FiberPool.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

#include <string>

namespace YimMenu::Features
{
	// 复制目标玩家的整套穿着（组件 + 饰品）
	class CopyOutfit : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			if (!player.IsValid() || player.IsLocal())
				return;

			FiberPool::Push([player]() mutable {
				auto src = player.GetPed();
				auto dst = Self::GetPed();
				if (!src || !dst)
					return;

				for (int comp = 0; comp < 12; comp++)
				{
					const int drawable = PED::GET_PED_DRAWABLE_VARIATION(src.GetHandle(), comp);
					if (drawable < 0)
						continue; // 源模型缺少该组件，跳过避免写入无效变体
					const int texture  = PED::GET_PED_TEXTURE_VARIATION(src.GetHandle(), comp);
					PED::SET_PED_COMPONENT_VARIATION(dst.GetHandle(), comp, drawable, texture, 0);
				}

				for (int prop = 0; prop < 8; prop++)
				{
					const int idx = PED::GET_PED_PROP_INDEX(src.GetHandle(), prop, 0);
					if (idx >= 0)
					{
						const int tex = PED::GET_PED_PROP_TEXTURE_INDEX(src.GetHandle(), prop);
						PED::SET_PED_PROP_INDEX(dst.GetHandle(), prop, idx, tex, true, 0);
					}
					else
					{
						PED::CLEAR_PED_PROP(dst.GetHandle(), prop, 0);
					}
				}

				Notifications::Show("复制外观", "已套用目标的穿着。", NotificationType::Success);
			});
		}
	};

	// 复制目标玩家的载具（模型 + 全套改装 + 颜色 + 车牌）
	class CopyVehicle : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			if (!player.IsValid() || player.IsLocal())
				return;

			auto ped = player.GetPed();
			auto veh = ped ? ped.GetVehicle() : Vehicle(nullptr);
			if (!veh)
			{
				Notifications::Show("复制载具", "目标不在载具中。", NotificationType::Warning);
				return;
			}

			const int src = veh.GetHandle();
			FiberPool::Push([src] {
				if (!ENTITY::DOES_ENTITY_EXIST(src))
					return;

				auto selfPed = Self::GetPed();
				if (!selfPed)
					return;

				const Hash model   = ENTITY::GET_ENTITY_MODEL(src);
				const auto native_pos = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(selfPed.GetHandle(), 0.0f, 4.0f, 0.5f);
				const rage::fvector3 spawn_pos{native_pos.x, native_pos.y, native_pos.z};

				auto clone = Vehicle::Create(model, spawn_pos, ENTITY::GET_ENTITY_HEADING(src), true);
				if (!clone)
				{
					Notifications::Show("复制载具", "生成复制载具失败。", NotificationType::Error);
					return;
				}

				const int handle = clone.GetHandle();
				VEHICLE::SET_VEHICLE_MOD_KIT(handle, 0);

				for (int i = 0; i < 50; i++)
				{
					const int mod = VEHICLE::GET_VEHICLE_MOD(src, i);
					if (mod >= 0)
						VEHICLE::SET_VEHICLE_MOD(handle, i, mod, false);
				}

				VEHICLE::SET_VEHICLE_WHEEL_TYPE(handle, VEHICLE::GET_VEHICLE_WHEEL_TYPE(src));

				int r, g, b;
				VEHICLE::GET_VEHICLE_CUSTOM_PRIMARY_COLOUR(src, &r, &g, &b);
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(handle, r, g, b);
				VEHICLE::GET_VEHICLE_CUSTOM_SECONDARY_COLOUR(src, &r, &g, &b);
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(handle, r, g, b);

				VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(handle, VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT(src));
				VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(handle, VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(src));

				Notifications::Show("复制载具", "已生成目标的载具复制品。", NotificationType::Success);
			});
		}
	};

	static CopyOutfit _CopyOutfit{"copyoutfit", "复制外观", "把目标玩家的整套穿着复制到自己身上。"};
	static CopyVehicle _CopyVehicle{"copyvehicle", "复制载具", "生成一辆与目标载具同款、同改装、同颜色的复制品。"};
}
