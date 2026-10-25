#include "core/commands/FloatCommand.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

#include <algorithm>

namespace YimMenu::Features
{
	struct TrailParticle
	{
		const char* name;
		const char* asset;
	};

	static const TrailParticle kTrailParticles[] = {
	    {"scr_sh_lighter_sparks", "scr_fm_mp_missioncreator"},   // 打火机火花
	    {"scr_sh_bong_smoke", "scr_fm_mp_missioncreator"},       // 烟雾
	    {"scr_mp_dust_cloud", "scr_fm_mp_missioncreator"},       // 尘土
	    {"sp_fire_trail", "scr_agencyheist"},                    // 火焰尾迹
	    {"ent_sht_flame", "core"},                               // 喷火
	    {"scr_sm_counter_chaff", "scr_sm_counter"},              // 箔条烟
	    {"scr_indep_firework_starburst", "scr_indep_fireworks"}, // 烟花·星爆
	    {"scr_indep_firework_trailburst", "scr_indep_fireworks"},// 烟花·拖尾
	    {"scr_indep_firework_fountain", "scr_indep_fireworks"},  // 烟花·喷泉
	    {"scr_exp_clown", "scr_rcbarry2"},                       // 小丑爆炸
	    {"scr_alien_teleport", "scr_rcbarry1"},                  // 外星传送
	    {"scr_alien_disintegrate", "scr_rcbarry1"},              // 外星分解
	    {"scr_rcpap1_camera", "scr_rcpaparazzo1"},               // 狗仔闪光
	    {"sp_ent_sparking_wires", "scr_agencyheistb"},           // 电线火花
	};

	static ListCommand _TrailParticle{"ptfxtrailparticle", "拖尾粒子", "选择拖尾的粒子效果。",
	    {{0, "打火机火花"}, {1, "烟雾"}, {2, "尘土"}, {3, "火焰尾迹"}, {4, "喷火"}, {5, "箔条烟"}, {6, "烟花·星爆"},
	        {7, "烟花·拖尾"}, {8, "烟花·喷泉"}, {9, "小丑爆炸"}, {10, "外星传送"}, {11, "外星分解"}, {12, "狗仔闪光"}, {13, "电线火花"}},
	    6};
	static ListCommand _TrailBone{"ptfxtrailbone", "挂点骨骼", "粒子从哪个部位喷出。",
	    {{0, "胸口"}, {1, "右手"}, {2, "头顶"}, {3, "右脚"}}, 0};
	static FloatCommand _TrailScale{"ptfxtrailscale", "粒子大小", "拖尾粒子的缩放。", 0.25f, 10.0f, 1.0f};
	static FloatCommand _TrailOffset{"ptfxtrailoffset", "挂点偏移", "粒子相对骨骼的额外偏移。", 0.0f, 2.0f, 0.0f};
	// 最小 50ms：避免 0 间隔导致每帧一个网络粒子（性能与同步双风险）
	static IntCommand _TrailInterval{"ptfxtrailinterval", "间隔（毫秒）", "两次喷射之间的间隔，越小越密（最小 50）。", 50, 1000, 140};

	// 粒子拖尾：持续在骨骼上喷射网络粒子（全场可见），Stand 同款思路
	class PtfxTrails : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Acc = 0;

		int GetBone() const
		{
			switch (_TrailBone.GetState())
			{
			case 1: return 57005;  // 右手
			case 2: return 37193;  // 头顶
			case 3: return 52301;  // 右脚
			default: return 23553; // 胸口
			}
		}

		void Spawn()
		{
			auto ped = Self::GetPed();
			if (!ped)
				return;

			const auto idx = std::clamp(_TrailParticle.GetState(), 0, (int)(sizeof(kTrailParticles) / sizeof(kTrailParticles[0])) - 1);
			const auto& particle = kTrailParticles[idx];

			STREAMING::REQUEST_NAMED_PTFX_ASSET(particle.asset);
			if (!STREAMING::HAS_NAMED_PTFX_ASSET_LOADED(particle.asset))
				return;

			GRAPHICS::USE_PARTICLE_FX_ASSET(particle.asset);
			GRAPHICS::START_NETWORKED_PARTICLE_FX_NON_LOOPED_ON_PED_BONE(particle.name, ped.GetHandle(), 0.0f, 0.0f, _TrailOffset.GetState(), 0.0f, 180.0f, 0.0f, GetBone(), _TrailScale.GetState(), false, false, false);
		}

		virtual void OnTick() override
		{
			m_Acc += 16;
			if (m_Acc < std::max(0, _TrailInterval.GetState()))
				return;
			m_Acc = 0;
			Spawn();
		}
	};

	static PtfxTrails _PtfxTrails{"ptfxtrails", "粒子拖尾", "身上持续喷出粒子拖尾（全场可见，粒子/部位/大小/密度可调）。"};
}
