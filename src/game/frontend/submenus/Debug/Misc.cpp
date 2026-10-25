#include "DrawVariable.hpp"
#include "Locals.hpp"
#include "core/filemgr/FileMgr.hpp"
#include "core/localization/Localization.hpp"
#include "game/backend/Self.hpp"
#include "game/pointers/Pointers.hpp"
#include "core/backend/FiberPool.hpp"
#include "game/gta/ScriptFunction.hpp"
#include "game/gta/Natives.hpp"
#include "types/script/globals/GlobalPlayerBD.hpp"
#include "types/script/ScriptEvent.hpp"

namespace YimMenu::Submenus
{
	std::shared_ptr<Category> BuildMiscMenu()
	{
		auto misc = std::make_unique<Category>("杂项");

		misc->AddItem(std::make_unique<ImGuiItem>([] {
			if (ImGui::Button("网络脱离"))
			{
				FiberPool::Push([] {
					NETWORK::NETWORK_BAIL(0, 24, 0);
				});
			}

			static int interiorIndex = 0;
			ImGui::InputInt("室内索引", &interiorIndex);

			static bool enterOwnerInterior = false;
			ImGui::Checkbox("进入房主室内", &enterOwnerInterior);

			if (ImGui::Button("执行传送"))
			{
				FiberPool::Push([] {
					SCRIPT_EVENT_SEND_TO_INTERIOR message;
					message.Interior = interiorIndex;
					message.EnterOwnerInterior = enterOwnerInterior;
					message.GoonsOnly = false;
					message.InstanceId = 0;
					message.SubInstanceId = -1;
					message.Owner = Self::GetPlayer().GetId();
					message.Distance = 99999;
					message.Position = {0, 0, 0};

					message.SetAllPlayers();
					message.Send();
				});
			}

			static int team;
			ImGui::InputInt("队伍", &team);
			if (ImGui::Button("执行队伍切换"))
			{
				FiberPool::Push([] {
					static ScriptFunction DoTeamSwap("fm_mission_controller"_J, ScriptPointer("DoTeamSwap", "2D 02 04 00 00 38 00 50"));
					DoTeamSwap.Call<void>(team, true);
				});
			}
		}));

		misc->AddItem(std::make_shared<CommandItem>("dumpdatahash"_J));

		return misc;
	}
}
