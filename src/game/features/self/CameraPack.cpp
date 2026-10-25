#include "core/commands/BoolCommand.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static ListCommand _CamViewMode{"camviewmode", "强制视角", "强制第一/第三人称视角（默认不干预）。", {{0, "不干预"}, {1, "第三人称"}, {2, "第一人称"}}, 0};
	static IntCommand _CamVehZoom{"camvehzoom", "载具镜头远近", "载具第三人称镜头远近（-1 关闭，数值越大越远）。", -1, 6, -1};
	static BoolCommand _CamNoShake{"camnoshake", "禁用镜头晃动", "关闭爆炸/撞击造成的镜头晃动。", false};
	static FloatCommand _CamFov{"camfov", "视野 FOV（实验性）", "覆盖游戏视野角度，-1 关闭。实验性：如无效果或异常请关闭。", -1.0f, 360.0f, -1.0f};

	class CameraPack : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			const int vm = _CamViewMode.GetState();
			if (vm == 1)
				CAMERA::SET_FOLLOW_PED_CAM_VIEW_MODE(0);
			else if (vm == 2)
				CAMERA::SET_FOLLOW_PED_CAM_VIEW_MODE(4);

			const int zoom = _CamVehZoom.GetState();
			if (zoom >= 0 && Self::GetVehicle())
				CAMERA::SET_FOLLOW_VEHICLE_CAM_ZOOM_LEVEL(zoom);

			if (_CamNoShake.GetState())
			{
				CAMERA::STOP_GAMEPLAY_CAM_SHAKING(true);
				CAMERA::SET_GAMEPLAY_CAM_SHAKE_AMPLITUDE(0.0f);
			}

			const float fov = _CamFov.GetState();
			if (fov >= 0.0f)
			{
				const auto cam = CAMERA::GET_RENDERING_CAM();
				if (cam)
					CAMERA::SET_CAM_FOV(cam, fov);
			}
		}
	};

	static CameraPack _CameraPack{"camerapack", "相机增强", "强制视角切换、载具镜头远近、禁用镜头晃动、实验性 FOV。"};
}
