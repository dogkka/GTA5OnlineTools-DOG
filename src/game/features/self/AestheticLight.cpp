#include "core/commands/ColorCommand.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static ColorCommand _LightColour{"aestheticcolour", "光环颜色", "人物光环的颜色。", ImVec4{1.0f, 0.25f, 0.25f, 1.0f}};
	static FloatCommand _LightRange{"aestheticrange", "光环范围", "光照半径（米）。", 1.0f, 50.0f, 8.0f};
	static FloatCommand _LightIntensity{"aestheticintensity", "光环强度", "光照强度。", 0.1f, 100.0f, 12.0f};
	static ListCommand _LightPlacement{"aestheticplace", "光环位置", "光照挂在人物身上还是相机上。", {{0, "人物"}, {1, "相机"}}, 0};

	// 人物光环：每帧在玩家/相机位置画一盏带范围的光（Stand 同款原生）
	class AestheticLight : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			rage::fvector3 pos{};

			if (_LightPlacement.GetState() == 1)
			{
				const auto cam = CAMERA::GET_FINAL_RENDERED_CAM_COORD();
				pos = rage::fvector3{cam.x, cam.y, cam.z};
			}
			else
			{
				auto ped = Self::GetPed();
				if (!ped)
					return;
				const auto ped_pos = ped.GetPosition();
				pos = rage::fvector3{ped_pos.x, ped_pos.y, ped_pos.z + 1.0f};
			}

			const auto colour = _LightColour.GetState();
			GRAPHICS::DRAW_LIGHT_WITH_RANGEEX(pos.x, pos.y, pos.z, (int)(colour.x * 255.0f), (int)(colour.y * 255.0f), (int)(colour.z * 255.0f),
			    _LightRange.GetState(), _LightIntensity.GetState(), 0.0f);
		}
	};

	static AestheticLight _AestheticLight{"aestheticlight", "人物光环", "给自己加一圈可调颜色/范围/强度的光（本地可见）。"};
}
