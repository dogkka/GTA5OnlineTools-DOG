#include "SavedPlayers.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/localization/Localization.hpp"
#include "core/frontend/widgets/imgui_colors.h"
#include "core/frontend/Notifications.hpp"
#include "game/backend/SavedPlayers.hpp"
#include "game/frontend/submenus/Debug/Scripts.hpp"
#include "game/gta/Network.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Submenus
{
	static std::uint64_t g_SelectedRid = 0;
	static SavedPlayerData* g_SelectedPlayer = nullptr;
	static char g_SelectedPlayerName[24]{};
	static char g_NameToSearch[24]{};

	static ImColor GetStatusCircleColor(SavedPlayerData* data)
	{
		if (!data->m_FetchedData)
			return ImGui::Colors::Gray;
		else if (data->m_FetchedData->m_GameState == FetchedPlayerData::GameState::INVALID)
			return ImGui::Colors::Red;
		else if (data->m_FetchedData->m_GameState == FetchedPlayerData::GameState::PUBLIC)
			return ImGui::Colors::Green;
		else
			return ImGui::Colors::Yellow;
	}

	static bool ShouldRenderPlayer(std::string_view name, std::string_view search)
	{
		if (search.empty())
			return true;

		if (name.size() < search.size())
			return false;

		// TODO: this doesn't do what YimMenuV1 did (which is to lowercase both inputs and perform a substring search)
		for (int i = 0; i < search.size(); i++)
			if (tolower(name[i]) != tolower(search[i]))
				return false;

		return true;
	}

	static void RenderPlayerItem(std::uint64_t rid, SavedPlayerData* data)
	{
		ImGui::PushID(rid);

		constexpr float circle_size = 7.5f;
		auto cursor_pos = ImGui::GetCursorScreenPos();

		// render status circle
		ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(cursor_pos.x + 4.f + circle_size, cursor_pos.y + 4.f + circle_size), circle_size, GetStatusCircleColor(data));

		// we need some padding
		ImVec2 cursor = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(cursor.x + 25.f, cursor.y));

		if (ImGui::Selectable(data->m_Name.data(), rid == g_SelectedRid))
		{
			g_SelectedRid = rid;
			g_SelectedPlayer = data;
			strncpy(g_SelectedPlayerName, data->m_Name.data(), sizeof(g_SelectedPlayerName));
		}

		if (data->m_FetchedData && ImGui::IsItemHovered())
		{
			const auto gameState = Localization::Translate(FetchedPlayerData::GameStateToString(data->m_FetchedData->m_GameState));
			ImGui::SetTooltip("%s", gameState.c_str());
		}

		ImGui::PopID();
	}

	static void RenderPlayerList()
	{
		ImGui::SetNextItemWidth(200.f);
		ImGui::InputTextWithHint("搜索", "搜索", g_NameToSearch, sizeof(g_NameToSearch));

		if (ImGui::BeginListBox("###player-list", {180, -100 /* static_cast<float>(*Pointers.ScreenResY - 700 - 38 * 4) */}))
		{
			auto& players = SavedPlayers::GetSavedPlayers();

			if (players.size() == 0)
			{
				ImGui::TextDisabled("%s", "暂无已保存玩家");
				ImGui::EndListBox();
				return;
			}

			// TODO: surely there must be a better way to do this

			for (auto& [rid, data] : players)
				if (data.m_FetchedData && data.m_FetchedData->m_GameState == FetchedPlayerData::GameState::PUBLIC && ShouldRenderPlayer(data.m_Name, g_NameToSearch))
					RenderPlayerItem(rid, &data);

			for (auto& [rid, data] : players)
				if (data.m_FetchedData && data.m_FetchedData->m_GameState != FetchedPlayerData::GameState::PUBLIC
				    && data.m_FetchedData->m_GameState != FetchedPlayerData::GameState::INVALID && ShouldRenderPlayer(data.m_Name, g_NameToSearch))
					RenderPlayerItem(rid, &data);

			for (auto& [rid, data] : players)
				if ((!data.m_FetchedData || (data.m_FetchedData->m_GameState == FetchedPlayerData::GameState::INVALID)) && ShouldRenderPlayer(data.m_Name, g_NameToSearch))
					RenderPlayerItem(rid, &data);


			ImGui::EndListBox();
		}
	}

	static void RenderPlayerEditor()
	{
		if (!g_SelectedPlayer || g_SelectedRid == 0)
			return;

		if (ImGui::BeginChild("##player-editor", {500, -100 /* static_cast<float>(*Pointers.ScreenResY - 688 - 38 * 4) */}, 0, ImGuiWindowFlags_NoBackground))
		{
			ImGui::SetNextItemWidth(180.f);
			if (ImGui::InputText("名称", g_SelectedPlayerName, sizeof(g_SelectedPlayerName)))
			{
				g_SelectedPlayer->m_Name = g_SelectedPlayerName;
			}

			int old_rid = g_SelectedRid;
			ImGui::SetNextItemWidth(180.0f);
			if (ImGui::InputScalar("R 星 ID", ImGuiDataType_U64, &g_SelectedRid))
			{
				SavedPlayers::UpdateRockstarId(old_rid, g_SelectedRid);
				g_SelectedPlayer = SavedPlayers::GetPlayerData(g_SelectedRid);
			}

			ImGui::Checkbox("追踪玩家", &g_SelectedPlayer->m_TrackPlayer);

			if (g_SelectedPlayer->m_FetchedData)
			{
				auto& data = *g_SelectedPlayer->m_FetchedData;
				constexpr auto yes = "是";
				constexpr auto no = "否";
				const auto gameState = Localization::Translate(FetchedPlayerData::GameStateToString(data.m_GameState));
				ImGui::Text("战局类型：%s", gameState.c_str());
				ImGui::Text("战局主持人：%s", data.m_HostOfSession ? yes : no);
				ImGui::Text("是否在观战：%s", data.m_Spectating ? yes : no);
				ImGui::Text("是否在差事大厅：%s", data.m_InTransition ? yes : no);
				ImGui::Text("差事大厅主持人：%s", data.m_HostOfTransition ? yes : no);
				if (data.m_MissionType != FetchedPlayerData::MissionType::NONE)
				{
					const auto missionType = Localization::Translate(FetchedPlayerData::MissionTypeToString(data.m_MissionType));
					ImGui::Text("任务类型：%s", missionType.c_str());
					if (data.m_MissionName)
					{
						const auto missionName = LocalizeScriptDisplayName(*data.m_MissionName);
						ImGui::Text("任务名称：%s", missionName.c_str());
					}
					else
						; // TODO: add fetch mission name
				}
			}
			else
			{
				ImGui::TextDisabled("%s", "尚未获取到数据");
			}

			if (ImGui::Button("加入"))
			{
				FiberPool::Push([] {
					Network::JoinRockstarId(g_SelectedRid);
				});
			}

			if (ImGui::Button("保存"))
			{
				SavedPlayers::Save();
			}
			ImGui::SameLine();
			if (ImGui::Button("移除"))
			{
				SavedPlayers::RemovePlayerData(g_SelectedRid);
				g_SelectedPlayer = nullptr;
				g_SelectedRid = 0;
			}
		}
		ImGui::EndChild();
	}

	static void RenderSavedPlayers()
	{
		RenderPlayerList();
		ImGui::SameLine();
		RenderPlayerEditor();
	}

	static void RenderAddNewPlayer()
	{
		static char name_buf[24]{};

		ImGui::SetNextItemWidth(180.0f);
		ImGui::InputText("用户名", name_buf, sizeof(name_buf));
		ImGui::SameLine();
		if (ImGui::Button("添加"))
			FiberPool::Push([] {
				auto rid = YimMenu::Network::ResolveRockstarId(name_buf);
				if (rid)
				{
					SavedPlayers::AddPlayerData(*rid, name_buf);
				}
				else
				{
					Notifications::Show("保存玩家", "无法通过用户名获取 R 星 ID。", NotificationType::Error);
				}
			});
	}

	std::shared_ptr<Category> BuildSavedPlayersMenu()
	{
		auto menu = std::make_shared<Category>("已保存玩家");
		auto players = std::make_shared<Group>("玩家");
		auto new_player = std::make_shared<Group>("新增");
		auto tracking = std::make_shared<Group>("追踪");
		auto notifications = std::make_shared<Group>("通知");

		players->AddItem(std::make_shared<ImGuiItem>([] {
			RenderSavedPlayers();
		}));

		new_player->AddItem(std::make_shared<ImGuiItem>([] {
			RenderAddNewPlayer();
		}));

		notifications->AddItem(std::make_shared<BoolCommandItem>("playerdbnotifywhenjoinable"_J));
		notifications->AddItem(std::make_shared<BoolCommandItem>("playerdbnotifywhenunjoinable"_J));
		notifications->AddItem(std::make_shared<BoolCommandItem>("playerdbnotifywhenonline"_J));
		notifications->AddItem(std::make_shared<BoolCommandItem>("playerdbnotifywhenoffline"_J));
		notifications->AddItem(std::make_shared<BoolCommandItem>("playerdbnotifyonseschange"_J));
		notifications->AddItem(std::make_shared<BoolCommandItem>("playerdbnotifyonmischange"_J));
		notifications->AddItem(std::make_shared<BoolCommandItem>("playerdbnotifyonjoblobby"_J));

		auto update = std::make_shared<Group>("", 1);
		update->AddItem(std::make_shared<BoolCommandItem>("playerdbautoupdate"_J));
		update->AddItem(std::make_shared<CommandItem>("playerdbupdatenow"_J));

		tracking->AddItem(std::move(update));
		tracking->AddItem(std::make_shared<BoolCommandItem>("playerdbnotify"_J));
		tracking->AddItem(std::make_shared<ConditionalItem>("playerdbnotify"_J, std::move(notifications)));

		menu->AddItem(players);
		menu->AddItem(new_player);
		menu->AddItem(tracking);

		return menu;
	}
}
