#include "Scripts.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/localization/Localization.hpp"
#include "game/gta/data/ScriptNames.hpp"
#include "game/gta/data/StackSizes.hpp"
#include "game/gta/Scripts.hpp"
#include "game/gta/Natives.hpp"
#include "game/pointers/Pointers.hpp"
#include "types/rage/atArray.hpp"
#include "types/script/GtaThread.hpp"
#include "types/script/scrProgram.hpp"
#include "types/script/CGameScriptHandlerNetComponent.hpp"
#include "types/network/CNetGamePlayer.hpp"
#include "misc/cpp/imgui_stdlib.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <string>
#include <string_view>
#include <unordered_map>

namespace YimMenu::Submenus
{
	namespace
	{
		using namespace std::literals;

		static const std::unordered_map<std::string_view, std::string_view> g_ScriptTokenMap = {
		    {"abigail"sv, "阿比盖尔"sv},
		    {"act"sv, "章节"sv},
		    {"achievement"sv, "成就"sv},
		    {"activity"sv, "活动"sv},
		    {"acid"sv, "致幻剂"sv},
		    {"agency"sv, "事务所"sv},
		    {"air"sv, "空中"sv},
		    {"aircraft"sv, "飞行器"sv},
		    {"airstrike"sv, "空袭"sv},
		    {"ai"sv, "人工智能"sv},
		    {"amb"sv, "环境"sv},
		    {"ambient"sv, "环境"sv},
		    {"ammo"sv, "弹药"sv},
		    {"am"sv, "活动"sv},
		    {"and"sv, "与"sv},
		    {"armenian"sv, "亚美尼亚人"sv},
		    {"animals"sv, "动物"sv},
		    {"apartment"sv, "公寓"sv},
		    {"app"sv, "应用"sv},
		    {"appbroadcast"sv, "应用 广播"sv},
		    {"appbusinesshub"sv, "应用 生意中心"sv},
		    {"appcamera"sv, "应用 相机"sv},
		    {"appchecklist"sv, "应用 清单"sv},
		    {"appcontacts"sv, "应用 联系人"sv},
		    {"appcovertops"sv, "应用 掩护行动"sv},
		    {"appemail"sv, "应用 电子邮件"sv},
		    {"appextraction"sv, "应用 撤离"sv},
		    {"appfixersecurity"sv, "应用 调停者安保"sv},
		    {"apphackerden"sv, "应用 黑客巢穴"sv},
		    {"apphackertruck"sv, "应用 黑客卡车"sv},
		    {"apphs"sv, "应用 豪劫"sv},
		    {"appimportexport"sv, "应用 进出口"sv},
		    {"appinternet"sv, "应用 网络"sv},
		    {"appjipmp"sv, "应用 即时加入线上"sv},
		    {"appmedia"sv, "应用 媒体"sv},
		    {"appmpbossagency"sv, "应用 首领事务所"sv},
		    {"appmpemail"sv, "应用 在线邮件"sv},
		    {"appmpjoblistnew"sv, "应用 在线任务列表"sv},
		    {"apporganiser"sv, "应用 组织器"sv},
		    {"appprogresshub"sv, "应用 进度中心"sv},
		    {"apprepeatplay"sv, "应用 重玩"sv},
		    {"appsecurohack"sv, "应用 保镖公司黑客"sv},
		    {"appsecuroserv"sv, "应用 保镖公司"sv},
		    {"appsettings"sv, "应用 设置"sv},
		    {"appsidetask"sv, "应用 支线任务"sv},
		    {"appsmuggler"sv, "应用 走私者"sv},
		    {"apptextmessage"sv, "应用 短信"sv},
		    {"apptrackify"sv, "应用 追踪器"sv},
		    {"appvinewoodmenu"sv, "应用 好麦坞菜单"sv},
		    {"appvlsi"sv, "应用 芯片破解"sv},
		    {"appzit"sv, "应用 音乐识别"sv},
		    {"arcade"sv, "游戏厅"sv},
		    {"arena"sv, "竞技场"sv},
		    {"armory"sv, "军械库"sv},
		    {"armwrestling"sv, "掰手腕"sv},
		    {"armybase"sv, "军事基地"sv},
		    {"assassin"sv, "暗杀"sv},
		    {"assault"sv, "突袭"sv},
		    {"auto"sv, "汽车"sv},
		    {"backup"sv, "支援"sv},
		    {"basement"sv, "地下室"sv},
		    {"bank"sv, "银行"sv},
		    {"bail"sv, "保释"sv},
		    {"bailbond"sv, "保释担保"sv},
		    {"bar"sv, "酒吧"sv},
		    {"barry"sv, "巴里"sv},
		    {"battles"sv, "争夺战"sv},
		    {"base"sv, "基地"sv},
		    {"beach"sv, "海滩"sv},
		    {"beast"sv, "野兽"sv},
		    {"bed"sv, "床位"sv},
		    {"biker"sv, "摩托帮"sv},
		    {"bj"sv, "摩天崖"sv},
		    {"block"sv, "封锁"sv},
		    {"blimp"sv, "飞艇"sv},
		    {"boat"sv, "船只"sv},
		    {"bot"sv, "机器人"sv},
		    {"bounty"sv, "悬赏"sv},
		    {"box"sv, "箱子"sv},
		    {"bru"sv, "布鲁"sv},
		    {"builder"sv, "构建器"sv},
		    {"bus"sv, "巴士"sv},
		    {"business"sv, "生意"sv},
		    {"cam"sv, "摄像头"sv},
		    {"cabinet"sv, "机柜"sv},
		    {"camera"sv, "相机"sv},
		    {"car"sv, "载具"sv},
		    {"arrive"sv, "抵达"sv},
		    {"cargo"sv, "货物"sv},
		    {"chop"sv, "小查"sv},
		    {"carmod"sv, "改车"sv},
		    {"carsteal"sv, "偷车"sv},
		    {"carwash"sv, "亲力亲为洗车行"sv},
		    {"casino"sv, "赌场"sv},
		    {"castl"sv, "城堡"sv},
		    {"castle"sv, "城堡"sv},
		    {"claw"sv, "抓娃娃机"sv},
		    {"crane"sv, "起重机"sv},
		    {"challenges"sv, "挑战"sv},
		    {"checklist"sv, "清单"sv},
		    {"cellphone"sv, "手机"sv},
		    {"chinese"sv, "中国城"sv},
		    {"cinema"sv, "电影院"sv},
		    {"cloud"sv, "云端"sv},
		    {"cinematic"sv, "过场动画"sv},
		    {"clothes"sv, "服装"sv},
		    {"club"sv, "俱乐部"sv},
		    {"combat"sv, "战斗"sv},
		    {"community"sv, "社区"sv},
		    {"collection"sv, "收集"sv},
		    {"content"sv, "内容"sv},
		    {"contact"sv, "联系人"sv},
		    {"contacts"sv, "联系人"sv},
		    {"contract"sv, "合约"sv},
		    {"contraband"sv, "违禁品"sv},
		    {"control"sv, "控制"sv},
		    {"country"sv, "乡村"sv},
		    {"ctrl"sv, "控制"sv},
		    {"controler"sv, "控制器"sv},
		    {"controller"sv, "控制器"sv},
		    {"cops"sv, "警察"sv},
		    {"crime"sv, "犯罪"sv},
		    {"cp"sv, "佩里科岛"sv},
		    {"crate"sv, "货箱"sv},
		    {"creator"sv, "创建器"sv},
		    {"criminal"sv, "犯罪"sv},
		    {"cutscene"sv, "过场动画"sv},
		    {"cut"sv, "过场"sv},
		    {"damage"sv, "破坏"sv},
		    {"data"sv, "数据"sv},
		    {"darts"sv, "飞镖"sv},
		    {"dead"sv, "死亡"sv},
		    {"deathmatch"sv, "死亡竞赛"sv},
		    {"debug"sv, "调试"sv},
		    {"defend"sv, "防守"sv},
		    {"defunct"sv, "废弃"sv},
		    {"delivery"sv, "交付"sv},
		    {"den"sv, "巢穴"sv},
		    {"destroy"sv, "摧毁"sv},
		    {"detector"sv, "探测器"sv},
		    {"distract"sv, "分散"sv},
		    {"docks"sv, "码头"sv},
		    {"dreyfuss"sv, "德雷福斯"sv},
		    {"drf"sv, "德雷福斯"sv},
		    {"driveway"sv, "车道"sv},
		    {"doors"sv, "车门"sv},
		    {"driver"sv, "司机"sv},
		    {"drone"sv, "无人机"sv},
		    {"drop"sv, "投放"sv},
		    {"drug"sv, "毒品"sv},
		    {"drinking"sv, "饮酒"sv},
		    {"drunk"sv, "醉酒"sv},
		    {"editor"sv, "编辑器"sv},
		    {"email"sv, "电子邮件"sv},
		    {"eps"sv, "邪教"sv},
		    {"error"sv, "错误"sv},
		    {"epsilon"sv, "邪教"sv},
		    {"event"sv, "事件"sv},
		    {"exile"sv, "流放"sv},
		    {"exp"sv, "出口"sv},
		    {"export"sv, "出口"sv},
		    {"extreme"sv, "极限"sv},
		    {"exports"sv, "出口"sv},
		    {"exotic"sv, "珍品"sv},
		    {"exposed"sv, "现形"sv},
		    {"ext"sv, "室外"sv},
		    {"exterior"sv, "室外"sv},
		    {"extraction"sv, "撤离"sv},
		    {"fameorshame"sv, "名与利"sv},
		    {"fanatic"sv, "狂热分子"sv},
		    {"family"sv, "家族"sv},
		    {"fbi"sv, "联邦调查局"sv},
		    {"fbi4"sv, "联邦调查局 4"sv},
		    {"ferriswheel"sv, "摩天轮"sv},
		    {"fortune"sv, "占卜"sv},
		    {"finale"sv, "终章"sv},
		    {"finalec"sv, "终章丙"sv},
		    {"fin"sv, "终章"sv},
		    {"field"sv, "场地"sv},
		    {"finders"sv, "拾荒者"sv},
		    {"example"sv, "示例"sv},
		    {"fistfight"sv, "拳斗"sv},
		    {"fixer"sv, "调停者"sv},
		    {"flow"sv, "流程"sv},
		    {"floor"sv, "楼层"sv},
		    {"fmmc"sv, "在线战局"sv},
		    {"fmmcstruct"sv, "在线战局结构"sv},
		    {"fm"sv, "自由模式"sv},
		    {"fps"sv, "帧率"sv},
		    {"friends"sv, "好友"sv},
		    {"fullmap"sv, "全地图"sv},
		    {"foursome"sv, "四人组"sv},
		    {"franklin"sv, "富兰克林"sv},
		    {"freemode"sv, "自由模式"sv},
		    {"ga"sv, "帮派攻击"sv},
		    {"gang"sv, "帮派"sv},
		    {"garage"sv, "车库"sv},
		    {"gb"sv, "帮派头目"sv},
		    {"gpb"sv, "高尔夫俱乐部会所"sv},
		    {"getaway"sv, "逃逸"sv},
		    {"ghosts"sv, "幽灵"sv},
		    {"global"sv, "全局"sv},
		    {"globals"sv, "全局变量"sv},
		    {"gooch"sv, "古奇"sv},
		    {"goods"sv, "货物"sv},
		    {"golf"sv, "高尔夫"sv},
		    {"ground"sv, "地面"sv},
		    {"gtest"sv, "测试"sv},
		    {"guard"sv, "守卫"sv},
		    {"gun"sv, "枪械"sv},
		    {"gunrunning"sv, "军火贸易"sv},
		    {"game"sv, "游戏"sv},
		    {"hairdo"sv, "发型"sv},
		    {"hack"sv, "入侵"sv},
		    {"hacker"sv, "黑客"sv},
		    {"hao"sv, "阿浩"sv},
		    {"hauler"sv, "拖车"sv},
		    {"hangar"sv, "机库"sv},
		    {"heatmap"sv, "热力图"sv},
		    {"high"sv, "高级"sv},
		    {"help"sv, "帮助"sv},
		    {"hotwire"sv, "热接线"sv},
		    {"heist"sv, "抢劫"sv},
		    {"helitours"sv, "希金斯直升机旅行社"sv},
		    {"heli"sv, "直升机"sv},
		    {"hi"sv, "高空"sv},
		    {"hideout"sv, "藏身处"sv},
		    {"hold"sv, "抢劫"sv},
		    {"holidays"sv, "节日"sv},
		    {"hot"sv, "热点"sv},
		    {"house"sv, "房屋"sv},
		    {"hq"sv, "总部"sv},
		    {"hs"sv, "豪劫"sv},
		    {"hs4"sv, "佩里科岛豪劫"sv},
		    {"hsw"sv, "豪改"sv},
		    {"hunt"sv, "狩猎"sv},
		    {"hunting"sv, "狩猎"sv},
		    {"hub"sv, "中心"sv},
		    {"imp"sv, "进口"sv},
		    {"ie"sv, "进出口"sv},
		    {"info"sv, "信息"sv},
		    {"init"sv, "初始化"sv},
		    {"invites"sv, "邀请"sv},
		    {"internet"sv, "网络"sv},
		    {"int"sv, "室内"sv},
		    {"interior"sv, "室内"sv},
		    {"intro"sv, "介绍"sv},
		    {"isd"sv, "岛屿"sv},
		    {"island"sv, "岛屿"sv},
		    {"jewelry"sv, "珠宝店"sv},
		    {"jewel"sv, "珠宝"sv},
		    {"job"sv, "任务"sv},
		    {"jobs"sv, "工作"sv},
		    {"install"sv, "安装"sv},
		    {"josh"sv, "乔什"sv},
		    {"juggalo"sv, "小丑帮"sv},
		    {"joyrider"sv, "兜风贼"sv},
		    {"keepers"sv, "守财者"sv},
		    {"kill"sv, "击杀"sv},
		    {"king"sv, "称王"sv},
		    {"lab"sv, "实验室"sv},
		    {"land"sv, "降落"sv},
		    {"launch"sv, "启动"sv},
		    {"lowrider"sv, "低底盘车"sv},
		    {"lsia"sv, "洛圣都国际机场"sv},
		    {"lester"sv, "莱斯特"sv},
		    {"launcher"sv, "启动器"sv},
		    {"leave"sv, "离开"sv},
		    {"line"sv, "线路"sv},
		    {"limo"sv, "豪华轿车"sv},
		    {"list"sv, "列表"sv},
		    {"lounge"sv, "休息室"sv},
		    {"lower"sv, "下层"sv},
		    {"lsa"sv, "洛圣都机场"sv},
		    {"luxury"sv, "豪华"sv},
		    {"main"sv, "主"sv},
		    {"maintain"sv, "维护"sv},
		    {"magdemo"sv, "弹匣演示"sv},
		    {"maude"sv, "莫德"sv},
		    {"mansion"sv, "豪宅"sv},
		    {"me"sv, "我"sv},
		    {"media"sv, "媒体"sv},
		    {"med"sv, "医疗"sv},
		    {"meet"sv, "聚会"sv},
		    {"menuped"sv, "菜单行人"sv},
		    {"menu"sv, "菜单"sv},
		    {"metal"sv, "金属"sv},
		    {"michael"sv, "麦克"sv},
		    {"minute"sv, "分钟"sv},
		    {"minigame"sv, "小游戏"sv},
		    {"mission"sv, "任务"sv},
		    {"mode"sv, "模式"sv},
		    {"mod"sv, "改装"sv},
		    {"modern"sv, "现代"sv},
		    {"movie"sv, "电影"sv},
		    {"mp"sv, "在线模式"sv},
		    {"mrsphilips"sv, "菲利普斯夫人"sv},
		    {"multistorey"sv, "多层"sv},
		    {"music"sv, "音乐"sv},
		    {"net"sv, "网络"sv},
		    {"nightclub"sv, "夜总会"sv},
		    {"nigel"sv, "奈杰尔"sv},
		    {"nimb"sv, "尼姆布斯"sv},
		    {"ob"sv, "老大业务"sv},
		    {"office"sv, "办公室"sv},
		    {"off"sv, "离开"sv},
		    {"omega"sv, "欧米伽"sv},
		    {"odd"sv, "零星"sv},
		    {"operations"sv, "行动"sv},
		    {"ops"sv, "行动"sv},
		    {"organiser"sv, "组织器"sv},
		    {"orbital"sv, "轨道炮"sv},
		    {"output"sv, "输出"sv},
		    {"paradise"sv, "天堂"sv},
		    {"outreach"sv, "外展"sv},
		    {"pausemenu"sv, "暂停菜单"sv},
		    {"paparazzo"sv, "狗仔队"sv},
		    {"parcel"sv, "包裹"sv},
		    {"pass"sv, "传递"sv},
		    {"peds"sv, "行人"sv},
		    {"ped"sv, "行人"sv},
		    {"penthouse"sv, "顶层豪宅"sv},
		    {"phantom"sv, "幻影"sv},
		    {"pickups"sv, "拾取物"sv},
		    {"pi"sv, "私人调查"sv},
		    {"pilot"sv, "飞行员"sv},
		    {"pilotschool"sv, "飞行学校"sv},
		    {"perf"sv, "性能"sv},
		    {"physics"sv, "物理"sv},
		    {"rc"sv, "遥控"sv},
		    {"plane"sv, "飞机"sv},
		    {"planning"sv, "规划"sv},
		    {"plaza"sv, "广场"sv},
		    {"player"sv, "玩家"sv},
		    {"penned"sv, "围困"sv},
		    {"pm"sv, "派对菜单"sv},
		    {"point"sv, "点位"sv},
		    {"possessed"sv, "附身"sv},
		    {"postrc"sv, "遥控载具"sv},
		    {"prep"sv, "前置任务"sv},
		    {"premium"sv, "高级"sv},
		    {"prison"sv, "监狱"sv},
		    {"progress"sv, "进度"sv},
		    {"progresshub"sv, "进度中心"sv},
		    {"public"sv, "公开"sv},
		    {"reward"sv, "奖励"sv},
		    {"organization"sv, "组织"sv},
		    {"prop"sv, "道具"sv},
		    {"property"sv, "资产"sv},
		    {"prostitute"sv, "应召女郎"sv},
		    {"props"sv, "道具"sv},
		    {"prototype"sv, "原型"sv},
		    {"p"sv, "P"sv},
		    {"pb"sv, "太平洋标准银行"sv},
		    {"race"sv, "竞速"sv},
		    {"rampage"sv, "暴走"sv},
		    {"races"sv, "竞速"sv},
		    {"random"sv, "随机"sv},
		    {"range"sv, "靶场"sv},
		    {"re"sv, "随机事件"sv},
		    {"reg"sv, "注册"sv},
		    {"registration"sv, "注册"sv},
		    {"repeat"sv, "重复"sv},
		    {"requests"sv, "请求"sv},
		    {"resupply"sv, "补货"sv},
		    {"rob"sv, "抢劫"sv},
		    {"robbery"sv, "抢劫"sv},
		    {"rural"sv, "乡村"sv},
		    {"rollercoaster"sv, "过山车"sv},
		    {"rpa"sv, "遥控"sv},
		    {"safehouse"sv, "安全屋"sv},
		    {"salvage"sv, "打捞场"sv},
		    {"sandbox"sv, "沙盒"sv},
		    {"sb"sv, "破坏行动"sv},
		    {"scripttest"sv, "脚本测试"sv},
		    {"scene"sv, "场景"sv},
		    {"script"sv, "脚本"sv},
		    {"selector"sv, "选择器"sv},
		    {"selling"sv, "出售"sv},
		    {"seating"sv, "座位"sv},
		    {"seats"sv, "座位"sv},
		    {"security"sv, "安保"sv},
		    {"sell"sv, "出售"sv},
		    {"setup"sv, "前置"sv},
		    {"shop"sv, "商店"sv},
		    {"sh"sv, "商店抢劫"sv},
		    {"short"sv, "短程"sv},
		    {"shooting"sv, "射击"sv},
		    {"shootout"sv, "枪战"sv},
		    {"shopping"sv, "购物"sv},
		    {"showroom"sv, "展厅"sv},
		    {"sightseeing"sv, "观光"sv},
		    {"sleep"sv, "睡眠"sv},
		    {"sleeping"sv, "沉睡"sv},
		    {"simeon"sv, "西门"sv},
		    {"slashers"sv, "连环杀手"sv},
		    {"smoking"sv, "吸烟"sv},
		    {"smuggler"sv, "走私者"sv},
		    {"smpl"sv, "简易"sv},
		    {"smoketest"sv, "冒烟测试"sv},
		    {"soaktest"sv, "压力测试"sv},
		    {"sofa"sv, "沙发"sv},
		    {"social"sv, "社交"sv},
		    {"solomon"sv, "所罗门"sv},
		    {"sonar"sv, "声呐"sv},
		    {"source"sv, "来源"sv},
		    {"sp"sv, "单人模式"sv},
		    {"spmc"sv, "单人模式挑战"sv},
		    {"spawn"sv, "生成"sv},
		    {"stat"sv, "统计"sv},
		    {"standard"sv, "标准"sv},
		    {"stats"sv, "统计"sv},
		    {"startup"sv, "启动"sv},
		    {"stoner"sv, "瘾君子"sv},
		    {"stripclub"sv, "脱衣舞俱乐部"sv},
		    {"streaming"sv, "流媒体"sv},
		    {"strength"sv, "力量"sv},
		    {"studio"sv, "工作室"sv},
		    {"submarine"sv, "潜艇"sv},
		    {"survival"sv, "生存战"sv},
		    {"t"sv, "T"sv},
		    {"take"sv, "起飞"sv},
		    {"takedown"sv, "击落"sv},
		    {"target"sv, "目标"sv},
		    {"taxi"sv, "出租车"sv},
		    {"tennis"sv, "网球"sv},
		    {"teller"sv, "算命师"sv},
		    {"test"sv, "测试"sv},
		    {"textmessage"sv, "短信"sv},
		    {"the"sv, ""sv},
		    {"thelastone"sv, "最后一战"sv},
		    {"tester"sv, "测试器"sv},
		    {"time"sv, "时间"sv},
		    {"to"sv, "前往"sv},
		    {"title"sv, "标题"sv},
		    {"trailer"sv, "拖车"sv},
		    {"tonya"sv, "托尼娅"sv},
		    {"tonyacall"sv, "托尼娅来电"sv},
		    {"tours"sv, "观光"sv},
		    {"trackify"sv, "追踪器"sv},
		    {"tracker"sv, "追踪器"sv},
		    {"train"sv, "列车"sv},
		    {"trail"sv, "踪迹"sv},
		    {"towing"sv, "拖吊"sv},
		    {"traffick"sv, "走私"sv},
		    {"trafficking"sv, "走私"sv},
		    {"trial"sv, "挑战赛"sv},
		    {"ufo"sv, "不明飞行物"sv},
		    {"ugc"sv, "用户创作内容"sv},
		    {"update"sv, "更新"sv},
		    {"upper"sv, "上层"sv},
		    {"vip"sv, "贵宾"sv},
		    {"tv"sv, "电视"sv},
		    {"wardrobe"sv, "衣柜"sv},
		    {"weapons"sv, "武器"sv},
		    {"whistle"sv, "哨声"sv},
		    {"vend"sv, "自动售货"sv},
		    {"zancudo"sv, "桑库多"sv},
		    {"trevor"sv, "崔佛"sv},
		    {"trigger"sv, "触发"sv},
		    {"triggerer"sv, "触发器"sv},
		    {"truck"sv, "卡车"sv},
		    {"tut"sv, "教程"sv},
		    {"tuner"sv, "改装车"sv},
		    {"ufos"sv, "不明飞行物"sv},
		    {"up"sv, "持械抢劫"sv},
		    {"valentine"sv, "情人节"sv},
		    {"valet"sv, "代客泊车"sv},
		    {"veh"sv, "载具"sv},
		    {"vehicle"sv, "载具"sv},
		    {"vehrob"sv, "载具抢劫"sv},
		    {"vel"sv, "维拉姆"sv},
		    {"vinewood"sv, "好麦坞"sv},
		    {"wash"sv, "洗车"sv},
		    {"washup"sv, "冲上海岸"sv},
		    {"warehouse"sv, "仓库"sv},
		    {"weazel"sv, "威索"sv},
		    {"weed"sv, "大麻"sv},
		    {"weapon"sv, "武器"sv},
		    {"widget"sv, "小组件"sv},
		    {"work"sv, "工作"sv},
		    {"yard"sv, "场地"sv},
		    {"yoga"sv, "瑜伽"sv},
		    {"xmas"sv, "圣诞节"sv},
		    {"yacht"sv, "游艇"sv},
		};

		std::string TranslateScriptToken(std::string_view token)
		{
			if (const auto it = g_ScriptTokenMap.find(token); it != g_ScriptTokenMap.end())
				return std::string(it->second);

			if (!token.empty() && std::all_of(token.begin(), token.end(), [](const char ch) {
				    return std::isdigit(static_cast<unsigned char>(ch));
			    }))
				return std::string(token);

			std::string composed;
			size_t offset = 0;
			while (offset < token.size())
			{
				std::string_view bestKey;
				std::string_view bestValue;
				for (const auto& [key, value] : g_ScriptTokenMap)
				{
					if (key.empty() || value.empty() || key.size() >= token.size() - offset + 1)
						continue;

					if (token.substr(offset).starts_with(key) && key.size() > bestKey.size())
					{
						bestKey = key;
						bestValue = value;
					}
				}

				if (bestKey.empty())
					break;

				if (!composed.empty())
					composed += ' ';
				composed += bestValue;
				offset += bestKey.size();
			}

			if (offset == token.size() && !composed.empty())
				return composed;

			std::string normalized(token);
			for (char& ch : normalized)
				if (ch == '-')
					ch = ' ';

			return normalized;
		}

		std::string TranslateRuntimeDisplayValue(std::string_view value)
		{
			if (value.empty())
				return {};

			const auto directlyTranslated = Localization::Translate(value);
			if (directlyTranslated != value)
				return directlyTranslated;

			std::string display;
			std::string token;
			auto flushToken = [&] {
				if (token.empty())
					return;

				const auto tokenExact = Localization::Translate(token);
				if (!display.empty())
					display.push_back(' ');

				if (tokenExact != token)
				{
					display += tokenExact;
				}
				else
				{
					std::string lowered = token;
					std::transform(lowered.begin(), lowered.end(), lowered.begin(), [](unsigned char ch) {
						return static_cast<char>(std::tolower(ch));
					});
					display += TranslateScriptToken(lowered);
				}

				token.clear();
			};

			for (size_t i = 0; i < value.size(); ++i)
			{
				const auto ch = static_cast<unsigned char>(value[i]);
				const bool isTokenChar = std::isalnum(ch) || ch == '_';
				if (isTokenChar)
				{
					token.push_back(static_cast<char>(ch));
				}
				else
				{
					flushToken();
				}
			}
			flushToken();

			return display.empty() ? std::string(value) : display;
		}

		std::string TranslateScriptDisplayName(std::string_view scriptName)
		{
			static const std::unordered_map<std::string_view, std::string_view> scriptNameMap = {
			    {"af_intro_t_sandy"sv, "序章介绍：桑迪海岸"sv},
			    {"agency_heist3a"sv, "事务所抢劫 3甲"sv},
			    {"agency_heist3b"sv, "事务所抢劫 3乙"sv},
			    {"aicover_test"sv, "人工智能掩护测试"sv},
			    {"ainewengland_test"sv, "人工智能新英格兰测试"sv},
			    {"altruist_cult"sv, "利他教邪教"sv},
			    {"ambient_diving"sv, "环境潜水"sv},
			    {"am_agency_suv"sv, "事务所休旅车活动"sv},
			    {"am_airstrike"sv, "空袭支援活动"sv},
			    {"am_ammo_drop"sv, "空投弹药活动"sv},
			    {"am_armwrestling"sv, "掰手腕活动"sv},
			    {"am_armwrestling_apartment"sv, "公寓掰手腕活动"sv},
			    {"am_arena_shp"sv, "竞技场商店抢劫活动"sv},
			    {"am_backup_heli"sv, "支援直升机活动"sv},
			    {"am_boat_taxi"sv, "船只出租活动"sv},
			    {"am_bru_box"sv, "布鲁补给箱活动"sv},
			    {"am_car_mod_tut"sv, "改车教程活动"sv},
			    {"am_challenges"sv, "挑战活动"sv},
			    {"am_contact_requests"sv, "联系人请求活动"sv},
			    {"am_cp_collection"sv, "佩里科收集活动"sv},
			    {"am_crate_drop"sv, "货箱空投活动"sv},
			    {"am_criminal_damage"sv, "犯罪破坏活动"sv},
			    {"am_cr_sell_drugs"sv, "出售毒品活动"sv},
			    {"am_darts_apartment"sv, "公寓飞镖活动"sv},
			    {"am_darts"sv, "飞镖活动"sv},
			    {"am_daily_objectives"sv, "每日目标活动"sv},
			    {"am_dead_drop"sv, "藏匿点活动"sv},
			    {"am_destroy_veh"sv, "摧毁载具活动"sv},
			    {"am_distract_cops"sv, "分散警察注意活动"sv},
			    {"am_doors"sv, "车门活动"sv},
			    {"am_dropoffhooker"sv, "送应召女郎活动"sv},
			    {"am_ferriswheel"sv, "摩天轮活动"sv},
			    {"am_fistfight"sv, "拳斗活动"sv},
			    {"am_gang_call"sv, "帮派呼叫活动"sv},
			    {"am_ga_pickups"sv, "帮派攻击拾取物活动"sv},
			    {"am_heist_int"sv, "抢劫任务室内"sv},
			    {"am_heli_taxi"sv, "直升机出租活动"sv},
			    {"am_hitchhiker"sv, "搭便车活动"sv},
			    {"am_hold_up"sv, "持枪抢劫活动"sv},
			    {"am_hot_property"sv, "热点资产活动"sv},
			    {"am_hot_target"sv, "热点目标活动"sv},
			    {"am_importexport"sv, "进出口活动"sv},
			    {"am_imp_exp"sv, "进出口活动"sv},
			    {"am_king_of_the_castle"sv, "称王争霸活动"sv},
			    {"am_lester_cut"sv, "莱斯特过场"sv},
			    {"am_launcher"sv, "活动启动器"sv},
			    {"am_lowrider_int"sv, "低底盘车室内活动"sv},
			    {"am_mission_launch"sv, "任务启动"sv},
			    {"am_mp_arcade_love_meter"sv, "线上游戏厅爱情测试机"sv},
			    {"am_mp_arc_cab_manager"sv, "线上游戏厅出租车管理"sv},
			    {"am_mp_boardroom_seating"sv, "线上董事会议室座位"sv},
			    {"am_mp_bunker"sv, "线上地堡"sv},
			    {"am_mp_carwash_launch"sv, "亲力亲为洗车行启动"sv},
			    {"am_mp_hotwire"sv, "线上热接线活动"sv},
			    {"am_mp_orbital_cannon"sv, "线上轨道炮"sv},
			    {"am_mp_sb_car_wash"sv, "亲力亲为洗车行"sv},
			    {"am_mp_sb_heli_tours"sv, "希金斯直升机旅行社"sv},
			    {"am_mp_sb_weed_shop"sv, "喷云吐雾馆"sv},
			    {"am_hunt_the_beast"sv, "狩猎野兽活动"sv},
			    {"am_npc_invites"sv, "非玩家角色邀请活动"sv},
			    {"am_pass_the_parcel"sv, "传递包裹活动"sv},
			    {"am_penned_in"sv, "围困活动"sv},
			    {"am_pi_menu"sv, "私人调查菜单"sv},
			    {"am_plane_takedown"sv, "击落飞机活动"sv},
			    {"am_prison"sv, "监狱活动"sv},
			    {"am_prostitute"sv, "应召女郎活动"sv},
			    {"am_safehouse"sv, "安全屋活动"sv},
			    {"am_rollercoaster"sv, "过山车活动"sv},
			    {"am_rontrevor_cut"sv, "罗恩与崔佛过场"sv},
			    {"am_ronTrevor_Cut"sv, "罗恩与崔佛过场"sv},
			    {"am_simosa"sv, "西莫萨活动"sv},
			    {"am_stripper"sv, "脱衣舞者活动"sv},
			    {"am_taxi"sv, "出租车活动"sv},
			    {"am_taxi_launcher"sv, "出租车启动活动"sv},
			    {"am_tennis"sv, "网球活动"sv},
			    {"am_time_trial"sv, "时间挑战赛活动"sv},
			    {"am_joyrider"sv, "兜风骑手活动"sv},
			    {"am_kill_list"sv, "击杀名单活动"sv},
			    {"am_island_backup_heli"sv, "岛屿支援直升机活动"sv},
			    {"am_casino_limo"sv, "赌场豪华轿车活动"sv},
			    {"am_mansion_limo"sv, "豪宅豪华轿车活动"sv},
			    {"am_casino_luxury_car"sv, "赌场豪车活动"sv},
			    {"am_mansion_luxury_car"sv, "豪宅豪车活动"sv},
			    {"am_vehicle_drop"sv, "空投载具活动"sv},
			    {"am_vehicle_spawn"sv, "载具生成活动"sv},
			    {"am_armybase"sv, "军事基地活动"sv},
			    {"animal_controller"sv, "动物控制器"sv},
			    {"appavengeroperations"sv, "应用：复仇者行动"sv},
			    {"appbunkerbusiness"sv, "应用：地堡生意"sv},
			    {"appsecurohack"sv, "应用：保镖公司黑客"sv},
			    {"appsecuroserv"sv, "应用：保镖公司"sv},
			    {"appvlsi"sv, "应用：芯片破解"sv},
			    {"appzit"sv, "应用：音乐识别"sv},
			    {"arena_box_bench_seats"sv, "竞技场包厢长椅座位"sv},
			    {"assassin_construction"sv, "暗杀：工地"sv},
			    {"assassin_hooker"sv, "暗杀：妓女"sv},
			    {"assassin_multi"sv, "暗杀：多人"sv},
			    {"assassin_rankup"sv, "暗杀：晋级"sv},
			    {"atm_trigger"sv, "自动提款机触发器"sv},
			    {"audiotest"sv, "音频测试"sv},
			    {"autosave_controller"sv, "自动保存控制器"sv},
			    {"base_corridor_seats"sv, "基地走廊座位"sv},
			    {"base_entrance_seats"sv, "基地入口座位"sv},
			    {"base_quaters_seats"sv, "基地宿舍座位"sv},
			    {"base_reception_seats"sv, "基地接待区座位"sv},
			    {"basic_creator"sv, "基础创建器"sv},
			    {"benchmark"sv, "基准测试"sv},
			    {"barry3a"sv, "巴里 3甲"sv},
			    {"barry3c"sv, "巴里 3丙"sv},
			    {"bigwheel"sv, "摩天轮"sv},
			    {"blackjack"sv, "二十一点"sv},
			    {"blip_controller"sv, "标记控制器"sv},
			    {"bootycallhandler"sv, "约会事件处理器"sv},
			    {"bootycall_debug_controller"sv, "约会调试控制器"sv},
			    {"buddydeathresponse"sv, "伙伴死亡响应"sv},
			    {"bugstar_mission_export"sv, "灭虫公司任务出口"sv},
			    {"buildingsiteambience"sv, "工地环境"sv},
			    {"building_controller"sv, "建筑控制器"sv},
			    {"cablecar"sv, "缆车"sv},
			    {"cam_coord_sender"sv, "摄像机坐标发送器"sv},
			    {"camhedz_arcade"sv, "卡姆赫兹街机厅"sv},
			    {"candidate_controller"sv, "候选控制器"sv},
			    {"casinoroulette"sv, "赌场轮盘"sv},
			    {"casino_lucky_wheel"sv, "赌场幸运转盘"sv},
			    {"casino_slots"sv, "赌场老虎机"sv},
			    {"carwash1"sv, "亲力亲为洗车行 1"sv},
			    {"carwash2"sv, "亲力亲为洗车行 2"sv},
			    {"car_roof_test"sv, "车顶测试"sv},
			    {"celebration_editor"sv, "庆祝动作编辑器"sv},
			    {"celebrations"sv, "庆祝动作"sv},
			    {"cellphone_flashhand"sv, "手机闪手动画"sv},
			    {"charactergoals"sv, "角色目标"sv},
			    {"charanimtest"sv, "角色动画测试"sv},
			    {"cheat_controller"sv, "作弊控制器"sv},
			    {"circuitblockhack"sv, "电路方块破解"sv},
			    {"code_controller"sv, "代码控制器"sv},
			    {"comms_controller"sv, "通讯控制器"sv},
			    {"completionpercentage_controller"sv, "完成度控制器"sv},
			    {"component_checker"sv, "组件检查器"sv},
			    {"context_controller"sv, "上下文控制器"sv},
			    {"coordinate_recorder"sv, "坐标记录器"sv},
			    {"controller_ambientarea"sv, "环境区域控制器"sv},
			    {"creation_startup"sv, "创建器启动"sv},
			    {"custom_config"sv, "自定义配置"sv},
			    {"cutscenemetrics"sv, "过场动画指标"sv},
			    {"cutscenesamples"sv, "过场动画示例"sv},
			    {"degenatron_games"sv, "德根纳创游戏"sv},
			    {"debug_app_select_screen"sv, "调试：应用选择界面"sv},
			    {"debug_clone_outfit_testing"sv, "调试：克隆服装测试"sv},
			    {"density_test"sv, "密度测试"sv},
			    {"dialogue_handler"sv, "对话处理器"sv},
			    {"director_mode"sv, "导演模式"sv},
			    {"heli_gun"sv, "直升机机枪活动"sv},
			    {"docks2asubhandler"sv, "码头 2甲子处理器"sv},
			    {"docks_heista"sv, "码头抢劫甲"sv},
			    {"docks_heistb"sv, "码头抢劫乙"sv},
			    {"docks_prep2b"sv, "码头前置任务 2乙"sv},
			    {"dont_cross_the_line"sv, "别越线"sv},
			    {"dynamixtest"sv, "动态测试"sv},
			    {"emergencycall"sv, "紧急呼叫"sv},
			    {"emergencycalllauncher"sv, "紧急呼叫启动器"sv},
			    {"epscars"sv, "邪教车辆"sv},
			    {"epsdesert"sv, "邪教沙漠"sv},
			    {"epsilontract"sv, "邪教拖拉机"sv},
			    {"epsrobes"sv, "邪教长袍"sv},
			    {"error_listener"sv, "错误监听器"sv},
			    {"error_thrower"sv, "错误抛出器"sv},
			    {"exile_city_denial"sv, "流放：城市拒止"sv},
			    {"fairgroundhub"sv, "游乐场中心"sv},
			    {"fake_interiors"sv, "伪装室内场景"sv},
			    {"fame_or_shame_set"sv, "名与利布景"sv},
			    {"family_scene_f0"sv, "家族场景：富兰克林 0"sv},
			    {"family_scene_f1"sv, "家族场景：富兰克林 1"sv},
			    {"family_scene_m"sv, "家族场景：麦克"sv},
			    {"family_scene_t0"sv, "家族场景：崔佛 0"sv},
			    {"family_scene_t1"sv, "家族场景：崔佛 1"sv},
			    {"fbi5a"sv, "联邦调查局 5甲"sv},
			    {"finalea"sv, "终章甲"sv},
			    {"finaleb"sv, "终章乙"sv},
			    {"finalec1"sv, "终章丙一"sv},
			    {"finalec2"sv, "终章丙二"sv},
			    {"finale_choice"sv, "终章抉择"sv},
			    {"finale_credits"sv, "终章制作名单"sv},
			    {"finale_endgame"sv, "终章结局"sv},
			    {"finale_heist2a"sv, "终章抢劫 2甲"sv},
			    {"finale_heist2b"sv, "终章抢劫 2乙"sv},
			    {"finale_heist_prepa"sv, "终章抢劫前置甲"sv},
			    {"finale_heist_prepb"sv, "终章抢劫前置乙"sv},
			    {"finale_heist_prepc"sv, "终章抢劫前置丙"sv},
			    {"finale_heist_prepd"sv, "终章抢劫前置丁"sv},
			    {"finale_heist_prepeamb"sv, "终章抢劫前置戊环境"sv},
			    {"fixer_hq_seating_op_floor"sv, "事务所总部办公层座位"sv},
			    {"fixer_hq_seating_pq"sv, "事务所总部私人套房座位"sv},
			    {"floating_help_controller"sv, "浮动帮助控制器"sv},
			    {"flowstartaccept"sv, "流程开始确认"sv},
			    {"flow_autoplay"sv, "流程自动播放"sv},
			    {"flyunderbridges"sv, "桥下飞行挑战"sv},
			    {"fmmc_contentquicklauncher"sv, "在线战局内容快速启动器"sv},
			    {"fmmc_playlist_controller"sv, "在线战局播放列表控制器"sv},
			    {"fm_capture_creator"sv, "自由模式捕获创建器"sv},
			    {"fm_content_acid_lab_setup"sv, "自由模式内容：致幻剂实验室设置"sv},
			    {"fm_content_acid_lab_source"sv, "自由模式内容：致幻剂实验室补货"sv},
			    {"fm_content_acid_lab_sell"sv, "自由模式内容：致幻剂实验室出货"sv},
			    {"fm_content_ammunation"sv, "自由模式内容：武装国度"sv},
			    {"fm_content_auto_shop_delivery"sv, "自由模式内容：改车铺交付"sv},
			    {"fm_content_armoured_truck"sv, "自由模式内容：装甲卡车"sv},
			    {"fm_content_arms_trafficking"sv, "自由模式内容：军火走私"sv},
			    {"fm_content_bar_resupply"sv, "自由模式内容：酒吧补货"sv},
			    {"fm_content_bank_shootout"sv, "自由模式内容：威索广场枪战"sv},
			    {"fm_content_bicycle_time_trial"sv, "自由模式内容：自行车时间挑战赛"sv},
			    {"fm_content_bike_shop_delivery"sv, "自由模式内容：摩托车店交付"sv},
			    {"fm_content_business_battles"sv, "自由模式内容：商业战"sv},
			    {"fm_content_bounty_targets"sv, "自由模式内容：悬赏目标"sv},
			    {"fm_content_cargo"sv, "自由模式内容：货物运输"sv},
			    {"fm_content_car_wash_work"sv, "自由模式内容：亲力亲为洗车行工作"sv},
			    {"fm_content_car_wash_detailing"sv, "自由模式内容：亲力亲为洗车行美容"sv},
			    {"fm_content_cerberus"sv, "自由模式内容：刻耳柏洛斯"sv},
			    {"fm_content_chop_shop_delivery"sv, "自由模式内容：废车回收场交付"sv},
			    {"fm_content_clubhouse_contracts"sv, "自由模式内容：会所合约"sv},
			    {"fm_content_club_management"sv, "自由模式内容：夜总会管理"sv},
			    {"fm_content_club_odd_jobs"sv, "自由模式内容：夜总会零工"sv},
			    {"fm_content_club_source"sv, "自由模式内容：夜总会货源"sv},
			    {"fm_content_community_outreach"sv, "自由模式内容：社区外展"sv},
			    {"fm_content_convoy"sv, "自由模式内容：车队"sv},
			    {"fm_content_crime_scene"sv, "自由模式内容：犯罪现场"sv},
			    {"fm_content_cutscene"sv, "自由模式内容：过场动画"sv},
			    {"fm_content_drug_lab_work"sv, "自由模式内容：制毒实验室工作"sv},
			    {"fm_content_drug_vehicle"sv, "自由模式内容：毒品载具"sv},
			    {"fm_content_daily_bounty"sv, "自由模式内容：每日悬赏"sv},
			    {"fm_content_dispatch_work"sv, "自由模式内容：调度工作"sv},
			    {"fm_content_export_cargo"sv, "自由模式内容：出口货物"sv},
			    {"fm_content_firefighter"sv, "自由模式内容：消防员"sv},
			    {"fm_content_forklift_operator"sv, "自由模式内容：叉车操作员"sv},
			    {"fm_content_ghosthunt"sv, "自由模式内容：猎鬼"sv},
			    {"fm_content_golden_gun"sv, "自由模式内容：黄金枪"sv},
			    {"fm_content_getaway_driver"sv, "自由模式内容：逃逸司机"sv},
			    {"fm_content_gunrunning"sv, "自由模式内容：军火走私补给"sv},
			    {"fm_content_hacker_cargo_finale"sv, "自由模式内容：制衣厂终章 货运"sv},
			    {"fm_content_hacker_cargo_prep"sv, "自由模式内容：制衣厂前置 货运"sv},
			    {"fm_content_hacker_house_finale"sv, "自由模式内容：制衣厂终章 住宅"sv},
			    {"fm_content_hacker_house_prep"sv, "自由模式内容：制衣厂前置 住宅"sv},
			    {"fm_content_hacker_whistle_fin"sv, "自由模式内容：制衣厂终章 吹哨人"sv},
			    {"fm_content_hacker_whistle_prep"sv, "自由模式内容：制衣厂前置 吹哨人"sv},
			    {"fm_content_hacker_zancudo_fin"sv, "自由模式内容：制衣厂终章 赞库多"sv},
			    {"fm_content_hacker_zancudo_prep"sv, "自由模式内容：制衣厂前置 赞库多"sv},
			    {"fm_content_helitours_work"sv, "自由模式内容：希金斯直升机旅行社工作"sv},
			    {"fm_content_helitours_tour"sv, "自由模式内容：希金斯直升机旅行社观光之旅"sv},
			    {"fm_content_hsw_setup"sv, "自由模式内容：阿浩特别工坊设置"sv},
			    {"fm_content_hsw_time_trial"sv, "自由模式内容：阿浩特别工坊时间挑战赛"sv},
			    {"fm_content_island_heist"sv, "自由模式内容：佩里科岛抢劫"sv},
			    {"fm_content_island_dj"sv, "自由模式内容：岛屿打碟师"sv},
			    {"fm_content_metal_detector"sv, "自由模式内容：金属探测"sv},
			    {"fm_content_movie_props"sv, "自由模式内容：电影道具"sv},
			    {"fm_content_mp_intro"sv, "自由模式内容：线上介绍"sv},
			    {"fm_content_parachuter"sv, "自由模式内容：跳伞者"sv},
			    {"fm_content_payphone_hit"sv, "自由模式内容：电话亭暗杀"sv},
			    {"fm_content_phantom_car"sv, "自由模式内容：幻影车"sv},
			    {"fm_content_pizza_delivery"sv, "自由模式内容：披萨配送"sv},
			    {"fm_content_postal_worker"sv, "自由模式内容：邮政员工"sv},
			    {"fm_content_possessed_animals"sv, "自由模式内容：附身动物"sv},
			    {"fm_content_robbery"sv, "自由模式内容：抢劫事件"sv},
			    {"fm_content_security_contract"sv, "自由模式内容：安保合约"sv},
			    {"fm_content_skydive"sv, "自由模式内容：高空跳伞"sv},
			    {"fm_content_slasher"sv, "自由模式内容：连环杀手"sv},
			    {"fm_content_sightseeing"sv, "自由模式内容：观光之旅"sv},
			    {"fm_content_smuggler_ops"sv, "自由模式内容：走私者行动"sv},
			    {"fm_content_smuggler_plane"sv, "自由模式内容：走私者飞机"sv},
			    {"fm_content_smuggler_resupply"sv, "自由模式内容：走私者补货"sv},
			    {"fm_content_smuggler_sell"sv, "自由模式内容：走私者出货"sv},
			    {"fm_content_smuggler_trail"sv, "自由模式内容：走私者踪迹"sv},
			    {"fm_content_source_research"sv, "自由模式内容：研究物资获取"sv},
			    {"fm_content_stash_house"sv, "自由模式内容：藏匿屋"sv},
			    {"fm_content_survival_grouping"sv, "自由模式内容：生存战组队"sv},
			    {"fm_content_survival"sv, "自由模式内容：生存战"sv},
			    {"fm_content_taxi_driver"sv, "自由模式内容：出租车司机"sv},
			    {"fm_content_tow_truck_work"sv, "自由模式内容：拖车工作"sv},
			    {"fm_content_tuner_robbery"sv, "自由模式内容：改装铺抢劫"sv},
			    {"fm_content_tycoon_odd_jobs"sv, "自由模式内容：大亨零工"sv},
			    {"fm_content_ufo_abduction"sv, "自由模式内容：不明飞行物绑架"sv},
			    {"fm_content_valentine_cheater"sv, "自由模式内容：情人节负心汉"sv},
			    {"fm_content_vehicle_list"sv, "自由模式内容：出口珍品名单"sv},
			    {"fm_content_vehrob_cargo_ship"sv, "自由模式内容：载具抢劫货船"sv},
			    {"fm_content_vehrob_casino_prize"sv, "自由模式内容：载具抢劫赌场奖品"sv},
			    {"fm_content_vehrob_disrupt"sv, "自由模式内容：载具抢劫破坏行动"sv},
			    {"fm_content_vehrob_police"sv, "自由模式内容：载具抢劫警方案件"sv},
			    {"fm_content_vehrob_prep"sv, "自由模式内容：载具抢劫前置任务"sv},
			    {"fm_content_vehrob_arena"sv, "自由模式内容：载具抢劫竞技场"sv},
			    {"fm_content_vehrob_scoping"sv, "自由模式内容：载具抢劫踩点"sv},
			    {"fm_content_vehrob_submarine"sv, "自由模式内容：载具抢劫潜艇"sv},
			    {"fm_content_vehrob_task"sv, "自由模式内容：载具抢劫任务"sv},
			    {"fm_content_vip_contract_1"sv, "自由模式内容：贵宾合约"sv},
			    {"fm_content_weed_shop_delivery"sv, "自由模式内容：喷云吐雾馆配送"sv},
			    {"fm_content_weed_shop_work"sv, "自由模式内容：喷云吐雾馆工作"sv},
			    {"fm_content_xmas_truck"sv, "自由模式内容：节日欢乐卡车"sv},
			    {"fm_content_xmas_mugger"sv, "自由模式内容：圣诞劫匪"sv},
			    {"fm_horde_controler"sv, "自由模式尸潮控制器"sv},
			    {"fm_impromptu_dm_controler"sv, "自由模式即兴死亡竞赛控制器"sv},
			    {"fm_intro_cut_dev"sv, "自由模式介绍过场（开发）"sv},
			    {"fm_lts_creator"sv, "自由模式团队生存战创建器"sv},
			    {"fm_maintain_cloud_header_data"sv, "自由模式维护云端头数据"sv},
			    {"fm_maintain_transition_players"sv, "自由模式维护过渡玩家"sv},
			    {"fm_street_dealer"sv, "自由模式街头毒贩"sv},
			    {"forsalesigns"sv, "待售招牌"sv},
			    {"fps_test_mag"sv, "帧率测试"sv},
			    {"freemode_clearglobals"sv, "自由模式清理全局变量"sv},
			    {"friendactivity"sv, "好友活动"sv},
			    {"game_server_test"sv, "游戏服务器测试"sv},
			    {"gb_airfreight"sv, "首领工作：航空货运"sv},
			    {"gb_amphibious_assault"sv, "首领工作：两栖突袭"sv},
			    {"gb_assault"sv, "首领工作：突袭"sv},
			    {"gb_bank_job"sv, "首领工作：银行工作"sv},
			    {"gb_airfreight"sv, "首领工作：航空货运"sv},
			    {"gb_amphibious_assault"sv, "首领工作：两栖突袭"sv},
			    {"gb_bellybeast"sv, "首领工作：野兽之腹"sv},
			    {"gb_biker_bad_deal"sv, "摩托帮工作：黑吃黑"sv},
			    {"gb_biker_burn_assets"sv, "摩托帮工作：焚毁资产"sv},
			    {"gb_biker_contract_killing"sv, "摩托帮工作：合约杀戮"sv},
			    {"gb_biker_contraband_defend"sv, "摩托帮工作：防守违禁品"sv},
			    {"gb_biker_contraband_sell"sv, "摩托帮工作：出售违禁品"sv},
			    {"gb_biker_criminal_mischief"sv, "摩托帮工作：犯罪恶作剧"sv},
			    {"gb_biker_destroy_vans"sv, "摩托帮工作：摧毁货车"sv},
			    {"gb_biker_driveby_assassin"sv, "摩托帮工作：驾车暗杀"sv},
			    {"gb_biker_free_prisoner"sv, "摩托帮工作：解救囚犯"sv},
			    {"gb_biker_joust"sv, "摩托帮工作：骑枪对决"sv},
			    {"gb_biker_last_respects"sv, "摩托帮工作：最后的敬意"sv},
			    {"gb_biker_race_p2p"sv, "摩托帮工作：点对点竞速"sv},
			    {"gb_biker_rescue_contact"sv, "摩托帮工作：营救联系人"sv},
			    {"gb_biker_rippin_it_up"sv, "摩托帮工作：一路狂飙"sv},
			    {"gb_biker_safecracker"sv, "摩托帮工作：保险箱破解器"sv},
			    {"gb_biker_search_and_destroy"sv, "摩托帮工作：搜索与摧毁"sv},
			    {"gb_biker_shuttle"sv, "摩托帮工作：接驳车"sv},
			    {"gb_biker_stand_your_ground"sv, "摩托帮工作：坚守阵地"sv},
			    {"gb_biker_steal_bikes"sv, "摩托帮工作：偷摩托车"sv},
			    {"gb_biker_unload_weapons"sv, "摩托帮工作：卸载武器"sv},
			    {"gb_biker_wheelie_rider"sv, "摩托帮工作：翘头骑士"sv},
			    {"gb_cajacking"sv, "首领工作：劫车"sv},
			    {"gb_carjacking"sv, "首领工作：劫车"sv},
			    {"gb_cashing_out"sv, "首领工作：兑现离场"sv},
			    {"gb_casino"sv, "首领工作：赌场"sv},
			    {"gb_casino_heist"sv, "首领工作：赌场豪劫"sv},
			    {"gb_collect_money"sv, "首领工作：收集现金"sv},
			    {"gb_contraband_buy"sv, "首领工作：购买违禁品"sv},
			    {"gb_contraband_defend"sv, "首领工作：防守违禁品"sv},
			    {"gb_contraband_sell"sv, "首领工作：出售违禁品"sv},
			    {"gb_data_hack"sv, "首领工作：数据破解"sv},
			    {"gb_deathmatch"sv, "首领工作：死亡竞赛"sv},
			    {"gb_finderskeepers"sv, "首领工作：先到先得"sv},
			    {"gb_fivestar"sv, "首领工作：五星通缉"sv},
			    {"gb_flying_in_style"sv, "首领工作：气派出行"sv},
			    {"gb_fortified"sv, "首领工作：坚固护送"sv},
			    {"gb_fragile_goods"sv, "首领工作：易碎货物"sv},
			    {"gb_fully_loaded"sv, "首领工作：满载出发"sv},
			    {"gb_gangops"sv, "首领工作：末日行动"sv},
			    {"gb_gunrunning"sv, "首领工作：军火走私"sv},
			    {"gb_gunrunning_defend"sv, "首领工作：防守军火走私"sv},
			    {"gb_headhunter"sv, "首领工作：猎头行动"sv},
			    {"gb_hunt_the_boss"sv, "首领工作：狩猎老大"sv},
			    {"gb_illicit_goods_resupply"sv, "摩托帮工作：非法货物补给"sv},
			    {"gb_infiltration"sv, "首领工作：潜入"sv},
			    {"gb_jewel_store_grab"sv, "首领工作：珠宝店劫案"sv},
			    {"gb_limo_attack"sv, "首领工作：豪华轿车袭击"sv},
			    {"gb_ploughed"sv, "首领工作：一路铲平"sv},
			    {"gb_point_to_point"sv, "首领工作：点对点"sv},
			    {"gb_ramped_up"sv, "首领工作：坡道狂飙"sv},
			    {"gb_rob_shop"sv, "首领工作：商店抢劫"sv},
			    {"gb_salvage"sv, "首领工作：打捞行动"sv},
			    {"gb_security_van"sv, "首领工作：运钞车"sv},
			    {"gb_sightseer"sv, "首领工作：观光客"sv},
			    {"gb_smuggler"sv, "首领工作：走私者"sv},
			    {"gb_steal_veh"sv, "首领工作：偷载具"sv},
			    {"gb_stockpiling"sv, "首领工作：囤货"sv},
			    {"gb_target_pursuit"sv, "首领工作：目标追击"sv},
			    {"gb_terminate"sv, "首领工作：终止目标"sv},
			    {"gb_transporter"sv, "首领工作：运输者"sv},
			    {"gb_velocity"sv, "首领工作：极速狂飙"sv},
			    {"gb_vehicle_export"sv, "首领工作：载具出口"sv},
			    {"gb_veh_surv"sv, "首领工作：载具生存战"sv},
			    {"gb_yacht_rob"sv, "首领工作：游艇劫案"sv},
			    {"business_battles"sv, "商业战"sv},
			    {"business_battles_defend"sv, "商业战：防守"sv},
			    {"business_battles_sell"sv, "商业战：出售"sv},
			    {"degenatron games"sv, "德根纳创游戏"sv},
			    {"example_arcade"sv, "示例街机厅"sv},
			    {"fm_bj_race_controler"sv, "自由模式摩天崖竞速控制器"sv},
			    {"fm_deathmatch_controler"sv, "自由模式死亡竞赛控制器"sv},
			    {"fm_hideout_controler"sv, "自由模式藏身处控制器"sv},
			    {"fm_hold_up_tut"sv, "自由模式抢劫教程"sv},
			    {"fm_intro"sv, "自由模式介绍"sv},
			    {"fm_impromptu_dm_controler"sv, "自由模式即兴死亡竞赛控制器"sv},
			    {"fm_mission_controller_2020"sv, "自由模式任务控制器 2020"sv},
			    {"fm_race_controler"sv, "自由模式竞速控制器"sv},
			    {"golf_mp"sv, "线上高尔夫"sv},
			    {"pilot_school_mp"sv, "线上飞行学校"sv},
			    {"range_modern_mp"sv, "线上现代射击场"sv},
			    {"sctv"sv, "社群俱乐部电视"sv},
			    {"sb_weed_shop_seating"sv, "喷云吐雾馆座位"sv},
			    {"stripclub_mp"sv, "线上脱衣舞俱乐部"sv},
			    {"tennis_network_mp"sv, "线上网球"sv},
			    {"tuner_sandbox_activity"sv, "改装铺沙盒活动"sv},
			    {"gb_biker_bad_deal"sv, "摩托帮工作：黑吃黑"sv},
			    {"gb_biker_burn_assets"sv, "摩托帮工作：焚毁资产"sv},
			    {"gb_biker_contract_killing"sv, "摩托帮工作：合约杀戮"sv},
			    {"gb_biker_criminal_mischief"sv, "摩托帮工作：犯罪恶作剧"sv},
			    {"gb_biker_destroy_vans"sv, "摩托帮工作：摧毁厢型车"sv},
			    {"gb_biker_driveby_assassin"sv, "摩托帮工作：驾车暗杀"sv},
			    {"gb_biker_free_prisoner"sv, "摩托帮工作：解救囚犯"sv},
			    {"gb_biker_joust"sv, "摩托帮工作：骑枪对决"sv},
			    {"gb_biker_last_respects"sv, "摩托帮工作：最后的敬意"sv},
			    {"gb_biker_race_p2p"sv, "摩托帮工作：点对点竞速"sv},
			    {"gb_biker_rescue_contact"sv, "摩托帮工作：营救联系人"sv},
			    {"gb_biker_rippin_it_up"sv, "摩托帮工作：大闹一场"sv},
			    {"gb_biker_safecracker"sv, "摩托帮工作：保险箱破解器"sv},
			    {"gb_biker_search_and_destroy"sv, "摩托帮工作：搜寻与摧毁"sv},
			    {"gb_biker_shuttle"sv, "摩托帮工作：接驳任务"sv},
			    {"gb_biker_stand_your_ground"sv, "摩托帮工作：坚守地盘"sv},
			    {"gb_biker_steal_bikes"sv, "摩托帮工作：偷摩托车"sv},
			    {"gb_biker_target_rival"sv, "摩托帮工作：锁定对手"sv},
			    {"gb_biker_unload_weapons"sv, "摩托帮工作：卸载武器"sv},
			    {"gb_biker_wheelie_rider"sv, "摩托帮工作：前轮离地骑手"sv},
			    {"gb_carjacking"sv, "首领工作：劫车"sv},
			    {"gb_cashing_out"sv, "首领工作：套现离场"sv},
			    {"gb_collect_money"sv, "首领工作：收钱"sv},
			    {"gb_contraband_buy"sv, "首领工作：购买违禁品"sv},
			    {"gb_fivestar"sv, "首领工作：五星通缉"sv},
			    {"gb_fortified"sv, "首领工作：武装护送"sv},
			    {"gb_fragile_goods"sv, "首领工作：易碎货物"sv},
			    {"gb_fully_loaded"sv, "首领工作：满载出发"sv},
			    {"gb_headhunter"sv, "首领工作：猎头"sv},
			    {"gb_hunt_the_boss"sv, "首领工作：猎杀首领"sv},
			    {"gb_illicit_goods_resupply"sv, "首领工作：非法货物补给"sv},
			    {"gb_infiltration"sv, "首领工作：渗透行动"sv},
			    {"gb_jewel_store_grab"sv, "首领工作：珠宝店抢夺"sv},
			    {"gb_ploughed"sv, "首领工作：横冲直撞"sv},
			    {"gb_ramped_up"sv, "首领工作：火力升级"sv},
			    {"gb_security_van"sv, "首领工作：运钞车"sv},
			    {"gb_sightseer"sv, "首领工作：观光客"sv},
			    {"gb_stockpiling"sv, "首领工作：囤积物资"sv},
			    {"gb_target_pursuit"sv, "首领工作：追击目标"sv},
			    {"gb_terminate"sv, "首领工作：清除目标"sv},
			    {"gb_transporter"sv, "首领工作：运输者"sv},
			    {"gb_velocity"sv, "首领工作：极速狂飙"sv},
			    {"general_test"sv, "通用测试"sv},
			    {"ggsm_arcade"sv, "太空猴街机厅"sv},
			    {"globals_fmmc_struct_registration"sv, "在线战局结构全局变量注册"sv},
			    {"golf_ai_foursome_putting"sv, "高尔夫人工智能四人组推杆"sv},
			    {"gpb_andymoon"sv, "高尔夫俱乐部会所：安迪·穆恩"sv},
			    {"gpb_baygor"sv, "高尔夫俱乐部会所：贝戈尔"sv},
			    {"gpb_billbinder"sv, "高尔夫俱乐部会所：比尔·宾德"sv},
			    {"gpb_clinton"sv, "高尔夫俱乐部会所：克林顿"sv},
			    {"gpb_griff"sv, "高尔夫俱乐部会所：格里夫"sv},
			    {"gpb_jane"sv, "高尔夫俱乐部会所：简"sv},
			    {"gpb_jerome"sv, "高尔夫俱乐部会所：杰罗姆"sv},
			    {"gpb_jesse"sv, "高尔夫俱乐部会所：杰西"sv},
			    {"gpb_mani"sv, "高尔夫俱乐部会所：曼尼"sv},
			    {"gpb_mime"sv, "高尔夫俱乐部会所：哑剧演员"sv},
			    {"gpb_pameladrake"sv, "高尔夫俱乐部会所：帕梅拉·德雷克"sv},
			    {"gpb_superhero"sv, "高尔夫俱乐部会所：超级英雄"sv},
			    {"gpb_zombie"sv, "高尔夫俱乐部会所：僵尸"sv},
			    {"grid_arcade_cabinet"sv, "方格街机机柜"sv},
			    {"gtest_avoidance"sv, "测试：避让"sv},
			    {"gtest_divingfromcar"sv, "测试：车内跳水"sv},
			    {"gtest_divingfromcarwhilefleeing"sv, "测试：逃跑时车内跳水"sv},
			    {"gtest_helicopter"sv, "测试：直升机"sv},
			    {"gtest_nearlymissedbycar"sv, "测试：险被车辆撞到"sv},
			    {"gunfighttest"sv, "枪战测试"sv},
			    {"gunslinger_arcade"sv, "神枪手街机厅"sv},
			    {"headertest"sv, "标题测试"sv},
			    {"hud_creator"sv, "抬头显示创建器"sv},
			    {"idlewarper"sv, "空闲传送器"sv},
			    {"ingamehud"sv, "游戏内抬头显示"sv},
			    {"initial"sv, "初始化"sv},
			    {"item_ownership_output"sv, "物品归属输出"sv},
			    {"jewelry_prep1a"sv, "珠宝店前置任务 1甲"sv},
			    {"jewelry_prep1b"sv, "珠宝店前置任务 1乙"sv},
			    {"jewelry_prep2a"sv, "珠宝店前置任务 2甲"sv},
			    {"lamar1"sv, "拉玛 1"sv},
			    {"landing_pre_startup"sv, "着陆前启动"sv},
			    {"laptop_trigger"sv, "笔记本触发器"sv},
			    {"launcher_basejumpheli"sv, "启动器：基地跳伞直升机"sv},
			    {"launcher_carwash"sv, "启动器：亲力亲为洗车行"sv},
			    {"launcher_basejumppack"sv, "启动器：基地跳伞背包"sv},
			    {"launcher_offroadracing"sv, "启动器：越野竞速"sv},
			    {"launcher_racing"sv, "启动器：竞速"sv},
			    {"launcher_stunts"sv, "启动器：特技竞速"sv},
			    {"launcher_triathlon"sv, "启动器：铁人三项"sv},
			    {"lesterhandler"sv, "莱斯特处理器"sv},
			    {"letterscraps"sv, "信件碎片"sv},
			    {"line_activation_test"sv, "线路激活测试"sv},
			    {"liverecorder"sv, "实时录制器"sv},
			    {"localpopulator"sv, "本地填充器"sv},
			    {"locates_tester"sv, "定位测试器"sv},
			    {"luxe_veh_activity"sv, "豪华载具活动"sv},
			    {"maintransition"sv, "主过渡流程"sv},
			    {"main_persistent"sv, "主常驻脚本"sv},
			    {"mansion_guest_bedroom_seating"sv, "豪宅客房卧室座位"sv},
			    {"mansion_lobby_seating"sv, "豪宅大厅座位"sv},
			    {"mansion_lower_east_wing_seating"sv, "豪宅下层东翼座位"sv},
			    {"mansion_outside_seating"sv, "豪宅室外座位"sv},
			    {"mansion_upper_balcony_seating"sv, "豪宅上层阳台座位"sv},
			    {"mansion_water_seating"sv, "豪宅水景座位"sv},
			    {"martin1"sv, "马丁 1"sv},
			    {"maude_postbailbond"sv, "莫德：保释后续"sv},
			    {"me_amanda1"sv, "任务事件：阿曼达 1"sv},
			    {"me_jimmy1"sv, "任务事件：吉米 1"sv},
			    {"me_tracey1"sv, "任务事件：崔西 1"sv},
			    {"mg_race_to_point"sv, "小游戏：竞速到目标点"sv},
			    {"michael4leadout"sv, "麦克 4 引导撤离"sv},
			    {"minigame_ending_stinger"sv, "小游戏结尾提示"sv},
			    {"missioniaaturret"sv, "任务国际事务局炮塔"sv},
			    {"mission_stat_alerter"sv, "任务统计警报器"sv},
			    {"mission_stat_watcher"sv, "任务统计监视器"sv},
			    {"mission_triggerer_a"sv, "任务触发器甲"sv},
			    {"mission_triggerer_b"sv, "任务触发器乙"sv},
			    {"mission_triggerer_c"sv, "任务触发器丙"sv},
			    {"mission_triggerer_d"sv, "任务触发器丁"sv},
			    {"mmmm"sv, "空白脚本"sv},
			    {"mp_awards"sv, "线上模式奖励"sv},
			    {"mp_gameplay_menu"sv, "线上模式游戏菜单"sv},
			    {"mp_player_damage_numbers"sv, "线上模式玩家伤害数字"sv},
			    {"mp_prop_special_global_block"sv, "线上模式特殊道具全局封锁"sv},
			    {"mp_save_game_global_block"sv, "线上模式存档全局封锁"sv},
			    {"mp_skycam_stuck_wiggler"sv, "线上模式天空摄像机脱困器"sv},
			    {"mp_unlocks"sv, "线上模式解锁项"sv},
			    {"murdermystery"sv, "谋杀谜案"sv},
			    {"music_studio_seating_external"sv, "音乐工作室外部座位"sv},
			    {"navmeshtest"sv, "导航网格测试"sv},
			    {"net_activity_creator_ui"sv, "网络活动创建器界面"sv},
			    {"net_apartment_activity_light"sv, "网络公寓活动灯光"sv},
			    {"net_bot_brain"sv, "网络机器人逻辑"sv},
			    {"net_bot_simplebrain"sv, "网络机器人简易逻辑"sv},
			    {"net_cloud_mission_loader"sv, "网络云端任务加载器"sv},
			    {"net_jacking_soaktest"sv, "网络劫车压力测试"sv},
			    {"net_session_soaktest"sv, "网络战局压力测试"sv},
			    {"net_test_drive"sv, "网络试驾"sv},
			    {"net_tunable_check"sv, "网络可调参数检查"sv},
			    {"nigel1a"sv, "奈杰尔 1甲"sv},
			    {"nigel1b"sv, "奈杰尔 1乙"sv},
			    {"nigel1c"sv, "奈杰尔 1丙"sv},
			    {"nigel1d"sv, "奈杰尔 1丁"sv},
			    {"nodemenututorial"sv, "节点菜单教程"sv},
			    {"nodeviewer"sv, "节点查看器"sv},
			    {"ob_franklin_beer"sv, "老大业务：富兰克林喝啤酒"sv},
			    {"ob_abatdoor"sv, "老大业务：屠宰场门口"sv},
			    {"ob_abattoircut"sv, "老大业务：屠宰场过场"sv},
			    {"ob_airdancer"sv, "老大业务：充气舞人"sv},
			    {"ob_bong"sv, "老大业务：水烟壶"sv},
			    {"ob_cashregister"sv, "老大业务：收银机"sv},
			    {"ob_drinking_shots"sv, "老大业务：喝烈酒"sv},
			    {"ob_foundry_cauldron"sv, "老大业务：铸造厂坩埚"sv},
			    {"ob_franklin_wine"sv, "老大业务：富兰克林喝红酒"sv},
			    {"ob_huffing_gas"sv, "老大业务：吸入气体"sv},
			    {"ob_jukebox"sv, "老大业务：点唱机"sv},
			    {"ob_mp_bed_low"sv, "老大业务：线上模式床位（低姿态）"sv},
			    {"ob_mp_shower_med"sv, "老大业务：线上模式淋浴"sv},
			    {"ob_mp_stripper"sv, "老大业务：线上模式脱衣舞者"sv},
			    {"ob_mr_raspberry_jam"sv, "老大业务：树莓果酱先生"sv},
			    {"ob_poledancer"sv, "老大业务：钢管舞者"sv},
			    {"ob_telescope"sv, "老大业务：望远镜"sv},
			    {"ob_wheatgrass"sv, "老大业务：小麦草饮品"sv},
			    {"offroad_races"sv, "越野竞速"sv},
			    {"paparazzo3a"sv, "狗仔队 3甲"sv},
			    {"paparazzo3b"sv, "狗仔队 3乙"sv},
			    {"pausemenucareerhublaunch"sv, "暂停菜单职业中心启动"sv},
			    {"pausemenu_map"sv, "暂停菜单地图"sv},
			    {"pausemenu_multiplayer"sv, "暂停菜单多人模式"sv},
			    {"pb_busker"sv, "太平洋标准银行：街头艺人"sv},
			    {"pb_homeless"sv, "太平洋标准银行：流浪汉"sv},
			    {"pb_preacher"sv, "太平洋标准银行：传教士"sv},
			    {"personal_carmod_shop"sv, "个人改车铺"sv},
			    {"photographymonkey"sv, "摄影：猴子"sv},
			    {"photographywildlife"sv, "摄影：野生动物"sv},
			    {"pickuptest"sv, "拾取测试"sv},
			    {"pickupvehicles"sv, "拾取载具"sv},
			    {"pickup_controller"sv, "拾取控制器"sv},
			    {"pilot_school"sv, "飞行学校"sv},
			    {"pilot_school_mp"sv, "线上飞行学校"sv},
			    {"placeholdermission"sv, "占位任务"sv},
			    {"placementtest"sv, "布置测试"sv},
			    {"planewarptest"sv, "飞机传送测试"sv},
			    {"player_controller_b"sv, "玩家控制器乙"sv},
			    {"player_scene_ft_franklin1"sv, "玩家场景：富兰克林步行 1"sv},
			    {"player_scene_f_lamgraff"sv, "玩家场景：富兰克林涂鸦"sv},
			    {"player_scene_f_lamtaunt"sv, "玩家场景：富兰克林挑衅"sv},
			    {"player_scene_f_taxi"sv, "玩家场景：富兰克林出租车"sv},
			    {"player_scene_mf_traffic"sv, "玩家场景：麦克/富兰克林交通"sv},
			    {"player_scene_m_cinema"sv, "玩家场景：麦克电影院"sv},
			    {"player_scene_m_fbi2"sv, "玩家场景：麦克联邦调查局 2"sv},
			    {"player_scene_m_kids"sv, "玩家场景：麦克与孩子"sv},
			    {"player_scene_m_shopping"sv, "玩家场景：麦克购物"sv},
			    {"player_scene_t_bbfight"sv, "玩家场景：崔佛拳斗"sv},
			    {"player_scene_t_chasecar"sv, "玩家场景：崔佛追车"sv},
			    {"player_scene_t_insult"sv, "玩家场景：崔佛辱骂"sv},
			    {"player_scene_t_park"sv, "玩家场景：崔佛公园"sv},
			    {"player_scene_t_tie"sv, "玩家场景：崔佛领带"sv},
			    {"player_timetable_scene"sv, "玩家时间表场景"sv},
			    {"playthrough_builder"sv, "流程构建器"sv},
			    {"pm_gang_attack"sv, "派对菜单：帮派攻击"sv},
			    {"pm_plane_promotion"sv, "派对菜单：飞机推广"sv},
			    {"pm_recover_stolen"sv, "派对菜单：追回失窃物"sv},
			    {"postkilled_bailbond2"sv, "击杀后保释担保 2"sv},
			    {"profiler_registration"sv, "性能分析器注册"sv},
			    {"prologue1"sv, "序章 1"sv},
			    {"puzzle"sv, "谜题"sv},
			    {"randomchar_controller"sv, "随机角色控制器"sv},
			    {"rcpdata"sv, "数据记录脚本"sv},
			    {"replay_controller"sv, "回放控制器"sv},
			    {"rerecord_recording"sv, "重新录制录像"sv},
			    {"restrictedareas"sv, "限制区域"sv},
			    {"re_abandonedcar"sv, "随机事件：废弃车辆"sv},
			    {"re_accident"sv, "随机事件：事故现场"sv},
			    {"re_arrests"sv, "随机事件：逮捕行动"sv},
			    {"re_atmrobbery"sv, "随机事件：自动取款机抢劫"sv},
			    {"re_bikethief"sv, "随机事件：偷车贼"sv},
			    {"re_border"sv, "随机事件：边境事件"sv},
			    {"re_burials"sv, "随机事件：埋尸事件"sv},
			    {"re_cartheft"sv, "随机事件：偷车事件"sv},
			    {"re_chasethieves"sv, "随机事件：追捕窃贼"sv},
			    {"re_crashrescue"sv, "随机事件：车祸救援"sv},
			    {"re_cultshootout"sv, "随机事件：邪教枪战"sv},
			    {"re_dealgonewrong"sv, "随机事件：交易失手"sv},
			    {"re_domestic"sv, "随机事件：家庭纠纷"sv},
			    {"re_duel"sv, "随机事件：决斗"sv},
			    {"re_gangfight"sv, "随机事件：帮派火并"sv},
			    {"re_gang_intimidation"sv, "随机事件：帮派恐吓"sv},
			    {"re_hitch_lift"sv, "随机事件：搭车请求"sv},
			    {"re_homeland_security"sv, "随机事件：国土安全"sv},
			    {"re_lossantosintl"sv, "随机事件：洛圣都国际机场"sv},
			    {"re_lured"sv, "随机事件：诱骗"sv},
			    {"re_monkey"sv, "随机事件：猴子"sv},
			    {"re_mountdance"sv, "随机事件：山地舞蹈"sv},
			    {"re_muggings"sv, "随机事件：抢劫案"sv},
			    {"re_paparazzi"sv, "随机事件：狗仔队"sv},
			    {"re_prisonerlift"sv, "随机事件：搭载囚犯"sv},
			    {"re_prisonvanbreak"sv, "随机事件：囚车越狱"sv},
			    {"re_rescuehostage"sv, "随机事件：营救人质"sv},
			    {"re_seaplane"sv, "随机事件：水上飞机"sv},
			    {"re_securityvan"sv, "随机事件：运钞车"sv},
			    {"re_snatched"sv, "随机事件：被抢走"sv},
			    {"re_stag_do"sv, "随机事件：单身派对"sv},
			    {"re_yetarian"sv, "随机事件：外星信徒"sv},
			    {"rng_output"sv, "随机数输出"sv},
			    {"road_arcade"sv, "公路街机厅"sv},
			    {"savegame_bed"sv, "存档床位"sv},
			    {"save_anywhere"sv, "随地保存"sv},
			    {"scaleformgraphictest"sv, "界面动画图形测试"sv},
			    {"scaleformminigametest"sv, "界面动画小游戏测试"sv},
			    {"scaleformprofiling"sv, "界面动画性能分析"sv},
			    {"scaleformtest"sv, "界面动画测试"sv},
			    {"sclub_front_bouncer"sv, "脱衣舞俱乐部门前保镖"sv},
			    {"scripted_cam_editor"sv, "脚本摄像机编辑器"sv},
			    {"scriptplayground"sv, "脚本试验场"sv},
			    {"script_metrics"sv, "脚本指标"sv},
			    {"scroll_arcade_cabinet"sv, "卷轴街机机柜"sv},
			    {"sctv"sv, "社群俱乐部电视"sv},
			    {"sc_lb_global_block"sv, "社群俱乐部排行榜全局封锁"sv},
			    {"shoprobberies"sv, "商店抢劫案"sv},
			    {"shot_bikejump"sv, "镜头：摩托飞跃"sv},
			    {"shrinkletter"sv, "缩水信件"sv},
			    {"sh_intro_f_hills"sv, "商店抢劫介绍：富兰克林与山丘"sv},
			    {"sh_intro_m_home"sv, "商店抢劫介绍：麦克家中"sv},
			    {"si_dancing_activity"sv, "跳舞活动"sv},
			    {"spaceshipparts"sv, "飞船零件"sv},
			    {"spawn_activities"sv, "生成活动"sv},
			    {"speech_reverb_tracker"sv, "语音混响追踪器"sv},
			    {"spmc_instancer"sv, "单人模式挑战实例器"sv},
			    {"spmc_preloader"sv, "单人模式挑战预加载器"sv},
			    {"sp_dlc_registration"sv, "单人模式下载内容注册"sv},
			    {"sp_editor_mission_instance"sv, "单人模式编辑器任务实例"sv},
			    {"startup_locationtest"sv, "启动位置测试"sv},
			    {"startup_positioning"sv, "启动定位"sv},
			    {"stock_controller"sv, "股票控制器"sv},
			    {"stripperhome"sv, "脱衣舞者住处"sv},
			    {"stunt_plane_races"sv, "特技飞行竞速"sv},
			    {"tasklist_1"sv, "任务列表 1"sv},
			    {"tattoo_shop"sv, "纹身店"sv},
			    {"taxiservice"sv, "出租车服务"sv},
			    {"taxitutorial"sv, "出租车教程"sv},
			    {"taxi_clowncar"sv, "出租车：小丑车"sv},
			    {"taxi_cutyouin"sv, "出租车：带你入局"sv},
			    {"taxi_followcar"sv, "出租车：跟车"sv},
			    {"taxi_gotyounow"sv, "出租车：这就带你走"sv},
			    {"taxi_gotyourback"sv, "出租车：我罩着你"sv},
			    {"taxi_needexcitement"sv, "出租车：想来点刺激"sv},
			    {"taxi_procedural"sv, "出租车：程序任务"sv},
			    {"taxi_takeiteasy"sv, "出租车：轻松点"sv},
			    {"taxi_taketobest"sv, "出租车：带你去最棒的地方"sv},
			    {"tempalpha"sv, "临时测试版"sv},
			    {"temptest"sv, "临时测试"sv},
			    {"three_card_poker"sv, "三张牌扑克"sv},
			    {"timershud"sv, "计时器抬头显示"sv},
			    {"traffickingsettings"sv, "走私设置"sv},
			    {"traffickingteleport"sv, "走私传送"sv},
			    {"train_create_widget"sv, "列车创建组件"sv},
			    {"triathlonsp"sv, "单人铁人三项"sv},
			    {"tunables_registration"sv, "可调参数注册"sv},
			    {"tuneables_processing"sv, "可调参数处理"sv},
			    {"turret_cam_script"sv, "炮塔摄像机脚本"sv},
			    {"underwaterpickups"sv, "水下拾取物"sv},
			    {"utvc"sv, "占位脚本"sv},
			    {"vehiclespawning"sv, "载具生成"sv},
			    {"vehicle_force_widget"sv, "载具力度组件"sv},
			    {"vehicle_gen_controller"sv, "载具生成控制器"sv},
			    {"vehicle_plate"sv, "载具车牌"sv},
			    {"vehicle_stealth_mode"sv, "载具隐形模式"sv},
			    {"veh_play_widget"sv, "载具播放组件"sv},
			    {"walking_ped"sv, "步行行人"sv},
			    {"weapon_audio_widget"sv, "武器音频组件"sv},
			    {"wizard_arcade"sv, "巫师街机厅"sv},
			    {"word_hack"sv, "单词破解"sv},
			    {"wp_partyboombox"sv, "派对音响"sv},
			    {"xml_menus"sv, "配置菜单脚本"sv},
			};
			std::string normalizedScriptName(scriptName);
			std::transform(normalizedScriptName.begin(), normalizedScriptName.end(), normalizedScriptName.begin(), [](unsigned char ch) {
				return static_cast<char>(std::tolower(ch));
			});

			if (const auto it = scriptNameMap.find(normalizedScriptName); it != scriptNameMap.end())
				return std::string(it->second);

			std::string display;
			std::string token;
			for (size_t i = 0; i <= normalizedScriptName.size(); ++i)
			{
				const bool atEnd = i == normalizedScriptName.size();
				const char ch = atEnd ? '\0' : normalizedScriptName[i];
				if (atEnd || ch == '_')
				{
					if (!token.empty())
					{
						std::string_view tokenView(token);
						size_t start = 0;
						while (start < tokenView.size())
						{
							size_t end = start + 1;
							const bool isDigit = std::isdigit(static_cast<unsigned char>(tokenView[start]));
							while (end < tokenView.size() && std::isdigit(static_cast<unsigned char>(tokenView[end])) == isDigit)
								++end;

							auto translated = TranslateScriptToken(tokenView.substr(start, end - start));
							if (!translated.empty())
							{
								if (!display.empty())
									display += ' ';
								display += translated;
							}
							start = end;
						}
						token.clear();
					}
					continue;
				}
				token += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
			}

			if (display.empty())
				return std::string(scriptName);

			return display;
		}
	}

	std::string LocalizeScriptDisplayName(std::string_view scriptName)
	{
		return TranslateScriptDisplayName(scriptName);
	}

	static constexpr std::pair<const char*, int> stackSizes[] = {
	    {"MICRO", static_cast<int>(eStackSizes::MICRO)},
	    {"MINI", static_cast<int>(eStackSizes::MINI)},
	    {"DEFAULT", static_cast<int>(eStackSizes::DEFAULT)},
	    {"SPECIAL_ABILITY", static_cast<int>(eStackSizes::SPECIAL_ABILITY)},
	    {"FRIEND", static_cast<int>(eStackSizes::FRIEND)},
	    {"SHOP", static_cast<int>(eStackSizes::SHOP)},
	    {"CELLPHONE", static_cast<int>(eStackSizes::CELLPHONE)},
	    {"VEHICLE_SPAWN", static_cast<int>(eStackSizes::VEHICLE_SPAWN)},
	    {"CAR_MOD_SHOP", static_cast<int>(eStackSizes::CAR_MOD_SHOP)},
	    {"PAUSE_MENU_SCRIPT", static_cast<int>(eStackSizes::PAUSE_MENU_SCRIPT)},
	    {"APP_INTERNET", static_cast<int>(eStackSizes::APP_INTERNET)},
	    {"MULTIPLAYER_MISSION", static_cast<int>(eStackSizes::MULTIPLAYER_MISSION)},
	    {"CONTACTS_APP", static_cast<int>(eStackSizes::CONTACTS_APP)},
	    {"INTERACTION_MENU", static_cast<int>(eStackSizes::INTERACTION_MENU)},
	    {"SCRIPT_XML", static_cast<int>(eStackSizes::SCRIPT_XML)},
	    {"PROPERTY_INT", static_cast<int>(eStackSizes::PROPERTY_INT)},
	    {"ACTIVITY_CREATOR_INT", static_cast<int>(eStackSizes::ACTIVITY_CREATOR_INT)},
	    {"SMPL_INTERIOR", static_cast<int>(eStackSizes::SMPL_INTERIOR)},
	    {"WAREHOUSE", static_cast<int>(eStackSizes::WAREHOUSE)},
	    {"IE_DELIVERY", static_cast<int>(eStackSizes::IE_DELIVERY)},
	    {"SHOP_CONTROLLER", static_cast<int>(eStackSizes::SHOP_CONTROLLER)},
	    {"AM_MP_YACHT", static_cast<int>(eStackSizes::AM_MP_YACHT)},
	    {"INGAMEHUD", static_cast<int>(eStackSizes::INGAMEHUD)},
	    {"TRANSITION", static_cast<int>(eStackSizes::TRANSITION)},
	    {"FMMC_LAUNCHER", static_cast<int>(eStackSizes::FMMC_LAUNCHER)},
	    {"MULTIPLAYER_FREEMODE", static_cast<int>(eStackSizes::MULTIPLAYER_FREEMODE)},
	    {"MISSION", static_cast<int>(eStackSizes::MISSION)},
	    {"MP_LAUNCH_SCRIPT", static_cast<int>(eStackSizes::MP_LAUNCH_SCRIPT)}};

	static void RenderBytecode(rage::scrProgram* program)
	{
		constexpr int bytesPerRow = 16;
		const std::uint32_t codeSize = program->GetFullCodeSize();
		const std::uint32_t totalRows = (codeSize + bytesPerRow - 1) / bytesPerRow;

		static bool shouldJump = false;
		static float targetScroll = -1.0f;
		static char offsetInput[9] = "";

		ImGui::SetNextItemWidth(150);
		ImGui::InputText("##jumpoffset", offsetInput, IM_ARRAYSIZE(offsetInput));
		ImGui::SameLine();
		if (ImGui::Button("跳转到偏移"))
		{
			char* end = nullptr;
			std::uint32_t offset = strtoul(offsetInput, &end, 0);
			if (end != offsetInput && offset < codeSize)
			{
				std::uint32_t row = offset / bytesPerRow;
				targetScroll = row * ImGui::GetFrameHeightWithSpacing();
				shouldJump = true;
			}
		}

		ImGui::BeginChild("##bytecode", ImVec2(610, 400), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);

		if (shouldJump && targetScroll >= 0.0f)
		{
			ImGui::SetScrollY(targetScroll);
			shouldJump = false;
		}

		ImGuiListClipper clipper;
		clipper.Begin(totalRows);
		while (clipper.Step())
		{
			for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
			{
				std::uint32_t offset = row * bytesPerRow;

				ImGui::Text("%08X: ", offset);
				ImGui::SameLine(80);

				for (int i = 0; i < bytesPerRow; ++i)
				{
					std::uint32_t index = offset + i;
					if (index >= codeSize)
						break;

					if (auto byte = program->GetCodeAddress(index))
					{
						char hexStr[3];
						snprintf(hexStr, sizeof(hexStr), "%02X", *byte);

						ImGui::SetNextItemWidth(24);
						ImGui::PushID(index);
						if (ImGui::InputText("##byte", hexStr, sizeof(hexStr), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue))
						{
							char* end = nullptr;
							std::uint8_t newVal = static_cast<std::uint8_t>(strtoul(hexStr, &end, 16));
							if (end != hexStr)
								*byte = newVal; // this will conflict with the ScriptPatches class, but still useful to keep for quick testings
						}
						ImGui::PopID();
						if (ImGui::IsItemActive() && ImGui::IsItemHovered())
							ImGui::SetTooltip("%s", "按 ENTER 写入。");

						if (i < bytesPerRow - 1)
							ImGui::SameLine();
					}
				}
			}
		}

		clipper.End();
		ImGui::EndChild();
	}

	std::shared_ptr<Category> BuildScriptsMenu()
	{
		auto menu = std::make_unique<Category>("脚本");

		auto tabBar = std::make_unique<TabBarItem>("脚本");
		auto threads = std::make_unique<TabItem>("线程");
		auto script = std::make_unique<TabItem>("启动脚本");

		threads->AddItem(std::make_unique<ImGuiItem>([] {
			static rage::scrThread* curThread = nullptr;
			static rage::scrProgram* curProgram = nullptr;

			if (!Pointers.ScriptThreads || Pointers.ScriptThreads->size() == 0)
			{
				curThread = nullptr;
				curProgram = nullptr;
				return ImGui::TextDisabled("%s", "无");
			}

			const auto currentThreadName = curThread ? TranslateScriptDisplayName(curThread->m_ScriptName) : std::string("（请选择）");
			if (ImGui::BeginCombo("线程", currentThreadName.c_str()))
			{
				for (auto thread : *Pointers.ScriptThreads)
				{
					if (thread)
					{
						if (thread->m_Context.m_State == rage::scrThread::State::KILLED || thread->m_Context.m_StackSize == 0)
							continue;

						ImGui::PushID(thread->m_Context.m_ThreadId);

						const auto translatedThreadName = TranslateScriptDisplayName(thread->m_ScriptName);
						if (ImGui::Selectable(translatedThreadName.c_str(), curThread == thread))
						{
							curThread = thread;
							curProgram = Scripts::FindScriptProgram(thread->m_ScriptHash);
						}

						if (curThread == thread)
							ImGui::SetItemDefaultFocus();

						ImGui::PopID();
					}
				}

				ImGui::EndCombo();
			}

			if (!curThread || !curProgram || !curProgram->IsValid() || curProgram->m_RefCount == 0)
			{
				curThread = nullptr;
				curProgram = nullptr;
				return;
			}

			static const std::array<rage::scrThread::State, 5> threadStates = {
			    rage::scrThread::State::IDLE,
			    rage::scrThread::State::RUNNING,
			    rage::scrThread::State::KILLED,
			    rage::scrThread::State::PAUSED,
			    rage::scrThread::State::UNK4};
			static const std::array<const char*, 5> threadStateKeys = {"空闲", "运行中", "已终止", "已暂停", "未知4"};

			const auto stateIt = std::find(threadStates.begin(), threadStates.end(), curThread->m_Context.m_State);
			const auto stateIndex = stateIt == threadStates.end() ? threadStates.size() - 1 : std::distance(threadStates.begin(), stateIt);
			if (ImGui::BeginCombo("状态", threadStateKeys[stateIndex]))
			{
				for (std::size_t i = 0; i < threadStates.size(); ++i)
				{
					const bool selected = curThread->m_Context.m_State == threadStates[i];
					if (ImGui::Selectable(threadStateKeys[i], selected))
						curThread->m_Context.m_State = threadStates[i];

					if (selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (curThread->m_Context.m_State == rage::scrThread::State::KILLED)
			{
				const auto exitReason = TranslateRuntimeDisplayValue(curThread->m_ErrorMessage);
				ImGui::Text("退出原因：%s", exitReason.c_str());
			}
			else
			{
				if (ImGui::Button("终止"))
				{
					curThread->Kill();
					curThread->m_Context.m_State = rage::scrThread::State::KILLED;
				}
				ImGui::SameLine();
				if (ImGui::Button("记录标签"))
				{
					FiberPool::Push([] {
						for (int i = 0; i < curProgram->m_StringsCount; i++)
						{
							if (auto str = curProgram->GetString(i))
							{
								if (HUD::DOES_TEXT_LABEL_EXIST(str))
								{
									LOGF(INFO, "{} - {} (0x{:X}): {}", i, str, Joaat(str), HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION(str));
								}
							}
						}
					});
				}

				if (ImGui::TreeNode("信息"))
				{
					if (auto netComponent = reinterpret_cast<GtaThread*>(curThread)->m_NetComponent)
					{
						if (auto host = netComponent->GetHost())
						{
							ImGui::Text("主机：%s", host->GetName());
						}
						ImGui::SameLine();
						ImGui::BeginDisabled(netComponent->IsLocalPlayerHost());
						if (ImGui::SmallButton("接管控制"))
						{
							FiberPool::Push([] {
								Scripts::ForceScriptHost(curThread);
							});
						}
						ImGui::EndDisabled();
					}
					ImGui::BeginGroup();
					ImGui::Text("线程 ID：%d", curThread->m_Context.m_ThreadId);
					ImGui::Text("堆栈大小：%d", curThread->m_Context.m_StackSize);
					ImGui::Text("堆栈指针：0x%X", curThread->m_Context.m_StackPointer);
					ImGui::Text("程序计数器：0x%X", curThread->m_Context.m_ProgramCounter); // This is not really accurate (always points to the WAIT)
					ImGui::Text("代码大小：%d", curProgram->m_CodeSize);
					ImGui::EndGroup();
					ImGui::SameLine();
					ImGui::BeginGroup();
					ImGui::Text("参数数量：%d", curProgram->m_ArgCount);
					ImGui::Text("局部变量数量：%d", curProgram->m_LocalCount);
					ImGui::Text("全局变量数量：%d", curProgram->m_GlobalCount);
					ImGui::Text("原生函数数量：%d", curProgram->m_NativeCount);
					ImGui::Text("字符串数量：%d", curProgram->m_StringsCount);
					ImGui::EndGroup();
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("字节码"))
				{
					RenderBytecode(curProgram);
					ImGui::TreePop();
				}
			}
		}));

		script->AddItem(std::make_unique<ImGuiItem>([] {
			static std::string scriptSearch = "";
			static std::string stackSizeName = stackSizes[0].first;
			static std::optional<int> launcherIndex = std::nullopt;
			static int stackSize = 0;
			static std::int64_t* args = nullptr;
			static int argCount = 0;
			static int previousArgCount = 0;
			static bool pauseAfterStarting = false;

			bool modified = ImGui::InputTextWithHint("脚本名称", "搜索", &scriptSearch);
			if (!scriptSearch.empty())
				ImGui::Text("%s", TranslateScriptDisplayName(scriptSearch).c_str());

			const auto selectedStackSize = Localization::Translate(stackSizeName);
			if (ImGui::BeginCombo("堆栈大小", selectedStackSize.c_str()))
			{
				for (auto& p : stackSizes)
				{
					const auto translatedStackSize = Localization::Translate(p.first);
					if (ImGui::Selectable(std::format("{} ({})", translatedStackSize, (int)p.second).c_str(), stackSize == p.second))
					{
						stackSizeName = p.first;
						stackSize = p.second;
					}

					if (p.second == stackSize)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (ImGui::InputInt("参数数量", &argCount))
			{
				if (argCount < 0) // should clamp this to a max value?
					argCount = 0;
			}
			if (argCount != previousArgCount)
			{
				std::int64_t* temp = nullptr;
				if (argCount > 0)
				{
					temp = new std::int64_t[argCount]{0};
					if (args)
						std::memcpy(temp, args, sizeof(int64_t) * std::min(argCount, previousArgCount));
				}
				delete[] args;
				args = temp;
				previousArgCount = argCount;
			}
			for (int i = 0; i < argCount; i++)
			{
				ImGui::Text("参数[%d]", i);
				ImGui::SameLine();
				ImGui::PushID(i);
				ImGui::InputScalar("##arg", ImGuiDataType_S64, &args[i]);
				ImGui::PopID();
			}

			if (ImGui::BeginListBox("##scripts"))
			{
				std::string lowerSearch = scriptSearch;
				std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);
				for (auto& script : scriptNames)
				{
					std::string lowerScript = script;
					std::transform(lowerScript.begin(), lowerScript.end(), lowerScript.begin(), ::tolower);
					const auto translatedScript = TranslateScriptDisplayName(script);
					auto lowerTranslatedScript = translatedScript;
					std::transform(lowerTranslatedScript.begin(), lowerTranslatedScript.end(), lowerTranslatedScript.begin(), ::tolower);
					if (lowerScript.find(lowerSearch) != std::string::npos || lowerTranslatedScript.find(lowerSearch) != std::string::npos)
					{
						if (ImGui::Selectable(translatedScript.c_str(), scriptSearch == script))
						{
							scriptSearch = script;
							modified = true;
						}
					}
				}

				ImGui::EndListBox();
			}

			if (modified)
			{
				launcherIndex = Scripts::GetLauncherIndexFromScript(Joaat(scriptSearch));
			}

			ImGui::Checkbox("启动后暂停", &pauseAfterStarting);

			if (ImGui::Button("启动脚本"))
			{
				FiberPool::Push([] {
					auto hash = Joaat(scriptSearch);

					if (!SCRIPT::DOES_SCRIPT_WITH_NAME_HASH_EXIST(hash))
					{
						Notifications::Show("启动脚本", "脚本不存在。", NotificationType::Error);
						return;
					}

					if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(hash) > 0)
					{
						Notifications::Show("启动脚本", "脚本已在运行。", NotificationType::Error);
						return;
					}

					if (MISC::GET_NUMBER_OF_FREE_STACKS_OF_THIS_SIZE(stackSize) == 0)
					{
						Notifications::Show("启动脚本", "没有该大小的空闲堆栈。", NotificationType::Error);
						return;
					}

					while (!SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED(hash))
					{
						SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH(hash);
						ScriptMgr::Yield();
					}

					int id = 0;
					if (args && argCount > 0)
					{
						id = BUILTIN::START_NEW_SCRIPT_WITH_NAME_HASH_AND_ARGS(hash, args, argCount, stackSize);
					}
					else
					{
						id = BUILTIN::START_NEW_SCRIPT_WITH_NAME_HASH(hash, stackSize);
					}

					if (pauseAfterStarting)
					{
						if (auto thread = Scripts::FindScriptThreadByID(id))
							thread->m_Context.m_State = rage::scrThread::State::PAUSED;
					}

					SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED(hash);
					Notifications::Show("启动脚本", std::format("已启动脚本，ID 为 {}。", id), NotificationType::Success);
				});
			}

			if (launcherIndex && *Pointers.IsSessionStarted)
			{
				ImGui::SameLine();
				if (ImGui::Button("启动战局脚本"))
				{
					FiberPool::Push([] {
						Scripts::StartLauncherScript(Joaat(scriptSearch));
					});
				}
				ImGui::SameLine();
				if (ImGui::Button("通过事件启动脚本"))
				{
					FiberPool::Push([] {
						Scripts::ForceScriptOnPlayer(Joaat(scriptSearch), -1);
					});
				}
			}
		}));

		tabBar->AddItem(std::move(threads));
		tabBar->AddItem(std::move(script));

		menu->AddItem(std::move(tabBar));

		return menu;
	}
}
