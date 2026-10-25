#include <algorithm>

#include "core/commands/LoopedCommand.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/frontend/SpeedoHUD.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static FloatCommand _SpeedometerX{"speedox", "速度表横向位置", "速度表在屏幕上的横向位置（0=最左，1=最右）；也可在菜单里拖动圆点设置", 0.0f, 1.0f, 1.0f};
	static FloatCommand _SpeedometerY{"speedoy", "速度表纵向位置", "速度表在屏幕上的纵向位置（0=最顶，1=最底）；也可在菜单里拖动圆点设置", 0.0f, 1.0f, 0.85f};
	static ListCommand _SpeedometerStyle{"speedostyle", "速度表样式", "选择速度表外观：原版运动表 / 双圆盘 / 环形组合 / 数字极简 / 科技圆盘 / 全息 HUD（自绘样式均为透明背景、可调大小）", {{0, "原版运动表"}, {1, "双圆盘"}, {2, "环形组合"}, {3, "数字极简"}, {4, "科技圆盘"}, {5, "全息 HUD"}}, 0};
	static ListCommand _SpeedometerUnits{"speedounits", "速度表单位", "速度显示单位：跟随游戏设置 / 公里每时 / 英里每时", {{0, "跟随游戏"}, {1, "公里/时"}, {2, "英里/时"}}, 0};
	static FloatCommand _SpeedometerScale{"speedoscale", "速度表大小", "自绘速度表的整体缩放（0.5 ~ 2.0 倍）；原版运动表不受影响", 0.5f, 2.0f, 1.0f};
	// ---- 动态量程 / 动态转速（2026-10-06）----
	static ListCommand _SpeedometerRange{"speedorange", "速度表量程", "满刻度来源：跟随车辆（按实时调教与实测峰值自动估算）或手动固定", {{0, "跟随车辆（动态）"}, {1, "手动固定"}}, 0};
	static FloatCommand _SpeedometerRangeAdd{"speedorangeadd", "量程余量", "动态量程比估算极速多留的余量（km/h），防止指针顶到头", 30.0f, 50.0f, 40.0f};
	static FloatCommand _SpeedometerRangeManual{"speedorangemanual", "手动量程", "手动模式下的满刻度（km/h）", 120.0f, 600.0f, 300.0f};
	static FloatCommand _SpeedometerWarn{"speedowarn", "时速爆闪起点", "达到表内估算极速的百分比后开始爆闪（真车一般在 85%~90%）", 0.80f, 0.95f, 0.88f};
	static FloatCommand _RpmWarn{"rpmwarn", "转速爆闪起点", "转速达到红线百分比后开始爆闪", 0.75f, 0.98f, 0.88f};
	static BoolCommand _RpmGearMode{"rpmgearmode", "转速换挡提示", "非顶档时接近断油才硬闪（换挡提示灯逻辑），避免常态闪烁", true};
	static FloatCommand _RpmRedline{"rpmredline", "转速满刻度", "转速数字的分母（千转/分）；游戏只给 0~1 归一化转速，找不到真值时的显示缩放", 6.0f, 12.0f, 8.0f};


	static bool UseMetricUnits()
	{
		const int units = _SpeedometerUnits.GetState();
		if (units == 1)
			return true;
		if (units == 2)
			return false;
		return MISC::SHOULD_USE_METRIC_MEASUREMENTS();
	}

	class Speedometer : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;
		int m_ScaleformHandle{};

		bool EnsureScaleformLoaded()
		{
			if (GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(m_ScaleformHandle) && GRAPHICS::HAS_SCALEFORM_MOVIE_FILENAME_LOADED("DRAG_RACE"))
				return true;

			m_ScaleformHandle = GRAPHICS::REQUEST_SCALEFORM_MOVIE("DRAG_RACE");
			return false;
		}

		int GetVehicleSpeed(Vehicle veh)
		{
			auto speed = veh.GetSpeed();

			if (UseMetricUnits())
				return speed * 3.6f;
			else
				return speed * 2.23694f;
		}

		virtual void OnTick() override
		{
			// 自绘样式（1-5）由 SpeedoHUD 渲染；确保 scaleform 已释放
			if (_SpeedometerStyle.GetState() != 0)
			{
				if (m_ScaleformHandle)
				{
					if (GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(m_ScaleformHandle))
						GRAPHICS::SET_SCALEFORM_MOVIE_AS_NO_LONGER_NEEDED(&m_ScaleformHandle);
					m_ScaleformHandle = 0;
				}
				return;
			}

			auto veh = Self::GetVehicle();

			if (!veh || !EnsureScaleformLoaded() || STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS())
				return;

			GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(m_ScaleformHandle, "SET_GEAR");
			GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(veh.GetGear());
			GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
			GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(m_ScaleformHandle, "SET_SPEED");
			GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(GetVehicleSpeed(veh));
			GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
			GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(m_ScaleformHandle, "SET_SPEED_UNITS");
			GRAPHICS::BEGIN_TEXT_COMMAND_SCALEFORM_STRING("STRING");
			HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(UseMetricUnits() ? "公里/时" : "英里/时");
			GRAPHICS::END_TEXT_COMMAND_SCALEFORM_STRING();
			GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
			// 表盘比例跟随动态量程（与自绘样式一致），避免调教车把原版表顶满
			float meter = veh.GetSpeed() / std::max(veh.GetMaxSpeed(), 1.0f);
			float est_kmh = 0.0f;
			float range_kmh = 0.0f;
			if (SpeedoHUD::ComputeRange(veh, veh.GetSpeed(), est_kmh, range_kmh) && range_kmh > 1.0f)
				meter = veh.GetSpeed() * 3.6f / range_kmh;
			meter = std::clamp(meter, 0.0f, 1.0f);

			GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(m_ScaleformHandle, "SET_METER_VALUE");
			GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_FLOAT(meter);
			GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
			GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(m_ScaleformHandle, "SET_OUTER_GOAL");
			GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_FLOAT(-1.0f);
			GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_FLOAT(-1.0f);
			GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
			GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(m_ScaleformHandle, "SET_INNER_GOAL");
			GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_FLOAT(-1.0f);
			GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_FLOAT(-1.0f);
			GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
			GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(m_ScaleformHandle, "SET_SCREEN_POSITION");
			GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_FLOAT(_SpeedometerX.GetState());
			GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_FLOAT(_SpeedometerY.GetState());
			GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
			GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(m_ScaleformHandle, "SET_IS_DRIFT_RACE");
			GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_BOOL(false);
			GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
			GRAPHICS::DRAW_SCALEFORM_MOVIE_FULLSCREEN(m_ScaleformHandle, 255, 255, 255, 255, 0);
		}

		virtual void OnDisable() override
		{
			if (GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(m_ScaleformHandle) && GRAPHICS::HAS_SCALEFORM_MOVIE_FILENAME_LOADED("DRAG_RACE"))
				GRAPHICS::SET_SCALEFORM_MOVIE_AS_NO_LONGER_NEEDED(&m_ScaleformHandle);

			m_ScaleformHandle = 0;
		}
	};

	static Speedometer _Speedometer{"speedometer", "速度表", "当你在载具中时显示速度表"};
}
