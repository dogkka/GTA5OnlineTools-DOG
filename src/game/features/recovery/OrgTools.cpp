#include "core/commands/Command.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/util/Joaat.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/ScriptGlobal.hpp"

#include <string>

namespace YimMenu::Features
{
	static IntCommand _OrgColourSlot{"ceoslot", "颜色槽位", "组织颜色槽位（0-9）。", 0, 9, 0};
	static IntCommand _OrgColourValue{"ceocolour", "颜色编号", "组织颜色编号（0-14）。", 0, 14, 0};

	// 组织颜色槽：写入 GSBD 颜色槽表（基址为我们版本已验证的 GSBD=2649164，内部偏移来自 Stand 参考）
	class ApplyOrgColour : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			const int slot  = _OrgColourSlot.GetState();
			const int value = _OrgColourValue.GetState();

			auto glb = ScriptGlobal(2649164 + 818 + 44).At(slot, 2).At(1);
			if (!glb.CanAccess())
			{
				Notifications::Show("组织颜色", "全局不可访问（请先进入在线战局）。", NotificationType::Warning);
				return;
			}

			*glb.As<int*>() = value;
			Notifications::Show("组织颜色", "已写入槽位 " + std::to_string(slot) + "（实验性，需自己是组织老大）。", NotificationType::Success);
		}
	};

	// 解除 CEO/MC 注册封禁：清空组织封禁计时 + 清除 VIP 禁用统计
	class OrgAppeal : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			const int id = Self::GetPlayer().GetId();

			// GPBD_FM_3（我们版本索引 1893070），组织封禁计时相对偏移来自 Stand 参考
			auto glb = ScriptGlobal(1893070).At(id, 615).At(10).At(182);
			if (glb.CanAccess())
				*glb.As<int*>() = 0;

			STATS::STAT_SET_INT("MPPLY_VIPGAMEPLAYDISABLEDTIMER"_J, 0, true);
			Notifications::Show("组织封禁", "已尝试清除 CEO/MC 注册封禁（实验性）。", NotificationType::Info);
		}
	};

	static ApplyOrgColour _ApplyOrgColour{"ceocolourapply", "应用组织颜色", "把所选颜色写入所选组织颜色槽（实验性）。"};
	static OrgAppeal _OrgAppeal{"ceoappeal", "解除组织封禁", "清除 CEO/MC 注册封禁计时（实验性）。"};
}
