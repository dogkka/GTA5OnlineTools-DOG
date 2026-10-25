#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Object.hpp"
#include "game/gta/Vehicle.hpp"

namespace YimMenu::Features
{
	// 扫把飞行：生成一辆隐形的暴君 MK2 作为物理载体，再把扫把挂在车头
	// 玩家坐上后即按 MK2 的操作方式自由飞行，看起来就是骑着扫把在飞
	class BroomFlight : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Vehicle = 0;
		int m_Broom   = 0;
		int m_Notice  = 0;

		void CleanupRig()
		{
			StopParticles();

			if (m_Broom && ENTITY::DOES_ENTITY_EXIST(m_Broom))
				Object(m_Broom).Delete();
			if (m_Vehicle && ENTITY::DOES_ENTITY_EXIST(m_Vehicle))
				Vehicle(m_Vehicle).Delete();

			m_Broom   = 0;
			m_Vehicle = 0;
		}

		void StopParticles()
		{
			// 目前扫把没有粒子，预留清理入口（后续加特效时在这里停）
		}

		void SpawnRig()
		{
			auto ped = Self::GetPed();
			if (!ped)
				return;

			if (auto current = ped.GetVehicle(); current && current.GetHandle() != m_Vehicle)
			{
				if (m_Notice <= 0)
				{
					Notifications::Show("扫把飞行", "请先离开当前载具，再启用扫把。", NotificationType::Warning);
					m_Notice = 300;
				}
				return;
			}

			const auto pos     = ped.GetPosition();
			const auto heading = ped.GetHeading();

			auto veh = Vehicle::Create("oppressor2"_J, rage::fvector3{pos.x, pos.y, pos.z + 0.5f}, heading, true);
			if (!veh)
				return;

			m_Vehicle = veh.GetHandle();
			VEHICLE::SET_VEHICLE_ENGINE_ON(m_Vehicle, true, true, false);
			ENTITY::SET_ENTITY_VISIBLE(m_Vehicle, false, false);
			ENTITY::SET_ENTITY_ALPHA(m_Vehicle, 0, false);

			auto broom = Object::Create("prop_tool_broom"_J, rage::fvector3{pos.x, pos.y, pos.z + 0.5f});
			if (broom)
			{
				m_Broom = broom.GetHandle();
				// 与 Chaos Mod V 相同的挂点：车头 +0.3 高度、-80 度俯仰
				ENTITY::ATTACH_ENTITY_TO_ENTITY(m_Broom, m_Vehicle, 0, 0.0f, 0.0f, 0.3f, -80.0f, 0.0f, 0.0f, true, false, false, false, 0, true, 0);
			}

			ped.SetInVehicle(Vehicle(m_Vehicle), -1);
			Notifications::Show("扫把飞行", "扫把已就绪：W 前进、空格上升，起飞吧！", NotificationType::Success);
		}

		virtual void OnTick() override
		{
			if (m_Notice > 0)
				m_Notice--;

			const bool veh_ok   = m_Vehicle != 0 && ENTITY::DOES_ENTITY_EXIST(m_Vehicle);
			const bool broom_ok = m_Broom != 0 && ENTITY::DOES_ENTITY_EXIST(m_Broom);

			if (!veh_ok || !broom_ok)
			{
				CleanupRig();
				SpawnRig();
				return;
			}

			// 保持隐形（网络同步有概率把可见性重置回来）
			ENTITY::SET_ENTITY_VISIBLE(m_Vehicle, false, false);
			ENTITY::SET_ENTITY_ALPHA(m_Vehicle, 0, false);
		}

		virtual void OnDisable() override
		{
			CleanupRig();
		}
	};

	static BroomFlight _BroomFlight{"broomflight", "扫把飞行", "召唤一把魔法扫把骑上去自由飞行（隐形暴君MK2驱动，禁止在别人载具里使用）。"};
}
