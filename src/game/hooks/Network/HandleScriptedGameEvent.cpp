#include "game/backend/Self.hpp"
#include "game/hooks/Hooks.hpp"
#include "game/pointers/Pointers.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/commands/Commands.hpp"
#include "core/frontend/Notifications.hpp"
#include "types/network/netGameEvent.hpp"
#include "types/script/globals/GPBD_FM_3.hpp"
#include "types/script/globals/GlobalPlayerBD.hpp"
#include "types/script/ScriptEvent.hpp"
#include "core/scripting/LuaManager.hpp"
#include "core/scripting/LuaUtils.hpp"
#include "core/util/Joaat.hpp"

#include <chrono>
#include <unordered_map>

namespace YimMenu::Hooks
{
	static bool IsProtectionEnabled()
	{
		static auto cmd = Commands::GetCommand<BoolCommand>("scripteventprotection"_J);
		return !cmd || cmd->GetState();
	}

	static const char* LocalizeEventName(const char* name)
	{
		static const std::unordered_map<std::string_view, std::string_view> s_Names = {
		    {"CeoMoney", "CEO 工资"},
		    {"CeoBan", "CEO 玩法禁用"},
		    {"Crash", "崩溃攻击"},
		    {"NotificationCrash", "通知崩溃"},
		    {"SoundSpam", "声音轰炸"},
		    {"NetworkBail", "强制断线"},
		    {"KickFromInterior", "室内踢出"},
		    {"VehicleKick", "踢出载具"},
		    {"TSECommand", "TSE 工具命令"},
		    {"Fake Money Notification", "假金钱通知"},
		    {"Notification", "假通知"},
		    {"GtaBanner", "假横幅"},
		    {"PersonalVehicle", "损毁个人载具"},
		    {"ClearWantedLevel", "清除通缉"},
		    {"RemoteOffradar", "强制关闭雷达"},
		    {"Spectate", "强制观战"},
		    {"ForceMission", "强制任务"},
		    {"StartActivity", "强制活动"},
		    {"StartScript", "强制启动脚本"},
		    {"MarkPlayerAsBeast", "变成野兽"},
		    {"RequestRandomEvent", "随机事件骚扰"},
		    {"Collectible", "塞收集品"},
		    {"Remote Teleport", "强制传送"},
		    {"TriggerCEORaid", "触发 CEO 突袭"},
		};

		if (auto it = s_Names.find(name); it != s_Names.end())
			return it->second.data();

		return name;
	}

	static bool BlockEvent(Player player, const char* name)
	{
		if (!IsProtectionEnabled())
			return false;

		static std::unordered_map<uint64_t, std::chrono::steady_clock::time_point> s_LastNotify;

		const auto key = (static_cast<uint64_t>(player.GetId()) << 32) | Joaat(name);
		const auto now = std::chrono::steady_clock::now();

		auto it = s_LastNotify.find(key);
		if (it != s_LastNotify.end() && now - it->second < std::chrono::seconds(5))
			return true;

		s_LastNotify[key] = now;

		Notifications::Show("脚本事件防护", "已拦截 '" + std::string(LocalizeEventName(name)) + "' 来自 " + player.GetName(), NotificationType::Warning);

		return true;
	}

	static bool CheckLuaScripts(Player player, CScriptedGameEvent& event)
	{
		return LuaManager::DispatchEvent(MenuEvent::ScriptedGameEventReceived, [player, &event](lua_State* state)
		{
			Lua::CreateObject<YimMenu::Player>(state, player);

			lua_newtable(state);
			auto length = event.m_ArgsSize / 8;
			for (int i = 0; i < length; i++)
			{
				lua_pushinteger(state, i == 0 ? (ptrdiff_t)(int)event.m_Args[i] : event.m_Args[i]);
				lua_rawseti(state, -2, i + 1);
			}

			return 2;
		}, true);
	}

	bool Network::HandleScriptedGameEvent(Player player, CScriptedGameEvent& event)
	{
		if (!CheckLuaScripts(player, event))
			return false;

		SCRIPT_EVENT* script_event = reinterpret_cast<SCRIPT_EVENT*>(event.m_Args);

		switch (static_cast<ScriptEventIndex>(script_event->GetEventIndex()))
		{
		case ScriptEventIndex::Bounty:
		{
			SCRIPT_EVENT_BOUNTY* bounty = static_cast<SCRIPT_EVENT_BOUNTY*>(script_event);

			if (event.m_ArgsSize != SCRIPT_EVENT_BOUNTY::GetSize())
			{
				//player.AddDetection();
				return false;
			}

			if (bounty->Target == Self::GetPlayer().GetId())
			{
				return false;
			}

			break;
		}
		case ScriptEventIndex::SendTextLabelSMS:
		{
			//player.AddDetection();
			return false;
		}
		case ScriptEventIndex::CeoKick:
		{
			if (player.GetId() != GPBD_FM_3::Get()->Entries[Self::GetPlayer().GetId()].BossGoon.Boss)
			{
				return false;
			}

			break;
		}
		case ScriptEventIndex::InteriorControl:
		{
			SCRIPT_EVENT_SEND_TO_INTERIOR* interior_control = static_cast<SCRIPT_EVENT_SEND_TO_INTERIOR*>(script_event);

			if (interior_control->Interior < 0 || interior_control->Interior >= static_cast<int>(eSimpleInteriorIndex::SIMPLE_INTERIOR_MAX)) // the upper bound will change after an update
			{
				// null function kick
				return false;
			}

			if (!interior_control->GoonsOnly)
			{
				// send to interior
				return false;
			}

			break;
		}
		case ScriptEventIndex::CeoMoney:
		{
			if (player.GetId() != GPBD_FM_3::Get()->Entries[Self::GetPlayer().GetId()].BossGoon.Boss && BlockEvent(player, "CeoMoney"))
				return false;

			break;
		}
		case ScriptEventIndex::CeoBan:
		{
			if (BlockEvent(player, "CeoBan"))
				return false;
			break;
		}
		case ScriptEventIndex::Crash:
		case ScriptEventIndex::Crash2:
		case ScriptEventIndex::Crash3:
		{
			if (BlockEvent(player, "Crash"))
				return false;
			break;
		}
		case ScriptEventIndex::NotificationCrash1:
		case ScriptEventIndex::NotificationCrash2:
		{
			if (BlockEvent(player, "NotificationCrash"))
				return false;
			break;
		}
		case ScriptEventIndex::SoundSpam:
		{
			if (BlockEvent(player, "SoundSpam"))
				return false;
			break;
		}
		case ScriptEventIndex::NetworkBail:
		{
			if (BlockEvent(player, "NetworkBail"))
				return false;
			break;
		}
		case ScriptEventIndex::KickFromInterior:
		{
			if (BlockEvent(player, "KickFromInterior"))
				return false;
			break;
		}
		case ScriptEventIndex::VehicleKick:
		{
			if (BlockEvent(player, "VehicleKick"))
				return false;
			break;
		}
		case ScriptEventIndex::TSECommand:
		case ScriptEventIndex::TSECommandRotateCam:
		case ScriptEventIndex::TSECommandSound:
		case ScriptEventIndex::TSECommandLaunchHeist:
		{
			if (BlockEvent(player, "TSECommand"))
				return false;
			break;
		}
		case ScriptEventIndex::NotificationMoneyBanked:
		case ScriptEventIndex::NotificationMoneyRemoved:
		case ScriptEventIndex::NotificationMoneyStolen:
		{
			if (BlockEvent(player, "Fake Money Notification"))
				return false;
			break;
		}
		case ScriptEventIndex::Notification:
		{
			if (BlockEvent(player, "Notification"))
				return false;
			break;
		}
		case ScriptEventIndex::GtaBanner:
		{
			if (BlockEvent(player, "GtaBanner"))
				return false;
			break;
		}
		case ScriptEventIndex::DestroyPersonalVehicle:
		case ScriptEventIndex::PersonalVehicleDestroyed:
		{
			if (BlockEvent(player, "PersonalVehicle"))
				return false;
			break;
		}
		case ScriptEventIndex::ClearWantedLevel:
		{
			if (BlockEvent(player, "ClearWantedLevel"))
				return false;
			break;
		}
		case ScriptEventIndex::RemoteOffradar:
		{
			if (BlockEvent(player, "RemoteOffradar"))
				return false;
			break;
		}
		case ScriptEventIndex::Spectate:
		{
			if (BlockEvent(player, "Spectate"))
				return false;
			break;
		}
		case ScriptEventIndex::ForceMission:
		{
			if (BlockEvent(player, "ForceMission"))
				return false;
			break;
		}
		case ScriptEventIndex::StartActivity:
		{
			if (BlockEvent(player, "StartActivity"))
				return false;
			break;
		}
		case ScriptEventIndex::StartScriptBegin:
		case ScriptEventIndex::StartScriptProceed:
		{
			if (BlockEvent(player, "StartScript"))
				return false;
			break;
		}
		case ScriptEventIndex::MarkPlayerAsBeast:
		{
			if (BlockEvent(player, "MarkPlayerAsBeast"))
				return false;
			break;
		}
		case ScriptEventIndex::RequestRandomEvent:
		{
			if (BlockEvent(player, "RequestRandomEvent"))
				return false;
			break;
		}
		case ScriptEventIndex::GiveCollectible:
		{
			if (BlockEvent(player, "Collectible"))
				return false;
			break;
		}
		case ScriptEventIndex::Teleport:
		case ScriptEventIndex::MCTeleport:
		case ScriptEventIndex::TeleportToWarehouse:
		case ScriptEventIndex::SendToLocation:
		case ScriptEventIndex::SendToCayoPerico:
		case ScriptEventIndex::SendToCutscene:
		{
			if (BlockEvent(player, "Remote Teleport"))
				return false;
			break;
		}
		case ScriptEventIndex::TriggerCEORaid:
		{
			if (BlockEvent(player, "TriggerCEORaid"))
				return false;
			break;
		}
		default:
			break;
		}

		return true;
	}
}
