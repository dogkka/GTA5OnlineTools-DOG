#pragma once
#include "core/commands/BoolCommand.hpp"
#include "core/commands/ColorCommand.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/commands/StringCommand.hpp"
#include "core/util/Math.hpp"
#include "core/util/RainbowColor.hpp"
#include "game/gta/Entity.hpp"
#include "game/gta/Natives.hpp"
#include "types/pad/ControllerInputs.hpp"

namespace YimMenu::Features
{
	enum class CustomWeapons
	{
		CAGE_GUN,
		DELETE_GUN,
		GRAVITY_GUN,
		STEAL_VEHICLE_GUN,
		REPAIR_GUN,
		VEHICLE_GUN,
		TELEPORT_GUN,
		PAINT_GUN
	};

	static const ControllerInputs attackControls[] = {
		ControllerInputs::INPUT_WEAPON_WHEEL_NEXT,
		ControllerInputs::INPUT_WEAPON_WHEEL_PREV,
		ControllerInputs::INPUT_ATTACK,
		ControllerInputs::INPUT_ATTACK2,
		ControllerInputs::INPUT_VEH_ATTACK,
		ControllerInputs::INPUT_VEH_ATTACK2,
		ControllerInputs::INPUT_VEH_PASSENGER_ATTACK,
		ControllerInputs::INPUT_VEH_FLY_ATTACK,
		ControllerInputs::INPUT_VEH_FLY_ATTACK2,
	};

	static std::vector<std::pair<int, const char*>> g_CustomWeaponTypes = {
		{static_cast<int>(CustomWeapons::CAGE_GUN), "笼子枪"},
		{static_cast<int>(CustomWeapons::DELETE_GUN), "删除枪"},
		{static_cast<int>(CustomWeapons::GRAVITY_GUN), "重力枪"},
		{static_cast<int>(CustomWeapons::STEAL_VEHICLE_GUN), "偷车枪"},
		{static_cast<int>(CustomWeapons::REPAIR_GUN), "修理枪"},
		{static_cast<int>(CustomWeapons::VEHICLE_GUN), "载具枪"},
		{static_cast<int>(CustomWeapons::TELEPORT_GUN), "传送枪"},
		{static_cast<int>(CustomWeapons::PAINT_GUN), "喷漆枪"},
	};
	static ListCommand _CustomWeaponType{"customweapontype", "武器类型", "要使用的自定义武器类型", g_CustomWeaponTypes, static_cast<int>(CustomWeapons::CAGE_GUN)};

	static std::vector<std::pair<int, const char*>> g_PaintGunRainbowColorStyles = {
		{static_cast<int>(RainbowColorStyle::Fade), "渐变"},
		{static_cast<int>(RainbowColorStyle::Spasm), "闪变"}
	};
	static ListCommand _PaintGunRainbowColorStyle{"paintgunrainbowcolorstyle", "彩虹颜色样式", "喷漆枪彩虹颜色的变化样式", g_PaintGunRainbowColorStyles, static_cast<int>(RainbowColorStyle::Fade)};

	class CustomWeapon : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override;
	};

	static CustomWeapon _CustomWeapon{"customweapon", "自定义武器", "修改武器能力"};
	static BoolCommand _CustomWeaponEnabledOnWeaponOut{"customweaponenabledonweaponout", "仅在持枪时启用", "仅在武器拿出时启用自定义武器", true};
	static BoolCommand _GravityGunLaunchOnRelease{"gravitygunlaunchonrelease", "松手时发射实体", "在释放实体时将其发射出去", false};
	static StringCommand _VehicleGunModel{"vehiclegunmodel", "载具模型", "载具的模型名称"};
	static ColorCommand _PaintGunColor{"paintguncolor", "喷漆枪颜色", "喷漆枪使用的颜色"};
	static BoolCommand _PaintGunRainbowColorEnabled{"paintgunrainbowcolorenabled", "彩虹颜色", "为喷漆枪启用彩虹颜色", false};
	static IntCommand _PaintGunRainbowColorSpeed{"paintgunrainbowcolorspeed", "彩虹颜色速度", "喷漆枪彩虹颜色的变化速度", 1, 10, 1};
}
