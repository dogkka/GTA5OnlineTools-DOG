#include "DeletePersonalVehicle.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/PersonalVehicles.hpp"
#include "game/gta/ScriptGlobal.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	void DeletePersonalVehicle(int expectedVehicleId)
	{
		if (!*Pointers.IsSessionStarted)
		{
			Notifications::Show("删除个人载具", "请先进入 GTA 在线模式。", NotificationType::Error);
			return;
		}

		if (!PersonalVehicles::GetCurrentHandle().IsValid())
		{
			Notifications::Show("删除个人载具", "当前没有已呼出的个人载具。", NotificationType::Error);
			return;
		}

		auto vehicle = PersonalVehicles::GetCurrent();
		if (!vehicle || vehicle->GetId() != expectedVehicleId)
		{
			Notifications::Show("删除个人载具", "当前载具已经变化，删除操作已取消。", NotificationType::Warning);
			return;
		}

		// Community-derived b1158.13 freemode personal-vehicle deletion request structure.
		const ScriptGlobal request(2733138);
		if (!request.At(472).CanAccess() || !request.At(473).CanAccess() || !request.At(474).CanAccess())
		{
			Notifications::Show("删除个人载具", "删除请求的全局变量当前不可访问。", NotificationType::Error);
			return;
		}

		LOG(INFO) << "正在请求永久删除个人载具，ID：" << expectedVehicleId;
		*request.At(472).As<int*>() = expectedVehicleId;
		*request.At(473).As<int*>() = 1;
		*request.At(474).As<int*>() = 0;
		Notifications::Show("删除个人载具", "已提交永久删除请求。请稍后刷新个人载具列表。", NotificationType::Success);
	}
}
