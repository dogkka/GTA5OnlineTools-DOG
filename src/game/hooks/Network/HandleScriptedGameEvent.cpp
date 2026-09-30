#include "game/backend/Self.hpp"
#include "game/hooks/Hooks.hpp"
#include "game/pointers/Pointers.hpp"
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
	static void BlockEvent(Player player, const char* name)
	{
		static std::unordered_map<uint64_t, std::chrono::steady_clock::time_point> s_LastNotify;

		const auto key = (static_cast<uint64_t>(player.GetId()) << 32) | Joaat(name);
		const auto now = std::chrono::steady_clock::now();

		auto it = s_LastNotify.find(key);
		if (it != s_LastNotify.end() && now - it->second < std::chrono::seconds(5))
			return;

		s_LastNotify[key] = now;

		Notifications::Show("脚本事件防护", "已拦截 '" + std::string(name) + "' 来自 " + player.GetName(), NotificationType::Warning);
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
			if (player.GetId() != GPBD_FM_3::Get()->Entries[Self::GetPlayer().GetId()].BossGoon.Boss)
			{
				BlockEvent(player, "CeoMoney");
				return false;
			}

			break;
		}
		case ScriptEventIndex::CeoBan:
		{
			BlockEvent(player, "CeoBan");
			return false;
		}
		case ScriptEventIndex::Crash:
		case ScriptEventIndex::Crash2:
		case ScriptEventIndex::Crash3:
		{
			BlockEvent(player, "Crash");
			return false;
		}
		case ScriptEventIndex::NotificationCrash1:
		case ScriptEventIndex::NotificationCrash2:
		{
			BlockEvent(player, "NotificationCrash");
			return false;
		}
		case ScriptEventIndex::SoundSpam:
		{
			BlockEvent(player, "SoundSpam");
			return false;
		}
		case ScriptEventIndex::NetworkBail:
		{
			BlockEvent(player, "NetworkBail");
			return false;
		}
		case ScriptEventIndex::KickFromInterior:
		{
			BlockEvent(player, "KickFromInterior");
			return false;
		}
		case ScriptEventIndex::VehicleKick:
		{
			BlockEvent(player, "VehicleKick");
			return false;
		}
		case ScriptEventIndex::TSECommand:
		case ScriptEventIndex::TSECommandRotateCam:
		case ScriptEventIndex::TSECommandSound:
		case ScriptEventIndex::TSECommandLaunchHeist:
		{
			BlockEvent(player, "TSECommand");
			return false;
		}
		case ScriptEventIndex::NotificationMoneyBanked:
		case ScriptEventIndex::NotificationMoneyRemoved:
		case ScriptEventIndex::NotificationMoneyStolen:
		{
			BlockEvent(player, "Fake Money Notification");
			return false;
		}
		case ScriptEventIndex::Notification:
		{
			BlockEvent(player, "Notification");
			return false;
		}
		case ScriptEventIndex::GtaBanner:
		{
			BlockEvent(player, "GtaBanner");
			return false;
		}
		case ScriptEventIndex::DestroyPersonalVehicle:
		case ScriptEventIndex::PersonalVehicleDestroyed:
		{
			BlockEvent(player, "PersonalVehicle");
			return false;
		}
		case ScriptEventIndex::ClearWantedLevel:
		{
			BlockEvent(player, "ClearWantedLevel");
			return false;
		}
		case ScriptEventIndex::RemoteOffradar:
		{
			BlockEvent(player, "RemoteOffradar");
			return false;
		}
		case ScriptEventIndex::Spectate:
		{
			BlockEvent(player, "Spectate");
			return false;
		}
		case ScriptEventIndex::ForceMission:
		{
			BlockEvent(player, "ForceMission");
			return false;
		}
		case ScriptEventIndex::StartActivity:
		{
			BlockEvent(player, "StartActivity");
			return false;
		}
		case ScriptEventIndex::StartScriptBegin:
		case ScriptEventIndex::StartScriptProceed:
		{
			BlockEvent(player, "StartScript");
			return false;
		}
		case ScriptEventIndex::MarkPlayerAsBeast:
		{
			BlockEvent(player, "MarkPlayerAsBeast");
			return false;
		}
		case ScriptEventIndex::RequestRandomEvent:
		{
			BlockEvent(player, "RequestRandomEvent");
			return false;
		}
		case ScriptEventIndex::GiveCollectible:
		{
			BlockEvent(player, "Collectible");
			return false;
		}
		case ScriptEventIndex::Teleport:
		case ScriptEventIndex::MCTeleport:
		case ScriptEventIndex::TeleportToWarehouse:
		case ScriptEventIndex::SendToLocation:
		case ScriptEventIndex::SendToCayoPerico:
		case ScriptEventIndex::SendToCutscene:
		{
			BlockEvent(player, "Remote Teleport");
			return false;
		}
		case ScriptEventIndex::TriggerCEORaid:
		{
			BlockEvent(player, "TriggerCEORaid");
			return false;
		}
		default:
			break;
		}

		return true;
	}
}
