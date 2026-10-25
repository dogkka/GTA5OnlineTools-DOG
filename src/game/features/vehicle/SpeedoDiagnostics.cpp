#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

#include "core/commands/BoolCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/VehicleHandling.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	// 临时诊断：每约 5 秒把速度表相关原始数据（含调教写入值）打到日志。
	// 用途：确认「载具调校」是否真的写入、官方估算为何偏低、动态量程的输入是否正确。
	class SpeedoDiag : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;
		int m_Timer = 0;

		void OnTick() override
		{
			if (++m_Timer < 300)
				return;
			m_Timer = 0;

			auto veh = Self::GetVehicle();
			if (!veh)
				return;

			void* handling = VehicleHandling::GetHandlingData(veh);
			const float mass = handling ? VehicleHandling::ReadFloat(handling, VehicleHandling::kMassOffset) : 0.0f;
			const float accel = handling ? VehicleHandling::ReadFloat(handling, VehicleHandling::kAccelerationOffset) : 0.0f;
			const float force = handling ? VehicleHandling::ReadFloat(handling, VehicleHandling::kDriveForceOffset) : 0.0f;
			const float flat = handling ? VehicleHandling::ReadFloat(handling, VehicleHandling::kMaxFlatVelOffset) : 0.0f;
			const float flat2 = handling ? VehicleHandling::ReadFloat(handling, VehicleHandling::kMaxFlatVel2Offset) : 0.0f;
			const float brake = handling ? VehicleHandling::ReadFloat(handling, VehicleHandling::kBrakeForceOffset) : 0.0f;
			const float trac = handling ? VehicleHandling::ReadFloat(handling, VehicleHandling::kTractionMaxOffset) : 0.0f;

			const int model = ENTITY::GET_ENTITY_MODEL(veh.GetHandle());
			const float est_native = veh.GetMaxSpeed() * 3.6f;
			const float est_model = VEHICLE::GET_VEHICLE_MODEL_ESTIMATED_MAX_SPEED(model) * 3.6f;

			LOGF(INFO, "SpeedoDiag: speed {:.1f} km/h | est(native) {:.1f} est(model) {:.1f} | handling(m/s) mass {:.1f} accel {:.3f} force {:.3f} flat {:.3f} flat2 {:.3f} brake {:.3f} trac {:.3f} | gear {}/{} rev {:.3f}", veh.GetSpeed() * 3.6f, est_native, est_model, mass, accel, force, flat, flat2, brake, trac, veh.GetGear(), VEHICLE::_GET_VEHICLE_MAX_DRIVE_GEAR_COUNT(veh.GetHandle()), veh.GetRevRatio());
		}
	};

	static SpeedoDiag _SpeedoDiag{"speedodiag", "速度表诊断读数", "[临时] 每 5 秒把速度、极速估算与调教字段写入日志（确认调教是否生效）"};

	// 转速字段定位：约 8 秒内逐帧采样车辆对象前 0x1000 字节里的每个 float，
	// 与归一化转速求皮尔逊相关，把最相关的偏移打到日志，用于把「真实 RPM」接进速度表。
	class RpmFieldProbe : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		static constexpr int kFloats = 0x1000 / 4;
		static constexpr int kSamples = 480;

		std::vector<double> m_SumX;
		std::vector<double> m_SumXX;
		std::vector<double> m_SumXY;
		void* m_VehPtr = nullptr;
		int m_N = 0;
		double m_SumY = 0.0;
		double m_SumYY = 0.0;

		// 独立静态函数：仅指针/引用，无对象展开，可直接使用 __try
		static int SampleFloats(const float* base, size_t count, float rev, double* sum_x, double* sum_xx, double* sum_xy)
		{
			__try
			{
				for (size_t i = 0; i < count; ++i)
				{
					const float v = base[i];
					if (!std::isfinite(v))
						continue;
					const double x = v;
					sum_x[i] += x;
					sum_xx[i] += x * x;
					sum_xy[i] += x * rev;
				}
				return 0;
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				return 1;
			}
		}

		void OnEnable() override
		{
			m_SumX.assign(kFloats, 0.0);
			m_SumXX.assign(kFloats, 0.0);
			m_SumXY.assign(kFloats, 0.0);
			m_VehPtr = nullptr;
			m_N = 0;
			m_SumY = m_SumYY = 0.0;
			Notifications::Show("转速校准", "请正常开车 8 秒：起步、加速、松油各来几次（转速要有明显变化）。", NotificationType::Info);
		}

		void OnTick() override
		{
			auto veh = Self::GetVehicle();
			if (!veh)
				return;

			void* veh_ptr = Pointers.HandleToPtr(veh.GetHandle());
			if (!veh_ptr)
				return;

			m_VehPtr = veh_ptr;
			const float rev = veh.GetRevRatio();

			size_t floats = kFloats;
			MEMORY_BASIC_INFORMATION mbi{};
			if (VirtualQuery(veh_ptr, &mbi, sizeof(mbi)))
			{
				const uintptr_t start = reinterpret_cast<uintptr_t>(veh_ptr);
				const uintptr_t end = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
				const size_t avail = (end > start) ? (end - start) : 0;
				floats = std::min(floats, avail / 4);
			}

			const auto* base = reinterpret_cast<const float*>(veh_ptr);
			if (SampleFloats(base, floats, rev, m_SumX.data(), m_SumXX.data(), m_SumXY.data()))
			{
				Notifications::Show("转速校准", "读取车辆内存时出错，已中止。", NotificationType::Error);
				SetState(false);
				return;
			}

			m_SumY += rev;
			m_SumYY += rev * rev;
			++m_N;

			if (m_N >= kSamples)
				Finish();
		}

		void Finish()
		{
			SetState(false);

			if (m_N < 60 || m_SumY <= 0.0)
			{
				Notifications::Show("转速校准", "采样不足（转速几乎没变化），请再试一次。", NotificationType::Warning);
				return;
			}

			std::vector<std::pair<double, int>> ranked;
			ranked.reserve(kFloats);
			for (int i = 0; i < kFloats; ++i)
			{
				const double n = static_cast<double>(m_N);
				const double num = n * m_SumXY[i] - m_SumX[i] * m_SumY;
				const double den = std::sqrt(std::max(0.0, n * m_SumXX[i] - m_SumX[i] * m_SumX[i])
				    * std::max(0.0, n * m_SumYY - m_SumY * m_SumY));
				if (den <= 0.0)
					continue;
				ranked.emplace_back(num / den, i);
			}
			std::sort(ranked.begin(), ranked.end(), [](const auto& a, const auto& b) {
				return std::fabs(a.first) > std::fabs(b.first);
			});

			LOGF(INFO, "RpmProbe: samples {} | top correlations (offset / r / current):", m_N);
			const int top = std::min<int>(20, static_cast<int>(ranked.size()));
			for (int k = 0; k < top; ++k)
			{
				const int index = ranked[k].second;
				float cur = 0.0f;
				if (m_VehPtr)
					cur = *reinterpret_cast<const float*>(reinterpret_cast<uintptr_t>(m_VehPtr) + index * 4);
				LOGF(INFO, "  +0x{:04X}  r={:+.3f}  now={:.4f}", index * 4, ranked[k].first, cur);
			}

			Notifications::Show("转速校准", "采样完成：请把日志（cout.log）发回。", NotificationType::Success);
		}
	};

	static RpmFieldProbe _RpmFieldProbe{"rpmprobe", "转速字段校准", "[临时] 采样 8 秒定位真实 RPM 字段：开启后正常开车，结束后把日志发回"};
}
