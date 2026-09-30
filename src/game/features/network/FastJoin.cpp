#include "core/commands/BoolCommand.hpp"
#include "game/backend/NativeHooks.hpp"
#include "game/gta/Natives.hpp"
#include "types/script/globals/GSBD.hpp"

namespace YimMenu::Features
{
	static void BroadcastHook(rage::scrNativeCallContext* ctx);

	class FastJoin : public BoolCommand
	{
		using BoolCommand::BoolCommand;

		virtual void OnEnable() override
		{
			static auto hooked = []()
			{
				NativeHooks::AddHook("freemode"_J, NativeIndex::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA, &BroadcastHook);
				NativeHooks::AddHook("fmmc_launcher"_J, NativeIndex::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA, &BroadcastHook);
				NativeHooks::AddHook("am_launcher"_J, NativeIndex::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA, &BroadcastHook);
				return true;
			}();
		}
	};

	static FastJoin _FastJoin{"fastjoin", "快速加入", "尝试通过异步加载主机广播变量来提升加入速度，部分任务可能异常，请谨慎使用"};

	static void BroadcastHook(rage::scrNativeCallContext* ctx)
	{
		if (!_FastJoin.GetState()
		    || NETWORK::NETWORK_IS_ACTIVITY_SESSION()
		    || NETWORK::NETWORK_IS_IN_TRANSITION()
		    || NETWORK::NETWORK_IS_TRANSITION_TO_GAME()
		    || NETWORK::NETWORK_IS_HOST_OF_THIS_SCRIPT()
		    || !STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS()) // if there's no player switch, we're most likely loading into a job
		{
			return ctx->SetReturnValue(NETWORK::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA());
		}
		else
		{
			if (SCRIPT::GET_HASH_OF_THIS_SCRIPT_NAME() == "freemode"_J)
			{
				GSBD::Get()->FreemodeState = eFreemodeState::RUNNING;
				GSBD::Get()->CayoPericoStrandedAnimalChoice = 1; // unbrick cayo just in case if the script host never responds
			}
			return ctx->SetReturnValue(TRUE);
		}
	}
}
