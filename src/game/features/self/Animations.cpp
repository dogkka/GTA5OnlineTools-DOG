#include "core/backend/FiberPool.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

#include <chrono>
#include <string>

namespace YimMenu::Features
{
	struct NamedAnim
	{
		const char* label;
		const char* dict;
		const char* anim;
	};

	// ---------------- 场景（Stand 同款场景名） ----------------
	static const char* kScenarios[] = {
	    "WORLD_HUMAN_MUSCLE_FLEX",
	    "WORLD_HUMAN_SMOKING",
	    "WORLD_HUMAN_BUM_SLUMPED",
	    "WORLD_HUMAN_CHEERING",
	    "WORLD_HUMAN_GUARD_STAND",
	    "WORLD_HUMAN_HAMMERING",
	    "WORLD_HUMAN_COP_IDLES",
	    "PROP_HUMAN_BUM_BIN",
	    "WORLD_HUMAN_MOBILE_FILM_SHOCKING",
	    "WORLD_HUMAN_HANG_OUT_STREET",
	    "WORLD_HUMAN_STAND_IMPATIENT",
	    "WORLD_HUMAN_STAND_MOBILE",
	    "WORLD_HUMAN_LEANING",
	    "CODE_HUMAN_MEDIC_KNEEL",
	    "CODE_HUMAN_CROSS_ROAD_WAIT",
	    "WORLD_HUMAN_MAID_CLEAN",
	    "PROP_HUMAN_ATM",
	    "WORLD_HUMAN_YOGA",
	    "WORLD_HUMAN_HUMAN_STATUE",
	    "WORLD_HUMAN_WELDING",
	    "WORLD_HUMAN_SUNBATHE_BACK",
	    "WORLD_HUMAN_SUNBATHE",
	    "WORLD_HUMAN_JOG_STANDING",
	    "WORLD_HUMAN_BINOCULARS",
	    "WORLD_HUMAN_BUM_STANDING",
	    "WORLD_HUMAN_GARDENER_PLANT",
	    "WORLD_HUMAN_PICNIC",
	    "WORLD_HUMAN_STAND_FISHING",
	    "WORLD_HUMAN_WINDOW_SHOP_BROWSE",
	    "PROP_HUMAN_SEAT_BENCH",
	};

	static ListCommand _ScenarioSelect{"animscenariosel", "选择场景", "选择要播放的休闲场景动作。",
	    {{0, "秀肌肉"}, {1, "抽烟"}, {2, "颓废坐地"}, {3, "欢呼"}, {4, "站岗"}, {5, "敲锤子"}, {6, "警察站姿"}, {7, "翻垃圾桶"},
	        {8, "手机拍视频"}, {9, "街头闲逛"}, {10, "不耐烦"}, {11, "玩手机"}, {12, "倚靠"}, {13, "医务跪地"}, {14, "等红灯"},
	        {15, "打扫卫生"}, {16, "取款机"}, {17, "瑜伽"}, {18, "人体雕像"}, {19, "焊接"}, {20, "趴着晒"}, {21, "日光浴"},
	        {22, "原地慢跑"}, {23, "望远镜"}, {24, "流浪汉站姿"}, {25, "种花"}, {26, "野餐"}, {27, "钓鱼"}, {28, "逛橱窗"}, {29, "坐长椅"}},
	    0};

	// ---------------- 舞蹈（Stand 同款舞蹈表） ----------------
	static const NamedAnim kDances[] = {
	    {"夜店舞·男A", "anim@amb@nightclub@mini@dance@dance_solo@male@var_a@", "low_center"},
	    {"夜店舞·男B", "anim@amb@nightclub@mini@dance@dance_solo@male@var_b@", "high_center_down"},
	    {"夜店舞·女A", "anim@amb@nightclub@mini@dance@dance_solo@female@var_a@", "high_center"},
	    {"夜店舞·女B", "anim@amb@nightclub@mini@dance@dance_solo@female@var_b@", "low_center_down"},
	    {"电臀舞", "anim@amb@nightclub@mini@dance@dance_solo@shuffle@", "high_center"},
	    {"Woogie", "anim@mp_player_intcelebrationfemale@the_woogie", "the_woogie"},
	    {"泰勒舞", "rcmnigel1bnmt_1b", "dance_loop_tyler"},
	    {"中国风舞", "misschinese2_crystalmazemcs1_cs", "dance_loop_tao"},
	    {"小丑摇摆", "move_clown@p_m_zero_idles@", "fidget_short_dance"},
	    {"狂欢群舞", "anim@amb@nightclub@dancers@crowddance_facedj@hi_intensity", "hi_dance_facedj_09_v2_female^1"},
	    {"夜店群舞", "anim@amb@nightclub@dancers@solomun_entourage@", "mi_dance_facedj_17_v1_female^1"},
	    {"舞台舞", "anim@amb@nightclub@dancers@podium_dancers@", "hi_dance_facedj_17_v2_male^5"},
	};

	static ListCommand _DanceSelect{"animdancesel", "选择舞蹈", "选择要播放的舞蹈。",
	    {{0, "夜店舞·男A"}, {1, "夜店舞·男B"}, {2, "夜店舞·女A"}, {3, "夜店舞·女B"}, {4, "电臀舞"}, {5, "Woogie"},
	        {6, "泰勒舞"}, {7, "中国风舞"}, {8, "小丑摇摆"}, {9, "狂欢群舞"}, {10, "夜店群舞"}, {11, "舞台舞"}},
	    0};

	// ---------------- 动作（Stand 同款动作表） ----------------
	static const NamedAnim kAnimList[] = {
	    {"空气吉他", "anim@mp_player_intcelebrationfemale@air_guitar", "air_guitar"},
	    {"空气键盘", "anim@mp_player_intcelebrationfemale@air_synth", "air_synth"},
	    {"飞吻", "anim@mp_player_intcelebrationfemale@blow_kiss", "blow_kiss"},
	    {"捂脸", "anim@mp_player_intcelebrationfemale@face_palm", "face_palm"},
	    {"空手道", "anim@mp_player_intcelebrationfemale@karate_chops", "karate_chops"},
	    {"影子拳击", "anim@mp_player_intcelebrationmale@shadow_boxing", "shadow_boxing"},
	    {"街舞·帽技", "anim@arena@celeb@flat@solo@no_props@", "cap_a_player_a"},
	    {"街舞·空翻", "anim@arena@celeb@flat@solo@no_props@", "flip_a_player_a"},
	    {"街舞·滑步A", "anim@arena@celeb@flat@solo@no_props@", "slide_a_player_a"},
	    {"街舞·滑步B", "anim@arena@celeb@flat@solo@no_props@", "slide_b_player_a"},
	    {"庆祝", "rcmfanatic1celebrate", "celebrate"},
	    {"紧张", "rcmme_tracey1", "nervous_loop"},
	    {"睡觉", "mp_sleep", "sleep_loop"},
	    {"洗澡", "mp_safehouseshower@male@", "male_shower_idle_b"},
	    {"醉汉站姿A", "random@drunk_driver_1", "drunk_driver_stand_loop_dd1"},
	    {"醉汉站姿B", "random@drunk_driver_1", "drunk_driver_stand_loop_dd2"},
	    {"醉倒", "random@drunk_driver_1", "drunk_fall_over"},
	    {"招手打车", "taxi_hail", "taxi_hail"},
	    {"吹哨招手", "rcmnigel1c", "hailing_whistle_waive_a"},
	    {"举手投降", "missminuteman_1ig_2", "handsup_base"},
	    {"摄影机跟拍", "missfinale_c2mcs_1", "fin_c2_mcs_1_camman"},
	    {"手势·可恶", "gestures@m@standing@casual", "gesture_damn"},
	    {"耸肩", "gestures@f@standing@casual", "gesture_shrug_hard"},
	    {"帮派手势", "mp_player_int_uppergang_sign_a", "mp_player_int_gang_sign_a"},
	    {"挑衅", "mini@triathlon", "want_some_of_this"},
	    {"来啊", "misscommon@response", "bring_it_on"},
	    {"抱箱子", "anim@heists@box_carry@", "idle"},
	    {"提油桶", "move_weapon@jerrycan@generic", "idle"},
	    {"喝啤酒", "amb@world_human_drinking@beer@male@idle_a", "idle_c"},
	    {"喝咖啡", "amb@world_human_drinking@coffee@male@idle_a", "idle_c"},
	    {"弹吉他", "amb@world_human_musician@guitar@male@idle_a", "idle_b"},
	    {"打手鼓", "amb@world_human_musician@bongos@male@base", "base"},
	    {"看手机", "cellphone@", "cellphone_text_read_base"},
	    {"打电话", "cellphone@", "cellphone_call_listen_base"},
	    {"心肺复苏", "mini@cpr@char_a@cpr_str", "cpr_pumpchest"},
	    {"被捕", "random@arrests@busted", "idle_a"},
	    {"敲门", "timetable@jimmy@doorknock@", "knockdoor_idle"},
	    {"俯卧撑", "amb@world_human_push_ups@male@idle_a", "idle_d"},
	    {"瑜伽姿势", "missfam5_yoga", "a2_pose"},
	    {"赛前指挥", "random@street_race", "grid_girl_race_start"},
	};

	static ListCommand _AnimSelect{"animlistsel", "选择动作", "选择要播放的动作。",
	    {{0, "空气吉他"}, {1, "空气键盘"}, {2, "飞吻"}, {3, "捂脸"}, {4, "空手道"}, {5, "影子拳击"}, {6, "街舞·帽技"}, {7, "街舞·空翻"},
	        {8, "街舞·滑步A"}, {9, "街舞·滑步B"}, {10, "庆祝"}, {11, "紧张"}, {12, "睡觉"}, {13, "洗澡"}, {14, "醉汉站姿A"}, {15, "醉汉站姿B"},
	        {16, "醉倒"}, {17, "招手打车"}, {18, "吹哨招手"}, {19, "举手投降"}, {20, "摄影机跟拍"}, {21, "手势·可恶"}, {22, "耸肩"}, {23, "帮派手势"},
	        {24, "挑衅"}, {25, "来啊"}, {26, "抱箱子"}, {27, "提油桶"}, {28, "喝啤酒"}, {29, "喝咖啡"}, {30, "弹吉他"}, {31, "打手鼓"},
	        {32, "看手机"}, {33, "打电话"}, {34, "心肺复苏"}, {35, "被捕"}, {36, "敲门"}, {37, "俯卧撑"}, {38, "瑜伽姿势"}, {39, "赛前指挥"}},
	    0};

	static void PlayScenario(const char* scenario)
	{
		FiberPool::Push([scenario] {
			auto ped = Self::GetPed();
			if (!ped)
				return;
			TASK::CLEAR_PED_TASKS(ped.GetHandle());
			TASK::TASK_START_SCENARIO_IN_PLACE(ped.GetHandle(), scenario, 0, true);
		});
	}

	static void PlayAnim(const char* dict, const char* anim)
	{
		FiberPool::Push([dict, anim] {
			auto ped = Self::GetPed();
			if (!ped)
				return;

			STREAMING::REQUEST_ANIM_DICT(dict);
			for (int i = 0; i < 200 && !STREAMING::HAS_ANIM_DICT_LOADED(dict); i++)
				ScriptMgr::Yield(std::chrono::milliseconds(10));

			if (!STREAMING::HAS_ANIM_DICT_LOADED(dict))
			{
				Notifications::Show("动画", "动画资源加载失败。", NotificationType::Error);
				return;
			}

			TASK::CLEAR_PED_TASKS(ped.GetHandle());
			// 1 = 循环，32 = 保留玩家控制（移动即可打断）
			TASK::TASK_PLAY_ANIM(ped.GetHandle(), dict, anim, 8.0f, -8.0f, -1, 33, 0.0f, false, false, false);
		});
	}

	class PlayScenarioCommand : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			const int idx = _ScenarioSelect.GetState();
			if (idx < 0 || idx >= (int)(sizeof(kScenarios) / sizeof(kScenarios[0])))
				return;
			PlayScenario(kScenarios[idx]);
		}
	};

	class PlayDanceCommand : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			const int idx = _DanceSelect.GetState();
			if (idx < 0 || idx >= (int)(sizeof(kDances) / sizeof(kDances[0])))
				return;
			PlayAnim(kDances[idx].dict, kDances[idx].anim);
		}
	};

	class PlayAnimCommand : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			const int idx = _AnimSelect.GetState();
			if (idx < 0 || idx >= (int)(sizeof(kAnimList) / sizeof(kAnimList[0])))
				return;
			PlayAnim(kAnimList[idx].dict, kAnimList[idx].anim);
		}
	};

	class StopAnimCommand : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			FiberPool::Push([] {
				auto ped = Self::GetPed();
				if (!ped)
					return;
				TASK::CLEAR_PED_TASKS(ped.GetHandle());
				TASK::CLEAR_PED_SECONDARY_TASK(ped.GetHandle());
			});
		}
	};

	static PlayScenarioCommand _PlayScenario{"animscenario", "播放场景", "播放上面选中的场景动作。"};
	static PlayDanceCommand _PlayDance{"animdance", "播放舞蹈", "播放上面选中的舞蹈。"};
	static PlayAnimCommand _PlayAnim{"animplay", "播放动作", "播放上面选中的动作。"};
	static StopAnimCommand _StopAnim{"animstop", "停止动画", "结束当前正在播放的动画/场景。"};
}
