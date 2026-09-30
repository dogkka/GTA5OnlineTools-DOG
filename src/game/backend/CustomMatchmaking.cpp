#include "CustomMatchmaking.hpp"
#include "core/util/Joaat.hpp"
#include "types/network/MatchmakingId.hpp"
#include "types/network/rlSessionDetail.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/hooks/Hooks.hpp"
#include "core/hooking/DetourHook.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/backend/ScriptMgr.hpp"

namespace YimMenu::Features
{
	static std::vector<std::pair<int, const char*>> g_RegionCodes = {
	    {0, "独联体"},
	    {1, "南美"},
	    {2, "美国东部"},
	    {3, "欧洲"},
	    {4, "中国"},
	    {5, "澳大利亚"},
	    {6, "美国西部"},
	    {7, "日本"},
	    {8, "未知"},
	};

	static std::vector<std::pair<int, const char*>> g_LanguageTypes = {
	    {0, "英语"},
	    {1, "法语"},
	    {2, "德语"},
	    {3, "意大利语"},
	    {4, "西班牙语（西班牙）"},
	    {5, "葡萄牙语（巴西）"},
	    {6, "波兰语"},
	    {7, "俄语"},
	    {8, "韩语"},
	    {9, "繁体中文"},
	    {10, "日语"},
	    {11, "西班牙语（墨西哥）"},
	    {12, "简体中文"},
	};

	BoolCommand _SpoofRegionType{
	    "mmspoofregiontype",
	    "伪装地区类型",
	    "伪装当前战局的地区类型"};
	ListCommand _RegionType{
	    "mmregiontype",
	    "地区类型",
	    "要将战局伪装成的地区",
		g_RegionCodes};

	BoolCommand _SpoofLanguage{
	    "mmspooflanguage",
	    "伪装语言",
	    "伪装当前战局的语言"};
	ListCommand _Language{
	    "mmlanguage",
	    "语言",
	    "要将战局伪装成的语言",
	    g_LanguageTypes};

	BoolCommand _SpoofPlayerCount{
	    "mmspoofplayercount",
	    "伪装玩家数量",
	    "伪装当前战局的玩家数量"};
	IntCommand _PlayerCount{
	    "mmplayercount",
	    "玩家数量",
	    "要将战局伪装成的玩家数量",
	    1,
	    32,
	    25};

	BoolCommand _MultiplexSession{
	    "mmmultiplexsession",
	    "多路广播战局",
	    "通过同时广播多个战局实例来显著提升你战局的可见性。数值较高时可能在几秒内塞满战局，请自行承担风险"};
	IntCommand _MultiplexCount{
	    "mmmultiplexsessioncount",
	    "多路广播数量",
	    "用于匹配广播的战局实例数量",
	    2,
	    7,
	    5};
}

namespace YimMenu
{
	static std::uint32_t GetIdHash(MatchmakingId* id)
	{
		return Joaat(id->m_data1);
	}

	static void PatchMatchmakingAttrs(MatchmakingAttributes* attrs)
	{
		if (Features::_SpoofRegionType.GetState())
		{
			attrs->m_ParamValues[4] = Features::_RegionType.GetState();
		}

		if (Features::_SpoofLanguage.GetState())
		{
			attrs->m_ParamValues[3] = Features::_Language.GetState();
		}

		if (Features::_SpoofPlayerCount.GetState())
		{
			attrs->m_ParamValues[2] = Features::_PlayerCount.GetState();
		}
	}

	CustomMatchmaking::CustomMatchmaking()
	{

	}

	bool CustomMatchmaking::OnAdvertiseImpl(int& num_slots, int& available_slots, rage::rlSessionInfo* info, MatchmakingAttributes* attrs, MatchmakingId* id, rage::rlTaskStatus* status)
	{
		PatchMatchmakingAttrs(attrs);

		if (!Features::_MultiplexSession.GetState())
			return true; // let the default game code run

		if (status->m_Status)
			return false;

		status->m_Status = 1; // set task in progress

		auto num_slots_copy = num_slots;
		auto available_slots_copy = available_slots;
		FiberPool::Push([this, num_slots_copy, available_slots_copy, info, attrs, id, status] {
			rage::rlTaskStatus first_advert_status;
			if (!BaseHook::Get<Hooks::Matchmaking::MatchmakingAdvertise, DetourHook<decltype(&Hooks::Matchmaking::MatchmakingAdvertise)>>()->Original()(0, num_slots_copy, available_slots_copy, attrs, -1, info, id, &first_advert_status))
			{
				LOGF(WARNING, "OnAdvertiseImpl(): MatchmakingAdvertise returned false for the initial advertisement");
				status->m_Status = 2;
				return;
			}

			while (first_advert_status.m_Status == 1)
				ScriptMgr::Yield();

			if (first_advert_status.m_Status == 2)
			{
				LOGF(WARNING, "OnAdvertiseImpl(): MatchmakingAdvertise failed for the initial advertisement with code {}", status->m_ErrorCode);
				status->m_Status = 2;
				return;
			}

			auto id_hash = GetIdHash(id);

			m_MultiplexedSessions.emplace(id_hash, std::vector<MatchmakingId>{});
			
			// create the multiplexed sessions
			for (int i = 0; i < Features::_MultiplexCount.GetState() - 1; i++)
			{
				FiberPool::Push([this, num_slots_copy, available_slots_copy, info, attrs, status, id_hash, i] {
					rage::rlTaskStatus additional_advert_status;
					MatchmakingId additional_id;

					if (!BaseHook::Get<Hooks::Matchmaking::MatchmakingAdvertise, DetourHook<decltype(&Hooks::Matchmaking::MatchmakingAdvertise)>>()->Original()(0, num_slots_copy, available_slots_copy, attrs, -1, info, &additional_id, &additional_advert_status))
					{
						LOGF(WARNING, "OnAdvertiseImpl(): MatchmakingAdvertise returned false for additional advertisement {}", i);
						return;
					}

					while (additional_advert_status.m_Status == 1)
						ScriptMgr::Yield();

					if (additional_advert_status.m_Status == 2)
					{
						LOGF(WARNING, "OnAdvertiseImpl(): MatchmakingAdvertise failed for the additional advertisement {} with code {}", i, status->m_ErrorCode);
						return;
					}

					if (auto it = m_MultiplexedSessions.find(id_hash); it != m_MultiplexedSessions.end())
						it->second.push_back(additional_id);
				});
			}

			status->m_Status = 3; // signal success to game
		});

		return false; // we're handling the request now
	}

	void CustomMatchmaking::OnUpdateImpl(int& num_slots, int& available_slots, rage::rlSessionInfo* info, MatchmakingAttributes* attrs, MatchmakingId* id)
	{
		PatchMatchmakingAttrs(attrs);
		auto hash = GetIdHash(id);

		// check if the game is trying to update a session we've multiplexed
		if (auto it = m_MultiplexedSessions.find(hash); it != m_MultiplexedSessions.end())
		{
			for (auto& session : it->second)
			{
				auto num_slots_copy = num_slots;
				auto available_slots_copy = available_slots;
				FiberPool::Push([session, num_slots_copy, available_slots_copy, info, attrs]() {
					auto session_copy = session; // the compiler doesn't like it if I use session directly
					BaseHook::Get<Hooks::Matchmaking::MatchmakingUpdate, DetourHook<decltype(&Hooks::Matchmaking::MatchmakingUpdate)>>()->Original()(0, &session_copy, num_slots_copy, available_slots_copy, info, attrs, nullptr); // life's too short to check the task result
				});
			}
		}
	}

	bool CustomMatchmaking::OnUnadvertiseImpl(MatchmakingId* id)
	{
		auto hash = GetIdHash(id);

		// 1) destroying host session
		if (auto it = m_MultiplexedSessions.find(hash); it != m_MultiplexedSessions.end())
		{
			for (auto& session : it->second)
			{
				FiberPool::Push([session]() {
					auto session_copy = session; // the compiler doesn't like it if I use session directly
					BaseHook::Get<Hooks::Matchmaking::MatchmakingUnadvertise, DetourHook<decltype(&Hooks::Matchmaking::MatchmakingUnadvertise)>>()->Original()(0, &session_copy, nullptr);
				});
			}

			return true; // we've destroyed the multiplexed sessions, let the game destroy the main session
		}

		// or 2) game tries to unadvertise multiplexed sessions every few seconds
		for (auto& [_, multiplex] : m_MultiplexedSessions)
		{
			for (auto& session : multiplex)
			{
				if (GetIdHash(&session) == hash)
				{
					return false; // don't let the game destroy our sessions
				}
			}
		}

		return true; 
	}

	void CustomMatchmaking::OnSendSessionDetailResponseImpl(rage::rlSessionDetailMsg* message)
	{
		PatchMatchmakingAttrs(&message->m_Detail.m_SessionConfig.m_MatchmakingAttributes);

		if (Features::_SpoofPlayerCount.GetState())
		{
			message->m_Detail.m_PlayerCount = Features::_PlayerCount.GetState();
			message->m_Detail.m_SpectatorCount = 0;
		}
	}
}
