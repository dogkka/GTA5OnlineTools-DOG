#include <cmath>

#include "core/commands/Command.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Object.hpp"
#include "game/gta/Pools.hpp"

namespace YimMenu::Features
{
	// spiral force field: pulls entities around the player up into a vortex
	class Tornado : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 2)
				return;

			m_Timer = 0;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			const auto pos = ped.GetPosition();

			auto swirl = [&](int handle, bool allow_up) {
				if (handle == ped.GetHandle())
					return;

				const auto p    = ENTITY::GET_ENTITY_COORDS(handle, true);
				const float dx  = pos.x - p.x;
				const float dy  = pos.y - p.y;
				const float dz  = pos.z - p.z;
				const float dsq = dx * dx + dy * dy;

				if (dsq > 60.0f * 60.0f || dsq < 0.5f)
					return;

				const float dist = sqrtf(dsq) + 0.1f;

				// tangential (spin) + radial (pull in) + upward
				const float tx = -dy / dist;
				const float ty = dx / dist;
				const float k  = 900.0f / (dist + 4.0f);

				ENTITY::APPLY_FORCE_TO_ENTITY(handle,
				    1,
				    (dx / dist) * k + tx * k * 1.8f,
				    (dy / dist) * k + ty * k * 1.8f,
				    allow_up ? k * 1.1f + dz * 0.4f : 0.0f,
				    0.0f,
				    0.0f,
				    0.0f,
				    0,
				    false,
				    false,
				    false,
				    false,
				    true);
			};

			for (auto veh : Pools::GetVehicles())
				swirl(veh.GetHandle(), true);

			for (auto iter : Pools::GetPeds())
			{
				if (!iter.IsPlayer())
					swirl(iter.GetHandle(), true);
			}

			for (auto obj : Pools::GetObjects())
				swirl(obj.GetHandle(), true);
		}
	};

	// spawns a hovering UFO above the player (tries several known model names)
	class UfoSighting : public Command
	{
		using Command::Command;

		static int g_Ufo;

		virtual void OnCall() override
		{
			auto ped = Self::GetPed();
			if (!ped)
				return;

			const auto pos = ped.GetPosition();

			if (g_Ufo != 0 && ENTITY::DOES_ENTITY_EXIST(g_Ufo))
			{
				Object(g_Ufo).Delete();
				g_Ufo = 0;
				Notifications::Show("UFO 目击", "UFO 已离开。", NotificationType::Info);
				return;
			}

			static const std::uint32_t models[] = {"prop_ufo_01"_J, "p_ufo"_J, "ufo"_J};

			for (auto model : models)
			{
				if (!STREAMING::IS_MODEL_IN_CDIMAGE(model))
					continue;

				auto ufo = Object::Create(model, rage::fvector3{pos.x, pos.y, pos.z + 35.0f});
				if (!ufo)
					continue;

				g_Ufo = ufo.GetHandle();
				ENTITY::FREEZE_ENTITY_POSITION(g_Ufo, true);
				break;
			}

			if (g_Ufo != 0)
				Notifications::Show("UFO 目击", "一个不明飞行物悬停在你的头顶。", NotificationType::Success);
			else
				Notifications::Show("UFO 目击", "当前游戏版本没有可用的 UFO 模型。", NotificationType::Warning);
		}
	};

	int UfoSighting::g_Ufo = 0;
}
