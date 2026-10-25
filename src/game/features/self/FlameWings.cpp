#include "core/commands/BoolCommand.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static FloatCommand _FlameWingsSize{"flamewingssize", "火焰大小", "火焰翅膀的粒子尺寸。", 0.5f, 6.0f, 2.2f};
	static FloatCommand _FlameWingsSpread{"flamewingsspread", "火焰间距", "左右两团火焰离身体中线的距离（米）。", 0.1f, 1.2f, 0.45f};
	static FloatCommand _FlameWingsHeight{"flamewingsheight", "火焰高度", "火焰挂点的高度（相对脚底，1.2 左右为背部）。", -0.2f, 2.0f, 1.2f};
	static BoolCommand _FlameWingsBurst{"flamewingsburst", "爆燃脉冲", "在两只火焰翅膀上周期性地爆燃，让火焰更狂野。", true};

	// 火焰翅膀：在角色背部两侧持续燃烧两团火焰，并周期性爆燃（粒子特效）
	class FlameWings : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_LeftFx  = 0;
		int m_RightFx = 0;
		int m_Owner   = 0;
		int m_Timer   = 0;
		bool m_Side   = false;

		void StopAll()
		{
			if (m_LeftFx && GRAPHICS::DOES_PARTICLE_FX_LOOPED_EXIST(m_LeftFx))
				GRAPHICS::STOP_PARTICLE_FX_LOOPED(m_LeftFx, false);
			if (m_RightFx && GRAPHICS::DOES_PARTICLE_FX_LOOPED_EXIST(m_RightFx))
				GRAPHICS::STOP_PARTICLE_FX_LOOPED(m_RightFx, false);

			m_LeftFx  = 0;
			m_RightFx = 0;
		}

		void Start()
		{
			auto ped = Self::GetPed();
			if (!ped)
				return;

			STREAMING::REQUEST_NAMED_PTFX_ASSET("des_trailerpark");
			if (!STREAMING::HAS_NAMED_PTFX_ASSET_LOADED("des_trailerpark"))
				return; // 资源尚未就绪，下一帧再试

			const int handle = ped.GetHandle();
			m_Owner          = handle;

			const float spread = _FlameWingsSpread.GetState();
			const float height = _FlameWingsHeight.GetState();
			const float scale  = _FlameWingsSize.GetState();

			GRAPHICS::USE_PARTICLE_FX_ASSET("des_trailerpark");
			m_LeftFx  = GRAPHICS::START_PARTICLE_FX_LOOPED_ON_ENTITY("ent_ray_trailerpark_fires", handle, spread, -0.1f, height, 0.0f, 0.0f, 0.0f, scale, false, false, false);
			m_RightFx = GRAPHICS::START_PARTICLE_FX_LOOPED_ON_ENTITY("ent_ray_trailerpark_fires", handle, -spread, -0.1f, height, 0.0f, 0.0f, 0.0f, scale, false, false, false);
		}

		virtual void OnTick() override
		{
			auto ped = Self::GetPed();
			if (!ped)
			{
				StopAll();
				return;
			}

			// 换角色 / 重生 / 粒子丢失 → 重建
			if (m_LeftFx == 0 || m_RightFx == 0 || m_Owner != ped.GetHandle() || !GRAPHICS::DOES_PARTICLE_FX_LOOPED_EXIST(m_LeftFx)
			    || !GRAPHICS::DOES_PARTICLE_FX_LOOPED_EXIST(m_RightFx))
			{
				StopAll();
				Start();
				return;
			}

			// 爆燃脉冲：左右交替打火球，火焰更狂野
			if (_FlameWingsBurst.GetState())
			{
				if (++m_Timer >= 24) // 约 0.4 秒
				{
					m_Timer = 0;
					m_Side  = !m_Side;

					STREAMING::REQUEST_NAMED_PTFX_ASSET("core");
					if (STREAMING::HAS_NAMED_PTFX_ASSET_LOADED("core"))
					{
						const float spread = _FlameWingsSpread.GetState();
						const float height = _FlameWingsHeight.GetState();
						const float scale  = _FlameWingsSize.GetState() * 0.6f;

						GRAPHICS::USE_PARTICLE_FX_ASSET("core");
						GRAPHICS::START_PARTICLE_FX_NON_LOOPED_ON_ENTITY("exp_air_molotov", ped.GetHandle(), m_Side ? spread : -spread, -0.1f, height, 0.0f, 0.0f, 0.0f, scale, false, false, false);
					}
				}
			}
		}

		virtual void OnDisable() override
		{
			StopAll();
			m_Owner = 0;
		}
	};

	static FlameWings _FlameWings{"flamewings", "火焰翅膀", "在角色背后生成两只燃烧的火焰翅膀（粒子特效，大小/间距/高度可调）。"};
}
