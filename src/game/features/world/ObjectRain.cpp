#include <iterator>
#include <vector>

#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Object.hpp"
#include "game/gta/Vehicle.hpp"

namespace YimMenu::Features
{
	static const struct
	{
		std::uint32_t Model;
		const char* Name;
		bool IsVehicle;
	} g_RainObjects[] = {
	    {"prop_toilet_01"_J, "马桶", false},
	    {"prop_container_01a"_J, "集装箱", false},
	    {"stt_prop_stunt_bowling_ball"_J, "巨型保龄球", false},
	    {"prop_ld_dstcover_01"_J, "垃圾桶", false},
	    {"adder"_J, "跑车（车雨）", true},
	    {"a_c_cow"_J, "奶牛（牛雨）", false},
	};

	static std::vector<std::pair<int, const char*>> MakeRainList()
	{
		std::vector<std::pair<int, const char*>> list;
		for (int i = 0; i < (int)std::size(g_RainObjects); i++)
			list.push_back({i, g_RainObjects[i].Name});
		return list;
	}

	static ListCommand _RainObject{"rainobject", "天降物", "选择要往下掉的物体", MakeRainList(), 0};

	class ObjectRain : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 40)
				return;

			m_Timer = 0;

			auto ped = Self::GetPed();
			if (!ped)
				return;

			const int index = _RainObject.GetState();
			if (index < 0 || index >= (int)std::size(g_RainObjects))
				return;

			const auto& entry = g_RainObjects[index];
			if (!STREAMING::IS_MODEL_IN_CDIMAGE(entry.Model))
				return;

			const auto pos = ped.GetPosition();
			const float x  = pos.x + (float)(rand() % 50 - 25);
			const float y  = pos.y + (float)(rand() % 50 - 25);
			const float z  = pos.z + 40.0f + (float)(rand() % 30);

			if (entry.IsVehicle)
			{
				auto veh = Vehicle::Create(entry.Model, rage::fvector3{x, y, z}, 0.0f, false);
				if (veh)
					ENTITY::SET_ENTITY_VELOCITY(veh.GetHandle(), 0.0f, 0.0f, -25.0f);
			}
			else
			{
				auto obj = Object::Create(entry.Model, rage::fvector3{x, y, z});
				if (obj)
					ENTITY::SET_ENTITY_VELOCITY(obj.GetHandle(), 0.0f, 0.0f, -35.0f);
			}
		}
	};

	static ObjectRain _ObjectRain{"objectrain", "天降杂物", "持续从天上掉落所选物体（马桶/集装箱/大球/跑车/奶牛…）"};
}
