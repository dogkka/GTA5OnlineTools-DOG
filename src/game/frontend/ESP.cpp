#include "ESP.hpp"
#include "game/gta/Object.hpp"
#include "common.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/commands/ColorCommand.hpp"
#include "core/util/Math.hpp"
#include "game/backend/Players.hpp"
#include "game/backend/Self.hpp"
#include "game/pointers/Pointers.hpp"
#include "game/gta/Pools.hpp"
#include "game/gta/Scripts.hpp"
#include "game/gta/invoker/Invoker.hpp"
#include "game/gta/Natives.hpp"
#include "core/localization/Localization.hpp"

namespace
{
	// Human
	constexpr int headBone = 37193; //FB_Brow_Centre_000 is a much better bone to use as head
	constexpr int neckBone = 39317;
	constexpr int torsoBone = 23553;
	constexpr int leftHandBone = 18905;
	constexpr int rightHandBone = 57005;
	constexpr int leftFootBone = 14201;
	constexpr int rightFootBone = 52301;
	constexpr int leftElbowBone = 22711;
	constexpr int rightElbowBone = 2992;
	constexpr int leftKneeBone = 46078;
	constexpr int rightKneeBone = 16335;
	constexpr int leftShoulderBone = 61163; // TODO verify all the bones
	constexpr int rightShoulderBone = 28252;
}

namespace YimMenu::Features
{
	// Players
	BoolCommand _ESPDrawPlayers("espdrawplayers", "绘制玩家", "是否为玩家显示 ESP？");
	BoolCommand _ESPDrawDeadPlayers("espdrawdeadplayers", "绘制死亡玩家", "是否为已死亡的玩家显示 ESP？");

	BoolCommand _ESPName("espnameplayers", "显示玩家名称", "是否显示玩家名称？");
	BoolCommand _ESPDistance("espdistanceplayers", "显示玩家距离", "是否显示玩家距离？");
	BoolCommand _ESPSkeleton("espskeletonplayers", "显示玩家骨骼", "是否显示玩家骨骼？");

	ColorCommand _NameColorPlayers("namecolorplayers", "玩家名称颜色", "更改玩家名称 ESP 的颜色。", ImVec4{1.0f, 1.0f, 1.0f, 1.0f});
	ColorCommand _DistanceColorPlayers("distancecolorplayers", "玩家距离颜色", "更改玩家距离 ESP 的颜色。", ImVec4{1.0f, 1.0f, 1.0f, 1.0f});
	ColorCommand _SkeletonColorPlayers("skeletoncolorplayers", "玩家骨骼颜色", "更改玩家骨骼 ESP 的颜色。", ImVec4{1.0f, 1.0f, 1.0f, 1.0f});

	// Peds
	BoolCommand _ESPDrawPeds("espdrawpeds", "绘制行人", "是否为行人显示 ESP？");
	BoolCommand _ESPDrawDeadPeds("espdrawdeadpeds", "绘制死亡行人", "是否为已死亡的行人显示 ESP？");

	BoolCommand _ESPModelPeds("espmodelspeds", "显示行人模型", "是否显示行人模型？");
	BoolCommand _ESPNetworkInfoPeds("espnetinfopeds", "显示行人网络信息", "是否显示行人的网络信息？");
	BoolCommand _ESPScriptInfoPeds("espscriptinfopeds", "显示行人脚本信息", "是否显示行人的脚本信息？");
	BoolCommand _ESPDistancePeds("espdistancepeds", "显示行人距离", "是否显示行人距离？");
	BoolCommand _ESPSkeletonPeds("espskeletonpeds", "显示行人骨骼", "是否显示行人骨骼？");

	ColorCommand _HashColorPeds("hashcolorpeds", "行人哈希颜色", "更改行人哈希 ESP 的颜色。", ImVec4{1.0f, 1.0f, 1.0f, 1.0f});
	ColorCommand _SkeletonColorPeds("skeletoncolorpeds", "行人骨骼颜色", "更改行人骨骼 ESP 的颜色。", ImVec4{1.0f, 1.0f, 1.0f, 1.0f});

	// Objects
	BoolCommand _ESPDrawObjects("espdrawobjects", "绘制特殊物体", "是否为特殊物体显示 ESP？");
	BoolCommand _ESPNetworkInfoObjects("espnetinfoobjects", "显示物体网络信息", "是否显示物体的网络信息？");
	BoolCommand _ESPScriptInfoObjects("espscriptinfoobjects", "显示物体脚本信息", "是否显示物体的脚本信息？");
	BoolCommand _ESPDistanceObjects("espdistanceobjects", "显示物体距离", "是否显示物体距离？");

	ColorCommand _HashColorObjects("hashcolorobjects", "物体哈希颜色", "更改物体哈希 ESP 的颜色。", ImVec4{1.0f, 1.0f, 1.0f, 1.0f});
}

namespace YimMenu
{
	static ImVec4 death_bg = ImVec4(0.117f, 0.113f, 0.172f, .75f);
	static ImVec4 armor_blue_bg = ImVec4(0.36f, 0.71f, 0.89f, .75f);
	static ImVec4 armor_blue = ImVec4(0.36f, 0.71f, 0.89f, 1.f);
	static ImVec4 health_green_bg = ImVec4(0.29f, 0.69f, 0.34f, .75f);
	static ImVec4 health_green = ImVec4(0.29f, 0.69f, 0.34f, 1.f);
	static ImVec4 health_yellow_bg = ImVec4(0.69f, 0.49f, 0.29f, .75f);
	static ImVec4 health_yellow = ImVec4(0.69f, 0.49f, 0.29f, 1.f);
	static ImVec4 health_red_bg = ImVec4(0.69f, 0.29f, 0.29f, .75f);
	static ImVec4 health_red = ImVec4(0.69f, 0.29f, 0.29f, 1.f);
	static ImVec4 Green = ImVec4(0.29f, 0.69f, 0.34f, 1.f);
	static ImVec4 Orange = ImVec4(0.69f, 0.49f, 0.29f, 1.f);
	static ImVec4 Red = ImVec4(0.69f, 0.29f, 0.29f, 1.f);
	static ImVec4 Blue = ImVec4(0.36f, 0.71f, 0.89f, 1.f);
	static ImVec4 White = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	// We want worldToScreen to return a bool since it will write {0.f, 0.f} to the screen vector if the entity is off-screen, messing up our draws
	static auto worldToScreen = [](rage::fvector3 coords) -> std::optional<ImVec2> {
		float screen_x{}, screen_y{};
		if (!GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(coords.x, coords.y, coords.z, &screen_x, &screen_y))
			return std::nullopt;

		// Instead of returning a zero vector if the bool is false, we will return a nullopt
		return ImVec2{screen_x * (*Pointers.ScreenResX), screen_y * (*Pointers.ScreenResY)};
	};

	void DrawSkeleton(Ped ped, ImDrawList* drawList, ImColor color)
	{
		if (!ped.IsValid())
			return;

		// Lambdas are great; use them so we don't have to individually edit parameters for every call to AddLine
		auto drawBoneLine = [&](int boneA, int boneB) {
			auto screenA = worldToScreen(ped.GetBonePosition(boneA));
			auto screenB = worldToScreen(ped.GetBonePosition(boneB));

			if (!screenA || !screenB)
				return;

			drawList->AddLine(*screenA, *screenB, color, 1.0f);
		};

		// What's the point of drawing a skeleton if we don't draw the most important bone?
		auto headScreen = worldToScreen(ped.GetBonePosition(headBone));
		auto neckScreen = worldToScreen(ped.GetBonePosition(neckBone));

		// We don't want the radius of the head circle to be static (too big at long range or too small close up), so let's scale it based on the length of the head to neck vector
		if (headScreen && neckScreen)
		{
			float dx = headScreen->x - neckScreen->x;
			float dy = headScreen->y - neckScreen->y;
			float neckToHeadLength = std::sqrt(dx * dx + dy * dy);
			float radius = neckToHeadLength * 0.25f;

			drawList->AddCircle(*headScreen, radius, color, 12, 1.5f);
		}

		drawBoneLine(headBone, neckBone);
		drawBoneLine(neckBone, leftShoulderBone);
		drawBoneLine(leftShoulderBone, leftElbowBone);
		drawBoneLine(leftElbowBone, leftHandBone);
		drawBoneLine(neckBone, rightShoulderBone);
		drawBoneLine(rightShoulderBone, rightElbowBone);
		drawBoneLine(rightElbowBone, rightHandBone);
		drawBoneLine(neckBone, torsoBone);
		drawBoneLine(torsoBone, leftKneeBone);
		drawBoneLine(leftKneeBone, leftFootBone);
		drawBoneLine(torsoBone, rightKneeBone);
		drawBoneLine(rightKneeBone, rightFootBone);
	}

	//TODO : Very bare bones currently, expand and possibly refactor
	static void DrawPlayer(Player plyr, ImDrawList* drawList)
	{
		if (!plyr.IsValid() || !plyr.GetPed().IsValid() || plyr == Self::GetPlayer()
		    || !worldToScreen(plyr.GetPed().GetBonePosition(torsoBone))
		    || (plyr.GetPed().IsDead() && !Features::_ESPDrawDeadPlayers.GetState()))
			return;

		float distanceToPlayer = Self::GetPed().GetPosition().GetDistance(plyr.GetPed().GetBonePosition(torsoBone));
		int alphaBasedOnDistance = 255;
		ImColor colorBasedOnDistance = Red;

		if (distanceToPlayer < 100.f)
			colorBasedOnDistance = Green, alphaBasedOnDistance = 255;
		else if (distanceToPlayer > 100.f && distanceToPlayer < 300.f)
			colorBasedOnDistance = Orange, alphaBasedOnDistance = 200;
		else if (distanceToPlayer > 300.f)
			colorBasedOnDistance = Red, alphaBasedOnDistance = 125;

		if (Features::_ESPName.GetState())
		{
			if (auto headScreen = worldToScreen(plyr.GetPed().GetBonePosition(headBone)))
			{
				drawList->AddText(*headScreen, plyr == Players::GetSelected() ? ImGui::ColorConvertFloat4ToU32(Blue) : ImGui::ColorConvertFloat4ToU32(Features::_NameColorPlayers.GetState()), plyr.GetName());
			}
		}
		if (Features::_ESPDistance.GetState())
		{
			std::string distanceStr = std::to_string((int)Self::GetPed().GetPosition().GetDistance(plyr.GetPed().GetBonePosition(torsoBone))) + "m";
			if (auto headScreen = worldToScreen(plyr.GetPed().GetBonePosition(headBone)))
				drawList->AddText({headScreen->x, headScreen->y + 20}, colorBasedOnDistance, distanceStr.c_str());
		}

		//TODO Boxes, Distance colors, Friendlies, Tracers, Health bars

		if (Features::_ESPSkeleton.GetState() /* && !plyr.GetPed().IsAnimal() */) // yes, this is neccesary.
		{
			if (distanceToPlayer < 250.f)
			{
				DrawSkeleton(plyr.GetPed(), drawList, ImGui::ColorConvertFloat4ToU32(Features::_SkeletonColorPlayers.GetState()));
			}
		}
	}

	static void DrawPed(Ped ped, ImDrawList* drawList)
	{
		if (!ped.IsValid() || ped.IsPlayer() || ped == Self::GetPlayer().GetPed() || !worldToScreen(ped.GetBonePosition(torsoBone)) || (ped.IsDead() && !Features::_ESPDrawDeadPeds.GetState()))
			return;

		float distanceToPed = 0.0f;

		if (auto local = Self::GetPed())
			distanceToPed = local.GetPosition().GetDistance(ped.GetBonePosition(torsoBone));

		int alphaBasedOnDistance = 255;
		ImColor colorBasedOnDistance = Red;

		if (distanceToPed < 100.f)
			colorBasedOnDistance = Green, alphaBasedOnDistance = 255;
		else if (distanceToPed > 100.f && distanceToPed < 300.f)
			colorBasedOnDistance = Orange, alphaBasedOnDistance = 200;
		else if (distanceToPed > 300.f)
			colorBasedOnDistance = Red, alphaBasedOnDistance = 125;

		std::string info = "";

		if (Features::_ESPModelPeds.GetState())
		{
			//if (auto it = Data::g_PedModels.find(ped.GetModel()); it != Data::g_PedModels.end())
			//	info += std::format("{} ", it->second);
			//else
			info += std::format("0x{:08X} ", (joaat_t)ped.GetModel());
		}

		if (Features::_ESPNetworkInfoPeds.GetState() && ped.IsNetworked())
		{
			auto owner = Player(ped.GetOwner());
			auto id = ped.GetNetworkObjectId();

			info += std::format("{} {} ", id, owner.GetName());
		}

		if (Features::_ESPScriptInfoPeds.GetState())
		{
			if (auto script = ENTITY::GET_ENTITY_SCRIPT(ped.GetHandle(), nullptr))
			{
				info += std::format("{} ", script);
			}
		}

		if (!info.empty())
			if (auto headScreen = worldToScreen(ped.GetBonePosition(headBone)))
				drawList->AddText(*headScreen, ImGui::ColorConvertFloat4ToU32(Features::_HashColorPeds.GetState()), info.c_str());

		if (Features::_ESPDistancePeds.GetState())
		{
			std::string distanceStr = std::to_string((int)distanceToPed) + "m";
			if (auto headScreen = worldToScreen(ped.GetBonePosition(headBone)))
				drawList->AddText({headScreen->x, headScreen->y + 20}, colorBasedOnDistance, distanceStr.c_str());
		}

		//TODO Boxes, Distance colors, Tracers, Health bars

		if (Features::_ESPSkeletonPeds.GetState() /* && !ped.IsAnimal() */)
		{
			if (distanceToPed < 250.f)
			{
				DrawSkeleton(ped, drawList, Features::_SkeletonColorPeds.GetState());
			}
		}
	}

	static void DrawObject(Object object, ImDrawList* drawList)
	{
		if (!object.IsValid())
			return;

		bool is_camera = object.IsCamera();
		bool is_signal_jammer = object.IsSignalJammer();
		bool is_mission_object = object.IsMissionEntity();

		if (!is_camera && !is_signal_jammer && !is_mission_object)
			return;

		float distanceToObject = 0.0f;

		if (auto local = Self::GetPed())
			distanceToObject = local.GetPosition().GetDistance(object.GetPosition());

		int alphaBasedOnDistance = 255;
		ImColor colorBasedOnDistance = Red;

		if (distanceToObject < 100.f)
			colorBasedOnDistance = Green, alphaBasedOnDistance = 255;
		else if (distanceToObject > 100.f && distanceToObject < 300.f)
			colorBasedOnDistance = Orange, alphaBasedOnDistance = 200;
		else if (distanceToObject > 300.f)
			colorBasedOnDistance = Red, alphaBasedOnDistance = 125;

		Vector3 coords = object.GetPosition();
		float distance = Self::GetPed().GetPosition().GetDistance(coords);
		ImColor color = ImGui::ColorConvertFloat4ToU32(Features::_HashColorObjects.GetState());
		std::string info = std::format("0x{:08X} ", (joaat_t)object.GetModel());

		if (Features::_ESPNetworkInfoObjects.GetState() && object.IsNetworked())
		{
			auto owner = Player(object.GetOwner());
			auto id = object.GetNetworkObjectId();

			info += std::format("{} {} ", id, owner.GetName());
		}

		if (Features::_ESPScriptInfoObjects.GetState())
		{
			if (auto script = ENTITY::GET_ENTITY_SCRIPT(object.GetHandle(), nullptr))
			{
				info += std::format("{} ", script);
			}
		}

		if (is_camera)
		{
			color = Red;
			info += std::format(" ({})", YimMenu::Localization::Translate("Camera"));
		}
		else if (is_signal_jammer)
		{
			color = Red;
			info += std::format(" ({})", YimMenu::Localization::Translate("Jammer"));
		}
		else if (is_mission_object)
		{
			info += std::format(" ({})", YimMenu::Localization::Translate("Mission Object"));
		}

		if (auto screenPos = worldToScreen(coords))
			drawList->AddText(*screenPos, color, info.c_str());

		if (Features::_ESPDistanceObjects.GetState())
		{
			std::string distanceStr = std::to_string((int)distanceToObject) + "m";
			if (auto screenPos = worldToScreen(object.GetPosition()))
				drawList->AddText({screenPos->x, screenPos->y + 20}, colorBasedOnDistance, distanceStr.c_str());
		}
	}

	void ESP::Draw()
	{
		if (!NativeInvoker::AreHandlersCached() || CAMERA::IS_SCREEN_FADED_OUT() || HUD::IS_WARNING_MESSAGE_ACTIVE() || HUD::IS_PAUSE_MENU_ACTIVE() || NETWORK::NETWORK_IS_IN_MP_CUTSCENE())
			return;

		const auto originalFontSize = ImGui::GetFont()->Scale;
		auto* currentFont = ImGui::GetFont();
		currentFont->Scale *= 1.2;
		ImGui::PushFont(ImGui::GetFont());

		if (const auto drawList = ImGui::GetBackgroundDrawList())
		{
			// lots of race conditions and other things make this particularly crash-prone
			__try
			{
				if (Features::_ESPDrawPlayers.GetState())
				{
					for (auto& [id, player] : Players::GetPlayers())
					{
						DrawPlayer(player, drawList);
					}
				}
				if (Features::_ESPDrawPeds.GetState() && GetPedPool())
				{
					for (Ped ped : Pools::GetPeds())
					{
						if (ped && ped.GetPointer<void*>())
							DrawPed(ped, drawList);
					}
				}
				if (Features::_ESPDrawObjects.GetState() && GetObjectPool())
				{
					for (auto obj : Pools::GetObjects())
					{
						if (obj)
							DrawObject(obj.As<Object>(), drawList);
					}
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
			}
		}

		currentFont->Scale = originalFontSize;
		ImGui::PopFont();
	}
}
