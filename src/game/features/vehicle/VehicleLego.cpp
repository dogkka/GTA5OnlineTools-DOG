#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/StringCommand.hpp"
#include "core/commands/Vector3Command.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/util/Joaat.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Entity.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Object.hpp"
#include "game/gta/Vehicle.hpp"

#include <string>
#include <vector>

namespace YimMenu::Features
{
	static StringCommand _LegoModel{"legomodel", "模型名称", "要拼接的载具或物体模型名（如 adder、sultan2、prop_container_01a）。", "adder"};
	static ListCommand _LegoMode{"legomode", "挂接方式", "新部件挂到哪个父节点上。", {{0, "挂在当前载具"}, {1, "挂在上一件"}}, 0};
	static Vector3Command _LegoOffset{"legooffset", "位置偏移", "部件相对挂点的 X/Y/Z 偏移（米）。", rage::fvector3{0.0f, 3.0f, 0.6f}};
	static Vector3Command _LegoRot{"legorot", "旋转角度", "部件相对挂点的旋转角度（度）。", rage::fvector3{0.0f, 0.0f, 0.0f}};

	static std::vector<int> g_LegoParts{};
	static int g_LegoLast = 0;

	static void PurgeDeadParts()
	{
		std::erase_if(g_LegoParts, [](int handle) { return handle == 0 || !ENTITY::DOES_ENTITY_EXIST(handle); });
		if (g_LegoLast != 0 && !ENTITY::DOES_ENTITY_EXIST(g_LegoLast))
		{
			g_LegoLast = g_LegoParts.empty() ? 0 : g_LegoParts.back();
		}
	}

	// 载具拼接（乐高式）：把任意官方载具/物体挂到当前载具或上一件部件上，拼出异形载具
	class LegoAttach : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			PurgeDeadParts();

			auto ped = Self::GetPed();
			if (!ped)
				return;

			const bool use_last = _LegoMode.GetState() == 1;
			int parent          = 0;

			if (use_last)
			{
				parent = g_LegoLast;
				if (parent == 0 || !ENTITY::DOES_ENTITY_EXIST(parent))
				{
					Notifications::Show("载具拼接", "还没有上一件部件，请先把挂接方式切回“当前载具”。", NotificationType::Warning);
					return;
				}
			}
			else
			{
				auto veh = Self::GetVehicle();
				if (!veh)
				{
					Notifications::Show("载具拼接", "请先坐进一辆载具（或在挂接方式里选择“挂在上一件”）。", NotificationType::Warning);
					return;
				}

				parent = veh.GetHandle();
			}

			const auto model_name = _LegoModel.GetString();
			if (model_name.empty())
			{
				Notifications::Show("载具拼接", "请先填写模型名称。", NotificationType::Warning);
				return;
			}

			const Hash model = Joaat(model_name);
			if (!STREAMING::IS_MODEL_IN_CDIMAGE(model))
			{
				Notifications::Show("载具拼接", "模型不存在：" + model_name, NotificationType::Error);
				return;
			}

			const auto native_pos = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped.GetHandle(), 0.0f, 3.0f, 0.5f);
			const rage::fvector3 spawn_pos{native_pos.x, native_pos.y, native_pos.z};

			Entity part(0);
			if (STREAMING::IS_MODEL_A_VEHICLE(model))
			{
				auto veh = Vehicle::Create(model, spawn_pos, ped.GetHeading(), false);
				if (!veh)
				{
					Notifications::Show("载具拼接", "生成载具失败：" + model_name, NotificationType::Error);
					return;
				}
				part = Entity(veh.GetHandle());
			}
			else
			{
				auto obj = Object::Create(model, spawn_pos);
				if (!obj)
				{
					Notifications::Show("载具拼接", "生成物体失败：" + model_name, NotificationType::Error);
					return;
				}
				part = Entity(obj.GetHandle());
			}

			// 冻结 + 关碰撞，挂上后就不会被物理拖拽或推挤
			ENTITY::SET_ENTITY_COLLISION(part.GetHandle(), false, false);
			ENTITY::FREEZE_ENTITY_POSITION(part.GetHandle(), true);

			const auto offset = _LegoOffset.GetState();
			const auto rot    = _LegoRot.GetState();
			ENTITY::ATTACH_ENTITY_TO_ENTITY(part.GetHandle(), parent, 0, offset.x, offset.y, offset.z, rot.x, rot.y, rot.z, true, false, false, false, 0, true, 0);

			g_LegoParts.push_back(part.GetHandle());
			g_LegoLast = part.GetHandle();

			Notifications::Show("载具拼接", "已拼接 " + model_name + "（共 " + std::to_string(g_LegoParts.size()) + " 件）。", NotificationType::Success);
		}
	};

	class LegoClear : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			PurgeDeadParts();

			for (int handle : g_LegoParts)
			{
				if (handle != 0 && ENTITY::DOES_ENTITY_EXIST(handle))
					Entity(handle).Delete();
			}

			g_LegoParts.clear();
			g_LegoLast = 0;

			Notifications::Show("载具拼接", "已清除全部拼接部件。", NotificationType::Info);
		}
	};

	static LegoAttach _LegoAttach{"legoattach", "拼接一件", "把指定模型拼接上去（可反复点击叠加，用“挂接方式”做成接龙）。"};
	static LegoClear _LegoClear{"legoclear", "清除拼接", "删除当前所有已拼接的部件。"};
}
