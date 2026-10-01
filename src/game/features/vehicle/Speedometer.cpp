#include "core/commands/LoopedCommand.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static FloatCommand _SpeedometerX{"speedox", "速度表横向位置", "速度表在屏幕上的横向位置（0=最左，1=最右）；也可在菜单里拖动圆点设置", 0.0f, 1.0f, 1.0f};
	static FloatCommand _SpeedometerY{"speedoy", "速度表纵向位置", "速度表在屏幕上的纵向位置（0=最顶，1=最底）；也可在菜单里拖动圆点设置", 0.0f, 1.0f, 0.85f};
	static ListCommand _SpeedometerStyle{"speedostyle", "速度表样式", "选择速度表外观：原版运动表 / 双圆盘 / 环形组合 / 数字极简（后三种均为转速+速度组合表）", {{0, "原版运动表"}, {1, "双圆盘"}, {2, "环形组合"}, {3, "数字极简"}}, 0};
	static ListCommand _SpeedometerUnits{"speedounits", "速度表单位", "速度显示单位：跟随游戏设置 / 公里每时 / 英里每时", {{0, "跟随游戏"}, {1, "公里/时"}, {2, "英里/时"}}, 0};

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
			// 自绘样式（1-3）由 SpeedoHUD 渲染；确保 scaleform 已释放
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
			GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(m_ScaleformHandle, "SET_METER_VALUE");
			GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_FLOAT(veh.GetSpeed() / veh.GetMaxSpeed());
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
