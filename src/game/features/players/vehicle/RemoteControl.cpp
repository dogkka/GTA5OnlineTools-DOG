#include <algorithm>
#include <initializer_list>

#include "core/commands/BoolCommand.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/frontend/GUI.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Vehicle.hpp"
#include "types/pad/ControllerInputs.hpp"

namespace YimMenu::Features
{
	static int g_RemoteControlHandle = 0;
	static bool g_RemoteControlNoControlNotice = false;

	static FloatCommand _RemoteControlSpeed{"remotecontrolspeed", "遥控速度", "遥控驾驶时的巡航速度（km/h）", 20.0f, 300.0f, 90.0f};

	class RemoteControlTarget : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			auto veh = player.GetPed().GetVehicle();
			if (!veh)
			{
				Notifications::Show("遥控驾驶", "该玩家不在载具中。", NotificationType::Warning);
				return;
			}

			g_RemoteControlHandle = veh.GetHandle();
			g_RemoteControlNoControlNotice = false;

			if (auto cmd = Commands::GetCommand<BoolCommand>("remotecontrol"_J))
				cmd->SetState(true);

			Notifications::Show("遥控驾驶", "已锁定目标载具：W/S 前进倒退，A/D 转向。对方正在驾驶时等他下车后自动接管。", NotificationType::Info);
		}
	};

	static RemoteControlTarget _RemoteControlTarget{"rcvehicle", "遥控载具", "锁定目标玩家的载具，之后用 W/A/S/D 遥控它"};

	// 遥控驾驶：先抢实体控制权，拿到后每帧把本地 W/A/S/D 输入映射到目标载具。
	// 本机没有控制权（目标正在驾驶）时不硬顶，等对方下车后自动接管。
	class RemoteControl : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_NoControlTicks = 0;

		virtual void OnDisable() override
		{
			if (g_RemoteControlHandle != 0 && ENTITY::DOES_ENTITY_EXIST(g_RemoteControlHandle))
			{
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(g_RemoteControlHandle, 0.0f);
				VEHICLE::SET_VEHICLE_DOORS_LOCKED(g_RemoteControlHandle, 1);
			}

			g_RemoteControlHandle = 0;
			m_NoControlTicks = 0;
		}

		virtual void OnTick() override
		{
			if (g_RemoteControlHandle == 0)
				return;

			if (!ENTITY::DOES_ENTITY_EXIST(g_RemoteControlHandle))
			{
				Notifications::Show("遥控驾驶", "目标载具已不存在，遥控结束。", NotificationType::Warning);
				g_RemoteControlHandle = 0;
				SetState(false);
				return;
			}

			const int handle = g_RemoteControlHandle;

			if (!NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(handle))
			{
				NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(handle);

				if (!g_RemoteControlNoControlNotice && ++m_NoControlTicks > 120)
				{
					g_RemoteControlNoControlNotice = true;
					Notifications::Show("遥控驾驶", "暂时无法接管：对方可能正在驾驶。等他下车后会自动接管。", NotificationType::Warning);
				}
				return;
			}

			m_NoControlTicks = 0;

			// 菜单打开时只保活、不吃键，避免翻菜单时把车开跑
			if (GUI::IsOpen())
				return;

			VEHICLE::SET_VEHICLE_ENGINE_ON(handle, true, true, false);
			VEHICLE::SET_VEHICLE_DOORS_LOCKED(handle, 4);

			// 遥控期间屏蔽本机走路的 WASD，避免自己的角色跟着跑（按键本身仍可被读取）
			PAD::DISABLE_CONTROL_ACTION(0, static_cast<int>(ControllerInputs::INPUT_MOVE_UP_ONLY), true);
			PAD::DISABLE_CONTROL_ACTION(0, static_cast<int>(ControllerInputs::INPUT_MOVE_DOWN_ONLY), true);
			PAD::DISABLE_CONTROL_ACTION(0, static_cast<int>(ControllerInputs::INPUT_MOVE_LEFT_ONLY), true);
			PAD::DISABLE_CONTROL_ACTION(0, static_cast<int>(ControllerInputs::INPUT_MOVE_RIGHT_ONLY), true);
			PAD::DISABLE_CONTROL_ACTION(0, static_cast<int>(ControllerInputs::INPUT_SPRINT), true);

			const float cruise = std::clamp(_RemoteControlSpeed.GetState(), 20.0f, 300.0f) / 3.6f;

			// 键盘（步行控制组）和手柄（载具控制组）两套输入都读，谁按下听谁的
			auto pressed = [](std::initializer_list<ControllerInputs> inputs) {
				for (auto input : inputs)
				{
					if (PAD::IS_CONTROL_PRESSED(0, static_cast<int>(input))
					    || PAD::IS_DISABLED_CONTROL_PRESSED(0, static_cast<int>(input)))
						return true;
				}
				return false;
			};

			const bool accel = pressed({ControllerInputs::INPUT_VEH_ACCELERATE,
			    ControllerInputs::INPUT_VEH_MOVE_UP_ONLY,
			    ControllerInputs::INPUT_MOVE_UP_ONLY});
			const bool brake = pressed({ControllerInputs::INPUT_VEH_BRAKE,
			    ControllerInputs::INPUT_VEH_MOVE_DOWN_ONLY,
			    ControllerInputs::INPUT_MOVE_DOWN_ONLY});
			const bool turn_left = pressed({ControllerInputs::INPUT_VEH_MOVE_LEFT_ONLY, ControllerInputs::INPUT_MOVE_LEFT_ONLY});
			const bool turn_right = pressed({ControllerInputs::INPUT_VEH_MOVE_RIGHT_ONLY, ControllerInputs::INPUT_MOVE_RIGHT_ONLY});

			if (accel && !brake)
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(handle, cruise);
			else if (brake && !accel)
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(handle, -cruise * 0.4f);
			else if (accel && brake)
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(handle, 0.0f);

			if (turn_left || turn_right)
			{
				const auto rot    = ENTITY::GET_ENTITY_ROTATION(handle, 2);
				const float blend = std::clamp(ENTITY::GET_ENTITY_SPEED(handle) / 10.0f + 0.4f, 0.4f, 1.6f);
				const float delta = (turn_left ? -1.0f : 1.0f) * blend * 1.1f;
				ENTITY::SET_ENTITY_ROTATION(handle, rot.x, rot.y, rot.z + delta, 2, true);
			}
		}
	};

	static RemoteControl _RemoteControl{"remotecontrol", "遥控开关", "用你的按键驾驶已锁定的目标载具"};
}
