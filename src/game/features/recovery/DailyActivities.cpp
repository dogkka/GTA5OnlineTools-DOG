#include "DailyActivities.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/localization/Localization.hpp"
#include "game/backend/Self.hpp"
#include "game/backend/ScriptPatches.hpp"
#include "game/backend/Tunables.hpp"
#include "game/gta/Ped.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Scripts.hpp"
#include "game/gta/Stats.hpp"
#include "game/gta/Vehicle.hpp"
#include "game/gta/ScriptFunction.hpp"
#include "game/gta/ScriptLocal.hpp"
#include "game/gta/ScriptGlobal.hpp"
#include "game/pointers/Pointers.hpp"
#include "types/blip/BlipSprite.hpp"
#include "types/script/ScriptEvent.hpp"
#include "types/script/MPScriptData.hpp"
#include "types/script/globals/GPBD_Flow.hpp"
#include "types/script/globals/g_SavedMPGlobals.hpp"
#include "types/script/globals/DailyObjectivesState.hpp"
#include "types/script/globals/WeeklyChallenge.hpp"
#include "types/script/globals/GSBD_RandomEvents.hpp"
#include "types/script/globals/FreemodeGeneral.hpp"
//#include "types/script/globals/FreemodeDelivery.hpp"
#include "types/script/locals/StandardTimeTrialData.hpp"
#include "types/script/locals/RCBanditoTimeTrialData.hpp"
#include "types/script/locals/BikeTimeTrialData.hpp"

namespace YimMenu::Features
{
	static void ShowLocalizedNotification(std::string_view title, std::string_view message, NotificationType type = NotificationType::Info)
	{
		Notifications::Show(Localization::Translate(title), Localization::Translate(message), type);
	}

	static ListCommand hiddenCacheIndex = {"hiddencacheindex", "隐藏补给箱", "当前隐藏补给箱", {{0, "隐藏补给箱 1"}, {1, "隐藏补给箱 2"}, {2, "隐藏补给箱 3"}, {3, "隐藏补给箱 4"}, {4, "隐藏补给箱 5"}, {5, "隐藏补给箱 6"}, {6, "隐藏补给箱 7"}, {7, "隐藏补给箱 8"}, {8, "隐藏补给箱 9"}, {9, "隐藏补给箱 10"}}};

	static ListCommand treasureChestIndex = {"treasurechestindex", "宝箱", "当前宝箱", {{0, "宝箱 1"}, {1, "宝箱 2"}}};

	static ListCommand buriedStashIndex = {"buriedstashindex", "埋藏战利品", "当前埋藏战利品", {{0, "埋藏战利品 1"}, {1, "埋藏战利品 2"}}};

	static ListCommand skydiveIndex = {"skydiveindex", "高空跳伞", "当前高空跳伞点", {{0, "高空跳伞 1"}, {1, "高空跳伞 2"}, {2, "高空跳伞 3"}, {3, "高空跳伞 4"}, {4, "高空跳伞 5"}, {5, "高空跳伞 6"}, {6, "高空跳伞 7"}, {7, "高空跳伞 8"}, {8, "高空跳伞 9"}, {9, "高空跳伞 10"}}};

	static ListCommand timeTrialIndex = {"timetrialindex", "时间挑战赛", "当前时间挑战赛", {{0, "标准时间挑战赛"}, {1, "RC 迷你车时间挑战赛"}, {2, "拉机能量单车时间挑战赛"}}};

	static ListCommand streetDealerIndex = {"streetdealerindex", "街头贩子", "当前街头贩子", {{0, "街头贩子 1"}, {1, "街头贩子 2"}, {2, "街头贩子 3"}}};

	static ListCommand lsTagIndex = {"lstagindex", "洛圣都涂鸦", "当前洛圣都涂鸦", {{0, "洛圣都涂鸦 1"}, {1, "洛圣都涂鸦 2"}, {2, "洛圣都涂鸦 3"}, {3, "洛圣都涂鸦 4"}, {4, "洛圣都涂鸦 5"}}};

	static ListCommand animalIndex = {"animalindex", "动物", "当前动物", {{0, "动物 1"}, {1, "动物 2"}, {2, "动物 3"}}};

	static ListCommand productIndex = {"productindex", "货物", "当前货物", {{0, "货物 1"}, {1, "货物 2"}, {2, "货物 3"}, {3, "货物 4"}, {4, "货物 5"}, {5, "货物 6"}, {6, "货物 7"}, {7, "货物 8"}, {8, "货物 9"}, {9, "货物 10"}}};

	static constexpr auto wildlifePhotographyAnimalHashes = std::to_array({"A_C_Boar"_J, "A_C_Cat_01"_J, "A_C_Cow"_J, "A_C_Coyote"_J, "A_C_Deer"_J, "A_C_Husky"_J, "A_C_MtLion"_J, "A_C_Pig"_J, "A_C_Poodle"_J, "A_C_Pug"_J, "A_C_Rabbit_01"_J, "A_C_Retriever"_J, "A_C_Rottweiler"_J, "A_C_shepherd"_J, "A_C_Westy"_J, "A_C_Chickenhawk"_J, "A_C_Cormorant"_J, "A_C_Crow"_J, "A_C_Hen"_J, "A_C_Seagull"_J});

	static void SetAllDailyActivitiesCompleted(bool completed)
	{
		Stats::SetPackedBool(36628, completed); // G's Cache
		Stats::SetPackedBool(36657, completed); // Stash House
		Stats::SetPackedBool(31734, completed); // Shipwreck
		Stats::SetPackedBool(30297, completed); // Hidden Cache 1
		Stats::SetPackedBool(30298, completed); // Hidden Cache 2
		Stats::SetPackedBool(30299, completed); // Hidden Cache 3
		Stats::SetPackedBool(30300, completed); // Hidden Cache 4
		Stats::SetPackedBool(30301, completed); // Hidden Cache 5
		Stats::SetPackedBool(30302, completed); // Hidden Cache 6
		Stats::SetPackedBool(30303, completed); // Hidden Cache 7
		Stats::SetPackedBool(30304, completed); // Hidden Cache 8
		Stats::SetPackedBool(30305, completed); // Hidden Cache 9
		Stats::SetPackedBool(30306, completed); // Hidden Cache 10
		Stats::SetPackedBool(30307, completed); // Treasure Chest 1
		Stats::SetPackedBool(30308, completed); // Treasure Chest 2
		Stats::SetPackedBool(25522, completed); // Buried Stash 1
		Stats::SetPackedBool(25523, completed); // Buried Stash 2
		Stats::SetPackedBool(42252, completed); // LS Tag 1
		Stats::SetPackedBool(42253, completed); // LS Tag 2
		Stats::SetPackedBool(42254, completed); // LS Tag 3
		Stats::SetPackedBool(42255, completed); // LS Tag 4
		Stats::SetPackedBool(42256, completed); // LS Tag 5
		Stats::SetPackedBool(42269, completed); // Madrazo Hit
		Stats::SetPackedBool(42059, completed); // Shoot Animals Photography 1
		Stats::SetPackedBool(42060, completed); // Shoot Animals Photography 2
		Stats::SetPackedBool(42061, completed); // Shoot Animals Photography 3
		Stats::SetPackedBool(54672, completed); // Smoke on the Water Product 1
		Stats::SetPackedBool(54673, completed); // Smoke on the Water Product 2
		Stats::SetPackedBool(54674, completed); // Smoke on the Water Product 3
		Stats::SetPackedBool(54675, completed); // Smoke on the Water Product 4
		Stats::SetPackedBool(54676, completed); // Smoke on the Water Product 5
		Stats::SetPackedBool(54677, completed); // Smoke on the Water Product 6
		Stats::SetPackedBool(54678, completed); // Smoke on the Water Product 7
		Stats::SetPackedBool(54679, completed); // Smoke on the Water Product 8
		Stats::SetPackedBool(54680, completed); // Smoke on the Water Product 9
		Stats::SetPackedBool(54681, completed); // Smoke on the Water Product 10
		Stats::SetPackedBool(54735, completed); // Golden Clover
		// Street Dealers
		for (int i = 0; i < 3; ++i)
		{
			static Tunable maxCoke{1238316723};
			static Tunable maxMeth{658190943};
			static Tunable maxWeed{803541362};
			static Tunable maxAcid{3123173154};

			int statIndex = 41218 + i * 5;
			int premium = completed ? 0 : static_cast<int>(FreemodeGeneral::Get()->StreetDealers.Dealers[i].PremiumProduct);
			int coke = completed ? 0 : (maxCoke.IsReady() ? maxCoke.Get<int>() : 1);
			int meth = completed ? 0 : (maxMeth.IsReady() ? maxMeth.Get<int>() : 2);
			int weed = completed ? 0 : (maxWeed.IsReady() ? maxWeed.Get<int>() : 10);
			int acid = completed ? 0 : (maxAcid.IsReady() ? maxAcid.Get<int>() : 10);
			Stats::SetPackedInt(statIndex + 0, premium);
			Stats::SetPackedInt(statIndex + 1, coke);
			Stats::SetPackedInt(statIndex + 2, meth);
			Stats::SetPackedInt(statIndex + 3, weed);
			Stats::SetPackedInt(statIndex + 4, acid);
		}
		for (int i = 0; i < 10; i++)
		{
			// see TSE 1916113629
			int location = Stats::GetInt("MPX_DAILYCOLLECT_SKYDIVES" + std::to_string(i));
			Stats::SetPackedInt((34837 + i * 4), completed ? location : -1); // Junk Energy Skydives Checkpoint
			Stats::SetPackedInt((34839 + i * 4), completed ? location : -1); // Junk Energy Skydives Accurate Landing
			Stats::SetPackedInt((34838 + i * 4), completed ? location : -1); // Junk Energy Skydives Partime
			Stats::SetPackedInt((34840 + i * 4), completed ? location : -1); // Junk Energy Skydives Gold
		}
		for (int i = 34252; i <= 34261; i++)
		{
			Stats::SetPackedBool(i, completed); // Trick or Treat
		}
		for (int i = 34512; i <= 34701; i++)
		{
			Stats::SetPackedBool(i, completed); // Trick or Treat
		}
		int ttLocation = 0;
		int rcttLocation = 0;
		int bttLocation = 0;
		if (auto thread = Scripts::FindScriptThread("freemode"_J))
		{
			ttLocation = StandardTimeTrialData::Get(thread)->Location; // gets its value from tunable TIMETRIALVARIATION
			// we can actually manually calculate these using FreemodeGeneral::Get()->DailyReset.Seed % 14
			rcttLocation = RCBanditoTimeTrialData::Get(thread)->Location;
			bttLocation = BikeTimeTrialData::Get(thread)->Location;
		}
		Stats::SetInt("MPPLY_TIMETRIAL_COMPLETED_WEEK", completed ? ttLocation : -1); // Standard Time Trial
		Stats::SetInt("MPPLY_RCTTCOMPLETEDWEEK", completed ? rcttLocation : -1);      // RC Bandito Time Trial
		Stats::SetInt("MPPLY_BTTCOMPLETED", completed ? bttLocation : -1);            // Junk Energy Bike Time Trial
		Stats::SetInt("MPX_CBV_DELIVERED_BS", completed ? 1023 : 0);                  // Exotic Exports
		Stats::SetInt("MPX_CBV_STATE", completed);                                    // Exotic Exports
	}

	static void TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables collectable, int index)
	{
		static ScriptFunction getCollectibleCoords("freemode"_J,
		    ScriptPointer("GetCollectibleCoords", "5D ? ? ? 7D 2C 10").Add(1).Rip());
		if (auto coords = getCollectibleCoords.Call<Vector3>(collectable, index))
		{
			Self::GetPed().TeleportTo(coords);
		}
	}

	static void CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables collectable, int index)
	{
		SCRIPT_EVENT_COLLECT_COLLECTABLE data;
		data.CollectableType = collectable;
		data.CollectableIndex = index;
		data.SetCollected = TRUE;
		data.DisplayNotification = TRUE;
		data.Send();
	}

	static joaat_t GetNextExoticExportsVehicleHash()
	{
		constexpr ScriptGlobal order(1950873);
		for (int i = 1; i <= 10; i++)
		{
			auto offset = *order.At(i).As<int*>();
			if (!GPBD_Flow::Get()->Entries[Self::GetPlayer().GetId()].TunerCBVDeliveredFlags.IsSet(offset))
			{
				auto hash = *ScriptGlobal(1950862 + (offset + 1)).As<joaat_t*>();
				return hash;
			}
		}
		return 0;
	}

	static bool initialized = false;

	void OpenStreetDealerMenu::RunScriptImpl()
	{
		while (g_Running)
		{
			ScriptMgr::Yield();

			if (!m_ShouldRunScript)
				continue;

			if (!m_Thread)
			{
				int id = Scripts::StartScript("fm_street_dealer"_J);
				m_Thread = Scripts::FindScriptThreadByID(id);
				if (m_Thread)
				{
					m_Thread->m_Context.m_State = rage::scrThread::State::PAUSED;
				}
				else
				{
					m_ShouldRunScript = false;
					continue;
				}
			}

			auto streetDealerData = ScriptLocal(m_Thread, 265).At(12);

			if (!initialized)
			{
				auto selected = streetDealerIndex.GetState();
				FreemodeGeneral::Get()->StreetDealers.ClosestDealerLocation =
				    FreemodeGeneral::Get()->StreetDealers.Dealers[selected].Location;
				FreemodeGeneral::Get()->StreetDealers.ClosetsDealerIndex = selected;

				static ScriptFunction initStreetDealerData("fm_street_dealer"_J, ScriptPointer("InitStreetDealerData", "2D 00 07 00 00 61"));
				initStreetDealerData.Call<void>();
				streetDealerData.At(5).As<SCR_BITSET<uint64_t>*>()->Set(0);
				initialized = true;
			}

			static ScriptFunction runStreetDealerMenu("fm_street_dealer"_J, ScriptPointer("RunStreetDealerMenu", "2D 01 03 00 00 5D ? ? ? 2A"));
			runStreetDealerMenu.Call<void>(streetDealerData.As<int*>());

			if (streetDealerData.At(5).As<SCR_BITSET<uint64_t>*>()->IsSet(2) || !*Pointers.IsSessionStarted)
			{
				// if we don't reset these, freemode won't start the script legitimately
				FreemodeGeneral::Get()->StreetDealers.ClosestDealerLocation = -1;
				FreemodeGeneral::Get()->StreetDealers.ClosetsDealerIndex = -1;

				m_Thread->Kill();
				m_Thread->m_Context.m_State = rage::scrThread::State::KILLED;
				m_Thread = nullptr;
				initialized = false;
				m_ShouldRunScript = false;
			}
		}
	}

	class SetAllActivitiesCompleted : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!Pointers.IsSessionStarted)
				return;

			SetAllDailyActivitiesCompleted(true);
			ShowLocalizedNotification("日常活动", "已将全部日常活动标记为完成，请切换战局以应用更改。");
		}
	};

	class ResetAllActivities : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!Pointers.IsSessionStarted)
				return;

			SetAllDailyActivitiesCompleted(false);
			ShowLocalizedNotification("日常活动", "已重置全部日常活动进度，请切换战局以应用更改。");
		}
	};

	class CompleteAllChallenges : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!Pointers.IsSessionStarted)
				return;

			for (int i = 0; i < 3; i++)
			{
				DailyObjectivesState::Get()->Objective[i] =
				    g_SavedMPGlobals::Get()->Entries[0].GeneralSaved.DOObjectives[i].Objective;
			}
			DailyObjectivesState::Get()->Completed = TRUE;
			WeeklyChallenge::Get()->Entries[0].CurrentObjective = WeeklyChallenge::Get()->Entries[0].ObjectiveCountOverride;
		}
	};

	class TeleportToHiddenCache : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(30297 + hiddenCacheIndex.GetState()))
			{
				TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::HiddenCaches, hiddenCacheIndex.GetState());
			}
			else
			{
				ShowLocalizedNotification("隐藏补给箱", "该隐藏补给箱已经收集过了。", NotificationType::Error);
			}
		}
	};

	class CollectHiddenCache : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(30297 + hiddenCacheIndex.GetState()))
			{
				CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::HiddenCaches, hiddenCacheIndex.GetState());
			}
			else
			{
				ShowLocalizedNotification("隐藏补给箱", "该隐藏补给箱已经收集过了。", NotificationType::Error);
			}
		}
	};

	class TeleportToTreasureChest : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(30307 + treasureChestIndex.GetState()))
			{
				TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::TresureChests, treasureChestIndex.GetState());
			}
			else
			{
				ShowLocalizedNotification("宝箱", "该宝箱已经收集过了。", NotificationType::Error);
			}
		}
	};

	class CollectTreasureChest : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(30307 + treasureChestIndex.GetState()))
			{
				CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::TresureChests, treasureChestIndex.GetState());
			}
			else
			{
				ShowLocalizedNotification("宝箱", "该宝箱已经收集过了。", NotificationType::Error);
			}
		}
	};

	class EnableTreasureChestInLS : public BoolCommand
	{
		using BoolCommand::BoolCommand;

		ScriptPatch m_EnableTreasureChestInLSPatch{};

		virtual void OnEnable() override
		{
			if (!m_EnableTreasureChestInLSPatch)
			{
				m_EnableTreasureChestInLSPatch = ScriptPatches::AddPatch("freemode"_J, ScriptPointer("EnableTreasureChestInLSPatch", "1F 56 ? ? 38 00 47 ? ? 73"), {0x72});
			}
			m_EnableTreasureChestInLSPatch->Enable();
		}

		virtual void OnDisable() override
		{
			if (m_EnableTreasureChestInLSPatch)
			{
				m_EnableTreasureChestInLSPatch->Disable();
			}
		}
	};

	class TeleportToShipwrecked : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(31734))
			{
				TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::Shipwrecked, 0);
			}
			else
			{
				ShowLocalizedNotification("沉船", "沉船奖励已经收集过了。", NotificationType::Error);
			}
		}
	};

	class CollectShipwrecked : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(31734))
			{
				CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::Shipwrecked, 0);
			}
			else
			{
				ShowLocalizedNotification("沉船", "沉船奖励已经收集过了。", NotificationType::Error);
			}
		}
	};

	class TeleportToBuriedStash : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(25522 + buriedStashIndex.GetState()))
			{
				TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::BuriedStash, buriedStashIndex.GetState());
			}
			else
			{
				ShowLocalizedNotification("埋藏战利品", "该埋藏战利品已经收集过了。", NotificationType::Error);
			}
		}
	};

	class CollectBuriedStash : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(25522 + buriedStashIndex.GetState()))
			{
				CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::BuriedStash, buriedStashIndex.GetState());
			}
			else
			{
				ShowLocalizedNotification("埋藏战利品", "该埋藏战利品已经收集过了。", NotificationType::Error);
			}
		}
	};

	class EnableBuriedStashInLS : public BoolCommand
	{
		using BoolCommand::BoolCommand;

		ScriptPatch m_EnableBuriedStashInLSPatch{};

		virtual void OnEnable() override
		{
			if (!m_EnableBuriedStashInLSPatch)
			{
				m_EnableBuriedStashInLSPatch = ScriptPatches::AddPatch("freemode"_J, ScriptPointer("EnableBuriedStashInLSPatch", "71 39 02 38 02 06 56 ? ? 2C"), {0x72});
			}
			m_EnableBuriedStashInLSPatch->Enable();
		}

		virtual void OnDisable() override
		{
			if (m_EnableBuriedStashInLSPatch)
			{
				m_EnableBuriedStashInLSPatch->Disable();
			}
		}
	};

	class TeleportToSkydive : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::Skydives, skydiveIndex.GetState()); // no stat check as you can complete a skydive multiple times
		}
	};

	class CompleteSkydive : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			static Tunable checkpointReward{"SKYDIVING_CHALLENGE_CASH_REWARD_ALL_CHECKPOINTS_COLLECTED"_J};
			static Tunable parTimeReward{"SKYDIVING_CHALLENGE_CASH_REWARD_PAR_TIME"_J};
			static Tunable landingReward{"SKYDIVING_CHALLENGE_CASH_REWARD_ACCURATE_LANDING"_J};

			if (!checkpointReward.IsReady() || !parTimeReward.IsReady() || !landingReward.IsReady())
				return;

			int index = skydiveIndex.GetState();
			int location = Stats::GetInt("MPX_DAILYCOLLECT_SKYDIVES" + std::to_string(index));
			bool checkpoints = Stats::GetPackedInt(34837 + (index * 4)) == location;
			bool partime = Stats::GetPackedInt(34838 + (index * 4)) == location;
			bool landing = Stats::GetPackedInt(34839 + (index * 4)) == location;

			*ScriptGlobal(1980496).As<int*>() = 1;
			*ScriptGlobal(1980496).At(1).As<int*>() = 1;
			*ScriptGlobal(1980496).At(3).As<int*>() = 5;
			*ScriptGlobal(1980496).At(4).As<int*>() = location + 1;
			*ScriptGlobal(1980496).At(8).At(1).As<int*>() = checkpointReward.Get<int>() / (1 + (checkpoints * 9));
			*ScriptGlobal(1980496).At(8).At(2).As<int*>() = parTimeReward.Get<int>() / (1 + (partime * 9));
			*ScriptGlobal(1980496).At(8).At(3).As<int*>() = landingReward.Get<int>() / (1 + (landing * 9));

			SET_SKYDIVE_COMPLETED data;
			data.SkydiveIndex = index;
			data.SkydiveLocation = location;
			data.AllCheckpointsHit = TRUE;
			data.ParTimeBeaten = TRUE;
			data.AccurateLanding = TRUE;
			data.Send();

			ScriptGlobal(1984467).At(4).As<SCR_BITSET<uint64_t>*>()->Clear(3);
		}
	};

	class TeleportToTimeTrial : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			std::string stat = "";
			switch (timeTrialIndex.GetState())
			{
			case 0: stat = "MPPLY_TIMETRIAL_COMPLETED_WEEK"; break;
			case 1: stat = "MPPLY_RCTTCOMPLETEDWEEK"; break;
			case 2: stat = "MPPLY_BTTCOMPLETED"; break;
			}

			if (stat.empty() || Stats::GetInt(stat) != -1)
			{
				ShowLocalizedNotification("时间挑战赛", "该时间挑战赛已经完成过了。", NotificationType::Error);
				return;
			}

			int sprite = -1;
			switch (timeTrialIndex.GetState())
			{
			case 0: sprite = static_cast<int>(BlipSprite::RADAR_TEMP_2); break;
			case 1: sprite = static_cast<int>(BlipSprite::RADAR_RC_TIME_TRIALS); break;
			case 2: sprite = static_cast<int>(BlipSprite::RADAR_BICYCLE_TRIAL); break;
			}

			auto blip = HUD::GET_FIRST_BLIP_INFO_ID(sprite);
			if (HUD::DOES_BLIP_EXIST(blip))
			{
				if (auto coords = HUD::GET_BLIP_COORDS(blip))
				{
					Self::GetPed().TeleportTo(coords);
				}
			}
			else
			{
				ShowLocalizedNotification("时间挑战赛", "请先在互动菜单中启用标记。", NotificationType::Error);
			}
		}
	};

	class BeatTimeTrial : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			std::string stat = "";
			switch (timeTrialIndex.GetState())
			{
			case 0: stat = "MPPLY_TIMETRIAL_COMPLETED_WEEK"; break;
			case 1: stat = "MPPLY_RCTTCOMPLETEDWEEK"; break;
			case 2: stat = "MPPLY_BTTCOMPLETED"; break;
			}

			if (stat.empty() || Stats::GetInt(stat) != -1)
			{
				ShowLocalizedNotification("时间挑战赛", "该时间挑战赛已经完成过了。", NotificationType::Error);
				return;
			}

			rage::scrThread* thread = nullptr;
			if (timeTrialIndex.GetState() == 2)
			{
				if (Scripts::IsScriptActive("fm_content_bicycle_time_trial"_J))
					return;

				MP_SCRIPT_DATA garbage{};
				thread = Scripts::FindScriptThreadByID(Scripts::StartScript("fm_content_bicycle_time_trial"_J, eStackSizes::MULTIPLAYER_FREEMODE, &garbage, SCR_SIZEOF(garbage)));
			}
			else
			{
				thread = Scripts::FindScriptThread("freemode"_J);
			}

			if (!thread)
				return;

			if (timeTrialIndex.GetState() == 0)
			{
				if (auto ttData = StandardTimeTrialData::Get(thread))
				{
					ttData->Timer1.Time = *Pointers.NetworkTime;
					static ScriptFunction beatStandardTimeTrial("freemode"_J, ScriptPointer("BeatStandardTimeTrial", "2D 01 19 00 00 38"));
					beatStandardTimeTrial.Call<void>(ttData);
				}
			}
			else if (timeTrialIndex.GetState() == 1)
			{
				if (auto rcttData = RCBanditoTimeTrialData::Get(thread))
				{
					rcttData->Timer1.Time = *Pointers.NetworkTime;
					static ScriptFunction beatRCTimeTrial("freemode"_J, ScriptPointer("BeatRCTimeTrial", "2D 01 17 00 00 38 00 40"));
					beatRCTimeTrial.Call<void>(rcttData);
				}
			}
			else if (timeTrialIndex.GetState() == 2)
			{
				thread->m_Context.m_State = rage::scrThread::State::PAUSED;

				*ScriptLocal(thread, 3135).At(131).At(1).As<int*>() = FreemodeGeneral::Get()->DailyReset.Seed % 14; // if we don't init this, the par time duration function will return 0 and the COMPLETED stat will be set to 0, which is bad
				*ScriptLocal(thread, 153).At(4).As<int*>() = 0;
				static ScriptFunction onBTTEnd("fm_content_bicycle_time_trial"_J,
				    ScriptPointer("OnBTTEnd", "64 ? ? ? 5D ? ? ? 75 77").Add(1).Rip());
				onBTTEnd.Call<void>();

				thread->m_Context.m_State = rage::scrThread::State::KILLED;
			}
		}
	};

	class TeleportToExoticExportsVehicle : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (GPBD_Flow::Get()->Entries[Self::GetPlayer().GetId()].TunerCBVDeliveredFlags.Value == 1023)
			{
				ShowLocalizedNotification("异域出口", "你已经交付了所有载具。", NotificationType::Error);
				return;
			}

			if (GSBD_RandomEvents::Get()->EventData[3].State != eRandomEventState::INACTIVE)
			{
				if (auto coords = GSBD_RandomEvents::Get()->EventData[3].TriggerPosition)
					Self::GetPed().TeleportTo(coords);
			}
		}
	};

	class DeliverNextExoticExportsVehicle : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (GPBD_Flow::Get()->Entries[Self::GetPlayer().GetId()].TunerCBVDeliveredFlags.Value == 1023)
			{
				ShowLocalizedNotification("异域出口", "你已经交付了所有载具。", NotificationType::Error);
				return;
			}

			/*if (!FreemodeDelivery::Get()->DeliveryCooldown1.HasTimePassed(30000))
			{
				ShowLocalizedNotification("异域出口", "你刚刚交付了一辆载具，请稍等片刻。", NotificationType::Error);
				return;
			}*/

			auto hash = GetNextExoticExportsVehicleHash();
			static ScriptFunction onFreemodeDeliveryEnd("freemode"_J, ScriptPointer("OnFreemodeDeliveryEnd", "2D 0C 2B 00"));
			onFreemodeDeliveryEnd.Call<void>(0, Self::GetPlayer().GetId(), 0, 0, 0, 0, 0, 0, 0, 0, hash, 273);
			//FreemodeDelivery::Get()->DeliveryCooldown1.Reset();
		}
	};

	class TeleportToDeadDrop : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(36628))
			{
				TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::DeadDrop, 0);
			}
			else
			{
				ShowLocalizedNotification("G 的藏匿点", "G 的藏匿点奖励已经收集过了。", NotificationType::Error);
			}
		}
	};

	class CollectDeadDrop : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(36628))
			{
				CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::DeadDrop, 0);
			}
			else
			{
				ShowLocalizedNotification("G 的藏匿点", "G 的藏匿点奖励已经收集过了。", NotificationType::Error);
			}
		}
	};

	class TeleportToStashHouse : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(36657))
			{
				auto blip = HUD::GET_FIRST_BLIP_INFO_ID(static_cast<int>(BlipSprite::RADAR_STASH_HOUSE));
				if (HUD::DOES_BLIP_EXIST(blip))
				{
					if (auto coords = HUD::GET_BLIP_COORDS(blip))
						Self::GetPed().TeleportTo(coords);
				}
			}
			else
			{
				ShowLocalizedNotification("藏匿屋", "藏匿屋已经洗劫过了。", NotificationType::Error);
			}
		}
	};

	class EnterStashHouseSafeCode : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (auto thread = Scripts::FindScriptThread("fm_content_stash_house"_J))
			{
				for (int i = 0; i < 3; i++)
				{
					int combination = *ScriptLocal(thread, 153).At(22).At(i, 2).At(1).As<int*>();
					*ScriptLocal(thread, 153).At(22).At(i, 2).As<float*>() = combination;
				}
			}
		}
	};

	class TeleportToStreetDealer : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			static ScriptFunction getStreetDealerCoords("freemode"_J,
			    ScriptPointer("GetStreetDealerCoords", "5D ? ? ? 5D ? ? ? 5D ? ? ? 18 1F").Add(1).Rip());
			if (auto coords = getStreetDealerCoords.Call<Vector3>(
			        FreemodeGeneral::Get()->StreetDealers.Dealers[streetDealerIndex.GetState()].Location))
				Self::GetPed().TeleportTo(coords);
		}
	};

	class _OpenStreetDealerMenu : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted || Scripts::IsScriptActive("fm_street_dealer"_J))
				return;

			OpenStreetDealerMenu::SetShouldRunScript(true);
		}
	};

	class TeleportToLSTag : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(42252 + lsTagIndex.GetState()))
			{
				TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::Tagging, lsTagIndex.GetState());
			}
			else
			{
				ShowLocalizedNotification("洛圣都涂鸦", "该洛圣都涂鸦已经喷涂过了。", NotificationType::Error);
			}
		}
	};

	class SprayLSTag : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(42252 + lsTagIndex.GetState()))
			{
				CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::Tagging, lsTagIndex.GetState());
			}
			else
			{
				ShowLocalizedNotification("洛圣都涂鸦", "该洛圣都涂鸦已经喷涂过了。", NotificationType::Error);
			}
		}
	};

	class TeleportToMadrazoHit : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (Stats::GetInt("MPX_BAIL_OFFICE_OWNED") == 0)
			{
				ShowLocalizedNotification("玛德拉索暗杀", "你必须拥有保释事务所。", NotificationType::Error);
				return;
			}

			if (!Stats::GetPackedBool(42269))
			{
				auto blip = HUD::GET_FIRST_BLIP_INFO_ID(static_cast<int>(BlipSprite::RADAR_DAILY_BOUNTY));
				if (HUD::DOES_BLIP_EXIST(blip))
				{
					if (auto coords = HUD::GET_BLIP_COORDS(blip))
						Self::GetPed().TeleportTo(coords);
				}
			}
			else
			{
				ShowLocalizedNotification("玛德拉索暗杀", "玛德拉索暗杀任务已经完成过了。", NotificationType::Error);
			}
		}
	};

	class TeleportToMadrazoHitTarget : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (auto thread = Scripts::FindScriptThread("fm_content_daily_bounty"_J))
			{
				if (auto coords = *ScriptLocal(thread, 250).At(434).At(1).At(0, 4).As<Vector3*>())
				{
					Self::GetPed().TeleportTo(coords);
				}
			}
		}
	};

	class SpawnAnimal : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(42059 + animalIndex.GetState()))
			{
				int index = Stats::GetPackedInt(28091 + animalIndex.GetState());
				if (index < 0 || index >= wildlifePhotographyAnimalHashes.size())
					return;

				Ped::Create(wildlifePhotographyAnimalHashes[index], Self::GetPed().GetPosition());
			}
			else
			{
				ShowLocalizedNotification("野生动物摄影", "这只动物已经拍摄过了。", NotificationType::Error);
			}
		}
	};

	class PhotographAnimal : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(42059 + animalIndex.GetState()))
			{
				int index = Stats::GetPackedInt(28091 + animalIndex.GetState());
				if (index < 0 || index >= wildlifePhotographyAnimalHashes.size())
					return;

				ScriptGlobal(2709520).At(545).As<SCR_BITSET<uint64_t>*>()->Set(6);
				*ScriptGlobal(2709520).At(549).As<joaat_t*>() = wildlifePhotographyAnimalHashes[index];
				*ScriptGlobal(2709520).At(550).As<int*>() = *Pointers.GameTimer - 1; // bypass 2 sec delay
			}
			else
			{
				ShowLocalizedNotification("野生动物摄影", "这只动物已经拍摄过了。", NotificationType::Error);
			}
		}
	};

	class TeleportToProduct : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (Stats::GetInt("MPX_SB_WEED_SHOP_OWNED") == 0)
			{
				ShowLocalizedNotification("喷云吐雾馆", "你必须拥有“喷云吐雾馆”产业。", NotificationType::Error);
				return;
			}

			if (!Stats::GetPackedBool(54672 + productIndex.GetState()))
			{
				TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::SmokeOnTheWater, productIndex.GetState());
			}
			else
			{
				ShowLocalizedNotification("喷云吐雾馆", "该货物已经收集过了。", NotificationType::Error);
			}
		}
	};

	class CollectProduct : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (Stats::GetInt("MPX_SB_WEED_SHOP_OWNED") == 0)
			{
				ShowLocalizedNotification("喷云吐雾馆", "你必须拥有“喷云吐雾馆”产业。", NotificationType::Error);
				return;
			}

			if (!Stats::GetPackedBool(54672 + productIndex.GetState()))
			{
				CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::SmokeOnTheWater, productIndex.GetState());
			}
			else
			{
				ShowLocalizedNotification("喷云吐雾馆", "该货物已经收集过了。", NotificationType::Error);
			}
		}
	};

	class TeleportToGoldenClover : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(54735))
			{
				TeleportToCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::GoldenClover, 0);
			}
			else
			{
				ShowLocalizedNotification("金色四叶草", "金色四叶草已经收集过了。", NotificationType::Error);
			}
		}
	};

	class CollectGoldenClover : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			if (!*Pointers.IsSessionStarted)
				return;

			if (!Stats::GetPackedBool(54735))
			{
				CollectCollectable(SCRIPT_EVENT_COLLECT_COLLECTABLE::eCollectables::GoldenClover, 0);
			}
			else
			{
				ShowLocalizedNotification("金色四叶草", "金色四叶草已经收集过了。", NotificationType::Error);
			}
		}
	};

	static SetAllActivitiesCompleted _SetAllActivitiesCompleted{"setallactivitiescompleted", "完成全部日常活动", "将全部日常活动标记为已完成；切换战局后生效。"};
	static ResetAllActivities _ResetAllActivities{"resetallactivities", "重置日常活动进度", "将全部日常活动重置为未完成；切换战局后生效。"};

	static CompleteAllChallenges _CompleteAllChallenges{"completeallchallenges", "完成每日与每周挑战", "完成全部每日目标和当前每周挑战。"};

	static TeleportToHiddenCache _TeleportToHiddenCache{"tptohiddencache", "传送到隐藏补给箱", "传送到所选隐藏补给箱。"};
	static CollectHiddenCache _CollectHiddenCache{"collecthiddencache", "收集隐藏补给箱", "收集所选隐藏补给箱。"};

	static TeleportToTreasureChest _TeleportToTreasureChest{"tptotreasurechest", "传送到宝箱", "传送到所选宝箱。"};
	static CollectTreasureChest _CollectTreasureChest{"collecttreasurechest", "收集宝箱", "收集所选宝箱。"};
	static EnableTreasureChestInLS _EnableTreasureChestInLS{"enabletreasurechestinls", "在洛圣都启用宝箱", "在洛圣都启用宝箱，这样你就不必前往佩里科岛。"};

	static TeleportToShipwrecked _TeleportToShipwrecked{"tptoshipwrecked", "传送到沉船", "传送到沉船位置。"};
	static CollectShipwrecked _CollectShipwrecked{"collectshipwrecked", "收集沉船奖励", "收集沉船奖励。"};

	static TeleportToBuriedStash _TeleportToBuriedStash{"tptoburiedstash", "传送到埋藏战利品", "传送到所选埋藏战利品。"};
	static CollectBuriedStash _CollectBuriedStash{"collectburiedstash", "收集埋藏战利品", "收集所选埋藏战利品。"};
	static EnableBuriedStashInLS _EnableBuriedStashInLS{"enableburiedstashinls", "在洛圣都启用埋藏战利品", "在洛圣都启用埋藏战利品，这样你就不必前往佩里科岛。"};

	static TeleportToSkydive _TeleportToSkydive{"tptoskydive", "传送到高空跳伞点", "传送到所选高空跳伞点。"};
	static CompleteSkydive _CompleteSkydive{"completeskydive", "完成高空跳伞", "完成所选高空跳伞。"};

	static TeleportToTimeTrial _TeleportToTimeTrial{"tptotimetrial", "传送到时间挑战赛", "传送到所选时间挑战赛。"};
	static BeatTimeTrial _BeatTimeTrial{"beattimetrial", "完成时间挑战赛", "完成所选时间挑战赛。"};

	static TeleportToExoticExportsVehicle _TeleportToExoticExportsVehicle{"tptoexoticexportsvehicle", "传送到出口珍品载具", "传送到下一辆出口珍品载具。"};
	static DeliverNextExoticExportsVehicle _DeliverNextExoticExportsVehicle{"delivernextexoticexportsvehicle", "交付下一辆出口珍品载具", "直接交付下一辆出口珍品载具。"};

	static TeleportToDeadDrop _TeleportToDeadDrop{"tptodeaddrop", "传送到 G 的藏匿点", "传送到 G 的藏匿点。"};
	static CollectDeadDrop _CollectDeadDrop{"collectdeaddrop", "领取 G 的藏匿点奖励", "直接领取 G 的藏匿点奖励。"};

	static TeleportToStashHouse _TeleportToStashHouse{"tptostashhouse", "传送到藏匿屋", "传送到藏匿屋。"};
	static EnterStashHouseSafeCode _EnterStashHouseSafeCode{"enterstashhousesafecode", "输入藏匿屋保险箱密码", "输入藏匿屋保险箱密码。"};

	static TeleportToStreetDealer _TeleportToStreetDealer{"tptostreetdealer", "传送到街头贩子", "传送到所选街头贩子。"};
	static _OpenStreetDealerMenu __OpenStreetDealerMenu{"openstreetdealermenu", "远程交易", "远程打开所选街头贩子的交易界面。"};

	static TeleportToLSTag _TeleportToLSTag{"tptolstag", "传送到洛圣都涂鸦", "传送到所选洛圣都涂鸦。"};
	static SprayLSTag _SprayLSTag{"spraylstag", "喷涂洛圣都涂鸦", "喷涂所选洛圣都涂鸦。"};

	static TeleportToMadrazoHit _TeleportToMadrazoHit{"tptomadrazohit", "传送到玛德拉索暗杀任务", "传送到玛德拉索暗杀任务地点。"};
	static TeleportToMadrazoHitTarget _TeleportToMadrazoHitTarget{"tptomadrazohittarget", "传送到玛德拉索暗杀目标", "传送到玛德拉索暗杀目标位置。"};

	static SpawnAnimal _SpawnAnimal{"spawnanimal", "生成动物", "生成所选动物。"};
	static PhotographAnimal _PhotographAnimal{"photographanimal", "拍摄动物", "拍摄所选动物。"};

	static TeleportToProduct _TeleportToProduct{"tptoproduct", "传送到货物", "传送到所选货物。"};
	static CollectProduct _CollectProduct{"collectproduct", "收集货物", "收集所选货物。"};

	static TeleportToGoldenClover _TeleportToGoldenClover{"tptogoldenclover", "传送到金色四叶草", "传送到金色四叶草。"};
	static CollectGoldenClover _CollectGoldenClover{"collectgoldenclover", "收集金色四叶草", "收集金色四叶草。"};
}
