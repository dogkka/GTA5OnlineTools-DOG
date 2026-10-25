#pragma once

namespace YimMenu
{
	class Vehicle;

	// 自绘速度表 HUD（样式 1-5，全透明背景、支持整体缩放；样式 0 为原版 scaleform，由 Features::Speedometer 处理）
	class SpeedoHUD
	{
	public:
		static void Draw();

		// 动态量程（km/h）：按实时调教字段 + 官方估算 + 实测峰值自动估算极速，含余量与平滑。
		// 供原版运动表（样式 0）等复用；返回 false 表示当前不可用（无载具等）。
		static bool ComputeRange(Vehicle veh, float speed_mps, float& est_kmh, float& range_kmh);
	};
}
