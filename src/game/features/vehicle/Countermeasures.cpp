#include "core/backend/FiberPool.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

#include <chrono>
#include <cstdlib>

namespace YimMenu::Features
{
	static IntCommand _CmCount{"cmcount", "释放数量", "一次释放的干扰弹数量。", 1, 10, 2};
	static ListCommand _CmPattern{"cmflarepattern", "释放花样", "热诱弹的散开方式。", {{0, "水平"}, {1, "水平向上"}, {2, "水平向下"}, {3, "垂直"}, {4, "垂直向下"}, {5, "随机"}}, 0};
	static BoolCommand _CmAirOnly{"cmaironly", "仅空中载具", "只在飞机/直升机/暴君MK2 等空中载具上释放。", true};
	static BoolCommand _CmRefill{"cmrefill", "自动补满干扰弹", "持续把载具的干扰弹弹药补满。", false};
	static BoolCommand _CmAutoDeploy{"cmautodeploy", "自动部署干扰弹", "在载具中按间隔自动释放干扰弹。", false};
	static ListCommand _CmAutoType{"cmautotype", "自动部署类型", "自动释放时使用哪种干扰弹。", {{1, "热诱弹"}, {2, "箔条"}, {3, "两者都放"}}, 3};
	static IntCommand _CmAutoInterval{"cmautointerval", "自动间隔（秒）", "自动部署的间隔秒数。", 2, 15, 5};

	static void GetPatternOffset(int pattern, float& x, float& y, float& z)
	{
		switch (pattern)
		{
		case 0: x = 5.0f; y = -5.0f; z = 0.0f; break;
		case 1: x = 5.0f; y = -5.0f; z = 5.0f; break;
		case 2: x = 5.0f; y = -5.0f; z = -5.0f; break;
		case 3: x = 1.0f; y = -1.0f; z = 5.0f; break;
		case 4: x = 1.0f; y = -1.0f; z = -5.0f; break;
		default:
			x = (float)(rand() % 5 + 1);
			y = -1.0f * (float)(rand() % 5 + 1);
			z = (float)(rand() % 6) * ((rand() % 2) ? -1.0f : 1.0f);
			break;
		}
	}

	static bool IsAirborneVehicle(int veh)
	{
		const int cls = VEHICLE::GET_VEHICLE_CLASS(veh);
		if (cls == 15 || cls == 16) // 直升机 / 飞机
			return true;
		return ENTITY::GET_ENTITY_HEIGHT_ABOVE_GROUND(veh) > 3.0f;
	}

	// 热诱弹：用信号枪弹丸朝不同角度打出，和 Stand 同款做法
	static void DeployFlares(int veh, int count, int pattern)
	{
		FiberPool::Push([veh, count, pattern] {
			STREAMING::REQUEST_MODEL("w_pi_flaregun_shell"_J);
			for (int i = 0; i < 60 && !STREAMING::HAS_MODEL_LOADED("w_pi_flaregun_shell"_J); i++)
				ScriptMgr::Yield(std::chrono::milliseconds(16));

			if (!ENTITY::DOES_ENTITY_EXIST(veh))
				return;

			AUDIO::PLAY_SOUND_FROM_ENTITY(-1, "flares_released", veh, "DLC_SM_Countermeasures_Sounds", true, 0);

			const auto owner = Self::GetPed().GetHandle();
			for (int i = 0; i < count; i++)
			{
				if (!ENTITY::DOES_ENTITY_EXIST(veh))
					return;

				const auto pos = ENTITY::GET_ENTITY_COORDS(veh, true);
				float px, py, pz;
				GetPatternOffset(pattern, px, py, pz);

				const auto off1 = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(veh, -px, py, pz);
				const auto off2 = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(veh, px, py, pz);

				MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(pos.x, pos.y, pos.z, off1.x, off1.y, off1.z, 1, true, "WEAPON_FLAREGUN"_J, owner, true, true, 1.0f);
				ScriptMgr::Yield(std::chrono::milliseconds(25));
				MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(pos.x, pos.y, pos.z, off2.x, off2.y, off2.z, 1, true, "WEAPON_FLAREGUN"_J, owner, true, true, 1.0f);
				ScriptMgr::Yield(std::chrono::milliseconds(150));
			}
		});
	}

	// 箔条：网络粒子 + 3 秒内禁止被锁定（Stand 同款逻辑，用原生替代内存写入）
	static void DeployChaff(int veh)
	{
		FiberPool::Push([veh] {
			if (!ENTITY::DOES_ENTITY_EXIST(veh))
				return;

			STREAMING::REQUEST_NAMED_PTFX_ASSET("scr_sm_counter");
			for (int i = 0; i < 60 && !STREAMING::HAS_NAMED_PTFX_ASSET_LOADED("scr_sm_counter"); i++)
				ScriptMgr::Yield(std::chrono::milliseconds(16));

			if (!ENTITY::DOES_ENTITY_EXIST(veh))
				return;

			const auto pos = ENTITY::GET_ENTITY_COORDS(veh, true);
			GRAPHICS::USE_PARTICLE_FX_ASSET("scr_sm_counter");
			GRAPHICS::START_NETWORKED_PARTICLE_FX_NON_LOOPED_AT_COORD("scr_sm_counter_chaff", pos.x, pos.y, pos.z, 0.0f, 0.0f, 0.0f, 5.0f, false, false, false, false);

			VEHICLE::SET_VEHICLE_ALLOW_HOMING_MISSLE_LOCKON_SYNCED(veh, false, false);
			ScriptMgr::Yield(std::chrono::milliseconds(3000));
			if (ENTITY::DOES_ENTITY_EXIST(veh))
				VEHICLE::SET_VEHICLE_ALLOW_HOMING_MISSLE_LOCKON_SYNCED(veh, true, false);
		});
	}

	static bool CheckVehicle(bool notify)
	{
		auto veh = Self::GetVehicle();
		if (!veh)
		{
			if (notify)
				Notifications::Show("干扰弹", "请先坐进一辆载具。", NotificationType::Warning);
			return false;
		}

		if (_CmAirOnly.GetState() && !IsAirborneVehicle(veh.GetHandle()))
		{
			if (notify)
				Notifications::Show("干扰弹", "当前不是空中载具（可在设置里关闭“仅空中载具”）。", NotificationType::Warning);
			return false;
		}

		return true;
	}

	class DeployChaffCommand : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!CheckVehicle(true))
				return;
			DeployChaff(Self::GetVehicle().GetHandle());
			Notifications::Show("干扰弹", "已释放箔条（3 秒内无法被锁定）。", NotificationType::Success);
		}
	};

	class DeployFlaresCommand : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!CheckVehicle(true))
				return;
			DeployFlares(Self::GetVehicle().GetHandle(), _CmCount.GetState(), _CmPattern.GetState());
			Notifications::Show("干扰弹", "已释放热诱弹。", NotificationType::Success);
		}
	};

	class DeployBothCommand : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!CheckVehicle(true))
				return;
			DeployChaff(Self::GetVehicle().GetHandle());
			DeployFlares(Self::GetVehicle().GetHandle(), _CmCount.GetState(), _CmPattern.GetState());
			Notifications::Show("干扰弹", "已同时释放箔条与热诱弹。", NotificationType::Success);
		}
	};

	class CountermeasuresAuto : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;
		int m_FastTimer = 0;

		virtual void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (!veh)
			{
				m_Timer = 0;
				m_FastTimer = 0;
				return;
			}

			if (++m_FastTimer >= 60)
			{
				m_FastTimer = 0;
				if (_CmRefill.GetState())
					VEHICLE::SET_VEHICLE_COUNTERMEASURE_AMMO(veh.GetHandle(), 100);
			}

			if (!_CmAutoDeploy.GetState())
			{
				m_Timer = 0;
				return;
			}

			const int interval_ticks = _CmAutoInterval.GetState() * 60;
			if (++m_Timer < interval_ticks)
				return;
			m_Timer = 0;

			if (_CmAirOnly.GetState() && !IsAirborneVehicle(veh.GetHandle()))
				return;

			const int type = _CmAutoType.GetState();
			if (type == 1 || type == 3)
				DeployFlares(veh.GetHandle(), _CmCount.GetState(), _CmPattern.GetState());
			if (type == 2 || type == 3)
				DeployChaff(veh.GetHandle());
		}
	};

	static DeployChaffCommand _DeployChaff{"deploychaff", "释放箔条", "释放箔条：3 秒内锁定你的导弹会脱锁。"};
	static DeployFlaresCommand _DeployFlares{"deployflares", "释放热诱弹", "朝多种花样打出热诱弹，骗走来袭导弹。"};
	static DeployBothCommand _DeployBoth{"deployboth", "箔条+热诱弹", "同时释放箔条与热诱弹。"};
	static CountermeasuresAuto _CountermeasuresAuto{"cmrefillauto", "干扰弹自动系统", "自动补满干扰弹，并按设定间隔自动释放。"};
}
