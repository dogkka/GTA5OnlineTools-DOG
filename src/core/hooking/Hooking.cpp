#include "Hooking.hpp"

#include "BaseHook.hpp"
#include "DetourHook.hpp"
#include "VMTHook.hpp"
#include "game/hooks/Hooks.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu
{
	Hooking::Hooking()
	{
		if (const auto user32 = GetModuleHandleA("user32.dll"))
		{
			if (const auto getRawInputData = GetProcAddress(user32, "GetRawInputData"))
				BaseHook::Add<Hooks::RawInput::GetRawInputData>(new DetourHook("GetRawInputData", reinterpret_cast<void*>(getRawInputData), Hooks::RawInput::GetRawInputData));
			else
				LOG(WARNING) << "无法解析 GetRawInputData，鼠标点击可能穿透菜单。";
		}
		else
		{
			LOG(WARNING) << "无法获取 user32.dll，鼠标点击可能穿透菜单。";
		}

		// BaseHook::Add<Hooks::Anticheat::QueueDependency>(new DetourHook("QueueDependency", Pointers.QueueDependency, Hooks::Anticheat::QueueDependency));
		BaseHook::Add<Hooks::Anticheat::GameSkeletonUpdate>(new DetourHook("GameSkeletonUpdate", Pointers.GameSkeletonUpdate, Hooks::Anticheat::GameSkeletonUpdate));
		BaseHook::Add<Hooks::Anticheat::PrepareMetricForSending>(new DetourHook("PrepareMetricForSending", Pointers.PrepareMetricForSending, Hooks::Anticheat::PrepareMetricForSending));
		BaseHook::Add<Hooks::Anticheat::GetThreadContext>(new DetourHook("GetThreadContext", reinterpret_cast<void*>(GetProcAddress(LoadLibraryA("kernel32.dll"), "GetThreadContext")), Hooks::Anticheat::GetThreadContext));
		BaseHook::Add<Hooks::Anticheat::HttpStartRequest>(new DetourHook("HttpStartRequest", Pointers.HttpStartRequest, Hooks::Anticheat::HttpStartRequest));
		BaseHook::Add<Hooks::Anticheat::BattlEyeServerProcessPlayerJoin>(new DetourHook("BattlEyeServerProcessPlayerJoin", Pointers.BattlEyeServerProcessPlayerJoin, Hooks::Anticheat::BattlEyeServerProcessPlayerJoin));
		BaseHook::Add<Hooks::Anticheat::GetAnticheatInitializedHash>(new DetourHook("GetAnticheatInitializedHash", Pointers.GetAnticheatInitializedHash, Hooks::Anticheat::GetAnticheatInitializedHash));
		BaseHook::Add<Hooks::Anticheat::GetAnticheatInitializedHash2>(new DetourHook("GetAnticheatInitializedHash2", Pointers.GetAnticheatInitializedHash2, Hooks::Anticheat::GetAnticheatInitializedHash2));

		BaseHook::Add<Hooks::Script::RunScriptThreads>(new DetourHook("RunScriptThreads", Pointers.RunScriptThreads, Hooks::Script::RunScriptThreads));
		BaseHook::Add<Hooks::Script::InitNativeTables>(new DetourHook("InitNativeTables", Pointers.InitNativeTables, Hooks::Script::InitNativeTables));
		BaseHook::Add<Hooks::Script::ScriptVM>(new DetourHook("ScriptVM", reinterpret_cast<void*>(Pointers.ScriptVM), Hooks::Script::ScriptVM));

		BaseHook::Add<Hooks::Info::AssignPhysicalIndex>(new DetourHook("AssignPhysicalIndex", Pointers.AssignPhysicalIndex, Hooks::Info::AssignPhysicalIndex));
		BaseHook::Add<Hooks::Info::NetworkPlayerMgrShutdown>(new DetourHook("NetworkPlayerMgrShutdown", Pointers.NetworkPlayerMgrShutdown, Hooks::Info::NetworkPlayerMgrShutdown));

		BaseHook::Add<Hooks::Network::ReceiveNetMessage>(new DetourHook("ReceiveNetMessage", Pointers.ReceiveNetMessage, Hooks::Network::ReceiveNetMessage));

		BaseHook::Add<Hooks::Spoofing::IsNodeInScope>(new DetourHook("IsNodeInScope", Pointers.IsNodeInScope, Hooks::Spoofing::IsNodeInScope));
		BaseHook::Add<Hooks::Spoofing::ShouldUseNodeCache>(new DetourHook("ShouldUseNodeCache", Pointers.ShouldUseNodeCache, Hooks::Spoofing::ShouldUseNodeCache));
		BaseHook::Add<Hooks::Spoofing::WriteNetArrayData>(new DetourHook("WriteNetArrayData", Pointers.WriteNetArrayData, Hooks::Spoofing::WriteNetArrayData));
		BaseHook::Add<Hooks::Spoofing::WriteNodeData>(new DetourHook("WriteNodeData", Pointers.WriteNodeData, Hooks::Spoofing::WriteNodeData));
		BaseHook::Add<Hooks::Spoofing::WriteSyncTree>(new DetourHook("WriteSyncTree", Pointers.WriteSyncTree, Hooks::Spoofing::WriteSyncTree));

		BaseHook::Add<Hooks::Network::GetPoolType>(new DetourHook("GetPoolType", Pointers.GetPoolType, Hooks::Network::GetPoolType));
		BaseHook::Add<Hooks::Network::GetDLCHash>(new DetourHook("GetDLCHash", Pointers.GetDLCHash, Hooks::Network::GetDLCHash));

		BaseHook::Add<Hooks::Matchmaking::MatchmakingAdvertise>(new DetourHook("MatchmakingAdvertise", Pointers.MatchmakingAdvertise, Hooks::Matchmaking::MatchmakingAdvertise));
		BaseHook::Add<Hooks::Matchmaking::MatchmakingSessionDetailSendResponse>(new DetourHook("MatchmakingSessionDetailSendResponse", Pointers.MatchmakingSessionDetailSendResponse, Hooks::Matchmaking::MatchmakingSessionDetailSendResponse));
		BaseHook::Add<Hooks::Matchmaking::MatchmakingUnadvertise>(new DetourHook("MatchmakingUnadvertise", Pointers.MatchmakingUnadvertise, Hooks::Matchmaking::MatchmakingUnadvertise));
		BaseHook::Add<Hooks::Matchmaking::MatchmakingUpdate>(new DetourHook("MatchmakingUpdate", Pointers.MatchmakingUpdate, Hooks::Matchmaking::MatchmakingUpdate));

		BaseHook::Add<Hooks::Misc::AssistedAimShouldReleaseEntity>(new DetourHook("AssistedAimShouldReleaseEntity", Pointers.AssistedAimShouldReleaseEntity, Hooks::Misc::AssistedAimShouldReleaseEntity));
	}

	Hooking::~Hooking()
	{
		DestroyImpl();
	}

	bool Hooking::Init()
	{
		return GetInstance().InitImpl();
	}

	void Hooking::Destroy()
	{
		GetInstance().DestroyImpl();
		BytePatches::RestoreAll();
	}

	bool Hooking::InitImpl()
	{
		const bool queued = BaseHook::EnableAll();
		const auto applyResult = m_MinHook.ApplyQueued();
		const bool applied = applyResult == MH_OK;
		BaseHook::CommitQueuedStates(applied);

		if (!queued || !applied)
		{
			if (!applied)
			{
				const auto rollbackResult = MH_DisableHook(MH_ALL_HOOKS);
				BaseHook::ReconcileEnabledStates(rollbackResult != MH_OK);
				if (rollbackResult != MH_OK)
					LOGF(FATAL, "Failed to roll back hooks after apply failure: {}", static_cast<int>(rollbackResult));
			}
			LOGF(FATAL, "Failed to enable all hooks (queued: {}, apply status: {})", queued, static_cast<int>(applyResult));
			return false;
		}

		return true;
	}

	void Hooking::DestroyImpl()
	{
		const bool queued = BaseHook::DisableAll();
		const auto applyResult = m_MinHook.ApplyQueued();
		const bool applied = applyResult == MH_OK;
		BaseHook::CommitQueuedStates(applied);

		if (!applied)
		{
			const auto fallbackResult = MH_DisableHook(MH_ALL_HOOKS);
			BaseHook::ReconcileEnabledStates(fallbackResult != MH_OK);
			if (fallbackResult != MH_OK)
				LOGF(FATAL, "Failed to disable hooks after queued apply failure: {}", static_cast<int>(fallbackResult));
		}

		if (!queued || !applied)
			LOGF(FATAL, "Failed to disable all hooks (queued: {}, apply status: {})", queued, static_cast<int>(applyResult));

		for (auto it : BaseHook::Hooks())
		{
			delete it;
		}
		BaseHook::Hooks().clear();
	}
}
