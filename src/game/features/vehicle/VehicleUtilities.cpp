#include "VehicleDeliveryCooldown.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	namespace
	{
		Vehicle GetTargetVehicle()
		{
			auto ped = Self::GetPed();
			if (!ped.IsValid())
				return nullptr;

			auto vehicle = Self::GetVehicle();
			if (!vehicle.IsValid())
				vehicle = ped.GetLastVehicle();
			return vehicle;
		}

		Vehicle RequireVehicle(std::string_view title)
		{
			auto vehicle = GetTargetVehicle();
			if (!vehicle.IsValid())
				Notifications::Show(std::string(title), "请先进入一辆载具，或靠近最近驾驶的载具。", NotificationType::Error);
			return vehicle;
		}
	}

	static std::vector<std::pair<int, const char*>> vehicleSeats = {
	    {-1, "驾驶位"},
	    {0, "前排乘客位"},
	    {1, "乘客位 2"},
	    {2, "乘客位 3"},
	    {3, "乘客位 4"},
	    {4, "乘客位 5"},
	    {5, "乘客位 6"},
	    {6, "乘客位 7"},
	    {7, "乘客位 8"},
	    {8, "乘客位 9"},
	    {9, "乘客位 10"},
	    {10, "乘客位 11"},
	    {11, "乘客位 12"},
	    {12, "乘客位 13"},
	    {13, "乘客位 14"},
	    {14, "乘客位 15"},
	    {15, "乘客位 16"},
	};

	static ListCommand _VehicleSeat{"vehicleseat", "座位", "选择要进入的载具座位。", vehicleSeats, -1};

	class EnterVehicleSeat final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			auto vehicle = RequireVehicle("切换载具座位");
			if (!vehicle.IsValid())
				return;

			const int seat = _VehicleSeat.GetState();
			const int maxPassengers = vehicle.GetMaxNumOfPassengers();
			if (seat < -1 || (seat >= 0 && seat >= maxPassengers))
			{
				Notifications::Show("切换载具座位", std::format("该载具只有 {} 个乘客位。", maxPassengers), NotificationType::Error);
				return;
			}
			if (!vehicle.IsSeatFree(seat))
			{
				Notifications::Show("切换载具座位", "所选座位已被占用。", NotificationType::Warning);
				return;
			}

			Self::GetPed().SetInVehicle(vehicle, seat);
			Notifications::Show("切换载具座位", "已进入所选座位。", NotificationType::Success);
		}
	};

	class ToggleVehicleDoors final : public Command
	{
		bool m_Open;

	public:
		ToggleVehicleDoors(std::string name, std::string label, std::string description, bool open) :
		    Command(std::move(name), std::move(label), std::move(description)),
		    m_Open(open)
		{
		}

		void OnCall() override
		{
			auto vehicle = RequireVehicle(m_Open ? "打开全部车门" : "关闭全部车门");
			if (!vehicle.IsValid())
				return;

			vehicle.ToggleAllDoors(m_Open);
			Notifications::Show(m_Open ? "打开全部车门" : "关闭全部车门", m_Open ? "车门已打开。" : "车门已关闭。", NotificationType::Success);
		}
	};

	static std::vector<std::pair<int, const char*>> hydraulicWheels = {
	    {0, "左前轮"},
	    {1, "右前轮"},
	    {2, "左后轮"},
	    {3, "右后轮"},
	};

	static ListCommand _HydraulicWheel{"hydraulicwheel", "车轮", "选择要控制的液压车轮。", hydraulicWheels, 0};
	static FloatCommand _HydraulicFactor{"hydraulicfactor", "抬升幅度", "设置所选液压车轮的抬升幅度。", 1.0f, 3.0f, 1.0f};

	class ControlHydraulicWheel final : public Command
	{
		bool m_Raise;

	public:
		ControlHydraulicWheel(std::string name, std::string label, std::string description, bool raise) :
		    Command(std::move(name), std::move(label), std::move(description)),
		    m_Raise(raise)
		{
		}

		void OnCall() override
		{
			auto vehicle = RequireVehicle(m_Raise ? "抬升液压车轮" : "放下液压车轮");
			if (!vehicle.IsValid())
				return;

			const bool changed = m_Raise ? vehicle.RaiseHydraulicWheel(_HydraulicWheel.GetState(), _HydraulicFactor.GetState()) :
			                               vehicle.LowerHydraulicWheel(_HydraulicWheel.GetState(), _HydraulicFactor.GetState());
			if (!changed)
			{
				Notifications::Show(m_Raise ? "抬升液压车轮" : "放下液压车轮", "该载具不支持液压悬挂，或车轮选择无效。", NotificationType::Warning);
				return;
			}

			Notifications::Show(m_Raise ? "抬升液压车轮" : "放下液压车轮", "液压车轮状态已更新。", NotificationType::Success);
		}
	};

	class ResetDeliveryCooldown final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!Pointers.IsSessionStarted || !*Pointers.IsSessionStarted)
			{
				Notifications::Show("重置载具配送冷却", "请先进入 GTA 在线模式。", NotificationType::Error);
				return;
			}

			if (!ResetVehicleDeliveryCooldown())
			{
				Notifications::Show("重置载具配送冷却", "配送冷却变量当前不可访问。", NotificationType::Error);
				return;
			}

			Notifications::Show("重置载具配送冷却", "技工与个人载具配送冷却已重置。", NotificationType::Success);
		}
	};

	static EnterVehicleSeat _EnterVehicleSeat{"entervehicleseat", "进入所选座位", "进入当前或最近驾驶载具的所选空座位。"};
	static ToggleVehicleDoors _OpenVehicleDoors{"openvehicledoors", "打开全部车门", "打开当前或最近驾驶载具的全部车门。", true};
	static ToggleVehicleDoors _CloseVehicleDoors{"closevehicledoors", "关闭全部车门", "关闭当前或最近驾驶载具的全部车门。", false};
	static ControlHydraulicWheel _RaiseHydraulicWheel{"raisehydraulicwheel", "抬升所选车轮", "抬升支持液压悬挂载具的所选车轮。", true};
	static ControlHydraulicWheel _LowerHydraulicWheel{"lowerhydraulicwheel", "放下所选车轮", "放下支持液压悬挂载具的所选车轮。", false};
	static ResetDeliveryCooldown _ResetDeliveryCooldown{"resetvehicledeliverycooldown", "重置载具配送冷却", "重置技工与个人载具配送冷却计时器。"};
}
