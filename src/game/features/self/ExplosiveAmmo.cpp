#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Pools.hpp"
#include "types/fx/ExplosionType.hpp"
#include <game/gta/Natives.hpp>
#include "core/commands/ListCommand.hpp"
#include "core/commands/FloatCommand.hpp"
#include "game/gta/Scripts.hpp"

namespace YimMenu::Features
{
	static const std::vector<std::pair<int, const char*>> g_ExplosionTypeArray = {
	    {static_cast<int>(ExplosionType::DONTCARE), "不指定"},
	    {static_cast<int>(ExplosionType::GRENADE), "手榴弹"},
	    {static_cast<int>(ExplosionType::GRENADELAUNCHER), "榴弹发射器"},
	    {static_cast<int>(ExplosionType::STICKYBOMB), "黏弹"},
	    {static_cast<int>(ExplosionType::MOLOTOV), "燃烧瓶"},
	    {static_cast<int>(ExplosionType::ROCKET), "火箭弹"},
	    {static_cast<int>(ExplosionType::TANKSHELL), "坦克炮弹"},
	    {static_cast<int>(ExplosionType::HI_OCTANE), "高辛烷燃爆"},
	    {static_cast<int>(ExplosionType::CAR), "汽车爆炸"},
	    {static_cast<int>(ExplosionType::PLANE), "飞机爆炸"},
	    {static_cast<int>(ExplosionType::PETROL_PUMP), "油泵爆炸"},
	    {static_cast<int>(ExplosionType::BIKE), "摩托车爆炸"},
	    {static_cast<int>(ExplosionType::DIR_STEAM), "定向蒸汽"},
	    {static_cast<int>(ExplosionType::DIR_FLAME), "定向火焰"},
	    {static_cast<int>(ExplosionType::DIR_WATER_HYDRANT), "定向消防栓水柱"},
	    {static_cast<int>(ExplosionType::DIR_GAS_CANISTER), "定向气罐"},
	    {static_cast<int>(ExplosionType::BOAT), "船只爆炸"},
	    {static_cast<int>(ExplosionType::SHIP_DESTROY), "船只摧毁"},
	    {static_cast<int>(ExplosionType::TRUCK), "卡车爆炸"},
	    {static_cast<int>(ExplosionType::BULLET), "子弹"},
	    {static_cast<int>(ExplosionType::SMOKEGRENADELAUNCHER), "烟雾榴弹发射器"},
	    {static_cast<int>(ExplosionType::SMOKEGRENADE), "烟雾弹"},
	    {static_cast<int>(ExplosionType::BZGAS), "BZ 毒气"},
	    {static_cast<int>(ExplosionType::FLARE), "信号弹"},
	    {static_cast<int>(ExplosionType::GAS_CANISTER), "气罐"},
	    {static_cast<int>(ExplosionType::EXTINGUISHER), "灭火器"},
	    {static_cast<int>(ExplosionType::PROGRAMMABLEAR), "可编程自动步枪"},
	    {static_cast<int>(ExplosionType::TRAIN), "列车爆炸"},
	    {static_cast<int>(ExplosionType::BARREL), "油桶爆炸"},
	    {static_cast<int>(ExplosionType::PROPANE), "丙烷爆炸"},
	    {static_cast<int>(ExplosionType::BLIMP), "飞艇爆炸"},
	    {static_cast<int>(ExplosionType::DIR_FLAME_EXPLODE), "定向火焰爆炸"},
	    {static_cast<int>(ExplosionType::TANKER), "油罐车爆炸"},
	    {static_cast<int>(ExplosionType::PLANE_ROCKET), "飞机火箭弹"},
	    {static_cast<int>(ExplosionType::VEHICLE_BULLET), "载具子弹"},
	    {static_cast<int>(ExplosionType::GAS_TANK), "油箱爆炸"},
	    {static_cast<int>(ExplosionType::BIRD_CRAP), "鸟粪爆炸"},
	    {static_cast<int>(ExplosionType::RAILGUN), "电磁炮"},
	    {static_cast<int>(ExplosionType::BLIMP2), "飞艇 2"},
	    {static_cast<int>(ExplosionType::FIREWORK), "烟花"},
	    {static_cast<int>(ExplosionType::SNOWBALL), "雪球"},
	    {static_cast<int>(ExplosionType::PROXMINE), "感应地雷"},
	    {static_cast<int>(ExplosionType::VALKYRIE_CANNON), "女武神机炮"},
	    {static_cast<int>(ExplosionType::AIR_DEFENCE), "防空爆炸"},
	    {static_cast<int>(ExplosionType::PIPEBOMB), "土制炸弹"},
	    {static_cast<int>(ExplosionType::VEHICLEMINE), "载具地雷"},
	    {static_cast<int>(ExplosionType::EXPLOSIVEAMMO), "爆炸弹药"},
	    {static_cast<int>(ExplosionType::APCSHELL), "APC 炮弹"},
	    {static_cast<int>(ExplosionType::BOMB_CLUSTER), "集束炸弹"},
	    {static_cast<int>(ExplosionType::BOMB_GAS), "毒气炸弹"},
	    {static_cast<int>(ExplosionType::BOMB_INCENDIARY), "燃烧弹"},
	    {static_cast<int>(ExplosionType::BOMB_STANDARD), "标准炸弹"},
	    {static_cast<int>(ExplosionType::TORPEDO), "鱼雷"},
	    {static_cast<int>(ExplosionType::TORPEDO_UNDERWATER), "水下鱼雷"},
	    {static_cast<int>(ExplosionType::BOMBUSHKA_CANNON), "轰炸机机炮"},
	    {static_cast<int>(ExplosionType::BOMB_CLUSTER_SECONDARY), "集束炸弹（次级）"},
	    {static_cast<int>(ExplosionType::HUNTER_BARRAGE), "猎杀者连射火箭"},
	    {static_cast<int>(ExplosionType::HUNTER_CANNON), "猎杀者机炮"},
	    {static_cast<int>(ExplosionType::ROGUE_CANNON), "叛逆者机炮"},
	    {static_cast<int>(ExplosionType::MINE_UNDERWATER), "水下地雷"},
	    {static_cast<int>(ExplosionType::ORBITAL_CANNON), "天基炮"},
	    {static_cast<int>(ExplosionType::BOMB_STANDARD_WIDE), "标准炸弹（广域）"},
	    {static_cast<int>(ExplosionType::EXPLOSIVEAMMO_SHOTGUN), "爆炸弹药（霰弹枪）"},
	    {static_cast<int>(ExplosionType::OPPRESSOR2_CANNON), "暴君 Mk II 机炮"},
	    {static_cast<int>(ExplosionType::MORTAR_KINETIC), "动能迫击炮"},
	    {static_cast<int>(ExplosionType::VEHICLEMINE_KINETIC), "载具地雷（动能）"},
	    {static_cast<int>(ExplosionType::VEHICLEMINE_EMP), "载具地雷（EMP）"},
	    {static_cast<int>(ExplosionType::VEHICLEMINE_SPIKE), "载具地雷（尖刺）"},
	    {static_cast<int>(ExplosionType::VEHICLEMINE_SLICK), "载具地雷（湿滑）"},
	    {static_cast<int>(ExplosionType::VEHICLEMINE_TAR), "载具地雷（焦油）"},
	    {static_cast<int>(ExplosionType::SCRIPT_DRONE), "脚本无人机"},
	    {static_cast<int>(ExplosionType::RAYGUN), "射线枪"},
	    {static_cast<int>(ExplosionType::BURIEDMINE), "埋藏地雷"},
	    {static_cast<int>(ExplosionType::SCRIPT_MISSILE), "脚本导弹"},
	    {static_cast<int>(ExplosionType::RCTANK_ROCKET), "RC 坦克火箭弹"},
	    {static_cast<int>(ExplosionType::BOMB_WATER), "水弹"},
	    {static_cast<int>(ExplosionType::BOMB_WATER_SECONDARY), "水弹（次级）"},
	    {static_cast<int>(ExplosionType::_0xF728C4A9), "未知爆炸类型（F728C4A9）"},
	    {static_cast<int>(ExplosionType::_0xBAEC056F), "未知爆炸类型（BAEC056F）"},
	    {static_cast<int>(ExplosionType::FLASHGRENADE), "闪光弹"},
	    {static_cast<int>(ExplosionType::STUNGRENADE), "震撼弹"},
	    {static_cast<int>(ExplosionType::_0x763D3B3B), "未知爆炸类型（763D3B3B）"},
	    {static_cast<int>(ExplosionType::SCRIPT_MISSILE_LARGE), "脚本导弹（大型）"},
	    {static_cast<int>(ExplosionType::SUBMARINE_BIG), "潜艇（大型）"},
	    {static_cast<int>(ExplosionType::EMPLAUNCHER_EMP), "EMP 发射器"},
	};

	static ListCommand _SelectedExplosion{
	    "selectedexplosion",
	    "爆炸类型",
	    "选择一种爆炸类型",
	    g_ExplosionTypeArray,
	    static_cast<int>(ExplosionType::BULLET)};

	static FloatCommand _ExplosionDamageScale{
	    "explosiondamage",
	    "爆炸伤害倍率",
	    "设置爆炸造成的伤害倍率",
	    0.0f,
	    1000.0f,
	    1.0f};

	static FloatCommand _CameraShake{
	    "explosioncamerashake",
	    "爆炸镜头震动",
	    "控制爆炸时镜头震动的强度",
	    0.0f,
	    10.0f,
	    0.1f};

	class ExplosiveAmmo : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		void OnTick() override
		{
			TriggerExplosion();
		}

		void TriggerExplosion()
		{
			Hash weapon = WEAPON::GET_SELECTED_PED_WEAPON(Self::GetPed().GetHandle());

			// Ensure ped is using a non-melee/non-explosive weapon AND that their not using melee of any kind before applying
			if (WEAPON::IS_PED_ARMED(Self::GetPed().GetHandle(), 4) && !PED::IS_PED_PERFORMING_MELEE_ACTION(Self::GetPed().GetHandle()))
			{
				Vector3 impactCoords;
				if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(Self::GetPed().GetHandle(), &impactCoords))
				{
					auto explosionType = static_cast<ExplosionType>(_SelectedExplosion.GetState());
					float damageScale = _ExplosionDamageScale.GetState();
					float shake = _CameraShake.GetState();

					Scripts::RunWithSpoofedThreadName("am_mp_orbital_cannon"_J, [=] {
						FIRE::ADD_OWNED_EXPLOSION(
						    Self::GetPed().GetHandle(),
						    impactCoords.x,
						    impactCoords.y,
						    impactCoords.z,
						    static_cast<int>(explosionType),
						    damageScale,
						    true,  // isAudible
						    false, // isInvisible
						    shake);
					});
				}
			}
		}
	};

	static ExplosiveAmmo _ExplosiveAmmo{"ExplosiveAmmo", "爆炸弹药", "你射出的每一发子弹都会爆炸。"};
}
