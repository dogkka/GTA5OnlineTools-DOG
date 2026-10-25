#include "Localization.hpp"

#include <format>
#include <string_view>
#include <unordered_map>

namespace YimMenu::Localization
{
	namespace
	{
		using namespace std::literals;

		const std::unordered_map<std::string_view, std::string_view> g_Translations = {
		    {"Unknown!"sv, "未知"sv},
		    {"Unknown list!"sv, "未知列表"sv},
		    {"Unknown string command!"sv, "未知字符串命令"sv},
		    {"Unknown color picker!"sv, "未知取色器"sv},
		    {"Close"sv, "关闭"sv},
		    {"Clear"sv, "清除"sv},
		    {"Save"sv, "保存"sv},
		    {"Delete"sv, "删除"sv},
		    {"Remove"sv, "移除"sv},
		    {"Add"sv, "添加"sv},
		    {"Search"sv, "搜索"sv},
		    {"Optional"sv, "可选"sv},
		    {"Infinite"sv, "无限"sv},
		    {"Name"sv, "名称"sv},
		    {"Basket"sv, "购物篮"sv},
		    {"Current"sv, "当前"sv},
		    {"Saved..."sv, "已保存..."sv},
		    {"Yes"sv, "是"sv},
		    {"No"sv, "否"sv},
		    {"Default"sv, "默认"sv},
		    {"FPS: %d"sv, "帧率：%d"sv},
		    {"Interior Index"sv, "室内索引"sv},
		    {"Enter Owner Interior"sv, "进入房主室内"sv},
		    {"Do Teleport"sv, "执行传送"sv},
		    {"Team"sv, "队伍"sv},
		    {"Join"sv, "加入"sv},
		    {"Join##session"sv, "加入##session"sv},
		    {"Join##username"sv, "加入##username"sv},
		    {"Join##rid"sv, "加入##rid"sv},
		    {"Refresh"sv, "刷新"sv},
		    {"Refresh##packed"sv, "刷新##packed"sv},
		    {"Write"sv, "写入"sv},
		    {"Write##packed"sv, "写入##packed"sv},
		    {"Write##packed_range"sv, "写入##packed_range"sv},
		    {"Abort"sv, "中止"sv},
		    {"Access"sv, "访问"sv},
		    {"Bounds"sv, "边界"sv},
		    {"By"sv, "按"sv},
		    {"Cell"sv, "单元格"sv},
		    {"Circle"sv, "圆形"sv},
		    {"Division"sv, "除法"sv},
		    {"Error"sv, "错误"sv},
		    {"Exceeded"sv, "超出"sv},
		    {"Frame Padding"sv, "控件内边距"sv},
		    {"Grab Min Size"sv, "滑块最小尺寸"sv},
		    {"Max"sv, "最大"sv},
		    {"Memory"sv, "内存"sv},
		    {"Null"sv, "空"sv},
		    {"Of"sv, "的"sv},
		    {"Overflow"sv, "溢出"sv},
		    {"Pointer"sv, "指针"sv},
		    {"Scroll bar Size"sv, "滚动条大小"sv},
		    {"Scrollbar Size"sv, "滚动条大小"sv},
		    {"Separator"sv, "分隔线"sv},
		    {"Separator Text Align"sv, "分隔符文字对齐"sv},
		    {"Stack"sv, "堆栈"sv},
		    {"Underflow"sv, "下溢"sv},
		    {"Violation"sv, "违规"sv},
		    {"Window Min Size"sv, "窗口最小尺寸"sv},
		    {"Display Window Padding"sv, "显示窗口边距"sv},
		    {"Cell Padding"sv, "单元格边距"sv},
		    {"Circle Tessellation Max Error"sv, "圆形细分最大误差"sv},
		    {"Value"sv, "值"sv},
		    {"Value##packed"sv, "值##packed"sv},
		    {"Value##packed_range"sv, "值##packed_range"sv},
		    {"Reload"sv, "重新加载"sv},
		    {"Unload"sv, "卸载"sv},
		    {"Pause"sv, "暂停"sv},
		    {"Resume"sv, "继续"sv},
		    {"Add Item"sv, "添加物品"sv},
		    {"Set"sv, "设置"sv},
		    {"Setup"sv, "配置"sv},
		    {"Apply Font Scale"sv, "应用字体缩放"sv},
		    {"Adjust"sv, "调整"sv},
		    {"Edit color for"sv, "编辑颜色："sv},
		    {"Self"sv, "个人"sv},
		    {"Vehicle"sv, "载具"sv},
		    {"Teleport"sv, "传送"sv},
		    {"Network"sv, "联机"sv},
		    {"Players"sv, "玩家"sv},
		    {"World"sv, "世界"sv},
		    {"Recovery"sv, "账号进度"sv},
		    {"Settings"sv, "设置"sv},
		    {"Debug"sv, "调试"sv},
		    {"Main"sv, "基础"sv},
		    {"Globals"sv, "全局"sv},
		    {"Movement"sv, "移动"sv},
		    {"Tools"sv, "工具"sv},
		    {"Special Ability"sv, "特殊能力"sv},
		    {"Wanted"sv, "通缉"sv},
		    {"Info"sv, "信息"sv},
		    {"Item"sv, "物品"sv},
		    {"Bytecode"sv, "字节码"sv},
		    {"Toxic"sv, "恶意"sv},
		    {"Troll"sv, "整蛊"sv},
		    {"Locals"sv, "局部变量"sv},
		    {"Damage"sv, "伤害"sv},
		    {"Griefing"sv, "骚扰"sv},
		    {"Kick"sv, "踢出"sv},
		    {"Kicks"sv, "踢人"sv},
		    {"General"sv, "常规"sv},
		    {"Spawn"sv, "生成"sv},
		    {"Saved"sv, "已保存"sv},
		    {"New"sv, "新增"sv},
		    {"Notifications"sv, "通知"sv},
		    {"Context Function"sv, "快捷操作"sv},
		    {"Tracking"sv, "追踪"sv},
		    {"Host"sv, "主机"sv},
		    {"Modder"sv, "作弊者"sv},
		    {"Godmode"sv, "无敌"sv},
		    {"Invisible"sv, "隐身"sv},
		    {"Customize"sv, "自定义"sv},
		    {"Colors"sv, "颜色"sv},
		    {"Rounding"sv, "圆角"sv},
		    {"Layout"sv, "布局"sv},
		    {"Border"sv, "边框"sv},
		    {"Global"sv, "全局"sv},
		    {"Bounty"sv, "悬赏"sv},
		    {"Misc"sv, "杂项"sv},
		    {"Session"sv, "战局"sv},
		    {"Join Session"sv, "加入战局"sv},
		    {"Join Session Type"sv, "加入战局类型"sv},
		    {"The session type to join"sv, "要加入的战局类型"sv},
		    {"Joins the specified session type"sv, "加入指定类型的战局"sv},
		    {"Enhancements"sv, "增强"sv},
		    {"Spoofing"sv, "伪装"sv},
		    {"Matchmaking (Client)"sv, "匹配伪装（客户端）"sv},
		    {"Matchmaking (Server)"sv, "匹配伪装（服务端）"sv},
		    {"Saved Players"sv, "已保存玩家"sv},
		    {"Random Events"sv, "随机事件"sv},
		    {"Network Bail"sv, "网络脱离"sv},
		    {"Do Team Swap"sv, "执行队伍切换"sv},
		    {"Hotkeys"sv, "热键"sv},
		    {"GUI"sv, "界面"sv},
		    {"Game"sv, "游戏"sv},
		    {"UI"sv, "界面"sv},
		    {"Player ESP"sv, "玩家透视"sv},
		    {"Ped ESP"sv, "行人透视"sv},
		    {"Object ESP"sv, "物体透视"sv},
		    {"Overlay"sv, "信息浮窗"sv},
		    {"Chat"sv, "聊天"sv},
		    {"Businesses"sv, "产业"sv},
		    {"Casino"sv, "赌场"sv},
		    {"Slot Machines"sv, "老虎机"sv},
		    {"Daily Activities"sv, "日常活动"sv},
		    {"Heists"sv, "抢劫"sv},
		    {"Transactions"sv, "交易"sv},
		    {"Stat Editor"sv, "数据编辑器"sv},
		    {"Regular"sv, "普通"sv},
		    {"Packed"sv, "打包"sv},
		    {"Packed Range"sv, "打包范围"sv},
		    {"From Clipboard"sv, "从剪贴板"sv},
		    {"Vehicle Editor"sv, "载具编辑器"sv},
		    {"Spawn Ped"sv, "生成行人"sv},
		    {"Outfit Editor"sv, "服装编辑器"sv},
		    {"Weapons"sv, "武器"sv},
		    {"Ammu-Nation"sv, "武装国度"sv},
		    {"Aimbot"sv, "自瞄"sv},
		    {"IPLs"sv, "场景集"sv},
		    {"Weather"sv, "天气"sv},
		    {"Time"sv, "时间"sv},
		    {"Other"sv, "其他"sv},
		    {"Categories"sv, "分类"sv},
		    {"Locations"sv, "地点"sv},
		    {"Category"sv, "分类"sv},
		    {"Location name"sv, "地点名称"sv},
		    {"Inventory Slot"sv, "库存槽位"sv},
		    {"New location"sv, "新地点"sv},
		    {"Save current location"sv, "保存当前位置"sv},
		    {"Double click to teleport\nShift click to delete"sv, "双击传送\nShift + 单击删除"sv},
		    {"Distance: %f"sv, "距离：%f"sv},
		    {"Are you sure you want to delete %s?"sv, "确定要删除 %s 吗？"sv},
		    {"Please enter a valid name"sv, "请输入有效名称"sv},
		    {"Custom Teleport"sv, "自定义传送"sv},
		    {"Location with name {} already exists"sv, "名为 {} 的地点已存在"sv},
		    {"Click on a location to select it. Add more at Teleport > Saved"sv, "点击地点以选择。可在 传送 > 已保存 中添加更多地点"sv},
		    {"Hover over the command name to change its hotkey"sv, "将鼠标悬停在命令名称上可修改热键"sv},
		    {"Press any registered key to remove"sv, "按任意已注册按键可移除"sv},
		    {"Enter a keystroke"sv, "请输入按键"sv},
		    {"Press any button..."sv, "按下任意按键..."sv},
		    {"No hotkey assigned"sv, "未分配热键"sv},
		    {"Modify Colors:"sv, "调整颜色："sv},
		    {"Adjust Rounding:"sv, "调整圆角："sv},
		    {"Layout & Alignment:"sv, "布局与对齐："sv},
		    {"Border Sizes:"sv, "边框尺寸："sv},
		    {"Global Settings:"sv, "全局设置："sv},
		    {"Font Configuration:"sv, "字体配置："sv},
		    {"Current Scale: %.2f"sv, "当前缩放：%.2f"sv},
		    {"Lua Scripts"sv, "Lua 脚本"sv},
		    {"Search:"sv, "搜索："sv},
		    {"All"sv, "全部"sv},
		    {"Class"sv, "类别"sv},
		    {"Garage"sv, "车库"sv},
		    {"Root"sv, "根目录"sv},
		    {"Folder"sv, "文件夹"sv},
		    {"File Name"sv, "文件名"sv},
		    {"Folder Name"sv, "文件夹名"sv},
		    {"Natives not cached yet."sv, "原生函数缓存尚未完成。"sv},
		    {"Stats not loaded yet."sv, "统计数据尚未加载。"sv},
		    {"Load From Clipboard: cannot find stat {}"sv, "从剪贴板加载：找不到属性 {}"sv},
		    {"Saved Vehicles"sv, "已保存载具"sv},
		    {"New Vehicle"sv, "新载具"sv},
		    {"Personal Vehicle"sv, "个人载具"sv},
		    {"Are you sure you want to spawn %s?"sv, "确定要生成 %s 吗？"sv},
		    {"Please enter GTA Online first."sv, "请先进入 GTA 在线模式。"sv},
		    {"Loaded successfully."sv, "加载成功。"sv},
		    {"ImGui Style Editor"sv, "界面样式编辑器"sv},
		    {"Stat not found"sv, "未找到该属性"sv},
		    {"Invalid index"sv, "索引无效"sv},
		    {"Type"sv, "类型"sv},
		    {"Category"sv, "分类"sv},
		    {"Action"sv, "操作"sv},
		    {"At"sv, "位置"sv},
		    {"Bitset"sv, "位集"sv},
		    {"Bool"sv, "布尔"sv},
		    {"Float"sv, "浮点"sv},
		    {"Int"sv, "整数"sv},
		    {"Int64"sv, "长整数"sv},
		    {"Value"sv, "值"sv},
		    {"Size"sv, "大小"sv},
		    {"String"sv, "字符串"sv},
		    {"Vector"sv, "向量"sv},
		    {"Index"sv, "索引"sv},
		    {"Start"sv, "起始"sv},
		    {"End"sv, "结束"sv},
		    {"Quantity"sv, "数量"sv},
		    {"Price"sv, "价格"sv},
		    {"Stat Value"sv, "属性值"sv},
		    {"Rockstar ID"sv, "R星 ID"sv},
		    {"Camera"sv, "摄像头"sv},
		    {"Jammer"sv, "信号干扰器"sv},
		    {"Mission Object"sv, "任务物品"sv},
		    {"Player List"sv, "玩家列表"sv},
		    {"Plate Number"sv, "车牌号"sv},
		    {"Apply in Minutes"sv, "按分钟应用"sv},
		    {"Cooldown and Availability"sv, "冷却与可用性"sv},
		    {"Data has not been fetched yet"sv, "数据尚未拉取"sv},
		    {"Transactions are unavailable while FSL local saves are enabled"sv, "启用本地存档时不支持交易功能"sv},
		    {"Catalog is not loaded"sv, "目录尚未加载"sv},
		    {"Ctrl+Click to set as player model"sv, "按住控制键点击可设为玩家模型"sv},
		    {"Invincible"sv, "无敌"sv},
		    {"Spawn Dead"sv, "生成死亡状态"sv},
		    {"Spawn as Bodyguard"sv, "作为保镖生成"sv},
		    {"Spawn as Cop"sv, "作为警察生成"sv},
		    {"Spawn in My Vehicle"sv, "生成到我的载具中"sv},
		    {"Spawn as Prostitute"sv, "作为妓女生成"sv},
		    {"Blip Ped"sv, "为行人添加标记"sv},
		    {"Remove All"sv, "移除全部"sv},
		    {"Give Weapon"sv, "给予武器"sv},
		    {"Remove Weapon"sv, "移除武器"sv},
		    {"Cannot spawn ped in vehicle because all seats are occupied. Free up a seat first, or disable \"Spawn in My Vehicle\"."sv, "无法在载具中生成行人，所有座位都已被占用。请先腾出座位，或关闭“生成到我的载具中”选项。"sv},
		    {"Kills With: %d"sv, "击杀数：%d"sv},
		    {"Deaths By: %d"sv, "死亡数：%d"sv},
		    {"K/D Ratio: %.2f"sv, "击杀/死亡比：%.2f"sv},
		    {"Headshots: %d"sv, "爆头数：%d"sv},
		    {"Accuracy: %d%%"sv, "命中率：%d%%"sv},
		    {"Custom Weapons"sv, "自定义武器"sv},
		    {"Spawn Inside"sv, "生成在车内"sv},
		    {"Spawn Maxed"sv, "满改生成"sv},
		    {"Spawn Clone"sv, "生成克隆"sv},
		    {"Stock Horn"sv, "原厂喇叭"sv},
		    {"Truck Horn"sv, "卡车喇叭"sv},
		    {"Cop Horn"sv, "警笛喇叭"sv},
		    {"Clown Horn"sv, "小丑喇叭"sv},
		    {"Musical Horn 1"sv, "音乐喇叭 1"sv},
		    {"Musical Horn 2"sv, "音乐喇叭 2"sv},
		    {"Musical Horn 3"sv, "音乐喇叭 3"sv},
		    {"Musical Horn 4"sv, "音乐喇叭 4"sv},
		    {"Musical Horn 5"sv, "音乐喇叭 5"sv},
		    {"Sad Trombone"sv, "哀伤长号"sv},
		    {"Classical Horn 1"sv, "古典喇叭 1"sv},
		    {"Classical Horn 2"sv, "古典喇叭 2"sv},
		    {"Classical Horn 3"sv, "古典喇叭 3"sv},
		    {"Classical Horn 4"sv, "古典喇叭 4"sv},
		    {"Classical Horn 5"sv, "古典喇叭 5"sv},
		    {"Classical Horn 6"sv, "古典喇叭 6"sv},
		    {"Classical Horn 7"sv, "古典喇叭 7"sv},
		    {"Classical Horn 8"sv, "古典喇叭 8"sv},
		    {"Classical Horn Loop 1"sv, "古典喇叭循环 1"sv},
		    {"Classical Horn Loop 2"sv, "古典喇叭循环 2"sv},
		    {"Scale Do"sv, "音阶 哆"sv},
		    {"Scale Re"sv, "音阶 来"sv},
		    {"Scale Mi"sv, "音阶 咪"sv},
		    {"Scale Fa"sv, "音阶 发"sv},
		    {"Scale Sol"sv, "音阶 唆"sv},
		    {"Scale La"sv, "音阶 啦"sv},
		    {"Scale Ti"sv, "音阶 西"sv},
		    {"Scale Do (High)"sv, "高音 哆"sv},
		    {"Jazz Horn 1"sv, "爵士喇叭 1"sv},
		    {"Jazz Horn 2"sv, "爵士喇叭 2"sv},
		    {"Jazz Horn 3"sv, "爵士喇叭 3"sv},
		    {"Jazz Horn Loop"sv, "爵士喇叭循环"sv},
		    {"Star Spangled Banner 1"sv, "星条旗之歌 1"sv},
		    {"Star Spangled Banner 2"sv, "星条旗之歌 2"sv},
		    {"Star Spangled Banner 3"sv, "星条旗之歌 3"sv},
		    {"Star Spangled Banner 4"sv, "星条旗之歌 4"sv},
		    {"Halloween Loop 1"sv, "万圣节循环 1"sv},
		    {"Halloween Loop 2"sv, "万圣节循环 2"sv},
		    {"San Andreas Loop"sv, "圣安地列斯循环"sv},
		    {"Liberty City Loop"sv, "自由城循环"sv},
		    {"Festive Loop 1"sv, "节日循环 1"sv},
		    {"Festive Loop 2"sv, "节日循环 2"sv},
		    {"Festive Loop 3"sv, "节日循环 3"sv},
		    {"Business"sv, "产业"sv},
		    {"God Mode"sv, "无敌模式"sv},
		    {"Blocks all incoming damage"sv, "抵挡所有传入伤害"sv},
		    {"Vehicle God Mode"sv, "载具无敌"sv},
		    {"Blocks all incoming damage to your current vehicle"sv, "抵挡当前载具受到的所有伤害"sv},
		    {"Locally Visible"sv, "本地可见"sv},
		    {"Be locally visible while remaining invisible to other players"sv, "对其他玩家保持隐形的同时，在本地仍然可见"sv},
		    {"Off The Radar"sv, "脱离雷达"sv},
		    {"No Ragdoll"sv, "防止倒地"sv},
		    {"No Idle Kick"sv, "防挂机踢出"sv},
		    {"Unlimited Oxygen"sv, "无限氧气"sv},
		    {"Format Money"sv, "金额格式化"sv},
		    {"Mobile Radio"sv, "移动电台"sv},
		    {"Keep Player Clean"sv, "保持角色整洁"sv},
		    {"Disable Critical Hits"sv, "免疫暴击"sv},
		    {"Skip Cutscene"sv, "跳过过场动画"sv},
		    {"Skip Conversation"sv, "跳过对话"sv},
		    {"Suicide"sv, "自杀"sv},
		    {"Heal"sv, "治疗"sv},
		    {"Clear Damage"sv, "清除伤痕"sv},
		    {"Fill Inventory"sv, "补满补给品"sv},
		    {"Open Wardrobe"sv, "打开衣柜"sv},
		    {"Stand on Vehicles"sv, "站在载具上"sv},
		    {"Disable Action Mode"sv, "禁用动作模式"sv},
		    {"Super Run"sv, "超级奔跑"sv},
		    {"Move Rate Override"sv, "移动速率覆盖"sv},
		    {"Multipler for base run/sprint speed"sv, "基础奔跑/冲刺速度倍率"sv},
		    {"Run/Sprint/Swim Multiplier"sv, "跑步/冲刺/游泳倍率"sv},
		    {"Multipler with which the run/sprint/swim speed is increased with until maximum speed is reached"sv, "提升跑步、冲刺和游泳速度直至达到最高速度的倍率"sv},
		    {"Super Jump"sv, "超级跳"sv},
		    {"Noclip"sv, "穿墙"sv},
		    {"Noclip Speed"sv, "穿墙速度"sv},
		    {"How fast you move while noclip is active"sv, "穿墙模式启用时的移动速度"sv},
		    {"Freecam"sv, "自由视角"sv},
		    {"Freecam Speed"sv, "自由视角速度"sv},
		    {"How fast the freecam will move positions"sv, "自由视角移动速度"sv},
		    {"Infinite Special Ability"sv, "无限特殊能力"sv},
		    {"Enable in MP"sv, "在多人模式启用"sv},
		    {"Special Ability in MP"sv, "线上模式特殊能力"sv},
		    {"The special ability to enable for the MP character"sv, "为线上角色启用的特殊能力"sv},
		    {"Matchmaking Region"sv, "匹配地区"sv},
		    {"Matchmaking region to spoof to"sv, "要伪装的匹配地区"sv},
		    {"Spoof Region Type"sv, "伪装地区类型"sv},
		    {"Spoof Language"sv, "伪装语言"sv},
		    {"Spoof Player Count"sv, "伪装玩家数"sv},
		    {"Multiplex Session"sv, "多路广播战局"sv},
		    {"Region Type"sv, "地区类型"sv},
		    {"Language"sv, "语言"sv},
		    {"Player Count"sv, "玩家数量"sv},
		    {"Multiplex Session Count"sv, "多路广播数量"sv},
		    {"Wanted Slider"sv, "通缉等级滑块"sv},
		    {"Wanted level to set/freeze"sv, "要设置或冻结的通缉等级"sv},
		    {"Move Rate Override"sv, "移动速率覆盖"sv},
		    {"Multipler for base run/sprint speed"sv, "基础奔跑/冲刺速度倍率"sv},
		    {"Run/Sprint/Swim Multiplier"sv, "跑步/冲刺/游泳倍率"sv},
		    {"Multipler with which the run/sprint/swim speed is increased with until maximum speed is reached"sv, "提升跑步、冲刺和游泳速度直至达到最高速度的倍率"sv},
		    {"Freecam Speed"sv, "自由视角速度"sv},
		    {"How fast the freecam will move positions"sv, "自由视角移动速度"sv},
		    {"Noclip Speed"sv, "穿墙速度"sv},
		    {"Weapon Damage Scale"sv, "武器伤害倍率"sv},
		    {"Sets the damage scale for weapons (including melee)"sv, "设置武器伤害倍率（包括近战）"sv},
		    {"Locally Visible"sv, "本地可见"sv},
		    {"Be locally visible while remaining invisible to other players"sv, "对其他玩家保持隐形的同时，在本地仍然可见"sv},
		    {"Special Ability in MP"sv, "线上模式特殊能力"sv},
		    {"The special ability to enable for the MP character"sv, "为线上角色启用的特殊能力"sv},
		    {"Spoofs the region type of the session"sv, "伪装战局地区类型"sv},
		    {"Spoofs the session language"sv, "伪装战局语言"sv},
		    {"Spoofs the session player count"sv, "伪装战局玩家数量"sv},
		    {"The region to spoof the session to"sv, "将战局地区伪装为"sv},
		    {"The language to spoof the session to"sv, "将战局语言伪装为"sv},
		    {"The player count to spoof the session to"sv, "将战局玩家数伪装为"sv},
		    {"The number of sessions to advertise for matchmaking"sv, "为匹配广播的战局实例数量"sv},
		    {"Matchmaking region to spoof to"sv, "要伪装的匹配地区"sv},
		    {"Makes your session much more visible to other players by hosting multiple instances of it. High values may fill your session in seconds. Use at your own risk"sv, "通过广播多个战局实例显著提升你的战局可见度。数值过高可能会在几秒内塞满战局，请自行承担风险"sv},
		    {"Public"sv, "公开"sv},
		    {"Solo Public"sv, "单人公开"sv},
		    {"SCTV"sv, "社群俱乐部电视"sv},
		    {"Crew"sv, "帮会"sv},
		    {"Join Crew"sv, "加入帮会"sv},
		    {"Closed Crew"sv, "封闭帮会"sv},
		    {"Closed Friend"sv, "封闭好友"sv},
		    {"Find Friend"sv, "寻找好友"sv},
		    {"Invite Only"sv, "仅邀请"sv},
		    {"Friends Only"sv, "仅好友"sv},
		    {"Solo"sv, "单人"sv},
		    {"Chinese (Simplified)"sv, "简体中文"sv},
		    {"Chinese (Traditional)"sv, "繁体中文"sv},
		    {"English"sv, "英语"sv},
		    {"French"sv, "法语"sv},
		    {"German"sv, "德语"sv},
		    {"Italian"sv, "意大利语"sv},
		    {"Japanese"sv, "日语"sv},
		    {"Korean"sv, "韩语"sv},
		    {"Polish"sv, "波兰语"sv},
		    {"Portuguese (Brazil)"sv, "葡萄牙语（巴西）"sv},
		    {"Russian"sv, "俄语"sv},
		    {"Spanish (Mexico)"sv, "西班牙语（墨西哥）"sv},
		    {"Spanish (Spain)"sv, "西班牙语（西班牙）"sv},
		    {"Australia"sv, "澳大利亚"sv},
		    {"CIS"sv, "独联体"sv},
		    {"China"sv, "中国"sv},
		    {"Europe"sv, "欧洲"sv},
		    {"Japan"sv, "日本"sv},
		    {"South America"sv, "南美"sv},
		    {"US East"sv, "美国东部"sv},
		    {"US West"sv, "美国西部"sv},
		    {"Unknown"sv, "未知"sv},
		    {"Ped Model"sv, "行人模型"sv},
		    {"The model name of the ped you wish to spawn."sv, "要生成的行人模型名称。"sv},
		    {"Object Model"sv, "物体模型"sv},
		    {"The model name of the object you wish to spawn."sv, "要生成的物体模型名称。"sv},
		    {"Hour"sv, "小时"sv},
		    {"Minute"sv, "分钟"sv},
		    {"Set hour (0-23)"sv, "设置小时（0-23）"sv},
		    {"Set minute (0-59)"sv, "设置分钟（0-59）"sv},
		    {"Set second (0-59)"sv, "设置秒数（0-59）"sv},
		    {"Saved Players"sv, "已保存玩家"sv},
		    {"Player Tracker"sv, "玩家追踪"sv},
		    {"Persist Car"sv, "已保存载具"sv},
		    {"Player Database Auto Update"sv, "玩家数据库自动更新"sv},
		    {"Tracking Notifications"sv, "追踪通知"sv},
		    {"Auto Update"sv, "自动更新"sv},
		    {"Update Now"sv, "立即更新"sv},
		    {"Track Player"sv, "追踪玩家"sv},
		    {"Data not fetched yet"sv, "数据尚未拉取"sv},
		    {"Username"sv, "用户名"sv},
		    {"Teleport To"sv, "传送到"sv},
		    {"Notify When Joinable"sv, "可加入时通知"sv},
		    {"Notify When Unjoinable"sv, "不可加入时通知"sv},
		    {"Notify When Online"sv, "上线时通知"sv},
		    {"Notify When Offline"sv, "离线时通知"sv},
		    {"Notify On Session Type Change"sv, "战局类型变更时通知"sv},
		    {"Notify On Mission Change"sv, "任务变更时通知"sv},
		    {"Notify On Job Lobby Change"sv, "差事大厅变更时通知"sv},
		    {"Automatically updates tracked player status every 3 minutes. This is required for tracking notifications to work"sv, "每 3 分钟自动更新一次被追踪玩家的状态。追踪通知功能需要开启此项"sv},
		    {"Notifies you when the state of a player you track changes"sv, "当你追踪的玩家状态发生变化时通知你"sv},
		    {"Notifies you when a tracked player becomes joinable"sv, "当被追踪玩家变为可加入状态时通知你"sv},
		    {"Notifies you when a tracked player becomes unjoinable"sv, "当被追踪玩家变为不可加入状态时通知你"sv},
		    {"Notifies you when a tracked player goes online"sv, "当被追踪玩家上线时通知你"sv},
		    {"Notifies you when a tracked player goes offline"sv, "当被追踪玩家离线时通知你"sv},
		    {"Notifies you when a tracked player's session type changes"sv, "当被追踪玩家的战局类型发生变化时通知你"sv},
		    {"Notifies you when a tracked player joins or leaves a mission"sv, "当被追踪玩家加入或离开任务时通知你"sv},
		    {"Notifies you when a tracked player joins or leaves a job lobby"sv, "当被追踪玩家加入或离开差事大厅时通知你"sv},
		    {"Session Type: %s"sv, "战局类型：%s"sv},
		    {"Host of Session: %s"sv, "战局主持人：%s"sv},
		    {"Is Spectating: %s"sv, "是否在观战：%s"sv},
		    {"Is Job Lobby: %s"sv, "是否在差事大厅：%s"sv},
		    {"Host of Job Lobby: %s"sv, "差事大厅主持人：%s"sv},
		    {"Mission Type: %s"sv, "任务类型：%s"sv},
		    {"Mission Name: %s"sv, "任务名称：%s"sv},
		    {"{} is no longer online"sv, "{} 已离线"sv},
		    {"{} is now in a joinable session"sv, "{} 当前已可加入"sv},
		    {"{} is now online"sv, "{} 已上线"sv},
		    {"{} is no longer in a joinable session"sv, "{} 当前不可加入"sv},
		    {"{} is now in a {} session"sv, "{} 当前处于 {} 战局"sv},
		    {"{} is no longer in a {}"sv, "{} 已离开 {}"sv},
		    {"{} is now in a {}"sv, "{} 当前正在进行 {}"sv},
		    {"{} has hosted a job lobby"sv, "{} 创建了一个差事大厅"sv},
		    {"{} has joined a job lobby"sv, "{} 加入了一个差事大厅"sv},
		    {"{} is no longer in a job lobby"sv, "{} 已离开差事大厅"sv},
		    {"{} vehicles fixed."sv, "已修复 {} 辆载具。"sv},
		    {"No vehicles to fix."sv, "没有可修复的载具。"sv},
		    {"Tried to save a vehicle which does not exist"sv, "尝试保存一辆不存在的载具"sv},
		    {"File does not exist."sv, "文件不存在。"sv},
		    {"Select a file first"sv, "请先选择一个文件"sv},
		    {"Spawned {}"sv, "已生成 {}"sv},
		    {"Unable to spawn {}"sv, "无法生成 {}"sv},
		    {"Invalid"sv, "无效"sv},
		    {"Hidden by Modder"sv, "被作弊玩家隐藏"sv},
		    {"Job"sv, "任务"sv},
		    {"Mission"sv, "任务"sv},
		    {"Deathmatch"sv, "死斗"sv},
		    {"Race"sv, "竞速"sv},
		    {"Survival"sv, "生存战"sv},
		    {"Gang Attack"sv, "帮派攻击"sv},
		    {"Golf"sv, "高尔夫"sv},
		    {"Tennis"sv, "网球"sv},
		    {"Shooting Range"sv, "射击场"sv},
		    {"HP: {}/{} ({:.2f}%)"sv, "生命值：{}/{}（{:.2f}%）"sv},
		    {"Please join GTA Online."sv, "请先进入 GTA 在线模式。"sv},
		    {"Cannot open gun locker while in an interior."sv, "在室内时无法打开武器柜。"sv},
		    {"Not safe to open the wardrobe at the moment."sv, "当前不适合打开衣柜。"sv},
		    {"Failed to open the wardrobe."sv, "打开衣柜失败。"sv},
		    {"Not safe to call the mechanic at the moment."sv, "当前不适合呼叫技工。"sv},
		    {"Failed to call the mechanic."sv, "呼叫技工失败。"sv},
		    {"No model name provided."sv, "未提供模型名称。"sv},
		    {"This vehicle cannot be saved as a personal vehicle."sv, "该载具无法保存为个人载具。"sv},
		    {"This vehicle is already a personal vehicle."sv, "该载具已经是个人载具。"sv},
		    {"Player entity cannot be deleted."sv, "玩家实体无法删除。"sv},
		    {"Invalid vehicle."sv, "无效载具。"sv},
		    {"No vehicle model name provided."sv, "未提供载具模型名称。"sv},
		    {"Freemode global block is not loaded."sv, "自由模式全局块未加载。"sv},
		    {"Random Events are not initialized."sv, "随机事件尚未初始化。"sv},
		    {"Freemode stack is not valid."sv, "自由模式堆栈无效。"sv},
		    {"Freemode is not running."sv, "自由模式未运行。"sv},
		    {"Completed all activities, switch session to apply the changes."sv, "已完成所有活动，请切换战局以应用更改。"sv},
		    {"Reset all activities, switch session to apply the changes."sv, "已重置所有活动，请切换战局以应用更改。"sv},
		    {"This Hidden Cache has already been collected."sv, "该隐藏补给箱已被收集。"sv},
		    {"This Treasure Chest has already been collected."sv, "该宝箱已被收集。"sv},
		    {"Shipwreck has already been collected."sv, "沉船奖励已被收集。"sv},
		    {"This Buried Stash has already been collected."sv, "该埋藏战利品已被收集。"sv},
		    {"This Time Trial has already been beaten."sv, "该时间挑战赛已被完成。"sv},
		    {"Please enable the blip from the Interaction Menu."sv, "请先在互动菜单中启用该地图标记。"sv},
		    {"You have already delivered all the vehicles."sv, "你已经交付了所有载具。"sv},
		    {"You have just delivered a vehicle, wait a moment."sv, "你刚刚交付过一辆载具，请稍等片刻。"sv},
		    {"G's Cache has already been collected."sv, "G 的藏匿点已被收集。"sv},
		    {"Stash House has already been raided."sv, "藏匿屋已经洗劫过了。"sv},
		    {"This LS Tag has already been sprayed."sv, "该洛圣都涂鸦已被喷涂。"sv},
		    {"You must own a Bail Office."sv, "你必须拥有一间保释事务所。"sv},
		    {"Madrazo Hit has already been completed."sv, "玛德拉索暗杀已完成。"sv},
		    {"This animal has already been photographed."sv, "该动物已被拍摄。"sv},
		    {"You must own a Smoke on the Water property."sv, "你必须拥有“喷云吐雾馆”产业。"sv},
		    {"This product has already been collected."sv, "该货物已被收集。"sv},
		    {"Golden Clover has already been collected."sv, "金色四叶草已被收集。"sv},
		    {"Active Events: %d/%d"sv, "活动事件：%d/%d"sv},
		    {"Launch Event"sv, "启动事件"sv},
		    {"Kill Event"sv, "终止事件"sv},
		    {"Teleport to Event"sv, "传送到事件"sv},
		    {"State: %s"sv, "状态：%s"sv},
		    {"Location: N/A"sv, "地点：无"sv},
		    {"Trigger Range: N/A"sv, "触发范围：无"sv},
		    {"Location: %d"sv, "地点：%d"sv},
		    {"Trigger Range: %.2f"sv, "触发范围：%.2f"sv},
		    {"Set Cooldown"sv, "设置冷却"sv},
		    {"Set Availability"sv, "设置可用性"sv},
		    {"Open GitHub"sv, "打开 GitHub 仓库"sv},
		    {"Open Matrix server"sv, "打开 Matrix 社区"sv},
		    {"Play with YimMenu users"sv, "与 YimMenu 用户一起游玩"sv},
		    {"Play with everyone (Broken!)"sv, "与所有人一起游玩（已失效）"sv},
		    {"IMPORTANT! PLEASE READ!"sv, "重要！请务必阅读！"sv},
		    {"Rank: %d (%d RP)"sv, "等级：%d（%d 声望）"sv},
		    {"Money: %d"sv, "金钱：%d"sv},
		    {"Coords: %.2f, %.2f, %.2f"sv, "坐标：%.2f, %.2f, %.2f"sv},
		    {"Distance: %.2f"sv, "距离：%.2f"sv},
		    {"Ped missing or deleted"sv, "角色不存在或已删除"sv},
		    {"RID:"sv, "R星 ID："sv},
		    {"Xbox User ID:"sv, "Xbox 用户编号："sv},
		    {"Steam ID:"sv, "Steam 账号编号："sv},
		    {"Epic Account ID:"sv, "Epic 账户编号："sv},
		    {"IP Address:"sv, "网络地址："sv},
		    {"Add to Saved"sv, "添加到已保存"sv},
		    {"View SC Profile"sv, "查看社交俱乐部档案"sv},
		    {"Add Friend"sv, "添加好友"sv},
		    {"More Info"sv, "更多信息"sv},
		    {"Average Latency: %.2f"sv, "平均延迟：%.2f"sv},
		    {"Packet Loss: %.2f"sv, "丢包率：%.2f"sv},
		    {"No players yet!"sv, "当前没有玩家！"sv},
		    {"FPS: %d"sv, "帧率：%d"sv},
		    {"Plate Number"sv, "车牌号"sv},
		    {"Please enter a vehicle."sv, "请先进入一辆载具。"sv},
		    {"Refresh Current Vehicle"sv, "刷新当前载具"sv},
		    {"Change Plate"sv, "修改车牌"sv},
		    {"Max Vehicle"sv, "满改载具"sv},
		    {"Modification Options"sv, "改装选项"sv},
		    {"Burstable Tires"sv, "轮胎可爆"sv},
		    {"Drift Tires"sv, "低抓地轮胎"sv},
		    {"Turbo"sv, "涡轮"sv},
		    {"Tire Smoke"sv, "轮胎烟雾"sv},
		    {"Modification Slots"sv, "改装槽位"sv},
		    {"Slot"sv, "槽位"sv},
		    {"Modification"sv, "改装"sv},
		    {"Mod"sv, "改装"sv},
		    {"Style"sv, "样式"sv},
		    {"Extras"sv, "额外部件"sv},
		    {"Neon Options"sv, "霓虹灯选项"sv},
		    {"Headlights"sv, "前灯"sv},
		    {"Left"sv, "左侧"sv},
		    {"Right"sv, "右侧"sv},
		    {"Front"sv, "前方"sv},
		    {"Rear"sv, "后方"sv},
		    {"Color Options"sv, "颜色选项"sv},
		    {"Primary Color"sv, "主色"sv},
		    {"Secondary Color"sv, "副色"sv},
		    {"Pearlescent"sv, "珠光"sv},
		    {"Interior"sv, "内饰"sv},
		    {"Dashboard"sv, "仪表台"sv},
		    {"Wheel Color"sv, "轮毂颜色"sv},
		    {"Neon"sv, "霓虹灯"sv},
		    {"Custom"sv, "自定义"sv},
		    {"Remove Custom"sv, "移除自定义"sv},
		    {"Chrome"sv, "镀铬"sv},
		    {"Classic"sv, "经典"sv},
		    {"Matte"sv, "哑光"sv},
		    {"Metal"sv, "金属"sv},
		    {"Util"sv, "实用"sv},
		    {"Worn"sv, "做旧"sv},
		    {"Chameleon"sv, "幻彩"sv},
		    {"Custom Vehicle Color"sv, "自定义载具颜色"sv},
		    {"Alloy"sv, "合金"sv},
		    {"Normalized name to: %s"sv, "名称规范化为：%s"sv},
		    {"Load from Clipboard"sv, "从剪贴板加载"sv},
		    {"Data type not supported"sv, "不支持的数据类型"sv},
		    {"Warning: You are solely responsible for what you do with this tool. If you don't know what you're doing, you'll likely get banned"sv, "警告：你需要对使用此工具的后果自行负责。如果你不知道自己在做什么，很可能会被封禁"sv},
		    {"Trigger"sv, "触发"sv},
		    {"Start Script"sv, "启动脚本"sv},
		    {"Start Session Script"sv, "启动战局脚本"sv},
		    {"Start Script With Event"sv, "通过事件启动脚本"sv},
		    {"Jump to Offset"sv, "跳转到偏移"sv},
		    {"Kill"sv, "终止"sv},
		    {"Log Labels"sv, "记录标签"sv},
		    {"Script Name"sv, "脚本名称"sv},
		    {"Add Offset"sv, "添加偏移"sv},
		    {"Add Read Player Id"sv, "添加读取玩家编号"sv},
		    {"Remove Offset"sv, "移除偏移"sv},
		    {"Network Bail"sv, "网络脱离"sv},
		    {"DoTeleport"sv, "执行传送"sv},
		    {"Randomize Outfit"sv, "随机服装"sv},
		    {"Start Script With Event"sv, "通过事件启动脚本"sv},
		    {"Skips any currently playing cutscene"sv, "跳过当前播放的过场动画"sv},
		    {"Skips current conversation in the phone."sv, "跳过当前电话对话。"sv},
		    {"Kills you"sv, "将你杀死"sv},
		    {"Restores your health and armour."sv, "恢复生命值与护甲。"sv},
		    {"Clears all the dirt and blood on you."sv, "清除身上的污渍和血迹。"sv},
		    {"Fills all of your snacks and armours."sv, "补满全部零食和护甲。"sv},
		    {"Invisibility"sv, "隐身"sv},
		    {"Be invisible"sv, "使自己隐身"sv},
		    {"Prevents you from ragdolling when standing on top of vehicles."sv, "站在载具顶部时不会摔倒。"sv},
		    {"Disables the player's janky movement during combat."sv, "禁用战斗时的动作模式移动。"sv},
		    {"Run faster than normal"sv, "跑得比平时更快"sv},
		    {"No Clip"sv, "穿墙"sv},
		    {"Allows you to fly through the map"sv, "允许你穿过地图飞行"sv},
		    {"Ensures that the special ability bar never runs out"sv, "确保特殊能力槽不会耗尽"sv},
		    {"Enable Special Abilities in MP"sv, "在多人模式启用特殊能力"sv},
		    {"Enables special abilities in multiplayer. This repurposes cut content and may be unstable"sv, "在多人模式启用特殊能力。该功能基于未启用内容，可能不稳定"sv},
		    {"Infinite Ammo"sv, "无限弹药"sv},
		    {"You will never run out of ammo"sv, "不会耗尽弹药"sv},
		    {"Rapid Fire"sv, "快速射击"sv},
		    {"Increases the fire rate of your weapons substantially"sv, "大幅提高武器射速"sv},
		    {"Infinite Parachutes"sv, "无限降落伞"sv},
		    {"Always have a parachute"sv, "始终拥有降落伞"sv},
		    {"Explosive Ammo"sv, "爆炸弹药"sv},
		    {"Every bullet you shoot explodes."sv, "你射出的每一发子弹都会爆炸。"sv},
		    {"Weapon Damage"sv, "武器伤害"sv},
		    {"Allows altering your weapons damage output"sv, "允许调整武器伤害输出"sv},
		    {"Weapon Damage Scale"sv, "武器伤害倍率"sv},
		    {"Sets the damage scale for weapons (including melee)"sv, "设置武器伤害倍率（包括近战）"sv},
		    {"Prevents your ped from ragdolling"sv, "防止角色进入布娃娃状态"sv},
		    {"Allows you to stay underwater without losing oxygen."sv, "允许你在水下停留而不损失氧气。"sv},
		    {"Hides you from other players on the radar"sv, "在雷达上对其他玩家隐藏自己"sv},
		    {"Never Wanted"sv, "永不通缉"sv},
		    {"Never gain a wanted level"sv, "永远不会获得通缉等级"sv},
		    {"Freeze Wanted"sv, "冻结通缉"sv},
		    {"Freeze your wanted level to the desired level"sv, "将通缉等级冻结在指定等级"sv},
		    {"Clear Wanted"sv, "清除通缉"sv},
		    {"Clears your wanted level"sv, "清除通缉等级"sv},
		    {"Set Wanted"sv, "设置通缉"sv},
		    {"Sets your wanted level to the desired level"sv, "将通缉等级设置为指定等级"sv},
		    {"Wanted Slider"sv, "通缉等级滑块"sv},
		    {"Wanted level to set/freeze"sv, "要设置或冻结的通缉等级"sv},
		    {"Locks on to enemies and other players"sv, "锁定敌人和其他玩家"sv},
		    {"Aim For Head"sv, "瞄准头部"sv},
		    {"Targets the head when locking on to enemies and other players"sv, "锁定敌人和其他玩家时瞄准头部"sv},
		    {"Target Drivers"sv, "瞄准驾驶员"sv},
		    {"Allows aimbot to lock on to enemies and other players in vehicles"sv, "允许自瞄锁定载具中的敌人和其他玩家"sv},
		    {"Repair Vehicle"sv, "修复载具"sv},
		    {"Fixes any damage to your current vehicle"sv, "修复当前载具的所有损坏"sv},
		    {"Fix All Vehicles"sv, "修复全部个人载具"sv},
		    {"Fixes all of your destroyed personal vehicles."sv, "修复你所有已损毁的个人载具。"sv},
		    {"Keep Vehicle Fixed"sv, "保持载具完好"sv},
		    {"Keeps your vehicle clean and fixed"sv, "保持载具清洁并自动修复"sv},
		    {"Prevents you from falling out of vehicles."sv, "防止你从载具中甩出。"sv},
		    {"Horn Boost"sv, "喇叭加速"sv},
		    {"Makes the vehicle boost when you hold the horn"sv, "按住喇叭时让载具加速"sv},
		    {"Speedometer"sv, "速度表"sv},
		    {"Shows a speedometer whenever you are in a vehicle"sv, "在载具中显示速度表"sv},
		    {"Allows you to request your personal vehicles."sv, "打开技工的个人载具选择界面。"sv},
		    {"Enter last vehicle"sv, "进入最近载具"sv},
		    {"Enters the last vehicle you were in"sv, "进入你上一次乘坐的载具"sv},
		    {"Requests your current Personal Vehicle."sv, "请求你当前的个人载具。"sv},
		    {"Returns your current Personal Vehicle to storage."sv, "将当前个人载具收回仓库。"sv},
		    {"Lowers your current vehicle's stance. Only some vehicles are supported"sv, "降低当前载具的车身姿态，仅部分载具受支持"sv},
		    {"Allows you to keep your hats or head gear inside vehicles."sv, "允许在载具内保留帽子或头饰。"sv},
		    {"Enables all the DLC vehicles on websites."sv, "在网站中启用所有扩展内容载具。"sv},
		    {"Saves the current vehicle you are in as your personal vehicle."sv, "将你当前乘坐的载具保存为个人载具。"sv},
		    {"Bring All Objects"sv, "传送全部物体到身边"sv},
		    {"Teleports all game objects to you"sv, "将游戏内所有物体传送到你身边"sv},
		    {"Bring All Peds"sv, "传送全部行人到身边"sv},
		    {"Teleports all game peds to you"sv, "将游戏内所有行人传送到你身边"sv},
		    {"Bring All Vehicles"sv, "传送全部载具到身边"sv},
		    {"Teleports all game vehicles to you"sv, "将游戏内所有载具传送到你身边"sv},
		    {"Kill All Peds"sv, "击杀所有行人"sv},
		    {"Kills all peds in the game world"sv, "击杀游戏世界中的所有行人"sv},
		    {"Kill All Enemies"sv, "击杀所有敌人"sv},
		    {"Kills all enemies in the game world"sv, "击杀游戏世界中的所有敌人"sv},
		    {"Delete All Objects"sv, "删除所有物体"sv},
		    {"Deletes all objects in the game world, including mission critical objects"sv, "删除游戏世界中的所有物体，包括任务关键物体"sv},
		    {"Delete All Peds"sv, "删除所有行人"sv},
		    {"Deletes all peds in the game world, including mission critical peds"sv, "删除游戏世界中的所有行人，包括任务关键行人"sv},
		    {"Delete All Vehicles"sv, "删除所有载具"sv},
		    {"Deletes all vehicles in the game world, including mission critical vehicles"sv, "删除游戏世界中的所有载具，包括任务关键载具"sv},
		    {"Nearby peds will ignore you and become oblivious to your actions."sv, "附近行人会无视你，不再对你的行为做出反应。"sv},
		    {"When you are wanted, no police will be spawned."sv, "当你被通缉时，不会再生成警察。"sv},
		    {"Enables the developer mode in Creator."sv, "在编辑器中启用开发者模式。"sv},
		    {"You won't die because of border restrictions."sv, "不会因边界限制而死亡。"sv},
		    {"All PEDs attack each other."sv, "所有行人会互相攻击。"sv},
		    {"Sets the current online time"sv, "设置当前线上时间"sv},
		    {"Freezes the online clock at the selected time"sv, "将线上时间冻结在所选时刻"sv},
		    {"Set hour (0-23)"sv, "设置小时（0-23）"sv},
		    {"Set minute (0-59)"sv, "设置分钟（0-59）"sv},
		    {"Set second (0-59)"sv, "设置秒数（0-59）"sv},
		    {"Weather to set or force. Note that this is local and cannot be seen by other players"sv, "要设置或强制应用的天气。此效果仅本地可见，其他玩家无法看到"sv},
		    {"Sets the specifed weather. Note that this effect is local and cannot be seen by other players"sv, "设置指定天气。该效果仅本地可见，其他玩家无法看到"sv},
		    {"Forces the specified weather. Note that this effect is local and cannot be seen by other players"sv, "强制指定天气。该效果仅本地可见，其他玩家无法看到"sv},
		    {"Spawns an object at your current location."sv, "在当前位置生成一个物体。"sv},
		    {"The model name of the ped you wish to spawn."sv, "要生成的行人模型名称。"sv},
		    {"Object Model"sv, "物体模型"sv},
		    {"The model name of the object you wish to spawn."sv, "要生成的物体模型名称。"sv},
		    {"Brings the player to you"sv, "将玩家传送到你身边"sv},
		    {"Teleports you to the selected player"sv, "将你传送到选中的玩家身边"sv},
		    {"Player Teleport Coords"sv, "玩家传送坐标"sv},
		    {"The coordinates to teleport the player to"sv, "要将玩家传送到的坐标"sv},
		    {"Teleport Player To Coord"sv, "将玩家传送到坐标"sv},
		    {"Teleports the player to the specified coordinates"sv, "将玩家传送到指定坐标"sv},
		    {"Send to Property"sv, "送往资产"sv},
		    {"Sends the player to the selected property"sv, "将玩家送到所选房产"sv},
		    {"Send to Interior"sv, "送往室内地点"sv},
		    {"Sends the player to the selected interior"sv, "将玩家送到所选室内"sv},
		    {"Set Bounty"sv, "设置悬赏"sv},
		    {"Sets the specified bounty to the player"sv, "为玩家设置指定金额的悬赏"sv},
		    {"The bounty amount to set"sv, "要设置的悬赏金额"sv},
		    {"Sets the bounty as anonymous"sv, "将悬赏设置为匿名"sv},
		    {"Deletes the vehicle the player is in"sv, "删除玩家所乘坐的载具"sv},
		    {"Bypasses all casino region restrictions"sv, "绕过所有赌场地区限制"sv},
		    {"Disables death barriers found under the map. This also prevents dying randomly when spectating someone"sv, "禁用地图下方的死亡屏障，同时防止观战时随机死亡"sv},
		    {"Automatically silences all incoming calls"sv, "自动静音所有来电"sv},
		    {"Forces the weather to thunder for all players in the session"sv, "为战局中的所有玩家强制雷暴天气"sv},
		    {"Matchmaking will put you into sessions with other YimMenu users."sv, "匹配系统会将你放入只有其他 YimMenu 用户的战局。"sv},
		    {"Forces you to become freemode script host. Note that this might break some missions."sv, "强制让你成为自由模式脚本主机。注意这可能会导致部分任务异常。"sv},
		    {"Allows you to pause the game in online (local only)."sv, "允许你在线上模式中暂停游戏（仅本地生效）。"sv},
		    {"Spoofs your position to make it appear as if you're next to all the players in the session at all times. Use with caution"sv, "伪装你的位置，让所有玩家看起来你始终在他们身边。请谨慎使用"sv},
		    {"Change the region you matchmake in to the specified value"sv, "将你的匹配地区改为指定值"sv},
		    {"Claims your safe earnings from the selected business."sv, "从所选产业领取保险箱收益。"sv},
		    {"Allows the player to change the character's appearance for free"sv, "允许玩家免费修改角色外观"sv},
		    {"Allows the player to change the character's gender"sv, "允许玩家更改角色性别"sv},
		    {"Removes the cooldown for changing appearance"sv, "移除更改外观的冷却时间"sv},
		    {"Force-unlocks GTA+ content. Use with caution"sv, "强制解锁 GTA+ 内容。请谨慎使用"sv},
		    {"Allows you to play any mission solo. Note that some missions might break if there aren't enough players"sv, "允许你单人游玩任意任务。注意某些任务在人数不足时可能异常"sv},
		    {"Heist difficulty"sv, "抢劫难度"sv},
		    {"Primary target"sv, "主要目标"sv},
		    {"Weapon category"sv, "武器类别"sv},
		    {"Heist Modifier"sv, "抢劫修改"sv},
		    {"Apartment Heist"sv, "公寓抢劫"sv},
		    {"Diamond Casino Heist"sv, "名钻赌场豪劫"sv},
		    {"Cayo Perico Heist"sv, "佩里科岛抢劫"sv},
		    {"Doomsday Heist"sv, "末日豪劫"sv},
		    {"Heist Cuts"sv, "分红比例"sv},
		    {"Heist Setups"sv, "抢劫配置"sv},
		    {"Loots"sv, "战利品"sv},
		    {"Set Cuts"sv, "应用分红"sv},
		    {"Sets heist cut"sv, "应用抢劫分红比例"sv},
		    {"Force Ready"sv, "强制全员准备"sv},
		    {"Forces all players to be ready"sv, "强制所有玩家进入准备状态"sv},
		    {"Heist approach"sv, "抢劫方案"sv},
		    {"Approach"sv, "方案"sv},
		    {"Gunman"sv, "枪手"sv},
		    {"Driver"sv, "司机"sv},
		    {"Hacker"sv, "黑客"sv},
		    {"Skip Hacking"sv, "跳过破解"sv},
		    {"Skips hacking process"sv, "跳过破解流程"sv},
		    {"Skip Drilling"sv, "跳过钻孔"sv},
		    {"Skips drilling process"sv, "跳过钻孔流程"sv},
		    {"Skip Swiping"sv, "跳过刷卡"sv},
		    {"Skips card swiping process"sv, "跳过刷卡流程"sv},
		    {"Instant Finish"sv, "立即完成"sv},
		    {"Instantly passes the heist"sv, "立即通过抢劫任务"sv},
		    {"Instant Finish (Pacific)"sv, "立即完成（太平洋标准银行）"sv},
		    {"Instantly passes Pacific Standard Job"sv, "立即通过太平洋标准银行差事"sv},
		    {"Select Heist"sv, "选择抢劫任务"sv},
		    {"Heist categories"sv, "抢劫分类"sv},
		    {"Sets up current apartment heist"sv, "配置当前公寓抢劫任务"sv},
		    {"Sets up diamond casino heist"sv, "配置名钻赌场豪劫"sv},
		    {"Sets up cayo perico heist"sv, "配置佩里科岛抢劫任务"sv},
		    {"Sets up doomsday heist"sv, "配置末日豪劫"sv},
		    {"Actual Take"sv, "实际收入"sv},
		    {"Updates actual take"sv, "更新实际收入"sv},
		    {"Set Potential Take"sv, "设置预计收入"sv},
		    {"Set Actual Take"sv, "设置实际收入"sv},
		    {"Primary Target Value"sv, "主要目标价值"sv},
		    {"Secondary Take Value"sv, "次要目标收益"sv},
		    {"Updates primary target value"sv, "更新主要目标价值"sv},
		    {"Updates secondary take value"sv, "更新次要目标收益"sv},
		    {"Set Primary Target Value"sv, "设置主要目标价值"sv},
		    {"Set Secondary Take Value"sv, "应用次要目标收益"sv},
		    {"Cut Sewer"sv, "切开排水管"sv},
		    {"Cuts the sewer"sv, "切开排水管"sv},
		    {"Cut Glass"sv, "切开玻璃"sv},
		    {"Cuts the glass"sv, "切开玻璃"sv},
		    {"Take Primary Target"sv, "获取主要目标"sv},
		    {"Takes primary target"sv, "获取主要目标"sv},
		    {"Solo Mantrap"sv, "单人通过安检门"sv},
		    {"The Fleeca Job"sv, "全福银行差事"sv},
		    {"Scope Out"sv, "探查"sv},
		    {"Adder"sv, "灵蛇"sv},
		    {"Akula"sv, "阿库拉"sv},
		    {"Baller"sv, "贝勒尔"sv},
		    {"Baller ST-D"sv, "悠游行者ST-D"sv},
		    {"Banshee"sv, "女妖"sv},
		    {"Brickade"sv, "布里凯德"sv},
		    {"Buffalo"sv, "水牛城"sv},
		    {"Castigator"sv, "惩戒者"sv},
		    {"Champion"sv, "冠军"sv},
		    {"Cinquemila"sv, "五百"sv},
		    {"Deluxo"sv, "德罗索"sv},
		    {"Dominator FX"sv, "公路霸者 FX"sv},
		    {"Draugur"sv, "德劳格"sv},
		    {"Elegy"sv, "骄子"sv},
		    {"Gauntlet Interceptor"sv, "铁腕拦截者"sv},
		    {"Impaler LX"sv, "穿刺者 LX"sv},
		    {"Kuruma"sv, "骷髅马"sv},
		    {"Kosatka"sv, "虎鲸潜艇"sv},
		    {"MonstroCiti"sv, "怪兽城"sv},
		    {"Niobe"sv, "尼俄柏"sv},
		    {"Omnis e-GT"sv, "欧尼斯 e-GT"sv},
		    {"Oppressor"sv, "暴君"sv},
		    {"Paragon"sv, "典范"sv},
		    {"Police Bike"sv, "警用摩托"sv},
		    {"Police Cruiser"sv, "警用巡逻车"sv},
		    {"Police Riot"sv, "警用防暴车"sv},
		    {"Police Transporter"sv, "警用运输车"sv},
		    {"North Yankton"sv, "北扬克顿"sv},
		    {"Dignity Party Yacht"sv, "尊荣号派对游艇"sv},
		    {"Aircraft Carrier"sv, "航空母舰"sv},
		    {"Sunken Cargo Ship"sv, "沉没货船"sv},
		    {"Destroyed Pillbox Hospital"sv, "被摧毁的中心医院"sv},
		    {"O'Neil Farm Destroyed"sv, "被摧毁的奥尼尔农场"sv},
		    {"LifeInvader Interior"sv, "生活入侵者公司内部"sv},
		    {"Jewelry Store Interior"sv, "珠宝店内部"sv},
		    {"Coroner Morgue Interior"sv, "验尸官停尸房内部"sv},
		    {"Prison Break"sv, "越狱"sv},
		    {"Raiju"sv, "雷兽"sv},
		    {"Scramjet"sv, "冲锋者"sv},
		    {"Terminus"sv, "终点"sv},
		    {"Terrorbyte"sv, "恐霸"sv},
		    {"Toreador"sv, "斗牛士"sv},
		    {"Vigilante"sv, "义警"sv},
		    {"Virtue"sv, "美德"sv},
		    {"Humane Raid"sv, "突袭人道研究实验室"sv},
		    {"Series A Funding"sv, "首轮募资"sv},
		    {"Pacific Standard"sv, "太平洋标准银行"sv},
		    {"Pacific Standard Job"sv, "太平洋标准银行差事"sv},
		    {"The Data Breaches"sv, "数据泄露"sv},
		    {"The Bogdan Problem"sv, "波格丹危机"sv},
		    {"The Doomsday Senario"sv, "末日将至"sv},
		    {"Panther Statue"sv, "猎豹雕像"sv},
		    {"Pink Diamond"sv, "粉钻"sv},
		    {"Madrazo Files"sv, "玛德拉索档案"sv},
		    {"Bearer Bonds"sv, "不记名债券"sv},
		    {"Ruby Necklace"sv, "红宝石项链"sv},
		    {"Sinsimito Tequila"sv, "西西米托龙舌兰"sv},
		    {"Spectates the selected player"sv, "观战选中的玩家"sv}
		    ,{"Cage Gun"sv, "笼子枪"sv}
		    ,{"Delete Gun"sv, "删除枪"sv}
		    ,{"Gravity Gun"sv, "重力枪"sv}
		    ,{"Steal Vehicle Gun"sv, "偷车枪"sv}
		    ,{"Repair Gun"sv, "修复枪"sv}
		    ,{"Vehicle Gun"sv, "载具枪"sv}
		    ,{"Teleport Gun"sv, "传送枪"sv}
		    ,{"Paint Gun"sv, "喷漆枪"sv}
		    ,{"Weapon Type"sv, "武器类型"sv}
		    ,{"The type of custom weapon to use"sv, "要使用的自定义武器类型"sv}
		    ,{"Custom Weapon"sv, "自定义武器"sv}
		    ,{"Modifies weapon ability"sv, "修改武器能力"sv}
		    ,{"Enable Only if Weapon is Out"sv, "仅在持枪时启用"sv}
		    ,{"Enables custom weapon only when weapon is out"sv, "仅在武器拿出时启用自定义武器"sv}
		    ,{"Launch Entity on Release"sv, "松手时发射实体"sv}
		    ,{"Launches entity when released"sv, "释放时发射抓取的实体"sv}
		    ,{"Vehicle Model"sv, "载具模型"sv}
		    ,{"Model of the vehicle"sv, "载具的模型名称"sv}
		    ,{"Paint Gun Color"sv, "喷漆枪颜色"sv}
		    ,{"Color of the paint gun"sv, "喷漆枪的颜色"sv}
		    ,{"Rainbow Color"sv, "彩虹颜色"sv}
		    ,{"Enables rainbow color for paint gun"sv, "为喷漆枪启用彩虹颜色"sv}
		    ,{"Rainbow Color Speed"sv, "彩虹变色速度"sv}
		    ,{"Speed of rainbow color for paint gun"sv, "喷漆枪彩虹颜色变化速度"sv}
		    ,{"Rainbow Color Style"sv, "彩虹颜色样式"sv}
		    ,{"Style of rainbow color for paint gun"sv, "喷漆枪彩虹颜色样式"sv}
		    ,{"Fade"sv, "渐变"sv}
		    ,{"Spasm"sv, "闪烁"sv}
		    ,{"Explosion Type"sv, "爆炸类型"sv}
		    ,{"Select an explosion type"sv, "选择一种爆炸类型"sv}
		    ,{"Explosion Damage Scale"sv, "爆炸伤害倍率"sv}
		    ,{"Sets the damage scale for explosions"sv, "设置爆炸伤害倍率"sv}
		    ,{"Explosion Camera Shake"sv, "爆炸镜头震动"sv}
		    ,{"Controls how much the camera shakes during explosions"sv, "控制爆炸时镜头震动幅度"sv}
		    ,{"Don't Care"sv, "无所谓"sv}
		    ,{"Grenade"sv, "手雷"sv}
		    ,{"Grenade Launcher"sv, "榴弹发射器"sv}
		    ,{"Sticky Bomb"sv, "黏弹"sv}
		    ,{"Molotov"sv, "燃烧瓶"sv}
		    ,{"Rocket"sv, "火箭弹"sv}
		    ,{"Tank Shell"sv, "坦克炮弹"sv}
		    ,{"Car"sv, "汽车"sv}
		    ,{"Plane"sv, "飞机"sv}
		    ,{"Boat"sv, "船只"sv}
		    ,{"Truck"sv, "卡车"sv}
		    ,{"Bullet"sv, "子弹"sv}
		    ,{"Train"sv, "列车"sv}
		    ,{"Barrel"sv, "油桶"sv}
		    ,{"Firework"sv, "烟花"sv}
		    ,{"Snowball"sv, "雪球"sv}
		    ,{"Proximity Mine"sv, "感应地雷"sv}
		    ,{"Pipe Bomb"sv, "土制炸弹"sv}
		    ,{"Explosive Ammo (Shotgun)"sv, "爆炸弹药（霰弹枪）"sv}
		    ,{"Oppressor Mk II Cannon"sv, "暴君 Mk II 机炮"sv}
		    ,{"Ray Gun"sv, "电磁枪"sv}
		    ,{"Flash Grenade"sv, "闪光弹"sv}
		    ,{"Stun Grenade"sv, "震撼弹"sv}
		    ,{"EMP Launcher"sv, "电磁脉冲发射器"sv}
		    ,{"Clear Weather"sv, "晴朗"sv}
		    ,{"Extra Sunny"sv, "烈日"sv}
		    ,{"Clouds"sv, "多云"sv}
		    ,{"Overcast"sv, "阴天"sv}
		    ,{"Rain"sv, "下雨"sv}
		    ,{"Clearing"sv, "转晴"sv}
		    ,{"Thunder"sv, "雷暴"sv}
		    ,{"Smog"sv, "雾霾"sv}
		    ,{"Foggy"sv, "大雾"sv}
		    ,{"Xmas"sv, "圣诞"sv}
		    ,{"Snow"sv, "下雪"sv}
		    ,{"Snow Light"sv, "小雪"sv}
		    ,{"Blizzard"sv, "暴风雪"sv}
		    ,{"Halloween"sv, "万圣节"sv}
		    ,{"Neutral"sv, "中性"sv}
		    ,{"Rain Halloween"sv, "万圣节雨天"sv}
		    ,{"Snow Halloween"sv, "万圣节雪天"sv}
		    ,{"Normal"sv, "普通"sv}
		    ,{"Hard"sv, "困难"sv}
		    ,{"Diamonds"sv, "钻石"sv}
		    ,{"Gold"sv, "黄金"sv}
		    ,{"Artwork"sv, "艺术品"sv}
		    ,{"Cash"sv, "现金"sv}
		    ,{"Silent & Sneaky"sv, "隐迹潜踪"sv}
		    ,{"The Big Con"sv, "兵不厌诈"sv}
		    ,{"Aggressive"sv, "气势汹汹"sv}
		    ,{"Remove Gunman"sv, "移除枪手"sv}
		    ,{"Remove Driver"sv, "移除司机"sv}
		    ,{"Remove Hacker"sv, "移除黑客"sv}
		    ,{"Hidden Cache"sv, "隐藏补给箱"sv}
		    ,{"Selected Hidden Cache"sv, "已选隐藏补给箱"sv}
		    ,{"Treasure Chest"sv, "宝箱"sv}
		    ,{"Selected Treasure Chest"sv, "已选宝箱"sv}
		    ,{"Buried Stash"sv, "埋藏战利品"sv}
		    ,{"Selected Buried Stash"sv, "已选埋藏战利品"sv}
		    ,{"Skydive"sv, "高空跳伞"sv}
		    ,{"Selected Skydive"sv, "已选高空跳伞"sv}
		    ,{"Time Trial"sv, "时间挑战赛"sv}
		    ,{"Selected Time Trial"sv, "已选时间挑战赛"sv}
		    ,{"Standard Time Trial"sv, "标准时间挑战赛"sv}
		    ,{"RC Bandito Time Trial"sv, "遥控匪徒时间挑战赛"sv}
		    ,{"Junk Energy Bike Time Trial"sv, "拉机能量自行车时间挑战赛"sv}
		    ,{"Street Dealer"sv, "街头贩子"sv}
		    ,{"Selected Street Dealer"sv, "已选街头贩子"sv}
		    ,{"LS Tag"sv, "洛圣都涂鸦"sv}
		    ,{"Selected LS Tag"sv, "已选洛圣都涂鸦"sv}
		    ,{"Animal"sv, "动物"sv}
		    ,{"Selected Animal"sv, "已选动物"sv}
		    ,{"Product"sv, "货物"sv}
		    ,{"Selected Product"sv, "已选货物"sv}
		    ,{"Set All Activities Completed"sv, "完成全部日常活动"sv}
		    ,{"Reset All Activities"sv, "重置日常活动进度"sv}
		    ,{"Complete All Challenges"sv, "完成每日与每周挑战"sv}
		    ,{"Teleport to Hidden Cache"sv, "传送到隐藏补给箱"sv}
		    ,{"Collect Hidden Cache"sv, "领取隐藏补给箱"sv}
		    ,{"Teleport to Treasure Chest"sv, "传送到宝箱"sv}
		    ,{"Collect Treasure Chest"sv, "领取宝箱"sv}
		    ,{"Enable Treasure Chests in LS"sv, "在洛圣都启用宝箱"sv}
		    ,{"Teleport to Shipwreck"sv, "传送到沉船"sv}
		    ,{"Collect Shipwreck"sv, "领取沉船奖励"sv}
		    ,{"Teleport to Buried Stash"sv, "传送到埋藏战利品"sv}
		    ,{"Collect Buried Stash"sv, "领取埋藏战利品"sv}
		    ,{"Enable Buried Stashes in LS"sv, "在洛圣都启用埋藏战利品"sv}
		    ,{"Teleport to G's Cache"sv, "传送到 G 的藏匿点"sv}
		    ,{"Collect G's Cache"sv, "领取 G 的藏匿点奖励"sv}
		    ,{"Teleport to Stash House"sv, "传送到藏匿屋"sv}
		    ,{"Enter Stash House Safe Code"sv, "输入藏匿屋保险箱密码"sv}
		    ,{"Teleport to Dealer"sv, "传送到街头贩子"sv}
		    ,{"Open Street Dealer Menu"sv, "远程交易"sv}
		    ,{"Teleport to LS Tag"sv, "传送到洛圣都涂鸦"sv}
		    ,{"Spray LS Tag"sv, "喷涂洛圣都涂鸦"sv}
		    ,{"Spawn Animal"sv, "生成动物"sv}
		    ,{"Photograph Animal"sv, "拍摄动物"sv}
		    ,{"Teleport to Product"sv, "传送到货物"sv}
		    ,{"Collect Product"sv, "领取货物"sv}
		    ,{"UI Style"sv, "界面风格"sv}
		    ,{"Choose the UI style"sv, "选择界面风格"sv}
		    ,{"Claim Safe Earnings"sv, "领取产业保险箱收益"sv}
		    ,{"ADD"sv, "添加"sv}
		    ,{"ALL"sv, "全部"sv}
		    ,{"COLOR_OPTIONS"sv, "颜色选项"sv}
		    ,{"GARAGE"sv, "车库"sv}
		    ,{"INTERIOR"sv, "室内"sv}
		    ,{"PLAYER"sv, "玩家"sv}
		    ,{"PLAYERS"sv, "玩家"sv}
		    ,{"REFRESH"sv, "刷新"sv}
		    ,{"REMOVE"sv, "删除"sv}
		    ,{"REMOVE_ALL"sv, "删除全部"sv}
		    ,{"SAVED_VEHICLES"sv, "已保存载具"sv}
		    ,{"SEARCH"sv, "搜索"sv}
		    ,{"SPAWN_IN"sv, "车内生成"sv}
		    ,{"SPAWN_MAXED"sv, "满改生成"sv}
		    ,{"STYLE"sv, "样式"sv}
		    ,{"VEHICLE"sv, "载具"sv}
		    ,{"VEHICLES"sv, "载具"sv}
		    ,{"WHEEL_COLOR"sv, "轮毂颜色"sv}
		    ,{"WHEEL_TYPE"sv, "轮毂类型"sv}
		    ,{"Force-updates all saved players"sv, "强制更新所有已保存玩家的信息"sv}
		    ,{"Set All Activities Completed"sv, "完成全部日常活动"sv}
		    ,{"Reset All Activities"sv, "重置日常活动进度"sv}
		    ,{"Complete All Challenges"sv, "完成每日与每周挑战"sv}
		    ,{"Teleport to Hidden Cache"sv, "传送到隐藏补给箱"sv}
		    ,{"Collect Hidden Cache"sv, "领取隐藏补给箱"sv}
		    ,{"Teleport to Treasure Chest"sv, "传送到宝箱"sv}
		    ,{"Collect Treasure Chest"sv, "领取宝箱"sv}
		    ,{"Enable Treasure Chests in LS"sv, "在洛圣都启用宝箱"sv}
		    ,{"Teleport to Shipwreck"sv, "传送到沉船"sv}
		    ,{"Collect Shipwreck"sv, "领取沉船奖励"sv}
		    ,{"Teleport to Buried Stash"sv, "传送到埋藏战利品"sv}
		    ,{"Collect Buried Stash"sv, "领取埋藏战利品"sv}
		    ,{"Enable Buried Stashes in LS"sv, "在洛圣都启用埋藏战利品"sv}
		    ,{"Teleport to Skydive"sv, "传送到高空跳伞点"sv}
		    ,{"Complete Skydive"sv, "完成高空跳伞"sv}
		    ,{"Teleport to Time Trial"sv, "传送到时间挑战赛"sv}
		    ,{"Beat Time Trial"sv, "完成时间挑战赛"sv}
		    ,{"Teleport to Exotic Exports Vehicle"sv, "传送到出口珍品载具"sv}
		    ,{"Deliver Next Exotic Exports Vehicle"sv, "交付下一辆出口珍品载具"sv}
		    ,{"Teleport to G's Cache"sv, "传送到 G 的藏匿点"sv}
		    ,{"Collect G's Cache"sv, "领取 G 的藏匿点奖励"sv}
		    ,{"Teleport to Stash House"sv, "传送到藏匿屋"sv}
		    ,{"Enter Stash House Safe Code"sv, "输入藏匿屋保险箱密码"sv}
		    ,{"Teleport to Dealer"sv, "传送到街头贩子"sv}
		    ,{"Open Street Dealer Menu"sv, "远程交易"sv}
		    ,{"Teleport to LS Tag"sv, "传送到洛圣都涂鸦"sv}
		    ,{"Spray LS Tag"sv, "喷涂洛圣都涂鸦"sv}
		    ,{"Teleport to Madrazo Hit"sv, "传送到玛德拉索暗杀"sv}
		    ,{"Teleport to Madrazo Hit Target"sv, "传送到玛德拉索暗杀目标"sv}
		    ,{"Spawn Animal"sv, "生成动物"sv}
		    ,{"Photograph Animal"sv, "拍摄动物"sv}
		    ,{"Teleport to Product"sv, "传送到货物"sv}
		    ,{"Collect Product"sv, "领取货物"sv}
		    ,{"Teleport to Golden Clover"sv, "传送到金色四叶草"sv}
		    ,{"Collect Golden Clover"sv, "收集金色四叶草"sv}
		    ,{"Open Gun Locker"sv, "打开武器柜"sv}
		    ,{"Allows you to access gun locker remotely."sv, "允许你远程访问武器柜。"sv}
		    ,{"Wardrobe"sv, "衣柜"sv}
		    ,{"Open Wardrobe"sv, "打开衣柜"sv}
		    ,{"Allows you to access your wardrobe remotely."sv, "允许你远程访问衣柜。"sv}
		    ,{"Call Mechanic"sv, "呼叫技工"sv}
		    ,{"Allows you to request your personal vehicles."sv, "打开技工的个人载具选择界面。"sv}
		    ,{"Fix All Vehicles"sv, "修复全部个人载具"sv}
		    ,{"Fixes all of your destroyed personal vehicles."sv, "修复你所有已损毁的个人载具。"sv}
		    ,{"Join YimMenu-only Sessions"sv, "加入仅限 YimMenu 的战局"sv}
		    ,{"Fast Join"sv, "快速加入"sv}
		    ,{"Spoof Matchmaking Region"sv, "伪装匹配地区"sv}
		    ,{"Overlay Enabled"sv, "启用叠加层"sv}
		    ,{"Overlay Show FPS"sv, "叠加层显示帧率"sv}
		    ,{"Show an info overlay at the top left corner of the screen"sv, "在屏幕左上角显示信息叠加层"sv}
		    ,{"Show frame rate in the info overlay"sv, "在信息叠加层中显示帧率"sv}
		    ,{"Clear Chat"sv, "清空聊天"sv}
		    ,{"Use this to clear the chat"sv, "用于清空聊天内容"sv}
		    ,{"Use this to open the chat"sv, "用于打开聊天窗口"sv}
		    ,{"Pause Game"sv, "暂停游戏"sv}
		    ,{"Allow Gender Change"sv, "允许更改性别"sv}
		    ,{"No Change Appearance Cooldown"sv, "移除外观更改冷却"sv}
		    ,{"Unlock GTA+"sv, "解锁 GTA+"sv}
		    ,{"Weapon Explosion RadiusScale"sv, "武器爆炸范围倍率"sv}
		    ,{"Modifying Weapon Explosion RadiusScale"sv, "修改武器爆炸范围倍率"sv}
		    ,{"RP Multiplier Input"sv, "RP 倍率"sv}
		    ,{"RP Mulitplier (< 0 = 0 -> No RP)"sv, "RP 倍率（小于 0 时按 0 计算，不获得 RP）"sv}
		    ,{"IPL set to load"sv, "要加载的场景集"sv}
		    ,{"Business to claim earning from."sv, "要领取保险箱收益的产业。"sv}
		    ,{"Hands on Car Wash"sv, "亲力亲为洗车行"sv}
		    ,{"Hands On Car Wash"sv, "亲力亲为洗车行"sv}
		    ,{"hands on car wash"sv, "亲力亲为洗车行"sv}
		    ,{"Switch session to apply the changes."sv, "请切换战局以应用更改。"sv}
		    ,{"Completes all the Daily Objectives and the Weekly Challenge."sv, "完成全部每日目标和当前每周挑战。"sv}
		    ,{"Teleports to the selected Hidden Cache."sv, "传送到所选隐藏补给箱。"sv}
		    ,{"Collects the selected Hidden Cache."sv, "收集所选隐藏补给箱。"sv}
		    ,{"Teleports to the selected Treasure Chest."sv, "传送到所选宝箱。"sv}
		    ,{"Collects the selected Treasure Chest."sv, "收集所选宝箱。"sv}
		    ,{"Enables Treasure Chests in Los Santos, so you don't have to go to Cayo Perico."sv, "在洛圣都启用宝箱，这样你就不必前往佩里科岛。"sv}
		    ,{"Teleports to Shipwreck."sv, "传送到沉船位置。"sv}
		    ,{"Collects Shipwreck."sv, "收集沉船奖励。"sv}
		    ,{"Teleports to the selected Buried Stash."sv, "传送到所选埋藏战利品。"sv}
		    ,{"Collects the selected Buried Stash."sv, "收集所选埋藏战利品。"sv}
		    ,{"Enables Buried Stashes in Los Santos, so you don't have to go to Cayo Perico."sv, "在洛圣都启用埋藏战利品，这样你就不必前往佩里科岛。"sv}
		    ,{"Teleports to the selected Skydive."sv, "传送到所选高空跳伞。"sv}
		    ,{"Completes the selected Skydive."sv, "完成所选高空跳伞。"sv}
		    ,{"Teleports to the selected Time Trial."sv, "传送到所选时间挑战赛。"sv}
		    ,{"Completes the selected Time Trial."sv, "完成所选时间挑战赛。"sv}
		    ,{"Teleports to the next Exotic Exports vehicle."sv, "传送到下一辆出口珍品载具。"sv}
		    ,{"Directly delivers the next Exotic Exports vehicle."sv, "直接交付下一辆异域出口载具。"sv}
		    ,{"Teleports to G's Cache."sv, "传送到 G 的藏匿点。"sv}
		    ,{"Collects G's Cache."sv, "领取 G 的藏匿点奖励。"sv}
		    ,{"Teleports to Stash House."sv, "传送到藏匿屋。"sv}
		    ,{"Enters the Stash House safe code."sv, "输入藏匿屋保险箱密码。"sv}
		    ,{"Allows you to access the selected Street Dealer remotely."sv, "远程打开所选街头贩子的交易界面。"sv}
		    ,{"Teleports to the selected LS Tag."sv, "传送到所选洛圣都涂鸦。"sv}
		    ,{"Sprays the selected LS Tag."sv, "喷涂所选洛圣都涂鸦。"sv}
		    ,{"Teleports to Madrazo Hit."sv, "传送到玛德拉索暗杀。"sv}
		    ,{"Teleports to Madrazo Hit Target."sv, "传送到玛德拉索暗杀目标。"sv}
		    ,{"Spawns the selected animal."sv, "生成所选动物。"sv}
		    ,{"Photographs the selected animal."sv, "拍摄所选动物。"sv}
		    ,{"Teleports to the selected product."sv, "传送到所选货物。"sv}
		    ,{"Collects the selected product."sv, "收集所选货物。"sv}
		    ,{"Teleports to the Golden Clover."sv, "传送到金色四叶草。"sv}
		    ,{"Collects the Golden Clover."sv, "收集金色四叶草。"sv}
		    ,{"Bounty Amount"sv, "悬赏金额"sv}
		    ,{"Anonymous Bounty"sv, "匿名悬赏"sv}
		    ,{"Spawn Object"sv, "生成物体"sv}
		    ,{"Spawns a ped at your current location."sv, "在当前位置生成一名行人。"sv}
		    ,{"Spawns an object at your current location."sv, "在当前位置生成一个物体。"sv}
		    ,{"Save Personal Vehicle"sv, "保存个人载具"sv}
		    ,{"Saves the current vehicle you are in as your personal vehicle."sv, "将你当前乘坐的载具保存为个人载具。"sv}
		    ,{"Send to Interior Index"sv, "室内地点"sv}
		    ,{"The interior index to send the player(s) to"sv, "要将玩家送往的室内地点"sv}
		    ,{"Send to Property Index"sv, "资产"sv}
		    ,{"The property index to send the player(s) to"sv, "要将玩家送往的资产"sv}
		    ,{"Player 1"sv, "玩家 1"sv}
		    ,{"Player 2"sv, "玩家 2"sv}
		    ,{"Player 3"sv, "玩家 3"sv}
		    ,{"Player 4"sv, "玩家 4"sv}
		    ,{"Player 1 cut"sv, "玩家 1 分红"sv}
		    ,{"Player 2 cut"sv, "玩家 2 分红"sv}
		    ,{"Player 3 cut"sv, "玩家 3 分红"sv}
		    ,{"Player 4 cut"sv, "玩家 4 分红"sv}
		    ,{"Difficulty"sv, "难度"sv}
		    ,{"Weapon"sv, "武器"sv}
		    ,{"Primary Target"sv, "主要目标"sv}
		    ,{"Potential Take"sv, "预计收入"sv}
		    ,{"Updates potential take"sv, "更新预计收入"sv}
		    ,{"Instant Finish (Act: III)"sv, "立即完成（第三幕）"sv}
		    ,{"Instantly passes Doomsday Scenario Act: III"sv, "立即通过《末日将至》第三幕"sv}
		    ,{"Aggressor"sv, "突击手"sv}
		    ,{"Conspirator"sv, "密谋者"sv}
		    ,{"Crack Shot"sv, "神枪手"sv}
		    ,{"Saboteur"sv, "破坏者"sv}
		    ,{"Marksman"sv, "精准射手"sv}
		    ,{"MK II Rifle Loadout"sv, "Mk II 步枪配置"sv}
		    ,{"MK II SMG Loadout"sv, "Mk II 冲锋枪配置"sv}
		    ,{"MK II Shotgun Loadout"sv, "Mk II 霰弹枪配置"sv}
		    ,{"Combat MG Loadout"sv, "战斗机枪配置"sv}
		    ,{"Combat PDW Loadout"sv, "战斗冲锋手枪配置"sv}
		    ,{"Machine Pistol Loadout"sv, "机器手枪配置"sv}
		    ,{"Micro SMG Loadout"sv, "微型冲锋枪配置"sv}
		    ,{"Revolver Loadout"sv, "左轮手枪配置"sv}
		    ,{"Rifle Loadout"sv, "步枪配置"sv}
		    ,{"SMG Loadout"sv, "冲锋枪配置"sv}
		    ,{"Shotgun Loadout"sv, "霰弹枪配置"sv}
		    ,{"Hidden Caches"sv, "隐藏补给箱"sv}
		    ,{"Buried Stashes"sv, "埋藏战利品"sv}
		    ,{"Junk Energy Skydives"sv, "拉机能量高空跳伞"sv}
		    ,{"Time Trials"sv, "时间挑战赛"sv}
		    ,{"Street Dealers"sv, "街头贩子"sv}
		    ,{"LS Tags"sv, "洛圣都涂鸦"sv}
		    ,{"Madrazo Hits"sv, "玛德拉索暗杀"sv}
		    ,{"Golden Clover"sv, "金色四叶草"sv}
		    ,{"Stash House"sv, "藏匿屋"sv}
		    ,{"Treasure Chests"sv, "宝箱"sv}
		    ,{"Hidden Cache 1"sv, "隐藏补给箱 1"sv}
		    ,{"Hidden Cache 2"sv, "隐藏补给箱 2"sv}
		    ,{"Hidden Cache 3"sv, "隐藏补给箱 3"sv}
		    ,{"Hidden Cache 4"sv, "隐藏补给箱 4"sv}
		    ,{"Hidden Cache 5"sv, "隐藏补给箱 5"sv}
		    ,{"Hidden Cache 6"sv, "隐藏补给箱 6"sv}
		    ,{"Hidden Cache 7"sv, "隐藏补给箱 7"sv}
		    ,{"Hidden Cache 8"sv, "隐藏补给箱 8"sv}
		    ,{"Hidden Cache 9"sv, "隐藏补给箱 9"sv}
		    ,{"Hidden Cache 10"sv, "隐藏补给箱 10"sv}
		    ,{"Treasure Chest 1"sv, "宝箱 1"sv}
		    ,{"Treasure Chest 2"sv, "宝箱 2"sv}
		    ,{"Buried Stash 1"sv, "埋藏战利品 1"sv}
		    ,{"Buried Stash 2"sv, "埋藏战利品 2"sv}
		    ,{"Skydive 1"sv, "高空跳伞 1"sv}
		    ,{"Skydive 2"sv, "高空跳伞 2"sv}
		    ,{"Skydive 3"sv, "高空跳伞 3"sv}
		    ,{"Skydive 4"sv, "高空跳伞 4"sv}
		    ,{"Skydive 5"sv, "高空跳伞 5"sv}
		    ,{"Skydive 6"sv, "高空跳伞 6"sv}
		    ,{"Skydive 7"sv, "高空跳伞 7"sv}
		    ,{"Skydive 8"sv, "高空跳伞 8"sv}
		    ,{"Skydive 9"sv, "高空跳伞 9"sv}
		    ,{"Skydive 10"sv, "高空跳伞 10"sv}
		    ,{"Street Dealer 1"sv, "街头贩子 1"sv}
		    ,{"Street Dealer 2"sv, "街头贩子 2"sv}
		    ,{"Street Dealer 3"sv, "街头贩子 3"sv}
		    ,{"LS Tag 1"sv, "洛圣都涂鸦 1"sv}
		    ,{"LS Tag 2"sv, "洛圣都涂鸦 2"sv}
		    ,{"LS Tag 3"sv, "洛圣都涂鸦 3"sv}
		    ,{"LS Tag 4"sv, "洛圣都涂鸦 4"sv}
		    ,{"LS Tag 5"sv, "洛圣都涂鸦 5"sv}
		    ,{"Animal 1"sv, "动物 1"sv}
		    ,{"Animal 2"sv, "动物 2"sv}
		    ,{"Animal 3"sv, "动物 3"sv}
		    ,{"Product 1"sv, "货物 1"sv}
		    ,{"Product 2"sv, "货物 2"sv}
		    ,{"Product 3"sv, "货物 3"sv}
		    ,{"Product 4"sv, "货物 4"sv}
		    ,{"Product 5"sv, "货物 5"sv}
		    ,{"Product 6"sv, "货物 6"sv}
		    ,{"Product 7"sv, "货物 7"sv}
		    ,{"Product 8"sv, "货物 8"sv}
		    ,{"Product 9"sv, "货物 9"sv}
		    ,{"Product 10"sv, "货物 10"sv}
		    ,{"Auto Teleport to Waypoint"sv, "自动传送到导航点"sv}
		    ,{"Automatically teleports you to the waypoint"sv, "自动将你传送到导航点"sv}
		    ,{"Bypass Casino Restrictions"sv, "绕过赌场限制"sv}
		    ,{"Change Appearance For Free"sv, "免费更改外观"sv}
		    ,{"Delete All Cameras"sv, "删除所有摄像头"sv}
		    ,{"Deletes all cameras"sv, "删除所有摄像头"sv}
		    ,{"Despawn Personal Vehicle"sv, "收回个人载具"sv}
		    ,{"Exotic Exports"sv, "异域出口"sv}
		    ,{"Force Script Host"sv, "强制脚本主机"sv}
		    ,{"Force Thunder"sv, "强制雷暴"sv}
		    ,{"Force Weather"sv, "强制天气"sv}
		    ,{"Give All Weapons"sv, "全部武器"sv}
		    ,{"Gives the player all weapons"sv, "为所选玩家添加全部武器。"sv}
		    ,{"Give Max Ammo"sv, "补满弹药"sv}
		    ,{"Gives you all weapons"sv, "为自己添加全部武器。"sv}
		    ,{"Refills your ammo to max."sv, "将你的弹药补充至上限。"sv}
		    ,{"Load IPL"sv, "加载场景集"sv}
		    ,{"Unload IPL"sv, "卸载场景集"sv}
		    ,{"Teleport to IPL"sv, "传送到场景集"sv}
		    ,{"Teleport to the selected IPL"sv, "传送到所选场景集"sv}
		    ,{"Play All Missions Solo"sv, "允许单人开始任务"sv}
		    ,{"Remove LSC Restrictions"sv, "移除洛圣都改车王限制"sv}
		    ,{"Removes Los Santos Customs vehicle modification restrictions"sv, "移除洛圣都改车王的载具改装限制"sv}
		    ,{"Request Personal Vehicle"sv, "呼叫个人载具"sv}
		    ,{"Save Personal Vehicle"sv, "保存个人载具"sv}
		    ,{"Silence Phone Calls"sv, "静音来电"sv}
		    ,{"Teleport to Waypoint"sv, "传送到导航点"sv}
		    ,{"Teleports you to the waypoint"sv, "将你传送到导航点"sv}
		    ,{"Teleport to Objective"sv, "传送到目标点"sv}
		    ,{"Teleports you to the Objective"sv, "将你传送到目标点"sv}
		    ,{"Take Control"sv, "接管"sv}
		    ,{"Player Name"sv, "玩家名称"sv}
		    ,{"Player Name Color"sv, "玩家名称颜色"sv}
		    ,{"Player Skeleton"sv, "玩家骨骼"sv}
		    ,{"Player Skeleton Color"sv, "玩家骨骼颜色"sv}
		    ,{"Player Distance"sv, "玩家距离"sv}
		    ,{"Player Distance Color"sv, "玩家距离颜色"sv}
		    ,{"Ped Model"sv, "行人模型"sv}
		    ,{"Ped Distance"sv, "行人距离"sv}
		    ,{"Ped Skeleton"sv, "行人骨骼"sv}
		    ,{"Ped Skeleton Color"sv, "行人骨骼颜色"sv}
		    ,{"Object Distance"sv, "物体距离"sv}
		    ,{"Show Player Name"sv, "显示玩家名称"sv}
		    ,{"Show Player Skeleton"sv, "显示玩家骨骼"sv}
		    ,{"Show Player Distance"sv, "显示玩家距离"sv}
		    ,{"Show Ped Model"sv, "显示行人模型"sv}
		    ,{"Show Ped Distance"sv, "显示行人距离"sv}
		    ,{"Show Ped Skeleton"sv, "显示行人骨骼"sv}
		    ,{"Show Object Distance"sv, "显示物体距离"sv}
		    ,{"Should the ESP draw player names?"sv, "透视信息是否显示玩家名称？"sv}
		    ,{"Should the ESP draw player skeletons?"sv, "透视信息是否显示玩家骨骼？"sv}
		    ,{"Should the ESP draw player distance?"sv, "透视信息是否显示玩家距离？"sv}
		    ,{"Should the ESP draw ped models?"sv, "透视信息是否显示行人模型？"sv}
		    ,{"Should the ESP draw distance?"sv, "透视信息是否显示距离？"sv}
		    ,{"Should the ESP draw the skeleton?"sv, "透视信息是否显示骨骼？"sv}
		    ,{"Should the ESP draw special objects?"sv, "透视信息是否显示特殊物体？"sv}
		    ,{"Should the ESP draw dead players?"sv, "透视信息是否显示已死亡玩家？"sv}
		    ,{"Should the ESP draw dead peds?"sv, "透视信息是否显示已死亡行人？"sv}
		    ,{"Draw Players"sv, "显示玩家"sv}
		    ,{"Draw Peds"sv, "显示行人"sv}
		    ,{"Draw Special Objects"sv, "显示特殊物体"sv}
		    ,{"Draw Dead Players"sv, "显示死亡玩家"sv}
		    ,{"Draw Dead Peds"sv, "显示死亡行人"sv}
		    ,{"Allow MP Vehicles in SP"sv, "在单机模式允许多人载具"sv}
		    ,{"Allows MP vehicles to be used in SP"sv, "允许在单机模式中使用多人模式载具"sv}
		    ,{"Delete Player Vehicles"sv, "删除玩家载具"sv}
		    ,{"Delete Vehicle"sv, "删除载具"sv}
		    ,{"Deliver Next Exotic Exports Vehicle"sv, "交付下一辆出口珍品载具"sv}
		    ,{"Directly delivers the next Exotic Exports vehicle."sv, "直接交付下一辆异域出口载具。"sv}
		    ,{"Enter Message"sv, "输入消息"sv}
		    ,{"Failed to call the mechanic."sv, "呼叫技工失败。"sv}
		    ,{"Failed to open the wardrobe."sv, "打开衣柜失败。"sv}
		    ,{"GivePlayerAllWeapon"sv, "给予玩家全部武器"sv}
		    ,{"Give All Weapon the player"sv, "给予该玩家全部武器"sv}
		    ,{"Invalid model name provided."sv, "提供的模型名称无效。"sv}
		    ,{"Invalid vehicle model name provided."sv, "提供的载具模型名称无效。"sv}
		    ,{"Instant recharge"sv, "瞬间充能"sv}
		    ,{"Keep Fixed"sv, "保持修复状态"sv}
		    ,{"Lower Stance"sv, "降低姿态"sv}
		    ,{"Lower Vehicle Stance"sv, "降低载具姿态"sv}
		    ,{"No model name provided."sv, "未提供模型名称。"sv}
		    ,{"No vehicle model name provided."sv, "未提供载具模型名称。"sv}
		    ,{"No vehicles to fix."sv, "没有可修复的载具。"sv}
		    ,{"Not safe to call the mechanic at the moment."sv, "当前不适合呼叫技工。"sv}
		    ,{"Not safe to open the wardrobe at the moment."sv, "当前不适合打开衣柜。"sv}
		    ,{"Plate Style"sv, "车牌样式"sv}
		    ,{"Spawn Object"sv, "生成物体"sv}
		    ,{"Spawns a ped at your current location."sv, "在当前位置生成一名行人。"sv}
		    ,{"Wheel Type"sv, "轮毂类型"sv}
		    ,{"Window Padding"sv, "窗口内边距"sv}
		    ,{"Item Spacing"sv, "项目间距"sv}
		    ,{"Item Inner Spacing"sv, "项目内部间距"sv}
		    ,{"Touch Extra Padding"sv, "触控额外边距"sv}
		    ,{"Display Safe Area Padding"sv, "显示安全区边距"sv}
		    ,{"Indent Spacing"sv, "缩进间距"sv}
		    ,{"Columns Min Spacing"sv, "列最小间距"sv}
		    ,{"Window Title Align"sv, "窗口标题对齐"sv}
		    ,{"Button Text Align"sv, "按钮文字对齐"sv}
		    ,{"Selectable Text Align"sv, "可选项文字对齐"sv}
		    ,{"Window Border Size"sv, "窗口边框大小"sv}
		    ,{"Frame Border Size"sv, "框架边框大小"sv}
		    ,{"Tab Border Size"sv, "标签边框大小"sv}
		    ,{"Popup Border Size"sv, "弹出窗口边框大小"sv}
		    ,{"Global Alpha"sv, "全局透明度"sv}
		    ,{"Disabled Alpha"sv, "禁用透明度"sv}
		    ,{"Mouse Cursor Scale"sv, "鼠标光标缩放"sv}
		    ,{"Curve Tessellation Tol"sv, "曲线细分容差"sv}
		    ,{"Imani Tech"sv, "伊玛尼科技"sv}
		    ,{"Window Rounding"sv, "窗口圆角"sv}
		    ,{"Frame Rounding"sv, "框架圆角"sv}
		    ,{"Grab Rounding"sv, "抓取圆角"sv}
		    ,{"Scrollbar Rounding"sv, "滚动条圆角"sv}
		    ,{"Child Rounding"sv, "子窗口圆角"sv}
		    ,{"Popup Rounding"sv, "弹出窗口圆角"sv}
		    ,{"Tab Rounding"sv, "标签圆角"sv}
		    ,{"WindowRounding"sv, "窗口圆角"sv}
		    ,{"FrameRounding"sv, "框架圆角"sv}
		    ,{"GrabRounding"sv, "抓取圆角"sv}
		    ,{"ScrollbarRounding"sv, "滚动条圆角"sv}
		    ,{"ChildRounding"sv, "子窗口圆角"sv}
		    ,{"PopupRounding"sv, "弹出窗口圆角"sv}
		    ,{"TabRounding"sv, "标签圆角"sv}
		    ,{"WindowPadding"sv, "窗口内边距"sv}
		    ,{"ItemSpacing"sv, "项目间距"sv}
		    ,{"ItemInnerSpacing"sv, "项目内部间距"sv}
		    ,{"TouchExtraPadding"sv, "触控额外边距"sv}
		    ,{"DisplaySafeAreaPadding"sv, "显示安全区边距"sv}
		    ,{"IndentSpacing"sv, "缩进间距"sv}
		    ,{"ColumnsMinSpacing"sv, "列最小间距"sv}
		    ,{"WindowTitleAlign"sv, "窗口标题对齐"sv}
		    ,{"ButtonTextAlign"sv, "按钮文本对齐"sv}
		    ,{"CellPadding"sv, "单元格边距"sv}
		    ,{"CircleTessellationMaxError"sv, "圆形细分最大误差"sv}
		    ,{"DisplayWindowPadding"sv, "显示窗口边距"sv}
		    ,{"SelectableTextAlign"sv, "可选项文本对齐"sv}
		    ,{"SeparatorTextAlign"sv, "分隔符文字对齐"sv}
		    ,{"GrabMinSize"sv, "滑块最小尺寸"sv}
		    ,{"ScrollbarSize"sv, "滚动条大小"sv}
		    ,{"WindowMinSize"sv, "窗口最小尺寸"sv}
		    ,{"WindowBorderSize"sv, "窗口边框大小"sv}
		    ,{"FrameBorderSize"sv, "框架边框大小"sv}
		    ,{"TabBorderSize"sv, "标签边框大小"sv}
		    ,{"PopupBorderSize"sv, "弹出窗口边框大小"sv}
		    ,{"GlobalAlpha"sv, "全局透明度"sv}
		    ,{"DisabledAlpha"sv, "禁用透明度"sv}
		    ,{"MouseCursorScale"sv, "鼠标光标缩放"sv}
		    ,{"CurveTessellationTol"sv, "曲线细分容差"sv}
		    ,{"Window Tint"sv, "车窗贴膜"sv}
		    ,{"Window"sv, "窗口"sv}
		    ,{"Frame"sv, "框架"sv}
		    ,{"Grab"sv, "抓取"sv}
		    ,{"X"sv, "X"sv}
		    ,{"Y"sv, "Y"sv}
		    ,{"Z"sv, "Z"sv}
		    ,{"X Coordinate"sv, "X 坐标"sv}
		    ,{"Y Coordinate"sv, "Y 坐标"sv}
		    ,{"Z Coordinate"sv, "Z 坐标"sv}
		    ,{"Scrollbar"sv, "滚动条"sv}
		    ,{"Child"sv, "子窗口"sv}
		    ,{"Popup"sv, "弹出窗口"sv}
		    ,{"Tab"sv, "标签"sv}
		    ,{"Padding"sv, "边距"sv}
		    ,{"Item"sv, "物品"sv}
		    ,{"Inner"sv, "内部"sv}
		    ,{"Touch"sv, "触控"sv}
		    ,{"Extra"sv, "额外"sv}
		    ,{"Display"sv, "显示"sv}
		    ,{"Safe"sv, "安全"sv}
		    ,{"Area"sv, "区域"sv}
		    ,{"Indent"sv, "缩进"sv}
		    ,{"Columns"sv, "列"sv}
		    ,{"Min"sv, "最小"sv}
		    ,{"Title"sv, "标题"sv}
		    ,{"Align"sv, "对齐"sv}
		    ,{"Button"sv, "按钮"sv}
		    ,{"Text"sv, "文本"sv}
		    ,{"Selectable"sv, "可选项"sv}
		    ,{"Size"sv, "大小"sv}
		    ,{"Alpha"sv, "透明度"sv}
		    ,{"Disabled"sv, "禁用"sv}
		    ,{"Mouse"sv, "鼠标"sv}
		    ,{"Cursor"sv, "光标"sv}
		    ,{"Scale"sv, "缩放"sv}
		    ,{"Curve"sv, "曲线"sv}
		    ,{"Tessellation"sv, "细分"sv}
		    ,{"Tol"sv, "容差"sv}
		    ,{"Bg"sv, "背景"sv}
		    ,{"Hovered"sv, "悬停"sv}
		    ,{"Active"sv, "启用中"sv}
		    ,{"Collapsed"sv, "折叠"sv}
		    ,{"Shadow"sv, "阴影"sv}
		    ,{"Check"sv, "勾选"sv}
		    ,{"Mark"sv, "标记"sv}
		    ,{"Slider"sv, "滑块"sv}
		    ,{"Header"sv, "标题栏"sv}
		    ,{"Separator"sv, "分隔线"sv}
		    ,{"Resize"sv, "调整大小"sv}
		    ,{"Grip"sv, "抓手"sv}
		    ,{"Plot"sv, "图表"sv}
		    ,{"Lines"sv, "线条"sv}
		    ,{"Histogram"sv, "直方图"sv}
		    ,{"Table"sv, "表格"sv}
		    ,{"Row"sv, "行"sv}
		    ,{"Alt"sv, "交替"sv}
		    ,{"Drag"sv, "拖拽"sv}
		    ,{"Drop"sv, "放置"sv}
		    ,{"Target"sv, "目标"sv}
		    ,{"Nav"sv, "导航"sv}
		    ,{"Highlight"sv, "高亮"sv}
		    ,{"Windowing"sv, "窗口切换"sv}
		    ,{"Modal"sv, "模态"sv}
		    ,{"Dim"sv, "变暗"sv}
		    ,{"Menu"sv, "菜单"sv}
		    ,{"Bar"sv, "栏"sv}
		    ,{"Selected"sv, "已选中"sv}
		    ,{"Preview"sv, "预览"sv}
		    ,{"Docking"sv, "停靠"sv}
		    ,{"Unfocused"sv, "未聚焦"sv}
		    ,{"Text Disabled"sv, "禁用文本"sv}
		    ,{"Window Bg"sv, "窗口背景"sv}
		    ,{"Child Bg"sv, "子窗口背景"sv}
		    ,{"Popup Bg"sv, "弹出窗口背景"sv}
		    ,{"Border Shadow"sv, "边框阴影"sv}
		    ,{"Frame Bg"sv, "框架背景"sv}
		    ,{"Frame Bg Hovered"sv, "框架背景（悬停）"sv}
		    ,{"Frame Bg Active"sv, "框架背景（启用中）"sv}
		    ,{"Title Bg"sv, "标题背景"sv}
		    ,{"Title Bg Active"sv, "标题背景（启用中）"sv}
		    ,{"Title Bg Collapsed"sv, "标题背景（折叠）"sv}
		    ,{"Menu Bar Bg"sv, "菜单栏背景"sv}
		    ,{"Scrollbar Bg"sv, "滚动条背景"sv}
		    ,{"Scrollbar Grab"sv, "滚动条抓手"sv}
		    ,{"Scrollbar Grab Hovered"sv, "滚动条抓手（悬停）"sv}
		    ,{"Scrollbar Grab Active"sv, "滚动条抓手（启用中）"sv}
		    ,{"Check Mark"sv, "勾选标记"sv}
		    ,{"Slider Grab"sv, "滑块抓手"sv}
		    ,{"Slider Grab Active"sv, "滑块抓手（启用中）"sv}
		    ,{"Button Hovered"sv, "按钮（悬停）"sv}
		    ,{"Button Active"sv, "按钮（启用中）"sv}
		    ,{"Header Hovered"sv, "标题栏（悬停）"sv}
		    ,{"Header Active"sv, "标题栏（启用中）"sv}
		    ,{"Separator Hovered"sv, "分隔线（悬停）"sv}
		    ,{"Separator Active"sv, "分隔线（启用中）"sv}
		    ,{"Resize Grip"sv, "调整大小抓手"sv}
		    ,{"Resize Grip Hovered"sv, "调整大小抓手（悬停）"sv}
		    ,{"Resize Grip Active"sv, "调整大小抓手（启用中）"sv}
		    ,{"Tab Hovered"sv, "标签（悬停）"sv}
		    ,{"Tab Active"sv, "标签（启用中）"sv}
		    ,{"Tab Unfocused"sv, "标签（未聚焦）"sv}
		    ,{"Tab Unfocused Active"sv, "标签（未聚焦且启用中）"sv}
		    ,{"Docking Preview"sv, "停靠预览"sv}
		    ,{"Docking Empty Bg"sv, "停靠空白背景"sv}
		    ,{"Plot Lines"sv, "图表线条"sv}
		    ,{"Plot Lines Hovered"sv, "图表线条（悬停）"sv}
		    ,{"Plot Histogram"sv, "图表直方图"sv}
		    ,{"Plot Histogram Hovered"sv, "图表直方图（悬停）"sv}
		    ,{"Table Header Bg"sv, "表格标题背景"sv}
		    ,{"Table Border Strong"sv, "表格边框（强）"sv}
		    ,{"Table Border Light"sv, "表格边框（浅）"sv}
		    ,{"Table Row Bg"sv, "表格行背景"sv}
		    ,{"Table Row Bg Alt"sv, "表格交替行背景"sv}
		    ,{"Text Selected Bg"sv, "选中文本背景"sv}
		    ,{"Drag Drop Target"sv, "拖放目标"sv}
		    ,{"Nav Highlight"sv, "导航高亮"sv}
		    ,{"Nav Windowing Highlight"sv, "导航窗口切换高亮"sv}
		    ,{"Nav Windowing Dim Bg"sv, "导航窗口切换变暗背景"sv}
		    ,{"Modal Window Dim Bg"sv, "模态窗口变暗背景"sv}
		    ,{"Horizontal"sv, "水平"sv}
		    ,{"Vertical"sv, "垂直"sv}
		    ,{"Agency"sv, "事务所"sv}
		    ,{"Arcade"sv, "游戏厅"sv}
		    ,{"Accessories"sv, "配件"sv}
		    ,{"Active"sv, "启用中"sv}
		    ,{"Air Defence"sv, "防空系统"sv}
		    ,{"Amount"sv, "数量"sv}
		    ,{"Anonymous"sv, "匿名"sv}
		    ,{"Armor"sv, "防弹衣"sv}
		    ,{"Bags"sv, "背包"sv}
		    ,{"Bike"sv, "摩托车"sv}
		    ,{"Blackjack"sv, "二十一点"sv}
		    ,{"Blimp"sv, "飞艇"sv}
		    ,{"Boost behavior"sv, "推进器行为"sv}
		    ,{"Bring"sv, "传送到身边"sv}
		    ,{"Bring All"sv, "全部传送到身边"sv}
		    ,{"CEO Kick All"sv, "踢出所有首席执行官"sv}
		    ,{"CEO Kick"sv, "首席执行官踢出"sv}
		    ,{"Challenges"sv, "挑战"sv}
		    ,{"Cleanup"sv, "清理"sv}
		    ,{"Components"sv, "服装部件"sv}
		    ,{"Undershirt"sv, "内搭"sv}
		    ,{"Legs"sv, "裤装"sv}
		    ,{"Feet"sv, "鞋子"sv}
		    ,{"MISSING_LABEL"sv, "缺少标签"sv}
		    ,{"Mask"sv, "面具"sv}
		    ,{"Gloves"sv, "手套"sv}
		    ,{"Hats"sv, "帽子"sv}
		    ,{"Glasses"sv, "眼镜"sv}
		    ,{"Ears"sv, "耳饰"sv}
		    ,{"Watches"sv, "手表"sv}
		    ,{"Cops Dispatch"sv, "警察调度"sv}
		    ,{"Decals"sv, "贴花"sv}
		    ,{"Default"sv, "默认"sv}
		    ,{"Idle"sv, "空闲"sv}
		    ,{"Running"sv, "运行中"sv}
		    ,{"Killed"sv, "已终止"sv}
		    ,{"Paused"sv, "已暂停"sv}
		    ,{"Unk4"sv, "未知 4"sv}
		    ,{"Micro"sv, "微型"sv}
		    ,{"Mini"sv, "迷你"sv}
		    ,{"Friend"sv, "好友"sv}
		    ,{"Shop"sv, "商店"sv}
		    ,{"Cellphone"sv, "手机"sv}
		    ,{"Vehicle Spawn"sv, "载具生成"sv}
		    ,{"Car Mod Shop"sv, "改车店"sv}
		    ,{"Pause Menu Script"sv, "暂停菜单脚本"sv}
		    ,{"App Internet"sv, "网络应用"sv}
		    ,{"Multiplayer Mission"sv, "多人任务"sv}
		    ,{"Contacts App"sv, "联系人应用"sv}
		    ,{"Interaction Menu"sv, "互动菜单"sv}
		    ,{"Property Int"sv, "房产业务"sv}
		    ,{"Activity Creator Int"sv, "活动创建器业务"sv}
		    ,{"Simple Interior"sv, "简易室内场景"sv}
		    ,{"Warehouse"sv, "仓库"sv}
		    ,{"IE Delivery"sv, "进出口交付"sv}
		    ,{"Shop Controller"sv, "商店控制器"sv}
		    ,{"AM MP Yacht"sv, "线上豪华游艇活动"sv}
		    ,{"Ingame HUD"sv, "游戏内抬头显示"sv}
		    ,{"Transition"sv, "过渡流程"sv}
		    ,{"FMMC Launcher"sv, "在线战局启动器"sv}
		    ,{"Multiplayer Freemode"sv, "多人自由模式"sv}
		    ,{"MP Launch Script"sv, "线上启动脚本"sv}
		    ,{"IMPORTANT! PLEASE READ!"sv, "重要！请务必阅读！"sv}
		    ,{"Play with YimMenu users"sv, "与 YimMenu 用户一起游玩"sv}
		    ,{"Play with everyone (Broken!)"sv, "与所有人一起游玩（已失效）"sv}
		    ,{"Open GitHub"sv, "打开 GitHub 仓库"sv}
		    ,{"Open Matrix server"sv, "打开 Matrix 社区"sv}
		    ,{"Skipped phone call from character {}"sv, "已跳过角色来电：{}"sv}
		    ,{"You are solely responsible for what you do with this tool. If you don't know what you're doing, you'll likely get banned"sv, "你需自行承担使用此工具的后果。如果不了解自己在做什么，很可能会被封禁"sv}
		    ,{"Allow Hats In Vehicles"sv, "允许在载具内佩戴帽子"sv}
		    ,{"Allows altering your melee damage output"sv, "允许调整你的近战伤害输出"sv}
		    ,{"Allows you to access gun locker remotely."sv, "允许你远程访问武器柜。"sv}
		    ,{"Allows you to access the selected Street Dealer remotely."sv, "远程打开所选街头贩子的交易界面。"sv}
		    ,{"Allows you to access your wardrobe remotely."sv, "允许你远程访问衣柜。"sv}
		    ,{"Allows you to join players with rpf mods (or a half installed game)."sv, "允许你加入安装了资源模组（或游戏安装不完整）的玩家。"sv}
		    ,{"Allows you to listen to any radio station on foot with Hotkey Q"sv, "允许你在步行时通过 Q 键收听任意电台"sv}
		    ,{"Attempts to improve join speed by loading host broadcast variables asynchronously. This may cause certain jobs to break, use with caution"sv, "尝试通过异步加载主机广播变量来提升加入速度。这可能导致某些任务异常，请谨慎使用"sv}
		    ,{"Beat Time Trial"sv, "完成时间挑战赛"sv}
		    ,{"Blows the player up"sv, "将玩家炸飞"sv}
		    ,{"Cannot open gun locker while in an interior."sv, "在室内时无法打开武器柜。"sv}
		    ,{"Changes the color of the distance ESP for players"sv, "修改玩家距离透视信息的颜色"sv}
		    ,{"Changes the color of the hash ESP for objects"sv, "修改物体哈希透视信息的颜色"sv}
		    ,{"Changes the color of the hash ESP for peds"sv, "修改行人哈希透视信息的颜色"sv}
		    ,{"Changes the color of the name ESP for players"sv, "修改玩家名称透视信息的颜色"sv}
		    ,{"Changes the color of the skeleton ESP for peds"sv, "修改行人骨骼透视信息的颜色"sv}
		    ,{"Changes the color of the skeleton ESP for players"sv, "修改玩家骨骼透视信息的颜色"sv}
		    ,{"Collects G's Cache."sv, "领取 G 的藏匿点奖励。"sv}
		    ,{"Collects Shipwreck."sv, "收集沉船奖励。"sv}
		    ,{"Collects the Golden Clover."sv, "收集金色四叶草。"sv}
		    ,{"Collects the selected Buried Stash."sv, "收集所选埋藏战利品。"sv}
		    ,{"Collects the selected Hidden Cache."sv, "收集所选隐藏补给箱。"sv}
		    ,{"Collects the selected Treasure Chest."sv, "收集所选宝箱。"sv}
		    ,{"Collects the selected product."sv, "收集所选货物。"sv}
		    ,{"Completed all activities, switch session to apply the changes."sv, "已完成所有活动，请切换战局以应用更改。"sv}
		    ,{"Completes all the Daily Objectives and the Weekly Challenge."sv, "完成全部每日目标和当前每周挑战。"sv}
		    ,{"Completes the selected Skydive."sv, "完成所选高空跳伞。"sv}
		    ,{"Completes the selected Time Trial."sv, "完成所选时间挑战赛。"sv}
		    ,{"Desired rocket boost behavior"sv, "期望的火箭推进器行为"sv}
		    ,{"Detaches your camera and allows you to go anywhere!"sv, "分离你的镜头，让你可以自由前往任何地方！"sv}
		    ,{"Dumps the current data hash into the console"sv, "将当前数据哈希输出到控制台"sv}
		    ,{"Enables Buried Stashes in Los Santos, so you don't have to go to Cayo Perico."sv, "在洛圣都启用埋藏战利品，这样你就不必前往佩里科岛。"sv}
		    ,{"Enables Treasure Chests in Los Santos, so you don't have to go to Cayo Perico."sv, "在洛圣都启用宝箱，这样你就不必前往佩里科岛。"sv}
		    ,{"Enters the Stash House safe code."sv, "输入藏匿屋保险箱密码。"sv}
		    ,{"Entity is not a ped."sv, "该实体不是行人。"sv}
		    ,{"Entity is too far."sv, "该实体距离过远。"sv}
		    ,{"Event is already active."sv, "事件已激活。"sv}
		    ,{"Event is not active."sv, "事件未激活。"sv}
		    ,{"Event script is not active. Are you a participant?"sv, "事件脚本未激活。你是参与者吗？"sv}
		    ,{"Failed to get RID from username"sv, "无法通过用户名获取 R星 ID"sv}
		    ,{"Failed to spawn personal vehicle."sv, "生成个人载具失败。"sv}
		    ,{"Failed to launch event. Are you freemode host?"sv, "启动事件失败。你是自由模式主机吗？"sv}
		    ,{"Failed to teleport to event. Coordinates are not valid."sv, "传送到事件失败，坐标无效。"sv}
		    ,{"Formats HUD money with commas."sv, "将界面中的金额按千位分隔显示。"sv}
		    ,{"Forces player into permadeath that can only be fixed by a game restart"sv, "强制玩家进入永久死亡状态，只能通过重启游戏恢复"sv}
		    ,{"Forces the player into an infinite loading screen that can only be fixed by restarting the game"sv, "强制玩家进入无限加载界面，只能通过重启游戏恢复"sv}
		    ,{"Gravity gun cannot be applied on player entity."sv, "重力枪不能作用于玩家实体。"sv}
		    ,{"Have an endless clip in your gun"sv, "让当前武器弹匣永不耗尽"sv}
		    ,{"Hold the button with the command name and enter a keystroke to change its hotkey"sv, "按住命令名按钮并输入按键即可修改热键"sv}
		    ,{"If a command has an existing hotkey, clicking the button will remove it"sv, "如果命令已有热键，点击该按钮会移除它"sv}
		    ,{"Jump higher than normal"sv, "跳得比正常更高"sv}
		    ,{"Joiner"sv, "加入器"sv}
		    ,{"Lua Scripting"sv, "Lua 脚本"sv}
		    ,{"Failed to join session"sv, "加入战局失败"sv}
		    ,{"An existing transition is in progress"sv, "当前已有过渡流程正在进行"sv}
		    ,{"Failed to get session info by RID"sv, "无法通过 R星 ID 获取战局信息"sv}
		    ,{"Script {} has been unloaded due to a malfunction. Check the console for more details"sv, "脚本 {} 因发生故障已被卸载。请查看控制台了解更多详情"sv}
		    ,{"Keeps the player clean from any blood, dirt, water."sv, "让玩家始终保持干净，不沾血迹、污渍和水渍。"sv}
		    ,{"Kicks the player by faking a vote kick"sv, "通过伪造投票踢出玩家"sv}
		    ,{"Kicks the player by forcing them to call a null script function"sv, "通过强制调用空脚本函数踢出玩家"sv}
		    ,{"Kicks the player by forcing them to end the freemode session"sv, "通过强制结束自由模式战局踢出玩家"sv}
		    ,{"Kicks the player from their organization or motorcycle club"sv, "将玩家踢出其组织或摩托帮"sv}
		    ,{"Kills the player"sv, "击杀该玩家"sv}
		    ,{"Lets you win the Rig Slot Machines every time"sv, "让你每次都能稳赢老虎机"sv}
		    ,{"Manipulate Rig Slot Machines"sv, "老虎机必胜"sv}
		    ,{"Modify boost behavior"sv, "修改推进器行为"sv}
		    ,{"Modify rocket boost behavior for vehicles that support it"sv, "修改支持该功能的载具的火箭推进器行为"sv}
		    ,{"Multiplies RP by the given value"sv, "按给定数值倍增声望"sv}
		    ,{"Only available as host and the target is unblockable"sv, "仅主机可用，且目标无法阻挡"sv}
		    ,{"Photographs the selected animal."sv, "拍摄所选动物。"sv}
		    ,{"Please enable the blip from the Interaction Menu."sv, "请先在互动菜单中启用该地图标记。"sv}
		    ,{"Please get in a vehicle."sv, "请先进入一辆载具。"sv}
		    ,{"Please join GTA Online."sv, "请先进入 GTA 在线模式。"sv}
		    ,{"Click to load this script"sv, "点击以加载该脚本"sv}
		    ,{"File name cannot be empty!"sv, "文件名不能为空！"sv}
		    ,{"Prevent you from getting Critical Hits"sv, "防止你受到暴击"sv}
		    ,{"Prevents you from being kicked while idling."sv, "防止你因挂机而被踢出。"sv}
		    ,{"Refresh List"sv, "刷新列表"sv}
		    ,{"Reset all activities, switch session to apply the changes."sv, "已重置所有活动，请切换战局以应用更改。"sv}
		    ,{"Riot Mode"sv, "暴动模式"sv}
		    ,{"Script does not exist."sv, "脚本不存在。"sv}
		    ,{"Script is already running."sv, "脚本已在运行。"sv}
		    ,{"Send All to Interior"sv, "将所有人送到室内"sv}
		    ,{"Send All to Property"sv, "将所有人送到房产"sv}
		    ,{"Send Gooch"sv, "发送古奇"sv}
		    ,{"Send Phantom Car"sv, "发送幻影车"sv}
		    ,{"Send Sext"sv, "发送短信"sv}
		    ,{"Send Sexts"sv, "群发短信"sv}
		    ,{"Sends a random sext to the specified player"sv, "向指定玩家发送一条随机短信"sv}
		    ,{"Sends the Gooch to the specified player."sv, "向指定玩家发送古奇。"sv}
		    ,{"Sends the Phantom Car to the specified player."sv, "向指定玩家发送幻影车。"sv}
		    ,{"Session Type"sv, "战局类型"sv}
		    ,{"Set Bounties"sv, "设置悬赏"sv}
		    ,{"Set Time"sv, "设置时间"sv}
		    ,{"Set Weather"sv, "设置天气"sv}
		    ,{"Sets the damage scale for melee"sv, "设置近战伤害倍率"sv}
		    ,{"Ship Destroy"sv, "摧毁船只"sv}
		    ,{"Shoot Animals Photography"sv, "野生动物摄影"sv}
		    ,{"Should the ESP draw network info?"sv, "透视信息是否显示网络信息？"sv}
		    ,{"Should the ESP draw peds?"sv, "透视信息是否显示行人？"sv}
		    ,{"Should the ESP draw players?"sv, "透视信息是否显示玩家？"sv}
		    ,{"Should the ESP draw script info?"sv, "透视信息是否显示脚本信息？"sv}
		    ,{"Show Object Network Info"sv, "显示物体网络信息"sv}
		    ,{"Show Object Script Info"sv, "显示物体脚本信息"sv}
		    ,{"Show Ped Network Info"sv, "显示行人的网络信息"sv}
		    ,{"Show Ped Script Info"sv, "显示行人的脚本信息"sv}
		    ,{"Smoke on the Water"sv, "喷云吐雾馆"sv}
		    ,{"Higgins Helitours"sv, "希金斯直升机旅行社"sv}
		    ,{"Spams the killfeed of all players with fake kill entries. Use with caution"sv, "向所有玩家的击杀提示刷入伪造击杀记录。请谨慎使用"sv}
		    ,{"Spawn Personal Vehicle"sv, "生成个人载具"sv}
		    ,{"Spawn a clone of the persone vehicle."sv, "生成个人载具的复制品。"sv}
		    ,{"Spawn inside the personal vehicle."sv, "在个人载具内生成。"sv}
		    ,{"Spawn inside the vehicle."sv, "在载具内生成。"sv}
		    ,{"Spawn the vehicle maxed."sv, "以满改状态生成载具。"sv}
		    ,{"Spawns the selected animal."sv, "生成所选动物。"sv}
		    ,{"Stand On Vehicles"sv, "站在载具上"sv}
		    ,{"Style {}"sv, "样式 {}"sv}
		    ,{"Stock"sv, "原厂"sv}
		    ,{"Switch session to apply the changes."sv, "请切换战局以应用更改。"sv}
		    ,{"Teleport Everyone"sv, "传送所有人"sv}
		    ,{"Teleport to Exotic Exports Vehicle"sv, "传送到出口珍品载具"sv}
		    ,{"Teleport to Golden Clover"sv, "传送到金色四叶草"sv}
		    ,{"Teleport to Madrazo Hit"sv, "传送到玛德拉索暗杀"sv}
		    ,{"Teleport to Madrazo Hit Target"sv, "传送到玛德拉索暗杀目标"sv}
		    ,{"Teleport to Player"sv, "传送到玩家"sv}
		    ,{"Teleport to Skydive"sv, "传送到高空跳伞点"sv}
		    ,{"Teleport to Time Trial"sv, "传送到时间挑战赛"sv}
		    ,{"Teleports to G's Cache."sv, "传送到 G 的藏匿点。"sv}
		    ,{"Teleports to Madrazo Hit Target."sv, "传送到玛德拉索暗杀目标。"sv}
		    ,{"Teleports to Madrazo Hit."sv, "传送到玛德拉索暗杀。"sv}
		    ,{"Teleports to Shipwreck."sv, "传送到沉船位置。"sv}
		    ,{"Teleports to Stash House."sv, "传送到藏匿屋。"sv}
		    ,{"Teleports to the Golden Clover."sv, "传送到金色四叶草。"sv}
		    ,{"Teleports to the next Exotic Exports vehicle."sv, "传送到下一辆出口珍品载具。"sv}
		    ,{"Teleports to the selected Buried Stash."sv, "传送到所选埋藏战利品。"sv}
		    ,{"Teleports to the selected Hidden Cache."sv, "传送到所选隐藏补给箱。"sv}
		    ,{"Teleports to the selected LS Tag."sv, "传送到所选洛圣都涂鸦。"sv}
		    ,{"Teleports to the selected Skydive."sv, "传送到所选高空跳伞。"sv}
		    ,{"Teleports to the selected Street Dealer."sv, "传送到所选街头贩子。"sv}
		    ,{"Teleports to the selected Time Trial."sv, "传送到所选时间挑战赛。"sv}
		    ,{"Teleports to the selected Treasure Chest."sv, "传送到所选宝箱。"sv}
		    ,{"Teleports to the selected product."sv, "传送到所选货物。"sv}
		    ,{"The transaction isn't valid. Ensure that all fields are filled out correctly"sv, "该交易无效。请确认所有字段都已正确填写"sv}
		    ,{"This Buried Stash has already been collected."sv, "该埋藏战利品已被收集。"sv}
		    ,{"This Hidden Cache has already been collected."sv, "该隐藏补给箱已被收集。"sv}
		    ,{"This LS Tag has already been sprayed."sv, "该洛圣都涂鸦已被喷涂。"sv}
		    ,{"This Time Trial has already been beaten."sv, "该时间挑战赛已被完成。"sv}
		    ,{"This Treasure Chest has already been collected."sv, "该宝箱已被收集。"sv}
		    ,{"This animal has already been photographed."sv, "该动物已被拍摄。"sv}
		    ,{"This item has been blocked for your safety"sv, "出于安全考虑，该项目已被屏蔽"sv}
		    ,{"Item not found!"sv, "未找到该项目！"sv}
		    ,{"Item category {} does not match txn category {}"sv, "项目分类 {} 与交易分类 {} 不匹配"sv}
		    ,{"Item price exceeds maximum allowed ({})"sv, "项目价格超过允许的最大值（{}）"sv}
		    ,{"Failed to create basket"sv, "创建购物篮失败"sv}
		    ,{"Failed to add {} (x{}) to basket"sv, "无法将 {}（x{}）加入购物篮"sv}
		    ,{"Failed to create service"sv, "创建服务失败"sv}
		    ,{"Failed to begin checkout"sv, "开始结账失败"sv}
		    ,{"This product has already been collected."sv, "该货物已被收集。"sv}
		    ,{"This stat should not be edited by the client. Right-click to force the write anyway"sv, "该属性不应由客户端修改。右键仍可强制写入"sv}
		    ,{"This vehicle cannot be saved as a personal vehicle."sv, "该载具无法保存为个人载具。"sv}
		    ,{"This vehicle is already a personal vehicle."sv, "该载具已经是个人载具。"sv}
		    ,{"Transaction complete"sv, "交易完成"sv}
		    ,{"Transaction failed"sv, "交易失败"sv}
		    ,{"Wardrobe"sv, "衣柜"sv}
		    ,{"You have already delivered all the vehicles."sv, "你已经交付了所有载具。"sv}
		    ,{"You have just delivered a vehicle, wait a moment."sv, "你刚刚交付过一辆载具，请稍等片刻。"sv}
		    ,{"You must own a Bail Office."sv, "你必须拥有一间保释事务所。"sv}
		    ,{"You must own a Smoke on the Water property."sv, "你必须拥有“喷云吐雾馆”产业。"sv}
		    ,{"Acid Lab"sv, "酸液实验室"sv}
		    ,{"Agency (Hawick)"sv, "事务所（霍伊克）"sv}
		    ,{"Agency (Little Seoul)"sv, "事务所（小首尔）"sv}
		    ,{"Agency (Rockford)"sv, "事务所（罗克福德）"sv}
		    ,{"Agency (Vespucci)"sv, "事务所（维斯普奇）"sv}
		    ,{"Arcade (Davis)"sv, "游戏厅（戴维斯）"sv}
		    ,{"Arcade (Grapeseed)"sv, "游戏厅（葡萄籽）"sv}
		    ,{"Arcade (La Mesa)"sv, "游戏厅（拉梅萨）"sv}
		    ,{"Arcade (Paleto Bay)"sv, "游戏厅（佩立托湾）"sv}
		    ,{"Arcade (Rockford Hills)"sv, "游戏厅（罗克福德山）"sv}
		    ,{"Arcade (West Vinewood)"sv, "游戏厅（西葡萄园）"sv}
		    ,{"Arcadius Business Center"sv, "阿卡迪厄斯商务中心"sv}
		    ,{"Auto Shop (Burton)"sv, "改装铺（伯顿）"sv}
		    ,{"Auto Shop (La Mesa)"sv, "改装铺（拉梅萨）"sv}
		    ,{"Auto Shop (Mission Row)"sv, "改装铺（使命街）"sv}
		    ,{"Auto Shop (Rancho)"sv, "改装铺（兰乔）"sv}
		    ,{"Auto Shop (Strawberry)"sv, "改装铺（草莓）"sv}
		    ,{"Bail Office"sv, "保释事务所"sv}
		    ,{"Bail Office (Davis)"sv, "保释事务所（戴维斯）"sv}
		    ,{"Bail Office (Del Perro)"sv, "保释事务所（德佩罗）"sv}
		    ,{"Bail Office (Mission Row)"sv, "保释事务所（使命街）"sv}
		    ,{"Bail Office (Paleto Bay)"sv, "保释事务所（佩立托湾）"sv}
		    ,{"Bail Office (West Vinewood)"sv, "保释事务所（西葡萄园）"sv}
		    ,{"Car Meet"sv, "车友会"sv}
		    ,{"Car Wash"sv, "亲力亲为洗车行"sv}
		    ,{"Casino Penthouse"sv, "赌场顶层豪宅"sv}
		    ,{"Del Perro Beach Clubhouse"sv, "德佩罗海滩会所"sv}
		    ,{"Downtown Vinewood Clubhouse"sv, "市中心葡萄园会所"sv}
		    ,{"Field Hangar"sv, "机场机库"sv}
		    ,{"Freakhouse"sv, "怪胎屋"sv}
		    ,{"Garage Innocence Blvd"sv, "因诺森斯大道车库"sv}
		    ,{"Garment Factory"sv, "制衣厂"sv}
		    ,{"Grapeseed Clubhouse"sv, "葡萄籽会所"sv}
		    ,{"Great Chaparral Clubhouse"sv, "大灌木丛会所"sv}
		    ,{"Hawick Clubhouse"sv, "霍伊克会所"sv}
		    ,{"Heli Tours"sv, "希金斯直升机旅行社"sv}
		    ,{"La Mesa Clubhouse"sv, "拉梅萨会所"sv}
		    ,{"Lombank West"sv, "隆班克西部"sv}
		    ,{"Maze Bank Tower"sv, "花园银行大楼"sv}
		    ,{"Maze Bank West"sv, "花园银行西部"sv}
		    ,{"Music Locker"sv, "音乐柜"sv}
		    ,{"Office Garage 1"sv, "办公室车库 1"sv}
		    ,{"Office Garage 2"sv, "办公室车库 2"sv}
		    ,{"Office Garage 3"sv, "办公室车库 3"sv}
		    ,{"Paleto Bay Clubhouse"sv, "佩立托湾会所"sv}
		    ,{"Private Yacht"sv, "私人游艇"sv}
		    ,{"Rancho Clubhouse"sv, "兰乔会所"sv}
		    ,{"Salvage Yard"sv, "赃车店"sv}
		    ,{"Salvage Yard (La Puerta)"sv, "赃车店（拉普尔塔）"sv}
		    ,{"Salvage Yard (Murietta Heights)"sv, "赃车店（穆列塔高地）"sv}
		    ,{"Salvage Yard (Paleto Bay)"sv, "赃车店（佩立托湾）"sv}
		    ,{"Salvage Yard (Sandy Shores)"sv, "赃车店（沙滩海岸）"sv}
		    ,{"Salvage Yard (Strawberry)"sv, "赃车店（草莓）"sv}
		    ,{"Sandy Shores Clubhouse"sv, "沙滩海岸会所"sv}
		    ,{"Vehicle Warehouse"sv, "载具仓库"sv}
		    ,{"Vespucci Beach Clubhouse"sv, "维斯普奇海滩会所"sv}
		    ,{"Weed Shop"sv, "喷云吐雾馆"sv}
		    ,{"Hour"sv, "小时"sv}
		    ,{"Minute"sv, "分钟"sv}
		    ,{"Second"sv, "秒"sv}
		    ,{"Font Scale"sv, "字体缩放"sv}
		    ,{"Fonts"sv, "字体"sv}
		    ,{"Script"sv, "脚本"sv}
		    ,{"Scripts"sv, "脚本"sv}
		    ,{"Select"sv, "选择"sv}
		    ,{"Empty"sv, "空"sv}
		    ,{"None"sv, "无"sv}
		    ,{"(Select)"sv, "（选择）"sv}
		    ,{"MOC"sv, "机动作战中心"sv}
		    ,{"Top"sv, "上衣"sv}
		    ,{"Mechanic"sv, "技工"sv}
		    ,{"Nightclub"sv, "夜总会"sv}
		    ,{"Editor"sv, "编辑器"sv}
		    ,{"Features"sv, "功能"sv}
		    ,{"State"sv, "状态"sv}
		    ,{"Thread"sv, "线程"sv}
		    ,{"Threads"sv, "线程"sv}
		    ,{"True"sv, "真"sv}
		    ,{"False"sv, "假"sv}
		    ,{"Explode"sv, "引爆"sv}
		    ,{"Explode All"sv, "引爆全部"sv}
		    ,{"Jump to Offset"sv, "跳转到偏移"sv}
		    ,{"Press ENTER to write."sv, "按回车写入。"sv}
		    ,{"Exit Reason: %s"sv, "退出原因：%s"sv}
		    ,{"Kill"sv, "终止"sv}
		    ,{"Log Labels"sv, "记录标签"sv}
		    ,{"Host: %s"sv, "主机：%s"sv}
		    ,{"Take Control"sv, "接管"sv}
		    ,{"Thread ID: %d"sv, "线程编号：%d"sv}
		    ,{"Stack Size: %d"sv, "栈大小：%d"sv}
		    ,{"Stack Pointer: 0x%X"sv, "栈指针：0x%X"sv}
		    ,{"Program Counter: 0x%X"sv, "程序计数器：0x%X"sv}
		    ,{"Code Size: %d"sv, "代码大小：%d"sv}
		    ,{"Arg Count: %d"sv, "参数数量：%d"sv}
		    ,{"Local Count: %d"sv, "局部变量数量：%d"sv}
		    ,{"Global Count: %d"sv, "全局变量数量：%d"sv}
		    ,{"Native Count: %d"sv, "原生函数数量：%d"sv}
		    ,{"String Count: %d"sv, "字符串数量：%d"sv}
		    ,{"Script Name"sv, "脚本名称"sv}
		    ,{"Stack Size"sv, "栈大小"sv}
		    ,{"MICRO"sv, "微型"sv}
		    ,{"MINI"sv, "迷你"sv}
		    ,{"DEFAULT"sv, "默认"sv}
		    ,{"SPECIAL_ABILITY"sv, "特殊能力"sv}
		    ,{"FRIEND"sv, "好友"sv}
		    ,{"SHOP"sv, "商店"sv}
		    ,{"CELLPHONE"sv, "手机"sv}
		    ,{"VEHICLE_SPAWN"sv, "载具生成"sv}
		    ,{"CAR_MOD_SHOP"sv, "改车店"sv}
		    ,{"PAUSE_MENU_SCRIPT"sv, "暂停菜单脚本"sv}
		    ,{"APP_INTERNET"sv, "互联网应用"sv}
		    ,{"MULTIPLAYER_MISSION"sv, "多人任务"sv}
		    ,{"CONTACTS_APP"sv, "联系人应用"sv}
		    ,{"INTERACTION_MENU"sv, "互动菜单"sv}
		    ,{"SCRIPT_XML"sv, "脚本 XML 配置"sv}
		    ,{"PROPERTY_INT"sv, "房产室内"sv}
		    ,{"ACTIVITY_CREATOR_INT"sv, "活动创建器室内"sv}
		    ,{"SMPL_INTERIOR"sv, "简单室内"sv}
		    ,{"WAREHOUSE"sv, "仓库"sv}
		    ,{"IE_DELIVERY"sv, "载具出口运送"sv}
		    ,{"SHOP_CONTROLLER"sv, "商店控制器"sv}
		    ,{"AM_MP_YACHT"sv, "线上游艇"sv}
		    ,{"INGAMEHUD"sv, "游戏内界面"sv}
		    ,{"TRANSITION"sv, "过渡"sv}
		    ,{"FMMC_LAUNCHER"sv, "自由模式启动器"sv}
		    ,{"MULTIPLAYER_FREEMODE"sv, "线上自由模式"sv}
		    ,{"MISSION"sv, "任务"sv}
		    ,{"MP_LAUNCH_SCRIPT"sv, "线上启动脚本"sv}
		    ,{"Arg Count"sv, "参数数量"sv}
		    ,{"Arg[%d]"sv, "参数[%d]"sv}
		    ,{"Pause After Starting"sv, "启动后暂停"sv}
		    ,{"Start Script"sv, "启动脚本"sv}
		    ,{"Started script with ID {}."sv, "已启动脚本，编号为 {}。"sv}
		    ,{"Start Session Script"sv, "启动战局脚本"sv}
		    ,{"Start Script With Event"sv, "通过事件启动脚本"sv}
		    ,{"No free stack of this size."sv, "没有这个大小的空闲栈。"sv}
		    ,{"Add Offset"sv, "添加偏移"sv}
		    ,{"Add Read Player Id"sv, "添加读取玩家编号"sv}
		    ,{"Remove Offset"sv, "移除偏移"sv}
		    ,{"Freeze"sv, "冻结"sv}
		    ,{"Seatbelt"sv, "安全带"sv}
		    ,{"Spectate"sv, "观战"sv}
		    ,{"Blimp 2"sv, "飞艇 2"sv}
		    ,{"APC Shell"sv, "装甲运兵车炮弹"sv}
		    ,{"BZ Gas"sv, "BZ 毒气"sv}
		    ,{"Bird Crap"sv, "鸟粪"sv}
		    ,{"Blackscreen"sv, "黑屏"sv}
		    ,{"Bombushka Cannon"sv, "邦布什卡机炮"sv}
		    ,{"Buried Mine"sv, "埋设地雷"sv}
		    ,{"Cluster Bomb"sv, "集束炸弹"sv}
		    ,{"Diamond"sv, "钻石"sv}
		    ,{"Directional Flame"sv, "定向火焰"sv}
		    ,{"Directional Flame Explode"sv, "定向火焰爆炸"sv}
		    ,{"Directional Gas Canister"sv, "定向气罐"sv}
		    ,{"Directional Steam"sv, "定向蒸汽"sv}
		    ,{"Directional Water Hydrant"sv, "定向消防栓水柱"sv}
		    ,{"Extinguisher"sv, "灭火器"sv}
		    ,{"Flare"sv, "信号弹"sv}
		    ,{"Gas Bomb"sv, "毒气炸弹"sv}
		    ,{"Gas Canister"sv, "气罐"sv}
		    ,{"Gas Tank"sv, "油箱"sv}
		    ,{"Hi Octane"sv, "高辛烷值燃料"sv}
		    ,{"Hunter Barrage"sv, "猎手齐射"sv}
		    ,{"Hunter Cannon"sv, "猎手机炮"sv}
		    ,{"Incendiary Bomb"sv, "燃烧弹"sv}
		    ,{"Kinetic Mortar"sv, "动能迫击炮"sv}
		    ,{"Orbital Cannon"sv, "轨道炮"sv}
		    ,{"Petrol Pump"sv, "汽油泵"sv}
		    ,{"Plane Rocket"sv, "飞机火箭弹"sv}
		    ,{"Programmable AR"sv, "可编程步枪"sv}
		    ,{"Propane"sv, "丙烷"sv}
		    ,{"RC Tank Rocket"sv, "遥控坦克火箭弹"sv}
		    ,{"Railgun"sv, "电磁炮"sv}
		    ,{"Rogue Cannon"sv, "叛逆者机炮"sv}
		    ,{"Script Drone"sv, "脚本无人机"sv}
		    ,{"Script Missile"sv, "脚本导弹"sv}
		    ,{"Script Missile (Large)"sv, "脚本导弹（大型）"sv}
		    ,{"Slipstream"sv, "尾流"sv}
		    ,{"Smoke Grenade"sv, "烟雾弹"sv}
		    ,{"Smoke Grenade Launcher"sv, "烟雾弹发射器"sv}
		    ,{"Standard Bomb"sv, "标准炸弹"sv}
		    ,{"Standard Bomb (Wide)"sv, "标准炸弹（广域）"sv}
		    ,{"Submarine (Big)"sv, "潜艇（大型）"sv}
		    ,{"Tanker"sv, "油罐车"sv}
		    ,{"Torpedo"sv, "鱼雷"sv}
		    ,{"Trevor Rage"sv, "崔佛狂怒"sv}
		    ,{"Underwater Mine"sv, "水下地雷"sv}
		    ,{"Underwater Torpedo"sv, "水下鱼雷"sv}
		    ,{"Valkyrie Cannon"sv, "女武神机炮"sv}
		    ,{"Water Bomb"sv, "水弹"sv}
		    ,{"Water Bomb (Secondary)"sv, "水弹（次级）"sv}
		    ,{"Unknown Explosion F728C4A9"sv, "未知爆炸类型（F728C4A9）"sv}
		    ,{"Unknown Explosion BAEC056F"sv, "未知爆炸类型（BAEC056F）"sv}
		    ,{"Unknown Explosion 763D3B3B"sv, "未知爆炸类型（763D3B3B）"sv}
		    ,{"No saved players"sv, "没有已保存的玩家"sv}
		    ,{"Select Event"sv, "选择事件"sv}
		    ,{"Select Location"sv, "选择地点"sv}
		    ,{"Host: %s"sv, "主机：%s"sv}
		    ,{"Player ped not found"sv, "未找到玩家角色实体"sv}
		    ,{"Natives not cached yet"sv, "原生函数尚未缓存"sv}
		    ,{"Props"sv, "配饰"sv}
		    ,{"Drawable"sv, "模型"sv}
		    ,{"Texture"sv, "贴图"sv}
		    ,{"Freemode script host is required."sv, "需要自由模式脚本主机权限。"sv}
		    ,{"Inactive - launching in "sv, "未激活，启动倒计时："sv}
		    ,{"Available - deactivating in "sv, "可用，失效倒计时："sv}
		    ,{"Drug Vehicle"sv, "毒品载具"sv}
		    ,{"Movie Props"sv, "电影道具"sv}
		    ,{"Sleeping Guard"sv, "沉睡守卫"sv}
		    ,{"The Slashers"sv, "连环杀手"sv}
		    ,{"Phantom Car"sv, "幻影车"sv}
		    ,{"Sightseeing"sv, "观光之旅"sv}
		    ,{"Smuggler Trail"sv, "走私者踪迹"sv}
		    ,{"Cerberus Surprise"sv, "地狱犬惊魂"sv}
		    ,{"Smuggler Plane"sv, "走私者飞机"sv}
		    ,{"Crime Scene"sv, "犯罪现场"sv}
		    ,{"Metal Detector"sv, "金属探测"sv}
		    ,{"Finders Keepers"sv, "先到先得"sv}
		    ,{"Shop Robbery"sv, "商店抢劫"sv}
		    ,{"The Gooch"sv, "古奇"sv}
		    ,{"Weazel Plaza Shootout"sv, "威索广场枪战"sv}
		    ,{"Armored Truck"sv, "运钞车"sv}
		    ,{"Possessed Animals"sv, "附身动物"sv}
		    ,{"Ghosts Exposed"sv, "幽灵现形"sv}
		    ,{"Happy Holidays Hauler"sv, "节日欢乐卡车"sv}
		    ,{"Community Outreach"sv, "社区外展"sv}
		    ,{"Getaway Driver"sv, "逃逸司机"sv}
		    ,{"Stoner Survival"sv, "瘾君子生存战"sv}
		    ,{"Valentine Cheater"sv, "情人节负心汉"sv}
		    ,{"Triggerer"sv, "触发器"sv}
		    ,{"Enable All DLC Vehicles"sv, "启用全部扩展内容载具"sv}
		    ,{"Enable Creator Dev Mode"sv, "启用创建器开发模式"sv}
		    ,{"Freeze Time"sv, "冻结时间"sv}
		    ,{"Harass Players"sv, "骚扰玩家"sv}
		    ,{"Lucky Wheel"sv, "幸运转盘"sv}
		    ,{"Kill All"sv, "击杀所有人"sv}
		    ,{"Level"sv, "等级"sv}
		    ,{"Null Function Kick"sv, "空函数踢出"sv}
		    ,{"Host Kick"sv, "主机踢出"sv}
		    ,{"Host Kick All"sv, "踢出所有玩家（主机）"sv}
		    ,{"Script Host Kick"sv, "脚本主机踢出"sv}
		    ,{"Freemode State Kick"sv, "自由模式状态踢出"sv}
		    ,{"Permadeath"sv, "永久死亡"sv}
		    ,{"Permadeath All"sv, "全员永久死亡"sv}
		    ,{"G's Cache"sv, "G 的藏匿点"sv}
		    ,{"Shipwreck"sv, "沉船"sv}
		    ,{"Collect Golden Clover"sv, "收集金色四叶草"sv}
		    ,{"Complete Skydive"sv, "完成高空跳伞"sv}
		    ,{"Madrazo Hit has already been completed."sv, "玛德拉索暗杀已完成。"sv}
		    ,{"G's Cache has already been collected."sv, "G 的藏匿点已被收集。"sv}
		    ,{"Golden Clover has already been collected."sv, "金色四叶草已被收集。"sv}
		    ,{"Shipwreck has already been collected."sv, "沉船奖励已被收集。"sv}
		    ,{"Object Hash Color"sv, "物体哈希颜色"sv}
		    ,{"Object Net Info"sv, "物体网络信息"sv}
		    ,{"Object Script Info"sv, "物体脚本信息"sv}
		    ,{"Ped Hash Color"sv, "行人哈希颜色"sv}
		    ,{"Ped Hashes"sv, "行人哈希值"sv}
		    ,{"Ped Net Info"sv, "行人网络信息"sv}
		    ,{"Ped Script Info"sv, "行人脚本信息"sv}
		    ,{"Peds Ignore"sv, "行人无视玩家"sv}
		    ,{"Player List"sv, "玩家列表"sv}
		    ,{"Populate Name"sv, "自动填入名称"sv}
		    ,{"Service"sv, "服务"sv}
		    ,{"Spoof Region"sv, "伪装地区"sv}
		    ,{"Secondary Item"sv, "次要物品"sv}
		    ,{"N/A"sv, "无"sv}
		    ,{"Actual Take"sv, "实际收入"sv}
		    ,{"Animal"sv, "动物"sv}
		    ,{"Animal 1"sv, "动物 1"sv}
		    ,{"Animal 2"sv, "动物 2"sv}
		    ,{"Animal 3"sv, "动物 3"sv}
		    ,{"Buried Stash"sv, "埋藏战利品"sv}
		    ,{"Buried Stash 1"sv, "埋藏战利品 1"sv}
		    ,{"Buried Stash 2"sv, "埋藏战利品 2"sv}
		    ,{"CEO Kick All"sv, "踢出所有首席执行官"sv}
		    ,{"Cluster Bomb (Secondary)"sv, "集束炸弹（副）"sv}
		    ,{"Difficulty"sv, "难度"sv}
		    ,{"Disable Death Barriers"sv, "禁用死亡屏障"sv}
		    ,{"Dump Data Hash"sv, "导出数据哈希"sv}
		    ,{"DLC Hash: "sv, "扩展内容哈希："}
		    ,{"ExplosiveAmmo"sv, "爆炸弹药"sv}
		    ,{"Hacker"sv, "黑客"sv}
		    ,{"Heist categories"sv, "抢劫分类"sv}
		    ,{"Heist difficulty"sv, "抢劫难度"sv}
		    ,{"Hidden Cache"sv, "隐藏补给箱"sv}
		    ,{"Hidden Cache 1"sv, "隐藏补给箱 1"sv}
		    ,{"Hidden Cache 2"sv, "隐藏补给箱 2"sv}
		    ,{"Hidden Cache 3"sv, "隐藏补给箱 3"sv}
		    ,{"Hidden Cache 4"sv, "隐藏补给箱 4"sv}
		    ,{"Hidden Cache 5"sv, "隐藏补给箱 5"sv}
		    ,{"Hidden Cache 6"sv, "隐藏补给箱 6"sv}
		    ,{"Hidden Cache 7"sv, "隐藏补给箱 7"sv}
		    ,{"Hidden Cache 8"sv, "隐藏补给箱 8"sv}
		    ,{"Hidden Cache 9"sv, "隐藏补给箱 9"sv}
		    ,{"Hidden Cache 10"sv, "隐藏补给箱 10"sv}
		    ,{"Host Kick All"sv, "踢出所有玩家（主机）"sv}
		    ,{"Kill All"sv, "击杀所有人"sv}
		    ,{"Level"sv, "等级"sv}
		    ,{"Melee Damage"sv, "近战伤害"sv}
		    ,{"Melee Damage Scale"sv, "近战伤害倍率"sv}
		    ,{"Player 1"sv, "玩家 1"sv}
		    ,{"Player 2"sv, "玩家 2"sv}
		    ,{"Player 3"sv, "玩家 3"sv}
		    ,{"Player 4"sv, "玩家 4"sv}
		    ,{"Player 1 cut"sv, "玩家 1 分红"sv}
		    ,{"Player 2 cut"sv, "玩家 2 分红"sv}
		    ,{"Player 3 cut"sv, "玩家 3 分红"sv}
		    ,{"Player 4 cut"sv, "玩家 4 分红"sv}
		    ,{"Potential Take"sv, "预计收入"sv}
		    ,{"Primary Target"sv, "主要目标"sv}
		    ,{"Primary Target Value"sv, "主要目标价值"sv}
		    ,{"Primary target"sv, "主要目标"sv}
		    ,{"Product"sv, "货物"sv}
		    ,{"Product 1"sv, "货物 1"sv}
		    ,{"Product 2"sv, "货物 2"sv}
		    ,{"Product 3"sv, "货物 3"sv}
		    ,{"Product 4"sv, "货物 4"sv}
		    ,{"Product 5"sv, "货物 5"sv}
		    ,{"Product 6"sv, "货物 6"sv}
		    ,{"Product 7"sv, "货物 7"sv}
		    ,{"Product 8"sv, "货物 8"sv}
		    ,{"Product 9"sv, "货物 9"sv}
		    ,{"Product 10"sv, "货物 10"sv}
		    ,{"RC Bandito Time Trial"sv, "遥控匪徒时间挑战赛"sv}
		    ,{"Release Dead Target"sv, "目标死亡后解除锁定"sv}
		    ,{"Releases the lock-on to the target after they are dead"sv, "目标死亡后解除锁定"sv}
		    ,{"Secondary Take Value"sv, "次要目标收益"sv}
		    ,{"Selected Animal"sv, "已选动物"sv}
		    ,{"Selected Buried Stash"sv, "已选埋藏战利品"sv}
		    ,{"Selected Hidden Cache"sv, "已选隐藏补给箱"sv}
		    ,{"Selected LS Tag"sv, "已选洛圣都涂鸦"sv}
		    ,{"Selected Product"sv, "已选货物"sv}
		    ,{"Selected Skydive"sv, "已选高空跳伞"sv}
		    ,{"Selected Street Dealer"sv, "已选街头贩子"sv}
		    ,{"Selected Time Trial"sv, "已选时间挑战赛"sv}
		    ,{"Selected Treasure Chest"sv, "已选宝箱"sv}
		    ,{"Select Heist"sv, "选择抢劫任务"sv}
		    ,{"Skydive"sv, "高空跳伞"sv}
		    ,{"Skydive 1"sv, "高空跳伞 1"sv}
		    ,{"Skydive 2"sv, "高空跳伞 2"sv}
		    ,{"Skydive 3"sv, "高空跳伞 3"sv}
		    ,{"Skydive 4"sv, "高空跳伞 4"sv}
		    ,{"Skydive 5"sv, "高空跳伞 5"sv}
		    ,{"Skydive 6"sv, "高空跳伞 6"sv}
		    ,{"Skydive 7"sv, "高空跳伞 7"sv}
		    ,{"Skydive 8"sv, "高空跳伞 8"sv}
		    ,{"Skydive 9"sv, "高空跳伞 9"sv}
		    ,{"Skydive 10"sv, "高空跳伞 10"sv}
		    ,{"Spam Killfeed"sv, "刷屏击杀提示"sv}
		    ,{"Spoof Data Hash"sv, "伪装数据哈希"sv}
		    ,{"Standard Time Trial"sv, "标准时间挑战赛"sv}
		    ,{"Street Dealer"sv, "街头贩子"sv}
		    ,{"Street Dealer 1"sv, "街头贩子 1"sv}
		    ,{"Street Dealer 2"sv, "街头贩子 2"sv}
		    ,{"Street Dealer 3"sv, "街头贩子 3"sv}
		    ,{"Time Trial"sv, "时间挑战赛"sv}
		    ,{"Treasure Chest"sv, "宝箱"sv}
		    ,{"Treasure Chest 1"sv, "宝箱 1"sv}
		    ,{"Treasure Chest 2"sv, "宝箱 2"sv}
		    ,{"Infinite Boundary"sv, "无限边界"sv}
		    ,{"Infinite Clip"sv, "无限弹匣"sv}
		    ,{"Override RP Multiplier"sv, "自定义 RP 倍率"sv}
		    ,{"Deadeye"sv, "死亡之眼"sv}
		    ,{"Snapshot (Aim at head)"sv, "快照（瞄准头部）"sv}
		    ,{"Insult"sv, "辱骂"sv}
		    ,{"Updates actual take"sv, "更新实际收入"sv}
		    ,{"Updates potential take"sv, "更新预计收入"sv}
		    ,{"Updates primary target value"sv, "更新主要目标价值"sv}
		    ,{"Updates secondary take value"sv, "更新次要目标收益"sv}
		    ,{"Vehicle Bullet"sv, "载具子弹"sv}
		    ,{"Vehicle Mine"sv, "载具地雷"sv}
		    ,{"Vehicle Mine (EMP)"sv, "载具地雷（电磁脉冲）"sv}
		    ,{"Vehicle Mine (Kinetic)"sv, "载具地雷（动能）"sv}
		    ,{"Vehicle Mine (Slick)"sv, "载具地雷（滑油）"sv}
		    ,{"Vehicle Mine (Spike)"sv, "载具地雷（尖刺）"sv}
		    ,{"Vehicle Mine (Tar)"sv, "载具地雷（焦油）"sv}
		    ,{"Weapon"sv, "武器"sv}
		    ,{"Weapon category"sv, "武器类别"sv}
		    ,{"Weapon Explosion Radius"sv, "武器爆炸半径"sv}
		    ,{"Modifying Weapon Explosion Radius"sv, "调整武器爆炸半径"sv}
		    ,{"Normal"sv, "普通"sv}
		    ,{"Hard"sv, "困难"sv}
		    ,{"Diamonds"sv, "钻石"sv}
		    ,{"Gold"sv, "黄金"sv}
		    ,{"Artwork"sv, "艺术品"sv}
		    ,{"Cash"sv, "现金"sv}
		    ,{"Gunman"sv, "枪手"sv}
		    ,{"Driver"sv, "司机"sv}
		    ,{"Approach"sv, "方案"sv}
		    ,{"Heist approach"sv, "抢劫方案"sv}
		    ,{"Remove Gunman"sv, "移除枪手"sv}
		    ,{"Remove Driver"sv, "移除司机"sv}
		    ,{"Remove Hacker"sv, "移除黑客"sv}
		    ,{"MK II Shotgun Loadout"sv, "Mk II 霰弹枪配置"sv}
		    ,{"MK II Rifle Loadout"sv, "Mk II 步枪配置"sv}
		    ,{"MK II SMG Loadout"sv, "Mk II 冲锋枪配置"sv}
		    ,{"Rifle Loadout"sv, "步枪配置"sv}
		    ,{"Shotgun Loadout"sv, "霰弹枪配置"sv}
		    ,{"Combat PDW Loadout"sv, "战斗冲锋手枪配置"sv}
		    ,{"Combat MG Loadout"sv, "战斗机枪配置"sv}
		    ,{"SMG Loadout"sv, "冲锋枪配置"sv}
		    ,{"Machine Pistol Loadout"sv, "机器手枪配置"sv}
		    ,{"Micro SMG Loadout"sv, "微型冲锋枪配置"sv}
		    ,{"Revolver Loadout"sv, "左轮手枪配置"sv}
		    ,{"Chester McCoy"sv, "切斯特·麦考伊"sv}
		    ,{"Gustavo Mota"sv, "古斯塔沃·莫塔"sv}
		    ,{"Patrick McReary"sv, "帕特里克·麦克里瑞"sv}
		    ,{"Charlie Reed"sv, "查理·里德"sv}
		    ,{"Karl Abolaji"sv, "卡尔·阿博拉吉"sv}
		    ,{"Eddie Toh"sv, "艾迪·托"sv}
		    ,{"Taliana Martinez"sv, "塔莉安娜·马丁内斯"sv}
		    ,{"Zach Nelson"sv, "扎克·尼尔森"sv}
		    ,{"Karim Denz"sv, "卡里姆·丹兹"sv}
		    ,{"Avi Schwartzman"sv, "艾维·施瓦茨曼"sv}
		    ,{"Paige Harris"sv, "佩奇·哈里斯"sv}
		    ,{"Christian Feltz"sv, "克里斯蒂安·费尔茨"sv}
		    ,{"Yohan Blair"sv, "约翰·布莱尔"sv}
		    ,{"Rickie Lukens"sv, "里基·卢肯斯"sv}
		    ,{"Zhaba"sv, "炸吧"sv}
		    ,{"Vagrant"sv, "流浪者"sv}
		    ,{"Outlaw"sv, "不法之徒"sv}
		    ,{"Everon"sv, "埃弗伦"sv}
		    ,{"Sultan Classic"sv, "王者经典版"sv}
		    ,{"Gauntlet Classic"sv, "铁腕经典版"sv}
		    ,{"Ellie"sv, "爱利"sv}
		    ,{"Komoda"sv, "科莫达"sv}
		    ,{"Retinue MK II"sv, "随行者 Mk II"sv}
		    ,{"Drifty Yosemite"sv, "漂移约塞米蒂"sv}
		    ,{"Sugoi"sv, "斯国一"sv}
		    ,{"Jugular"sv, "扼喉"sv}
		    ,{"Manchez"sv, "曼切兹"sv}
		    ,{"Stryder"sv, "斯特德"sv}
		    ,{"Defiler"sv, "亵渎者"sv}
		    ,{"Lectro"sv, "雷克托"sv}
		    ,{"Issi Classic"sv, "天威经典版"sv}
		    ,{"Asbo"sv, "埃斯波"sv}
		    ,{"Kanjo"sv, "羽黑"sv}
		    ,{"Blista Kanjo"sv, "旅行家羽黑"sv}
		    ,{"Sentinel Classic"sv, "卫士经典款"sv}
		    ,{"Albany"sv, "奥本尼"sv}
		    ,{"Annis"sv, "安尼斯"sv}
		    ,{"Benefactor"sv, "贝飞特"sv}
		    ,{"BF"sv, "平民之车"sv}
		    ,{"Bravado"sv, "冒险家"sv}
		    ,{"Buckingham"sv, "白金汉"sv}
		    ,{"Cheval"sv, "雪佛"sv}
		    ,{"Coil"sv, "线圈"sv}
		    ,{"Declasse"sv, "绝致"sv}
		    ,{"Dewbauchee"sv, "杜威堡"sv}
		    ,{"Dinka"sv, "丁卡"sv}
		    ,{"Dundreary"sv, "敦追里"sv}
		    ,{"Emperor"sv, "皇霸天"sv}
		    ,{"Enus"sv, "埃努斯"sv}
		    ,{"Fathom"sv, "深蓝"sv}
		    ,{"Gallivanter"sv, "盖里万特"sv}
		    ,{"Grotti"sv, "古罗帝"sv}
		    ,{"Hijak"sv, "海逐"sv}
		    ,{"HVY"sv, "重工"sv}
		    ,{"Imponte"sv, "英奔提"sv}
		    ,{"Invetero"sv, "英维特罗"sv}
		    ,{"Karin"sv, "卡林"sv}
		    ,{"Lampadati"sv, "兰帕达缇"sv}
		    ,{"LCC"sv, "自由城机车"sv}
		    ,{"Maibatsu"sv, "麦霸子"sv}
		    ,{"Mammoth"sv, "猛犸"sv}
		    ,{"Maxwell"sv, "麦斯威尔"sv}
		    ,{"MTL"sv, "重卡"sv}
		    ,{"Nagasaki"sv, "长崎"sv}
		    ,{"Obey"sv, "奥贝"sv}
		    ,{"Ocelot"sv, "欧斯洛"sv}
		    ,{"Overflod"sv, "奥弗弗洛德"sv}
		    ,{"Pegassi"sv, "佩嘉西"sv}
		    ,{"Pfister"sv, "菲斯特"sv}
		    ,{"Principe"sv, "普林西比"sv}
		    ,{"Progen"sv, "普罗根"sv}
		    ,{"Schyster"sv, "诗津"sv}
		    ,{"Shitzu"sv, "诗津"sv}
		    ,{"Truffade"sv, "特卢菲"sv}
		    ,{"Ubermacht"sv, "绝品"sv}
		    ,{"Vapid"sv, "威皮"sv}
		    ,{"Vulcar"sv, "威尔卡"sv}
		    ,{"Weeny"sv, "威尼"sv}
		    ,{"Western"sv, "西部"sv}
		    ,{"Willard"sv, "威拉德"sv}
		    ,{"Locally Visible"sv, "本地可见"sv}
		    ,{"Be locally visible while remaining invisible to other players"sv, "对其他玩家保持隐形的同时，在本地仍然可见"sv}
		    ,{"Special Ability in MP"sv, "线上模式特殊能力"sv}
		    ,{"The special ability to enable for the MP character"sv, "为线上角色启用的特殊能力"sv}
		    ,{"Weapon Type"sv, "武器类型"sv}
		    ,{"The type of custom weapon to use"sv, "要使用的自定义武器类型"sv}
		    ,{"Cage Gun"sv, "笼子枪"sv}
		    ,{"Delete Gun"sv, "删除枪"sv}
		    ,{"Gravity Gun"sv, "重力枪"sv}
		    ,{"Steal Vehicle Gun"sv, "偷车枪"sv}
		    ,{"Repair Gun"sv, "修复枪"sv}
		    ,{"Vehicle Gun"sv, "载具枪"sv}
		    ,{"Teleport Gun"sv, "传送枪"sv}
		    ,{"Paint Gun"sv, "喷漆枪"sv}
		    ,{"Rainbow Color Style"sv, "彩虹颜色样式"sv}
		    ,{"Style of rainbow color for paint gun"sv, "喷漆枪彩虹颜色样式"sv}
		    ,{"Fade"sv, "渐变"sv}
		    ,{"Spasm"sv, "闪烁"sv}
		    ,{"Custom Weapon"sv, "自定义武器"sv}
		    ,{"Modifies weapon ability"sv, "修改武器能力"sv}
		    ,{"Enable Only if Weapon is Out"sv, "仅在持枪时启用"sv}
		    ,{"Enables custom weapon only when weapon is out"sv, "仅在武器拿出时启用自定义武器"sv}
		    ,{"Launch Entity on Release"sv, "松手时发射实体"sv}
		    ,{"Launches entity when released"sv, "释放时发射抓取的实体"sv}
		    ,{"Vehicle Model"sv, "载具模型"sv}
		    ,{"Model of the vehicle"sv, "载具的模型名称"sv}
		    ,{"Paint Gun Color"sv, "喷漆枪颜色"sv}
		    ,{"Color of the paint gun"sv, "喷漆枪的颜色"sv}
		    ,{"Rainbow Color"sv, "彩虹颜色"sv}
		    ,{"Enables rainbow color for paint gun"sv, "为喷漆枪启用彩虹颜色"sv}
		    ,{"Rainbow Color Speed"sv, "彩虹变色速度"sv}
		    ,{"Speed of rainbow color for paint gun"sv, "喷漆枪彩虹颜色变化速度"sv}
		    ,{"Player Database Auto Update"sv, "玩家数据库自动更新"sv}
		    ,{"Automatically updates tracked player status every 3 minutes. This is required for tracking notifications to work"sv, "每 3 分钟自动更新一次被追踪玩家的状态。追踪通知功能需要开启此项"sv}
		    ,{"Tracking Notifications"sv, "追踪通知"sv}
		    ,{"Notifies you when the state of a player you track changes"sv, "当你追踪的玩家状态发生变化时通知你"sv}
		    ,{"Notify When Joinable"sv, "可加入时通知"sv}
		    ,{"Notifies you when a tracked player becomes joinable"sv, "当被追踪玩家变为可加入状态时通知你"sv}
		    ,{"Notify When Unjoinable"sv, "不可加入时通知"sv}
		    ,{"Notifies you when a tracked player becomes unjoinable"sv, "当被追踪玩家变为不可加入状态时通知你"sv}
		    ,{"Notify When Online"sv, "上线时通知"sv}
		    ,{"Notifies you when a tracked player goes online"sv, "当被追踪玩家上线时通知你"sv}
		    ,{"Notify When Offline"sv, "离线时通知"sv}
		    ,{"Notifies you when a tracked player goes offline"sv, "当被追踪玩家离线时通知你"sv}
		    ,{"Notify On Session Type Change"sv, "战局类型变更时通知"sv}
		    ,{"Notifies you when a tracked player's session type changes"sv, "当被追踪玩家的战局类型发生变化时通知你"sv}
		    ,{"Notify On Mission Change"sv, "任务变更时通知"sv}
		    ,{"Notifies you when a tracked player joins or leaves a mission"sv, "当被追踪玩家加入或离开任务时通知你"sv}
		    ,{"Notify On Job Lobby Change"sv, "差事大厅变更时通知"sv}
		    ,{"Notifies you when a tracked player joins or leaves a job lobby"sv, "当被追踪玩家加入或离开差事大厅时通知你"sv}
		    ,{"Update Saved Players Now"sv, "立即更新已保存玩家"sv}
		    ,{"Force-updates all saved players"sv, "强制更新所有已保存玩家的信息"sv}
		    ,{"IPL set to load"sv, "要加载的场景集"sv}
		    ,{"Weather to set or force. Note that this is local and cannot be seen by other players"sv, "要设置或强制应用的天气。此效果仅本地可见，其他玩家无法看到"sv}
		    ,{"Ped Model"sv, "行人模型"sv}
		    ,{"The model name of the ped you wish to spawn."sv, "要生成的行人模型名称。"sv}
		    ,{"Object Model"sv, "物体模型"sv}
		    ,{"The model name of the object you wish to spawn."sv, "要生成的物体模型名称。"sv}
		    ,{"Hour"sv, "小时"sv}
		    ,{"Set hour (0-23)"sv, "设置小时（0-23）"sv}
		    ,{"Minute"sv, "分钟"sv}
		    ,{"Set minute (0-59)"sv, "设置分钟（0-59）"sv}
		    ,{"Second"sv, "秒"sv}
		    ,{"Set second (0-59)"sv, "设置秒数（0-59）"sv}
		    ,{"Business"sv, "产业"sv}
		    ,{"Business to claim earning from."sv, "要领取保险箱收益的产业。"sv}
		    ,{"Bounty Amount"sv, "悬赏金额"sv}
		    ,{"The bounty amount to set"sv, "要设置的悬赏金额"sv}
		    ,{"Anonymous Bounty"sv, "匿名悬赏"sv}
		    ,{"Sets the bounty as anonymous"sv, "将悬赏设置为匿名"sv}
		    ,{"Matchmaking Region"sv, "匹配地区"sv}
		    ,{"Matchmaking region to spoof to"sv, "要伪装的匹配地区"sv}
		    ,{"Send to Interior Index"sv, "室内地点"sv}
		    ,{"The interior index to send the player(s) to"sv, "要将玩家送往的室内地点"sv}
		    ,{"Boost behavior"sv, "推进器行为"sv}
		    ,{"Desired rocket boost behavior"sv, "期望的火箭推进器行为"sv}
		    ,{"Send to Property Index"sv, "资产"sv}
		    ,{"The property index to send the player(s) to"sv, "要将玩家送往的资产"sv}
		    ,{"RP Multiplier Input"sv, "RP 倍率"sv}
		    ,{"RP Mulitplier (< 0 = 0 -> No RP)"sv, "RP 倍率（小于 0 时按 0 计算，不获得 RP）"sv}
		    ,{"Junk Energy Bike Time Trial"sv, "拉机能量自行车时间挑战赛"sv}
		    ,{"Wanted Slider"sv, "通缉等级滑块"sv}
		    ,{"Wanted level to set/freeze"sv, "要设置或冻结的通缉等级"sv}
		    ,{"Noclip Speed"sv, "穿墙速度"sv}
		    ,{"Freecam Speed"sv, "自由视角速度"sv}
		    ,{"How fast the freecam will move positions"sv, "自由视角移动速度"sv}
		    ,{"Weapon Damage Scale"sv, "武器伤害倍率"sv}
		    ,{"Sets the damage scale for weapons (including melee)"sv, "设置武器伤害倍率（包括近战）"sv}
		    ,{"Move Rate Override"sv, "移动速率覆盖"sv}
		    ,{"Multipler for base run/sprint speed"sv, "基础奔跑/冲刺速度倍率"sv}
		    ,{"Run/Sprint/Swim Multiplier"sv, "跑步/冲刺/游泳倍率"sv}
		    ,{"Multipler with which the run/sprint/swim speed is increased with until maximum speed is reached"sv, "提升跑步、冲刺和游泳速度直至达到最高速度的倍率"sv}
		    ,{"Weapon Explosion RadiusScale"sv, "武器爆炸范围倍率"sv}
		    ,{"Modifying Weapon Explosion RadiusScale"sv, "修改武器爆炸范围倍率"sv}
		    ,{"Notify on Player Join"sv, "玩家加入时通知"sv}
		    ,{"Notifies you when a player joins above the map"sv, "当有玩家加入时在地图上方显示通知"sv}
		};
	}

	std::string Translate(std::string_view text)
	{
		if (text.empty())
			return {};

		if (const auto it = g_Translations.find(text); it != g_Translations.end())
			return std::string(it->second);

		return std::string(text);
	}

	std::string TranslateLabel(std::string_view text)
	{
		const auto id_pos = text.find("##");
		if (id_pos == std::string_view::npos)
			return Translate(text);

		const auto visible = text.substr(0, id_pos);
		const auto suffix = text.substr(id_pos);
		return std::format("{}{}", Translate(visible), suffix);
	}

	std::string FormatHotkeyWindowTitle(std::string_view label)
	{
		return std::format("{} 热键", Translate(label));
	}
}
