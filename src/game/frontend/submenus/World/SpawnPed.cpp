#include "SpawnPed.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/data/PedDisplayNames.hpp"
#include "game/gta/data/PedModels.hpp"
#include "game/gta/Ped.hpp"
#include "game/gta/Natives.hpp"
#include "types/ped/PedCombatAttribute.hpp"
#include "game/gta/data/Weapons.hpp"
#include "game/gta/Scripts.hpp"
#include "game/backend/NativeHooks.hpp"
#include "core/localization/Localization.hpp"

#include <array>
#include <cctype>
#include <string_view>
#include <unordered_map>

namespace YimMenu::Submenus
{
	namespace
	{
		using namespace std::literals;

		bool ContainsCjk(std::string_view text)
		{
			for (unsigned char ch : text)
				if (ch & 0x80)
					return true;

			return false;
		}

		std::string TrimWhitespace(std::string_view text)
		{
			const auto start = text.find_first_not_of(' ');
			if (start == std::string_view::npos)
				return {};

			const auto end = text.find_last_not_of(' ');
			return std::string(text.substr(start, end - start + 1));
		}

		std::string NormalizePedSourceLabel(std::string_view label)
		{
			std::string normalized;
			normalized.reserve(label.size());

			bool lastWasSpace = false;
			for (std::size_t i = 0; i < label.size(); ++i)
			{
				const unsigned char ch = static_cast<unsigned char>(label[i]);

				if (ch == 0xC2 && i + 1 < label.size())
				{
					const unsigned char next = static_cast<unsigned char>(label[i + 1]);
					if (next == 0xB5 || next == 0xA0)
					{
						if (!lastWasSpace && !normalized.empty())
						{
							normalized += ' ';
							lastWasSpace = true;
						}
						++i;
						continue;
					}
				}

				if (std::isspace(ch))
				{
					if (!lastWasSpace && !normalized.empty())
					{
						normalized += ' ';
						lastWasSpace = true;
					}
					continue;
				}

				normalized += static_cast<char>(ch);
				lastWasSpace = false;
			}

			return TrimWhitespace(normalized);
		}

		bool LooksLikePedModelId(std::string_view text)
		{
			if (text.empty())
				return false;

			if (text.find('_') == std::string_view::npos)
				return false;

			static constexpr std::array<std::string_view, 16> prefixes = {
			    "a_c"sv,
			    "a_f_m"sv,
			    "a_f_o"sv,
			    "a_f_y"sv,
			    "a_m_m"sv,
			    "a_m_o"sv,
			    "a_m_y"sv,
			    "g_f_y"sv,
			    "g_m_m"sv,
			    "g_m_y"sv,
			    "cs"sv,
			    "csb"sv,
			    "ig"sv,
			    "mp_"sv,
			    "p_"sv,
			    "s_"sv,
			};

			for (const auto prefix : prefixes)
			{
				if (text.starts_with(prefix))
					return true;
			}

			return text.starts_with("u_"sv) || text.starts_with("slod_"sv) || text.starts_with("player_"sv);
		}

		std::string FinalizePedLabel(std::string text)
		{
			static const std::unordered_map<std::string_view, std::string_view> phraseMap = {
			    {"Hammerhead 鲨鱼"sv, "双髻鲨"sv},
			    {"Tiger 鲨鱼"sv, "虎鲨"sv},
			    {"Generic DJ"sv, "通用打碟师"sv},
			    {"shop owner"sv, "店主"sv},
			    {"Tao's 翻译"sv, "陶先生的翻译"sv},
			    {"DOA 缉毒探员"sv, "缉毒局探员"sv},
			    {"DW 机场工人"sv, "机场工人"sv},
			    {"DW 机场工人 2"sv, "机场工人 2"sv},
			};

			if (const auto it = phraseMap.find(text); it != phraseMap.end())
				return std::string(it->second);

			return TrimWhitespace(text);
		}

		std::string TranslateEnglishPedLabel(std::string_view label)
		{
			const auto normalizedLabel = NormalizePedSourceLabel(label);

			static const std::unordered_map<std::string_view, std::string_view> directMap = {
			    {"Molly"sv, "莫莉"sv},
			    {"Nigel"sv, "奈杰尔"sv},
			    {"Omega"sv, "欧米伽"sv},
			    {"Priest"sv, "神父"sv},
			    {"Stretch"sv, "斯崔奇"sv},
			    {"Tanisha"sv, "塔妮莎"sv},
			    {"Sss"sv, "Sss"sv},
			    {"Generic DJ"sv, "通用打碟师"sv},
			    {"DJ Rob T"sv, "打碟师罗布·T"sv},
			    {"Sessanta"sv, "塞桑塔"sv},
			    {"Talina"sv, "塔莉娜"sv},
			    {"Misty"sv, "米丝缇"sv},
			    {"Pros"sv, "职业人士"sv},
			    {"Claude Speed"sv, "克劳德·斯皮德"sv},
			    {"John Marston"sv, "约翰·马斯顿"sv},
			    {"Niko Bellic"sv, "尼克·贝里奇"sv},
			    {"Retailstaff"sv, "零售店员"sv},
			    {"Gaffer"sv, "灯光师"sv},
			    {"Racer Organisator"sv, "赛车组织者"sv},
			    {"Busker"sv, "街头艺人"sv},
			    {"Ammu Nation City Clerk"sv, "武装国度店员"sv},
			    {"Autopsy Tech"sv, "尸检技师"sv},
			    {"Barman"sv, "酒吧男侍"sv},
			    {"Busboy"sv, "餐厅勤杂工"sv},
			    {"费迪南德 克里莫夫 先生 K"sv, "费迪南德·克里莫夫（K先生）"sv},
			    {"FOS 代表"sv, "FOS 代表"sv},
			    {"Grip"sv, "剧务"sv},
			    {"Robber"sv, "劫匪"sv},
			    {"Mask Salesman"sv, "面具售货员"sv},
			    {"Warehouse Technician"sv, "仓库技师"sv},
			    {"Window Cleaner"sv, "擦窗工"sv},
			    {"Miranda"sv, "米兰达"sv},
			    {"Carol"sv, "卡萝尔"sv},
			    {"Eileen"sv, "艾琳"sv},
			    {"Beth"sv, "贝丝"sv},
			    {"Lauren"sv, "劳伦"sv},
			    {"Mistress"sv, "女主人"sv},
			    {"Poppy Mitchell"sv, "波比·米切尔"sv},
			    {"Princess"sv, "公主"sv},
			    {"Taylor"sv, "泰勒"sv},
			    {"Blane"sv, "布莱恩"sv},
			    {"Curtis"sv, "柯蒂斯"sv},
			    {"Ed Toh"sv, "艾德·托"sv},
			    {"FIB Architect"sv, "联邦调查局建筑师"sv},
			    {"Mark Fostenburg"sv, "马克·福斯滕伯格"sv},
			    {"Party Target"sv, "派对目标"sv},
			    {"Rival Paparazzo"sv, "敌对狗仔"sv},
			    {"Vince"sv, "文斯"sv},
			    {"Love Fist Willy"sv, "爱拳威利"sv},
			    {"Dean"sv, "迪恩"sv},
			    {"Abner"sv, "阿布纳"sv},
			    {"Baby D"sv, "宝贝·D"sv},
			    {"Caleb"sv, "凯勒布"sv},
			    {"Chip"sv, "奇普"sv},
			    {"Dead Courier"sv, "死去的信使"sv},
			    {"FIB Mugger"sv, "联邦调查局劫匪"sv},
			    {"FIB 西装"sv, "联邦调查局西装人员"sv},
			    {"FIB 保安"sv, "联邦调查局保安"sv},
			    {"FIB 办公室 工人 2"sv, "联邦调查局办公室职员 2"sv},
			    {"FIB 建筑师"sv, "联邦调查局建筑师"sv},
			    {"FIB 抢匪"sv, "联邦调查局抢匪"sv},
			    {"Gabriel"sv, "加布里埃尔"sv},
			    {"Guido"sv, "圭多"sv},
			    {"Justin"sv, "贾斯汀"sv},
			    {"Partygoer"sv, "派对客"sv},
			    {"Ushi"sv, "乌希"sv},
			    {"Shopkeeper"sv, "店主"sv},
			    {"African American 男性"sv, "非裔美国男性"sv},
			    {"Midlife Crisis 赌场 Bikers"sv, "中年危机赌场摩托帮"sv},
			    {"Ammu Nation 乡村 Clerk"sv, "武装国度乡村店员"sv},
			    {"Tao's 翻译 赌场"sv, "陶先生的赌场翻译"sv},
			    {"Chemical Plant 工人"sv, "化工厂工人"sv},
			    {"DJ 黑人 Madonna"sv, "打碟师黑色麦当娜"sv},
			    {"DJ 黑人麦当娜"sv, "打碟师黑色麦当娜"sv},
			    {"DJ 鲁珀特"sv, "打碟师鲁珀特"sv},
			    {"DJ 瑞安 S"sv, "打碟师瑞安·S"sv},
			    {"DJ 迪克森 经理"sv, "打碟师迪克森经理"sv},
			    {"通用 DJ"sv, "通用打碟师"sv},
			    {"DJ 福蒂奥斯"sv, "打碟师福蒂奥斯"sv},
			    {"DJ 雅各布"sv, "打碟师雅各布"sv},
			    {"DJ 麦克 T"sv, "打碟师麦克·T"sv},
			    {"DJ 罗布 T"sv, "打碟师罗布·T"sv},
			    {"DJ 奥蕾莉亚"sv, "打碟师奥蕾莉亚"sv},
			    {"DJ 伊格纳齐奥"sv, "打碟师伊格纳齐奥"sv},
			    {"岛屿 DJ"sv, "岛屿打碟师"sv},
			    {"岛屿 DJ 1"sv, "岛屿打碟师 1"sv},
			    {"岛屿 DJ 2"sv, "岛屿打碟师 2"sv},
			    {"岛屿 DJ 3"sv, "岛屿打碟师 3"sv},
			    {"岛屿 DJ 4"sv, "岛屿打碟师 4"sv},
			    {"岛屿 DJ 4 D"sv, "岛屿打碟师 4-D"sv},
			    {"岛屿 DJ 4 D 2"sv, "岛屿打碟师 4-D2"sv},
			    {"岛屿 DJ 4 E"sv, "岛屿打碟师 4-E"sv},
			    {"IAA 特工"sv, "国际事务局特工"sv},
			    {"IT 专家"sv, "信息技术专家"sv},
			    {"DW 机场工人"sv, "机场工人"sv},
			    {"DW 机场工人 2"sv, "机场工人 2"sv},
			    {"DOA 男性"sv, "缉毒局探员"sv},
			    {"宝贝 D"sv, "宝贝·D"sv},
			    {"Clubhouse Bar 女性"sv, "会所酒吧女员工"sv},
			    {"Boat 员工 女性"sv, "船只女员工"sv},
			    {"Boat 员工 男性"sv, "船只男员工"sv},
			    {"Heli 员工 女性"sv, "直升机女员工"sv},
			    {"赌场 Cashier"sv, "赌场收银员"sv},
			    {"赌场 shop owner"sv, "赌场店主"sv},
			    {"Weapon Exp 男性"sv, "武器实验男员工"sv},
			    {"Weapon Work 男性"sv, "武器工坊男员工"sv},
			    {"办公室 Garage 机修工 女性"sv, "办公室车库女机修工"sv},
			    {"销售 助手 Low End"sv, "低端销售助理"sv},
			    {"United Paper 男人"sv, "联合纸业男员工"sv},
			    {"Grove 街头 贩子"sv, "格罗夫街头贩子"sv},
			    {"Money 男人"sv, "金钱男员工"sv},
			    {"Thornton 杜根"sv, "桑顿·杜根"sv},
			    {"瓦戈斯 Speak"sv, "瓦戈斯发言人"sv},
			    {"Cartel 守卫"sv, "贩毒集团守卫"sv},
			    {"Cartel 守卫 2"sv, "贩毒集团守卫 2"sv},
			    {"DW Airport 工人"sv, "机场工人"sv},
			    {"DW Airport 工人 2"sv, "机场工人 2"sv},
			    {"序章 Mourner 女性"sv, "序章女性哀悼者"sv},
			    {"序章 Mourner 男性"sv, "序章男性哀悼者"sv},
			    {"女性 夜店 舞者 Burlesque"sv, "女性夜店歌舞秀舞者"sv},
			    {"女性 夜店 舞者 Leather"sv, "女性夜店皮革舞者"sv},
			    {"女性 夜店 舞者 Rave"sv, "女性夜店锐舞舞者"sv},
			    {"男性 夜店 舞者 Burlesque"sv, "男性夜店歌舞秀舞者"sv},
			    {"男性 夜店 舞者 Leather"sv, "男性夜店皮革舞者"sv},
			    {"男性 夜店 舞者 Rave"sv, "男性夜店锐舞舞者"sv},
			    {"珠宝 Thief"sv, "珠宝窃贼"sv},
			    {"Vinewood 女性 2"sv, "好麦坞女性 2"sv},
			    {"Vinewood 女性 3"sv, "好麦坞女性 3"sv},
			    {"Vinewood 男性 4"sv, "好麦坞男性 4"sv},
			    {"网球 Coach"sv, "网球教练"sv},
			    {"街头 Punk"sv, "街头朋克"sv},
			    {"街头 Punk 2"sv, "街头朋克 2"sv},
			    {"摩托帮 Cocaine 女性"sv, "摩托帮可卡因女员工"sv},
			    {"摩托帮 Cocaine 男性"sv, "摩托帮可卡因男员工"sv},
			    {"摩托帮 Counterfeit 女性"sv, "摩托帮伪钞女员工"sv},
			    {"摩托帮 Counterfeit 男性"sv, "摩托帮伪钞男员工"sv},
			    {"摩托帮 Forgery 女性"sv, "摩托帮伪造女员工"sv},
			    {"摩托帮 Forgery 男性"sv, "摩托帮伪造男员工"sv},
			    {"摩托帮 Weed 女性"sv, "摩托帮大麻女员工"sv},
			    {"摩托帮 Weed 男性"sv, "摩托帮大麻男员工"sv},
			    {"脱衣舞娘 Lite 女性"sv, "轻量装束脱衣舞娘"sv},
			    {"脱衣舞娘 Lite"sv, "轻量装束脱衣舞娘"sv},
			    {"Sweatshop 工人"sv, "血汗工厂工人"sv},
			    {"Sweatshop 工人 年轻"sv, "年轻血汗工厂工人"sv},
			    {"汽车 Buyer"sv, "汽车买家"sv},
			    {"Undercover 警察"sv, "卧底警察"sv},
			    {"华裔 打手 Older"sv, "华裔年长打手"sv},
			    {"帮派 Slasher 男性"sv, "帮派砍杀者男性"sv},
			    {"Best 男人"sv, "最佳男员工"sv},
			    {"Dressy 女性"sv, "时髦女性"sv},
			    {"Breakdancer 男性"sv, "霹雳舞男性"sv},
			    {"Downhill 骑行者"sv, "速降骑行者"sv},
			    {"Sunbather 男性"sv, "日光浴男性"sv},
			    {"纹身 Cust 男性"sv, "纹身顾客男性"sv},
			    {"墨西哥裔 Thug"sv, "墨西哥裔打手"sv},
			    {"海滩 Bar 员工"sv, "海滩酒吧员工"sv},
			    {"Hospital Scrubs 女性"sv, "医院手术服女性"sv},
			    {"Crew 成员"sv, "帮会成员"sv},
			    {"Shopkeeper 男性"sv, "店主男性"sv},
			    {"DW Airport 工人"sv, "机场工人"sv},
			    {"DW Airport 工人 2"sv, "机场工人 2"sv},
			    {"IAA 特工"sv, "国际事务局特工"sv},
			    {"FIB 特工"sv, "联邦调查局特工"sv},
			    {"IT 专家"sv, "信息技术专家"sv},
			    {"mp_headtargets"sv, "多人模式头部目标"sv},
			    {"s_m_y_xmech_02_mp"sv, "特殊男性机修工 02（多人版）"sv},
			    {"u_m_m_juggernaut_03"sv, "独特男性重甲兵 03"sv},
			    {"u_m_y_juggernaut_02"sv, "独特年轻男性重甲兵 02"sv},
			    {"u_m_m_yeti"sv, "独特男性雪人"sv},
			    {"u_m_m_yulemonster"sv, "独特男性圣诞怪物"sv},
			    {"Debbie 阿加莎 s Secretary"sv, "黛比·阿加莎的秘书"sv},
			    {"性感 Posh 女性"sv, "性感名媛女性"sv},
			    {"Al Di Napoli 男性"sv, "阿尔·迪·那波利男性"sv},
			    {"DOA 男人"sv, "缉毒局探员"sv},
			    {"Glen Stank 男性"sv, "格伦·斯坦克男性"sv},
			    {"街头 Art 男性"sv, "街头艺术男性"sv},
			    {"电影 尸体 Suited"sv, "西装电影尸体"sv},
			    {"Jesco 白人 Tapdancing 乡巴佬"sv, "杰斯科白人踢踏舞乡巴佬"sv},
			    {"Stag Party 新郎"sv, "单身派对新郎"sv},
			};
			if (const auto it = directMap.find(normalizedLabel); it != directMap.end())
				return std::string(it->second);

			static const std::unordered_map<std::string_view, std::string_view> wordMap = {
			    {"Abigail"sv, "阿比盖尔"sv}, {"Agent"sv, "特工"sv}, {"Alan"sv, "艾伦"sv}, {"Amanda"sv, "阿曼达"sv},
			    {"Anita"sv, "安妮塔"sv}, {"Ary"sv, "艾莉"sv}, {"Aurelia"sv, "奥蕾莉亚"sv}, {"Avi"sv, "艾维"sv},
			    {"Avon"sv, "埃文"sv}, {"Baker"sv, "贝克"sv}, {"Ballas"sv, "巴拉斯"sv}, {"Billionaire"sv, "富豪"sv},
			    {"Bryony"sv, "布莱奥妮"sv}, {"Callgirl"sv, "应召女郎"sv}, {"Celeb"sv, "名人"sv}, {"Charlie"sv, "查理"sv},
			    {"Chef"sv, "厨师"sv}, {"Cop"sv, "警察"sv}, {"Dax"sv, "达克斯"sv}, {"Dave"sv, "戴夫"sv},
			    {"Debra"sv, "黛布拉"sv}, {"DJ"sv, "打碟师"sv}, {"Drugdealer"sv, "毒贩"sv}, {"English"sv, "英国"sv},
			    {"Faber"sv, "费伯"sv}, {"Female"sv, "女性"sv}, {"Fish"sv, "鱼"sv}, {"Fotios"sv, "福蒂奥斯"sv},
			    {"Gerald"sv, "杰拉德"sv}, {"Golfer"sv, "高尔夫球手"sv}, {"Guadalope"sv, "瓜达卢佩"sv}, {"Guard"sv, "守卫"sv},
			    {"GURK"sv, "古尔克"sv}, {"Hammerhead"sv, "双髻"sv}, {"Harris"sv, "哈里斯"sv}, {"Helmsman"sv, "舵手"sv},
			    {"Hick"sv, "乡巴佬"sv}, {"Hipster"sv, "潮人"sv}, {"Howitzer"sv, "豪威泽"sv}, {"Huang"sv, "黄"sv},
			    {"Hugh"sv, "休"sv}, {"Hunter"sv, "亨特"sv}, {"Ignazio"sv, "伊格纳齐奥"sv}, {"Imani"sv, "伊玛尼"sv},
			    {"Imran"sv, "伊姆兰"sv}, {"Island"sv, "岛屿"sv}, {"Jack"sv, "杰克"sv}, {"Jackie"sv, "杰姬"sv},
			    {"Jakob"sv, "雅各布"sv}, {"Jamalamir"sv, "贾马拉米尔"sv}, {"Janet"sv, "珍妮特"sv}, {"Janitor"sv, "清洁工"sv},
			    {"Jay"sv, "杰伊"sv}, {"Jio"sv, "吉欧"sv}, {"Joe"sv, "乔"sv}, {"Johnny"sv, "约翰尼"sv},
			    {"John"sv, "约翰"sv}, {"Josh"sv, "乔什"sv}, {"Juan"sv, "胡安"sv}, {"Karen"sv, "凯伦"sv},
			    {"Kaylee"sv, "凯莉"sv}, {"Kerry"sv, "凯瑞"sv}, {"Killer"sv, "杀手"sv}, {"Klebitz"sv, "克莱比兹"sv},
			    {"Lacy"sv, "蕾西"sv}, {"Lamar"sv, "拉玛"sv}, {"Male"sv, "男性"sv}, {"Manuel"sv, "曼努埃尔"sv},
			    {"Marnie"sv, "玛妮"sv}, {"Mary"sv, "玛丽"sv}, {"Maude"sv, "莫德"sv}, {"Maxim"sv, "马克西姆"sv},
			    {"Merc"sv, "佣兵"sv}, {"Michelle"sv, "米歇尔"sv}, {"Mike"sv, "麦克"sv}, {"Milton"sv, "米尔顿"sv},
			    {"Minuteman"sv, "民兵"sv}, {"Mjo"sv, "姆乔"sv}, {"Molly"sv, "莫莉"sv}, {"Merryweather"sv, "梅利威瑟"sv},
			    {"Natalia"sv, "娜塔莉亚"sv}, {"Nervous"sv, "紧张的"sv}, {"Nigel"sv, "奈杰尔"sv}, {"Norris"sv, "诺里斯"sv},
			    {"Omega"sv, "欧米伽"sv}, {"Ortega"sv, "奥尔特加"sv}, {"Oscar"sv, "奥斯卡"sv}, {"Paige"sv, "佩奇"sv},
			    {"Panther"sv, "黑豹"sv}, {"Pavel"sv, "帕维尔"sv}, {"Peter"sv, "彼得"sv}, {"Porn"sv, "色情"sv},
			    {"Priest"sv, "神父"sv}, {"Prince"sv, "普林斯"sv}, {"Rat"sv, "老鼠"sv}, {"Rashkovsky"sv, "拉什科夫斯基"sv},
			    {"Reed"sv, "里德"sv}, {"Reporter"sv, "记者"sv}, {"Rhesus"sv, "恒河猴"sv}, {"Richards"sv, "理查兹"sv},
			    {"Rocco"sv, "罗科"sv}, {"Ron"sv, "罗恩"sv}, {"Rupert"sv, "鲁珀特"sv}, {"Russian"sv, "俄罗斯"sv},
			    {"Ryan"sv, "瑞安"sv}, {"Screenwriter"sv, "编剧"sv}, {"sessanta"sv, "塞桑塔"sv}, {"Shinowa"sv, "希诺瓦"sv},
			    {"Solomon"sv, "索罗门"sv}, {"Steve"sv, "史蒂夫"sv}, {"Stingray"sv, "黄貂鱼"sv}, {"Stretch"sv, "斯崔奇"sv},
			    {"Strickler"sv, "斯特里克勒"sv}, {"Tanisha"sv, "塔妮莎"sv}, {"Terry"sv, "特里"sv}, {"Tiger"sv, "虎"sv},
			    {"Tonya"sv, "托尼娅"sv}, {"Tony"sv, "托尼"sv}, {"Topless"sv, "上空"sv}, {"Traffic"sv, "交通"sv},
			    {"Wade"sv, "韦德"sv}, {"Warden"sv, "协管"sv}, {"Welsh"sv, "威尔士"sv}, {"Whale"sv, "鲸"sv},
			    {"Wendy"sv, "温蒂"sv}, {"Young"sv, "年轻"sv}, {"Yusuf"sv, "优素福"sv}, {"Zimbor"sv, "津博尔"sv},
			    {"Age"sv, "年龄"sv}, {"African"sv, "非裔"sv}, {"Airport"sv, "机场"sv}, {"American"sv, "美国"sv},
			    {"Army"sv, "军队"sv}, {"Bar"sv, "酒吧"sv}, {"Best"sv, "最佳"sv}, {"Bike"sv, "自行车"sv},
			    {"Bikers"sv, "摩托帮"sv}, {"Boat"sv, "船只"sv}, {"Breakdancer"sv, "霹雳舞者"sv}, {"Burlesque"sv, "歌舞秀"sv},
			    {"Buyer"sv, "买家"sv}, {"Cashier"sv, "收银员"sv}, {"Chemical"sv, "化工厂"sv}, {"Chic"sv, "时髦"sv},
			    {"Clerk"sv, "店员"sv}, {"Clubhouse"sv, "会所"sv}, {"Coach"sv, "教练"sv}, {"Cocaine"sv, "可卡因"sv},
			    {"Connors"sv, "康纳斯"sv}, {"Counterfeit"sv, "伪钞"sv}, {"Crew"sv, "成员"sv}, {"Crisis"sv, "危机"sv},
			    {"Cust"sv, "顾客"sv}, {"D"sv, "D"sv}, {"DD"sv, "末日"sv}, {"Dead"sv, "尸体"sv},
			    {"Debbie"sv, "黛比"sv}, {"De"sv, "德"sv}, {"Devin's"sv, "德温的"sv}, {"Director"sv, "导演"sv},
			    {"Doomsday"sv, "末日豪劫"sv}, {"Downhill"sv, "速降"sv}, {"Dressy"sv, "时髦"sv}, {"DW"sv, "DW"sv},
			    {"E"sv, "E"sv}, {"End"sv, "低端"sv}, {"Ex"sv, "前"sv}, {"Exp"sv, "实验"sv},
			    {"Field"sv, "野外"sv}, {"FOS"sv, "FOS"sv}, {"Forgery"sv, "伪造"sv}, {"Funeral"sv, "葬礼"sv}, {"Garage"sv, "车库"sv},
			    {"Georgina"sv, "乔治娜"sv}, {"Giver"sv, "给予者"sv}, {"Grove"sv, "格罗夫"sv}, {"Gun"sv, "武器"sv},
			    {"Hairdresser"sv, "发型师"sv}, {"Hangar"sv, "机库"sv}, {"Heli"sv, "直升机"sv}, {"Hospital"sv, "医院"sv},
			    {"IAA"sv, "国际事务局"sv}, {"IT"sv, "信息技术"sv}, {"Jackson"sv, "杰克逊"sv}, {"Jewelry"sv, "珠宝"sv},
			    {"Juggernaut"sv, "重甲兵"sv}, {"K"sv, "K"sv}, {"Leather"sv, "皮革"sv}, {"Lite"sv, "轻量"sv},
			    {"Labourer"sv, "工人"sv}, {"Los"sv, "洛圣都"sv}, {"Low"sv, "低端"sv}, {"Madonna"sv, "麦当娜"sv}, {"Malibu"sv, "马里布"sv},
			    {"Martin"sv, "马丁"sv}, {"Miguel"sv, "米格尔"sv}, {"Midlife"sv, "中年"sv}, {"Money"sv, "金钱"sv},
			    {"Mourner"sv, "哀悼者"sv}, {"Nation"sv, "国度"sv}, {"Older"sv, "年长"sv}, {"owner"sv, "店主"sv},
			    {"Pain"sv, "痛苦"sv}, {"Pavel"sv, "帕维尔"sv}, {"Paper"sv, "纸业"sv}, {"Phillips"sv, "菲利普斯"sv},
			    {"Plant"sv, "工厂"sv}, {"Prison"sv, "监狱"sv}, {"Processer"sv, "加工员"sv}, {"Punk"sv, "朋克"sv}, {"Rackman"sv, "拉克曼"sv},
			    {"Rave"sv, "锐舞"sv}, {"Rich"sv, "富豪"sv}, {"S"sv, "S"sv}, {"Sacha"sv, "萨沙"sv},
			    {"Schawrtzman"sv, "施瓦茨曼"sv}, {"Schwartzman"sv, "施瓦茨曼"sv}, {"Scrubs"sv, "手术服"sv},
			    {"Secruity"sv, "保安"sv}, {"Securoserve"sv, "保镖公司"sv}, {"Sheriff"sv, "警长"sv}, {"shop"sv, "商店"sv},
			    {"Simeon"sv, "西门"sv}, {"Slasher"sv, "砍杀者"sv}, {"Snow"sv, "雪地"sv}, {"Santos"sv, "圣托斯"sv},
			    {"Speak"sv, "发言人"sv}, {"Speaker"sv, "司仪"sv}, {"Sports"sv, "运动"sv}, {"Stag"sv, "单身派对"sv},
			    {"Sunbather"sv, "日光浴客"sv}, {"Sweatshop"sv, "血汗工厂"sv}, {"T"sv, "T"sv}, {"Tao's"sv, "陶先生的"sv},
			    {"Thief"sv, "窃贼"sv}, {"Thornhill"sv, "桑希尔"sv}, {"Thornton"sv, "桑顿"sv}, {"Thug"sv, "打手"sv}, {"Transport"sv, "运输"sv},
			    {"Tuners"sv, "改车铺"sv}, {"Tyler"sv, "泰勒"sv}, {"Undercover"sv, "卧底"sv}, {"United"sv, "联合"sv},
			    {"Vinewood"sv, "好麦坞"sv}, {"Warehouse"sv, "仓库"sv}, {"Weapon"sv, "武器"sv}, {"Weed"sv, "大麻"sv},
			    {"Wei"sv, "魏"sv}, {"Work"sv, "工人"sv},
			};

			std::string translated;
			std::string token;
			for (size_t i = 0; i <= normalizedLabel.size(); ++i)
			{
				const bool atEnd = i == normalizedLabel.size();
				const unsigned char ch = atEnd ? 0 : static_cast<unsigned char>(normalizedLabel[i]);
				if (atEnd || std::isspace(ch))
				{
					if (!token.empty())
					{
						if (!translated.empty())
							translated += ' ';
						if (const auto it = wordMap.find(token); it != wordMap.end())
							translated += it->second;
						else
							translated += token;
						token.clear();
					}
					continue;
				}

				if (std::ispunct(ch) && ch != '\'' && ch != '&')
				{
					if (!token.empty())
					{
						if (!translated.empty())
							translated += ' ';
						if (const auto it = wordMap.find(token); it != wordMap.end())
							translated += it->second;
						else
							translated += token;
						token.clear();
					}
					if (!translated.empty())
						translated += ' ';
					translated += static_cast<char>(ch);
					continue;
				}

				token += static_cast<char>(ch);
			}

			return FinalizePedLabel(translated);
		}

		std::string BuildFallbackPedDisplayName(std::string_view model)
		{
			static const std::unordered_map<std::string_view, std::string_view> prefixMap = {
			    {"a_c"sv, "动物"sv},
			    {"a_f_m"sv, "路人女性"sv},
			    {"a_f_o"sv, "年长女性"sv},
			    {"a_f_y"sv, "年轻女性"sv},
			    {"a_m_m"sv, "路人男性"sv},
			    {"a_m_o"sv, "年长男性"sv},
			    {"a_m_y"sv, "年轻男性"sv},
			    {"g_f_y"sv, "帮派女性"sv},
			    {"g_m_m"sv, "帮派男性"sv},
			    {"g_m_y"sv, "帮派男性"sv},
			    {"ig"sv, "任务角色"sv},
			    {"cs"sv, "过场角色"sv},
			    {"csb"sv, "过场角色"sv},
			    {"mp_f"sv, "多人女性角色"sv},
			    {"mp_m"sv, "多人男性角色"sv},
			    {"player"sv, "主角"sv},
			    {"s_f_m"sv, "特殊女性"sv},
			    {"s_f_y"sv, "特殊女性"sv},
			    {"s_m_m"sv, "特殊男性"sv},
			    {"s_m_y"sv, "特殊男性"sv},
			    {"u_f_m"sv, "独特女性"sv},
			    {"u_f_o"sv, "独特年长女性"sv},
			    {"u_f_y"sv, "独特年轻女性"sv},
			    {"u_m_m"sv, "独特男性"sv},
			    {"u_m_o"sv, "独特年长男性"sv},
			    {"u_m_y"sv, "独特年轻男性"sv},
			};
			static const std::unordered_map<std::string_view, std::string_view> tokenMap = {
			    {"beach"sv, "海滩"sv}, {"bevhills"sv, "贝弗利山"sv}, {"business"sv, "商务"sv}, {"carclub"sv, "车友会"sv},
			    {"genbiker"sv, "摩托帮路人"sv}, {"bankrobber"sv, "银行劫匪"sv}, {"billionaire"sv, "富豪"sv}, {"dax"sv, "达克斯"sv},
			    {"agent14"sv, "特工 14"sv}, {"amandatownley"sv, "阿曼达·汤利"sv}, {"ary"sv, "艾莉"sv}, {"avi"sv, "艾维"sv},
			    {"avischwartzman"sv, "艾维·施瓦茨曼"sv}, {"ballas"sv, "巴拉斯帮"sv}, {"callgirl"sv, "应召女郎"sv},
			    {"charlie"sv, "查理"sv}, {"chef"sv, "厨师"sv}, {"drfriedlander"sv, "弗里德兰德医生"sv},
			    {"englishdave"sv, "英格利希·戴夫"sv}, {"faber"sv, "法伯"sv}, {"golfer"sv, "高尔夫球手"sv},
			    {"helmsmanpavel"sv, "舵手帕维尔"sv}, {"imani"sv, "伊玛尼"sv}, {"jamalamir"sv, "贾马拉米尔"sv}, {"jenettee"sv, "珍妮特"sv},
			    {"jio"sv, "吉欧"sv}, {"jodimarshall"sv, "乔迪·马歇尔"sv}, {"johnny"sv, "强尼"sv}, {"labrat"sv, "实验鼠"sv},
			    {"lamardavis"sv, "拉玛·戴维斯"sv}, {"lestercrest"sv, "莱斯特·克雷斯特"sv}, {"luchadora"sv, "女摔角手"sv},
			    {"martinmadrazo"sv, "马丁·马德拉索"sv}, {"mjo"sv, "姆乔"sv}, {"musician"sv, "音乐人"sv}, {"nervousron"sv, "紧张的罗恩"sv},
			    {"oscar"sv, "奥斯卡"sv}, {"party"sv, "派对"sv}, {"promo"sv, "宣传员"sv}, {"rafdeangelis"sv, "拉夫·德安杰利斯"sv},
			    {"req"sv, "记者"sv}, {"soundeng"sv, "音效工程师"sv}, {"vagos"sv, "瓦戈斯帮"sv}, {"valentina"sv, "瓦伦蒂娜"sv},
			    {"vernon"sv, "弗农"sv}, {"vincent"sv, "文森特"sv}, {"weiss"sv, "魏斯"sv}, {"yusufamir"sv, "优素福·阿米尔"sv},
			    {"fooliganz"sv, "癫仔帮"sv}, {"cartelgoons"sv, "贩毒集团打手"sv}, {"friedlandergoons"sv, "费蓝德打手"sv},
			    {"genthug"sv, "通用打手"sv}, {"goons"sv, "打手"sv}, {"knoway"sv, "无路可逃"sv}, {"maragrande"sv, "马拉格兰德"sv},
			    {"zombie"sv, "丧尸"sv}, {"undeadmage"sv, "亡灵法师"sv}, {"acidlabcook"sv, "致幻剂实验室厨师"sv}, {"agent"sv, "特工"sv}, {"ahronward"sv, "阿隆·沃德"sv},
			    {"armsmanufac"sv, "军火工厂"sv}, {"bountytarget"sv, "悬赏目标"sv}, {"entourage"sv, "随从"sv},
			    {"djblamryanh"sv, "打碟师布莱恩·瑞安"sv}, {"furry"sv, "福瑞人士"sv}, {"gunvanseller"sv, "枪械厢型车商贩"sv},
			    {"gustavo"sv, "古斯塔沃"sv}, {"hippyleader"sv, "嬉皮士头目"sv}, {"jaywalker"sv, "乱穿马路者"sv},
			    {"warehouseboss"sv, "仓库主管"sv}, {"fibleader"sv, "联邦调查局头目"sv},
			    {"headtargets"sv, "头部目标"sv}, {"studioassist"sv, "录音棚助理"sv}, {"warehouse"sv, "仓库"sv},
			    {"bailoffice"sv, "保释事务所"sv}, {"ccrew"sv, "施工队"sv}, {"hazmatworker"sv, "防化工人"sv},
			    {"highsec"sv, "高级保安"sv}, {"studioprod"sv, "录音棚制作人"sv}, {"studiosoueng"sv, "录音棚音效工程师"sv},
			    {"subcrew"sv, "潜艇船员"sv}, {"xmech"sv, "机修工"sv}, {"slod"sv, "低细节"sv}, {"human"sv, "人类"sv},
			    {"secretary"sv, "秘书"sv}, {"drowned"sv, "溺亡者"sv}, {"posh"sv, "名媛"sv}, {"suited"sv, "西装"sv},
			    {"tapdancing"sv, "踢踏舞"sv}, {"guadalope"sv, "瓜达卢佩"sv}, {"maudebf"sv, "莫德的男友"sv},
			    {"roostermccraw"sv, "鲁斯特·麦克劳"sv}, {"valencheat"sv, "情人节负心汉"sv}, {"subcrewhead"sv, "潜艇船员头目"sv},
			    {"juggernaut"sv, "重甲兵"sv}, {"yeti"sv, "雪人"sv}, {"yulemonster"sv, "圣诞怪物"sv},
			    {"clubcust"sv, "夜店顾客"sv}, {"cop"sv, "警察"sv}, {"eastsa"sv, "东圣安地列斯"sv}, {"epsilon"sv, "爱普西隆"sv},
			    {"fitness"sv, "健身"sv}, {"genstreet"sv, "街头"sv}, {"golfer"sv, "高尔夫球手"sv}, {"hiker"sv, "徒步者"sv},
			    {"hippie"sv, "嬉皮士"sv}, {"hipster"sv, "潮人"sv}, {"indian"sv, "印第安裔"sv}, {"juggalo"sv, "小丑帮"sv},
			    {"ktown"sv, "韩裔城"sv}, {"mechanic"sv, "机修工"sv}, {"musclbeac"sv, "肌肉海滩"sv}, {"paparazzi"sv, "狗仔"sv},
			    {"pilot"sv, "飞行员"sv}, {"polynesian"sv, "波利尼西亚裔"sv}, {"prolhost"sv, "序章人质"sv}, {"rurmeth"sv, "乡村冰毒"sv},
			    {"salton"sv, "沙尔顿"sv}, {"skater"sv, "滑板手"sv}, {"skidrow"sv, "贫民区"sv}, {"smartcaspat"sv, "赌场贵宾"sv},
			    {"soucent"sv, "南中部"sv}, {"soucentmc"sv, "南中部摩托帮"sv}, {"studioparty"sv, "录音棚派对"sv}, {"tennis"sv, "网球"sv},
			    {"tourist"sv, "游客"sv}, {"tramp"sv, "流浪汉"sv}, {"trampbeac"sv, "海滩流浪汉"sv}, {"vinewood"sv, "好麦坞"sv},
			    {"yoga"sv, "瑜伽"sv}, {"boar"sv, "野猪"sv}, {"cat"sv, "猫"sv}, {"chickenhawk"sv, "捕鸡鹰"sv}, {"chimp"sv, "黑猩猩"sv},
			    {"chop"sv, "小查"sv}, {"cormorant"sv, "鸬鹚"sv}, {"cow"sv, "奶牛"sv}, {"coyote"sv, "郊狼"sv}, {"crow"sv, "乌鸦"sv},
			    {"deer"sv, "鹿"sv}, {"dolphin"sv, "海豚"sv}, {"fish"sv, "鱼"sv}, {"hen"sv, "母鸡"sv}, {"humpback"sv, "座头鲸"sv},
			    {"husky"sv, "哈士奇"sv}, {"killerwhale"sv, "虎鲸"sv}, {"mtlion"sv, "美洲狮"sv}, {"panther"sv, "黑豹"sv},
			    {"pig"sv, "猪"sv}, {"pigeon"sv, "鸽子"sv}, {"poodle"sv, "贵宾犬"sv}, {"pug"sv, "巴哥犬"sv}, {"rabbit"sv, "兔子"sv},
			    {"rat"sv, "老鼠"sv}, {"retriever"sv, "寻回犬"sv}, {"rhesus"sv, "恒河猴"sv}, {"rottweiler"sv, "罗威纳犬"sv},
			    {"seagull"sv, "海鸥"sv}, {"sharkhammer"sv, "双髻鲨"sv}, {"sharktiger"sv, "虎鲨"sv}, {"shepherd"sv, "牧羊犬"sv},
			    {"stingray"sv, "黄貂鱼"sv}, {"westy"sv, "西高地梗"sv},
			};

			const auto firstUnderscore = model.find('_');
			const auto secondUnderscore = firstUnderscore == std::string_view::npos ? std::string_view::npos : model.find('_', firstUnderscore + 1);
			const auto thirdUnderscore = secondUnderscore == std::string_view::npos ? std::string_view::npos : model.find('_', secondUnderscore + 1);

			std::string result;
			if (thirdUnderscore != std::string_view::npos)
			{
				const auto prefix = model.substr(0, thirdUnderscore);
				if (const auto it = prefixMap.find(prefix); it != prefixMap.end())
					result = std::string(it->second);
			}
			else if (secondUnderscore != std::string_view::npos)
			{
				const auto prefix = model.substr(0, secondUnderscore);
				if (const auto it = prefixMap.find(prefix); it != prefixMap.end())
					result = std::string(it->second);
			}
			else if (firstUnderscore != std::string_view::npos)
			{
				const auto prefix = model.substr(0, firstUnderscore);
				if (const auto it = prefixMap.find(prefix); it != prefixMap.end())
					result = std::string(it->second);
			}

			const auto labelStart = thirdUnderscore != std::string_view::npos ? thirdUnderscore + 1
			                     : (secondUnderscore != std::string_view::npos ? secondUnderscore + 1
			                     : (firstUnderscore != std::string_view::npos ? firstUnderscore + 1 : 0));
			std::string_view labelPart = model.substr(labelStart);
			std::string token;
			for (size_t i = 0; i <= labelPart.size(); ++i)
			{
				if (i == labelPart.size() || labelPart[i] == '_')
				{
					if (!token.empty())
					{
						if (!result.empty())
							result += ' ';
						if (const auto it = tokenMap.find(token); it != tokenMap.end())
							result += it->second;
						else
							result += token;
						token.clear();
					}
					continue;
				}
				token += labelPart[i];
			}

			return result.empty() ? std::string(model) : result;
		}

		std::string GetPedDisplayName(int index)
		{
			if (index >= 0 && index < static_cast<int>(g_PedDisplayNames.size()))
			{
				const auto display = std::string_view(g_PedDisplayNames[index]);
				const auto model = std::string_view(g_PedModels[index]);

				if (!display.empty() && LooksLikePedModelId(display))
					return BuildFallbackPedDisplayName(display);

				if (!display.empty() && display != model)
				{
					if (ContainsCjk(display))
						return FinalizePedLabel(TranslateEnglishPedLabel(display));
					return TranslateEnglishPedLabel(display);
				}
				return BuildFallbackPedDisplayName(model);
			}

			return {};
		}
	}

	std::shared_ptr<Category> BuildSpawnPedMenu()
	{
		auto menu = std::make_shared<Category>("生成行人");

		static bool invincible;
		static bool spawnDead;
		static bool spawnAsBodyguard;
		static bool spawnAsCop;
		static bool spawnInMyVehicle;
		static bool giveAllWeapons;
		static bool spawnAsProstitute;
		static bool randomizeOutfit;
		static bool blipPed;
		static std::vector<Ped> spawnedPeds;

		menu->AddItem(std::make_unique<ImGuiItem>([] {
			static char search[64];
			ImGui::SetNextItemWidth(300.f);
			ImGui::InputTextWithHint("名称", "搜索", search, sizeof(search));

			const int visible = std::min(20, static_cast<int>(g_PedModels.size()));
			const float height = visible * ImGui::GetTextLineHeightWithSpacing();
			if (ImGui::BeginListBox("##peds", {300.f, height}))
			{
				std::string lower = search;
				std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
				for (int veh = 0; veh < g_PedModels.size(); veh++)
				{
					auto name = g_PedModels[veh];
					auto displayName = GetPedDisplayName(veh);
					auto lowerName = std::string(name);
					auto lowerDisplayName = displayName;
					std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
					std::transform(lowerDisplayName.begin(), lowerDisplayName.end(), lowerDisplayName.begin(), ::tolower);

					bool matchesSearch = lowerName.find(lower) != std::string::npos || lowerDisplayName.find(lower) != std::string::npos;
					if (matchesSearch)
					{
						ImGui::PushID(name);
						if (ImGui::Selectable(displayName.c_str()))
						{
							auto set_player = ImGui::GetIO().KeyCtrl;
							FiberPool::Push([name, set_player] {
								if (spawnInMyVehicle)
								{
									auto vehicle = Self::GetVehicle();
									if (vehicle)
									{
										if (!vehicle.IsSeatFree(-2) && 
										    !vehicle.IsSeatFree(-1))
										{
											Notifications::Show(
											    "生成行人",
											    "无法在载具内生成行人，因为所有座位都已被占用。请先腾出一个座位，或关闭“生成到我的载具内”。",
											    NotificationType::Warning);
											return;
										}
									}
								}

								auto hash = Joaat(name);
								auto handle = Ped::Create(hash, Self::GetPed().GetPosition(), Self::GetPed().GetHeading());

								if (!handle)
									return;

								handle.SetCombatAttribute(PedCombatAttribute::AlwaysFight, true);
								handle.SetCombatAttribute(PedCombatAttribute::DisableAllRandomsFlee, true);
								handle.SetCombatAttribute(PedCombatAttribute::DisableFleeFromCombat, true);
								handle.SetCombatAttribute(PedCombatAttribute::AlwaysFlee, false);
								handle.SetCombatAttribute(PedCombatAttribute::FleesFromInvincibleOpponents, false);
								handle.SetCombatAttribute(PedCombatAttribute::CanUseVehicles, true);
								handle.SetCombatAttribute(PedCombatAttribute::CanLeaveVehicle, true);

								if (invincible)
									handle.SetInvincible(true);

								if (spawnDead)
									handle.Kill();

								if (spawnAsBodyguard && !set_player)
								{
									handle.SetCombatAttribute(PedCombatAttribute::CanCharge, true);
									handle.SetCombatAttribute(PedCombatAttribute::CanCommandeerVehicles, true);
									handle.SetCombatAttribute(PedCombatAttribute::DisableInjuredOnGround, true);
									handle.SetCombatAttribute(PedCombatAttribute::AllowDogFighting, true);
									handle.SetCombatAttribute(PedCombatAttribute::PerfectAccuracy, true);
									handle.SetCombatAttribute(PedCombatAttribute::UseVehicleAttack, true);
									handle.SetCombatAttribute(PedCombatAttribute::CanDoDrivebys, true);
									handle.SetCombatAttribute(PedCombatAttribute::CanThrowSmokeGrenade, true);
									handle.SetCombatAttribute(PedCombatAttribute::CanSeeUnderwaterPeds, true);
									
									auto group = Self::GetPlayer().GetGroup();
									handle.AddToGroup(group);
									PED::SET_GROUP_SEPARATION_RANGE(group, 9999.9f);
									PED::SET_PED_CAN_TELEPORT_TO_GROUP_LEADER(handle.GetHandle(), group, true);
									PED::SET_GROUP_FORMATION_SPACING(group, 10.0f, 0.0f, 0.0f);
								}

								if (spawnAsCop)
								{
									handle.SetAsCop();
								}

								if (spawnInMyVehicle)
								{
									auto vehicle = Self::GetVehicle();
									if (vehicle) {
										if (vehicle.IsSeatFree(-1))
											handle.SetInVehicle(vehicle, -1);
										else
											handle.SetInVehicle(vehicle, -2);
									}
								}

								if (giveAllWeapons)
								{
									for (auto hash : g_WeaponHashes)
										handle.GiveWeapon(hash);
								}

								if (randomizeOutfit)
									handle.RandomizeOutfit();

								if (blipPed)
								{
									HUD::SET_BLIP_COLOUR(HUD::ADD_BLIP_FOR_ENTITY(handle.GetHandle()), 3);
								}

								if (spawnAsProstitute)
								{
									handle.StartScenario("WORLD_HUMAN_PROSTITUTE_HIGH_CLASS");
									handle.SetKeepTask(true);

									if (!*Pointers.IsSessionStarted)
									{
										int ped = handle.GetHandle();
										Scripts::StartScript("pb_prostitute"_J, eStackSizes::FRIEND, &ped, 1);
									}
								}

								if (set_player)
								{
									static auto hooked = []()
									{
										NativeHooks::AddHook("freemode"_J, NativeIndex::GET_ENTITY_MODEL, [](rage::scrNativeCallContext* ctx) {
											auto model = ENTITY::GET_ENTITY_MODEL(ctx->GetArg<int>(0));

											if (ctx->GetArg<int>(0) == Self::GetPed().GetHandle() && (model != "mp_m_freemode_01"_J && model != "mp_f_freemode_01"_J))
											{
												return ctx->SetReturnValue("mp_m_freemode_01"_J);
											}

											return ctx->SetReturnValue(model);
										});
										for (auto script : {"main"_J, "respawn_controller"_J, "pi_menu"_J})
										{
											NativeHooks::AddHook(script, NativeIndex::GET_ENTITY_MODEL, [](rage::scrNativeCallContext* ctx) {
												auto model = ENTITY::GET_ENTITY_MODEL(ctx->GetArg<int>(0));

												if (ctx->GetArg<int>(0) == Self::GetPed().GetHandle() && (model != "player_zero"_J && model != "player_one"_J && model != "player_two"_J))
												{
													return ctx->SetReturnValue("player_zero"_J);
												}

												return ctx->SetReturnValue(model);
											}); 
										}
										return true;
									}();
									Self::GetPlayer().SetPed(handle);
								}
								else
								{
									spawnedPeds.push_back(handle);
								}
							});
						}
						ImGui::PopID();
					}
				}

				ImGui::EndListBox();
			}

			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::BulletText("%s", "按住 Ctrl 点击可设为玩家模型");
			ImGui::Checkbox("无敌", &invincible);
			ImGui::Checkbox("生成死亡状态", &spawnDead);
			ImGui::Checkbox("生成保镖", &spawnAsBodyguard);
			ImGui::Checkbox("生成警察", &spawnAsCop);
			ImGui::Checkbox("生成到我的载具内", &spawnInMyVehicle);
			ImGui::Checkbox("为角色配备全部武器", &giveAllWeapons);
			ImGui::Checkbox("生成妓女", &spawnAsProstitute);
			ImGui::Checkbox("随机服装", &randomizeOutfit);
			ImGui::Checkbox("为行人添加标记", &blipPed);
			if (ImGui::Button("移除全部"))
			{
				FiberPool::Push([] {
					for (auto& ped : spawnedPeds)
						if (ped)
							ped.Delete();

					spawnedPeds.clear();
				});
			}
			ImGui::EndGroup();
		}));

		return menu;
	}
}
