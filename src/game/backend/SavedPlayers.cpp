#include "SavedPlayers.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/commands/Command.hpp"
#include "core/filemgr/FileMgr.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/localization/Localization.hpp"
#include "game/backend/PlayerData.hpp"
#include "game/pointers/Pointers.hpp"
#include "types/socialclub/rlQueryPresenceAttributesContext.hpp"
#include "types/socialclub/rlScGamerHandle.hpp"
#include "types/socialclub/rlScTaskStatus.hpp"

namespace YimMenu::Features
{
	static BoolCommand _AutoUpdateEnabled{"playerdbautoupdate", "自动更新玩家数据库", "每 3 分钟自动更新一次已追踪玩家的状态。追踪通知依赖此功能", true};
	static BoolCommand _PlayerTracking{"playerdbnotify", "追踪通知", "当你追踪的玩家状态发生变化时通知你", true};
	static BoolCommand _NotifyWhenJoinable{"playerdbnotifywhenjoinable", "可加入时通知", "当已追踪玩家变为可加入时通知你", true};
	static BoolCommand _NotifyWhenUnjoinable{"playerdbnotifywhenunjoinable", "不可加入时通知", "当已追踪玩家变为不可加入时通知你", true};
	static BoolCommand _NotifyWhenOnline{"playerdbnotifywhenonline", "上线时通知", "当已追踪玩家上线时通知你", true};
	static BoolCommand _NotifyWhenOffline{"playerdbnotifywhenoffline", "离线时通知", "当已追踪玩家离线时通知你", true};
	static BoolCommand _NotifyOnSessionTypeChange{"playerdbnotifyonseschange", "战局类型变化时通知", "当已追踪玩家的战局类型发生变化时通知你"};
	static BoolCommand _NotifyOnMissionChange{"playerdbnotifyonmischange", "任务变化时通知", "当已追踪玩家加入或离开任务时通知你"};
	static BoolCommand _NotifyOnTransitionChange{"playerdbnotifyonjoblobby", "任务大厅变化时通知", "当已追踪玩家加入或离开任务大厅时通知你"};

	class UpdateSavedPlayersNow : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			SavedPlayers::FetchPlayerInfo();
		}
	};

	UpdateSavedPlayersNow _UpdateSavedPlayersNow{"playerdbupdatenow", "立即更新保存玩家", "强制更新全部已保存玩家"};
}

namespace YimMenu
{
	static bool IsInJoinableSession(FetchedPlayerData::GameState state)
	{
		return state == FetchedPlayerData::GameState::PUBLIC;
	}

	static bool IsValidSessionType(FetchedPlayerData::GameState state)
	{
		return state > FetchedPlayerData::GameState::INVALID && state < FetchedPlayerData::GameState::MAX;
	}

	SavedPlayers::SavedPlayers() :
	    m_PlayersFile(FileMgr::GetProjectFile("./saved_players.json").Path())
	{
		LoadImpl();
	}

	SavedPlayers::~SavedPlayers()
	{
	}

	void SavedPlayers::UpdateFetchedData(SavedPlayerData& saved_data, const FetchedPlayerData& fetched_data)
	{
		if (!saved_data.m_FetchedData || !Features::_PlayerTracking.GetState() || !saved_data.m_TrackPlayer)
		{
			saved_data.m_FetchedData = fetched_data;
			return;
		}

		if (saved_data.m_FetchedData->m_GameState != FetchedPlayerData::GameState::INVALID && fetched_data.m_GameState == FetchedPlayerData::GameState::INVALID && Features::_NotifyWhenOffline.GetState())
		{
			Notifications::Show("玩家追踪", std::format("{} 已离线。", saved_data.m_Name));
		}
		else if (!IsInJoinableSession(saved_data.m_FetchedData->m_GameState) && IsInJoinableSession(fetched_data.m_GameState) && Features::_NotifyWhenJoinable.GetState())
		{
			Notifications::Show("玩家追踪", std::format("{} 现在位于可加入的战局中。", saved_data.m_Name));
		}
		else if (saved_data.m_FetchedData->m_GameState == FetchedPlayerData::GameState::INVALID && saved_data.m_FetchedData->m_GameState != FetchedPlayerData::GameState::INVALID && Features::_NotifyWhenOnline.GetState())
		{
			Notifications::Show("玩家追踪", std::format("{} 已上线。", saved_data.m_Name));
		}
		else if (IsInJoinableSession(saved_data.m_FetchedData->m_GameState) && !IsInJoinableSession(fetched_data.m_GameState) && Features::_NotifyWhenUnjoinable.GetState())
		{
			Notifications::Show("玩家追踪", std::format("{} 已不在可加入的战局中。", saved_data.m_Name));
		}

		if (IsValidSessionType(saved_data.m_FetchedData->m_GameState) && IsValidSessionType(fetched_data.m_GameState)
		    && saved_data.m_FetchedData->m_GameState != fetched_data.m_GameState && Features::_NotifyOnSessionTypeChange.GetState())
		{
			const auto gameState = Localization::Translate(FetchedPlayerData::GameStateToString(fetched_data.m_GameState));
			Notifications::Show("玩家追踪", std::format("{} 当前位于 {} 战局。", saved_data.m_Name, gameState));
		}

		if (Features::_NotifyOnMissionChange.GetState())
		{
			if (saved_data.m_FetchedData->m_MissionType != FetchedPlayerData::MissionType::NONE && fetched_data.m_MissionType == FetchedPlayerData::MissionType::NONE)
			{
				const auto missionType = Localization::Translate(FetchedPlayerData::MissionTypeToString(saved_data.m_FetchedData->m_MissionType));
				Notifications::Show("玩家追踪", std::format("{} 已离开 {}。", saved_data.m_Name, missionType));
			}
			else if (fetched_data.m_MissionType != FetchedPlayerData::MissionType::NONE && fetched_data.m_MissionType != saved_data.m_FetchedData->m_MissionType)
			{
				const auto missionType = Localization::Translate(FetchedPlayerData::MissionTypeToString(fetched_data.m_MissionType));
				Notifications::Show("玩家追踪", std::format("{} 当前正在进行 {}。", saved_data.m_Name, missionType));
			}
		}

		if (Features::_NotifyOnTransitionChange.GetState())
		{
			if (saved_data.m_FetchedData->m_InTransition && !fetched_data.m_InTransition)
			{
				if (fetched_data.m_HostOfTransition)
				{
					Notifications::Show("玩家追踪", std::format("{} 正在主持任务大厅。", saved_data.m_Name));
				}
				else
				{
					Notifications::Show("玩家追踪", std::format("{} 已加入任务大厅。", saved_data.m_Name));
				}
			}
			else if ((!saved_data.m_FetchedData->m_InTransition && fetched_data.m_InTransition)
			    && (!Features::_NotifyOnMissionChange.GetState() || (fetched_data.m_MissionType == saved_data.m_FetchedData->m_MissionType)))
			{
				Notifications::Show("玩家追踪", std::format("{} 已离开任务大厅。", saved_data.m_Name));
			}
		}

		// TODO: session host tracking

		saved_data.m_FetchedData = fetched_data;
	}

	void SavedPlayers::RunScriptImpl()
	{
		while (g_Running)
		{
			if (Features::_AutoUpdateEnabled.GetState() && std::chrono::system_clock::now() - m_LastPlayerInfoFetch > AUTO_REFRESH_TIME && Pointers.GetPresenceAttributes)
			{
				FetchPlayerInfo(true);
				m_LastPlayerInfoFetch = std::chrono::system_clock::now();
			}
			ScriptMgr::Yield();
		}
	}

	void SavedPlayers::FetchPlayerInfoImpl(bool tracked_only)
	{
		if (m_FetchingPlayerInfo)
			return;

		if (!Pointers.GetPresenceAttributes)
		{
			LOG(WARNING) << "Socialclub.dll not loaded, cannot fetch player info";
			return;
		}

		m_FetchingPlayerInfo = true;

		std::vector<std::vector<rage::rlScGamerHandle>> player_buckets;
		int current_bucket_idx = 0;

		if (!tracked_only)
			player_buckets.reserve(1 + (current_bucket_idx / PLAYERS_PER_BUCKET));

		for (auto& player : m_SavedPlayers)
		{
			if (!tracked_only || player.second.m_TrackPlayer)
			{
				if (player_buckets.size() <= current_bucket_idx / PLAYERS_PER_BUCKET)
				{
					std::vector<rage::rlScGamerHandle> new_bucket;
					new_bucket.reserve(PLAYERS_PER_BUCKET);
					player_buckets.push_back(std::move(new_bucket));
				}

				player_buckets[current_bucket_idx / PLAYERS_PER_BUCKET].push_back(rage::rlScGamerHandle(player.first));
				current_bucket_idx++;
			}
		}

		// setup base context first
		static std::array base_contexts = {
		    rage::rlQueryPresenceAttributesContext("gstype", rage::rlQueryPresenceAttributesContext::Type::INT, -1),           // 0
		    rage::rlQueryPresenceAttributesContext("gsinfo", rage::rlQueryPresenceAttributesContext::Type::STRING, ""),        // 1
		    rage::rlQueryPresenceAttributesContext("sctv", rage::rlQueryPresenceAttributesContext::Type::INT, 0ull),           // 2
		    rage::rlQueryPresenceAttributesContext("gshost", rage::rlQueryPresenceAttributesContext::Type::INT, 0ull),         // 3
		    rage::rlQueryPresenceAttributesContext("trinfo", rage::rlQueryPresenceAttributesContext::Type::STRING, ""),        // 4
		    rage::rlQueryPresenceAttributesContext("trhost", rage::rlQueryPresenceAttributesContext::Type::INT, 0ull),         // 5
		    rage::rlQueryPresenceAttributesContext("mp_mis_str", rage::rlQueryPresenceAttributesContext::Type::STRING, ""),    // 6
		    rage::rlQueryPresenceAttributesContext("mp_mis_id", rage::rlQueryPresenceAttributesContext::Type::STRING, ""),     // 7
		    rage::rlQueryPresenceAttributesContext("mp_curr_gamemode", rage::rlQueryPresenceAttributesContext::Type::INT, -1), // 8
		};

		for (auto& bucket : player_buckets)
		{
			rage::rlQueryPresenceAttributesContext contexts[PLAYERS_PER_BUCKET][base_contexts.size()];
			rage::rlQueryPresenceAttributesContext* context_ptrs[PLAYERS_PER_BUCKET];

			for (int i = 0; i < bucket.size(); i++)
			{
				memcpy(contexts[i], base_contexts.data(), sizeof(contexts[i]));
				context_ptrs[i] = contexts[i];
			}

			rage::rlScTaskStatus status{};

			if (Pointers.GetPresenceAttributes(0, bucket.data(), bucket.size(), context_ptrs, base_contexts.size(), &status))
			{
				while (status.m_Status == 1)
					ScriptMgr::Yield();

				if (status.m_Status == 3)
				{
					for (int i = 0; i < bucket.size(); i++)
					{
						auto player = GetPlayerData(bucket[i].m_RockstarId);

						if (!player)
							continue;

						auto& context = contexts[i];

						FetchedPlayerData data;
						data.m_GameState = static_cast<FetchedPlayerData::GameState>(context[0].m_Value.m_IntValue);
						data.m_Spectating = context[2].m_Value.m_IntValue;
						data.m_HostOfSession = context[3].m_Value.m_IntValue;
						data.m_HostOfTransition = context[5].m_Value.m_IntValue;
						data.m_MissionType = static_cast<FetchedPlayerData::MissionType>(context[8].m_Value.m_IntValue);
						data.m_MissionId = context[6].m_Value.m_StringValue;

						if (auto mission_name = context[6].m_Value.m_StringValue; mission_name && mission_name[0])
							data.m_MissionName = mission_name;
						else if (player->m_FetchedData && player->m_FetchedData->m_MissionName && player->m_FetchedData->m_MissionId == data.m_MissionId)
							data.m_MissionName = player->m_FetchedData->m_MissionName;

						if (auto session_info = context[1].m_Value.m_StringValue; !session_info || !session_info[0])
							data.m_GameState = FetchedPlayerData::GameState::INVALID;

						if (auto transition_info = context[4].m_Value.m_StringValue; transition_info && transition_info[0])
							data.m_InTransition = true;

						UpdateFetchedData(*player, data);
					}
				}
			}
			else
			{
				// failed, do something here?
				LOG(WARNING) << "Failed to start get presence attributes task";
			}

			m_FetchingPlayerInfo = false;
		}
	}

	SavedPlayerData* SavedPlayers::GetPlayerDataImpl(std::uint64_t id)
	{
		if (auto it = m_SavedPlayers.find(id); it != m_SavedPlayers.end())
			return &it->second;

		return nullptr;
	}

	SavedPlayerData* SavedPlayers::GetPlayerDataImpl(Player player, bool create)
	{
		if (auto data = GetPlayerDataImpl(player.GetRID()))
			return data;

		if (!create)
			return nullptr;

		SavedPlayerData data;
		data.m_Name = player.GetName();

		auto res = &m_SavedPlayers.emplace(player.GetRID(), data).first->second;

		Save();

		return res;
	}

	void SavedPlayers::AddPlayerDataImpl(std::uint64_t id, std::string_view username)
	{
		if (auto data = GetPlayerDataImpl(id))
			return;

		SavedPlayerData data;
		data.m_Name = username;

		m_SavedPlayers.emplace(id, data);

		Save();
	}

	void SavedPlayers::UpdateRockstarIdImpl(std::uint64_t id, std::uint64_t new_id)
	{
		if (auto player = GetPlayerDataImpl(id))
		{
			m_SavedPlayers.emplace(new_id, *player);
			m_SavedPlayers.erase(id);
		}

		Save();
	}

	void SavedPlayers::RemovePlayerDataImpl(std::uint64_t id)
	{
		m_SavedPlayers.erase(id);
	}

	void SavedPlayers::LoadImpl()
	{
		if (std::filesystem::exists(m_PlayersFile))
		{
			try
			{
				std::ifstream file(m_PlayersFile);

				nlohmann::json json;
				file >> json;
				file.close();

				for (auto& [key, val] : json.items())
				{
					auto rid = std::stoll(key);
					SavedPlayerData data = val;

					m_SavedPlayers.emplace(rid, data);
				}
			}
			catch (const std::exception& e)
			{
				LOGF(WARNING, "Failed to load saved players: {}", e.what());
			}
		}
	}

	void SavedPlayers::SaveImpl()
	{
		try
		{
			nlohmann::json json;

			for (auto& [rid, data] : m_SavedPlayers)
				json[std::to_string(rid)] = data;

			std::ofstream file(m_PlayersFile, std::ios::trunc);
			file << json.dump(4);
			file.close();
		}
		catch (const std::exception& e)
		{
			LOGF(WARNING, "Failed to save saved players: {}", e.what());
		}
	}

	void SavedPlayers::OnPlayerJoinImpl(Player player)
	{
		if (auto data = GetPlayerData(player.GetRID()))
		{
			player.GetData().m_IsModder = data->m_IsModder;
			if (data->m_Name != player.GetName())
			{
				data->m_Name = player.GetName();
				Save(); // should we queue a save instead?
			}
		}
	}
}
