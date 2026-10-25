#include "core/commands/LoopedCommand.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	// Stand 同款 QoL 禁用包
	class NoStuntJumps : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			MISC::CANCEL_STUNT_JUMP();
		}
	};

	class NoIdleCam : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			CAMERA::INVALIDATE_IDLE_CAM();
			CAMERA::INVALIDATE_CINEMATIC_VEHICLE_IDLE_MODE();
		}
	};

	class NoFlightMusic : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			AUDIO::SET_AUDIO_FLAG("DisableFlightMusic", true);
		}
	};

	class NoWantedMusic : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			AUDIO::SET_AUDIO_FLAG("WantedMusicDisabled", true);
		}
	};

	class NoCinematicCam : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			// 实验性：关闭脚本层影院模式（R* 相机键的切换无法从原生层直接禁）
			CAMERA::SET_CINEMATIC_MODE_ACTIVE(false);
		}
	};

	static NoStuntJumps _NoStuntJumps{"nostuntjump", "禁用特技跳跃", "被特技跳跃判定打断时自动取消（不触发慢动作与奖励）。"};
	// 命令名加 qol 前缀：TinkerScripts.lua 里已有同名 "noidlecam"，重名会让整包脚本加载失败。
	static NoIdleCam _NoIdleCam{"qolnoidlecam", "禁用挂机镜头", "挂机时不再切到闲置摄像机。"};
	static NoFlightMusic _NoFlightMusic{"noflightmusic", "禁用飞行音乐", "开飞机时不再播放飞行 BGM。"};
	static NoWantedMusic _NoWantedMusic{"nowantedmusic", "禁用通缉音乐", "被通缉时不再播放警匪追逐音乐。"};
	static NoCinematicCam _NoCinematicCam{"nocinematiccam", "禁用影院镜头（实验性）", "压制脚本层影院模式；对 R* 相机键的影院切换无效。"};
}
