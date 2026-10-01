# CHANGES-DOG（二次开发改动清单）

对比上游 `sch-lda/GTA5OnlineTools` 的改动，按文件列出。

---

## 菜单增强 v2.1.5（2026-10-01）

| 文件 | 改动 |
| --- | --- |
| `GTA5Shared/Files/YimMenu/NewBase.dll` | 替换为 DOG 增强版 v2.1.5（SHA256 `b12667aadc149d866e937096b3cdfc01f253dac826f3b7af8b5fc5b655449ddb`） |

### 菜单新增

- **速度表位置自定义**：载具 → 速度表。新增 X/Y 滑动条 + 菜单内"拖动圆点"预览面板 + 一键重置（默认右下角 X 1.00 / Y 0.85，位置随设置保存）

---

## 菜单增强 v2.1.4（2026-10-01）

### 内置 DLL 更新

| 文件 | 改动 |
| --- | --- |
| `GTA5Shared/Files/YimMenu/NewBase.dll` | 替换为 DOG 增强版 v2.1.4（SHA256 `3cf3298ac7d06e29d59ab5ac06f599029acb1d7ef25f3c2884d4b924d6d2cacc`） |

### 菜单新增功能

- **远程控制台（网页/手机）**：设置 → 游戏 → 远程控制台。浏览器打开 `http://127.0.0.1:7777` 即可查看战局玩家、执行任意菜单命令、发送脚本事件、重载 Lua；可选开放局域网 + 访问令牌
- **锁战局**：联机 → 战局 → 杂项。阻止新玩家加入当前战局
- **防笼子**：设置 → 游戏 → 防护。自动清除贴身放置的笼子/保险箱/围墙等困人道具
- **循环摇晃**：玩家 → 恶意 → 骚扰。隐形无声无伤害震屏，每 1.5 秒一次
- **假通知 / 假横幅 / 声音轰炸（+循环版）**：玩家 → 恶意 → 脚本事件（实验性参数布局）
- **脚本事件发送器**：调试 → 脚本事件发送器。输入任意事件哈希+参数，直接发送到选中玩家
- **遥控开关面板**：玩家 → 载具。遥控载具的状态开关现在可见可关

### 修复

- **载具调校整套失效**：菜单开关与实现命令名不一致（`handlingedit` vs `handlingeditor`），修复后重量/动力/极速/刹车/抓地滑条恢复实时生效
- **主机踢出按钮无效**：命令名大小写不一致（`HKick` vs `hkick`）导致按钮显示"未知"，已修复（含"对全部玩家执行"）
- **玩家距离颜色**未挂菜单，已补入 设置 → 游戏 → 玩家 ESP

## 新增文件

| 文件 | 说明 |
| --- | --- |
| `GTA5Shared/Files/FSL/WINMM.dll` | 内置 FSL V6（离线安装用） |
| `GTA5OnlineTools/Assets/contact_wechat.jpg` | 作者微信二维码（"联系"窗口使用） |
| `GTA5OnlineTools/Windows/ContactWindow.xaml` / `.xaml.cs` | "联系"窗口 |
| `README-DOG.md` / `CHANGES-DOG.md` | 二改说明 |

## 修改文件

| 文件 | 改动 |
| --- | --- |
| `GTA5Shared/Helper/HttpHelper.cs` | 彻底禁网：`DownloadString` / `PostAsync` 直接返回空字符串 |
| `GTA5Shared/Helper/FileHelper.cs` | 新增 `Res_FSL_WINMM` / `Dir_FSL` / `File_FSL_WINMM` |
| `GTA5Shared/GTA5Shared.csproj` | 新增嵌入资源 `Files\FSL\WINMM.dll` |
| `GTA5Shared/Files/YimMenu/NewBase.dll` | 替换为自编译简体中文 YimMenuV2 |
| `GTA5OnlineTools/Utils/CoreUtil.cs` | 窗口名改为 `GTA5线上小助手-DOG版 ` |
| `GTA5OnlineTools/MainWindow.xaml` | 标题栏文案；导航栏末尾新增"联系"按钮 |
| `GTA5OnlineTools/MainWindow.xaml.cs` | 移除公告/更新检查线程；首次运行显示说明弹窗；`Navigate` 处理 "Contact" |
| `GTA5OnlineTools/LoadWindow.xaml.cs` | 释放内置 FSL V6；移除 2027 域名过期警告 |
| `GTA5OnlineTools/Views/HomeView.xaml` | 重写为教程风：软件介绍 / 三步教程 / 名词解释 / 故障排查 |
| `GTA5OnlineTools/Views/AboutView.xaml` | 重写：二改声明、原作者项目地址、DOG 署名、"查看使用说明"按钮 |
| `GTA5OnlineTools/Views/AboutView.xaml.cs` | 新增 `Button_About_Click` |
| `GTA5OnlineTools/Views/HacksView.xaml.cs` | 禁用"网络版"与"在线 Lua / 在线下载"入口并提示 |
| `GTA5OnlineTools/Windows/NotificationWindow.xaml` / `.xaml.cs` | 由"服务器公告"改为本地《关于 / 使用说明》 |
| `GTA5OnlineTools/Windows/FSLWindow.xaml.cs` | FSL 安装由联网下载改为复制内置 FSL V6 |
| `GTA5OnlineTools/GTA5OnlineTools.csproj` | 新增 `Assets\contact_wechat.jpg` 资源 |

## 行为变化

- 启动不再请求 `1007890.xyz` 等第三方域名（公告、更新、哈希校验全部移除）
- "网络版" YimMenu / 在线 Lua 按钮点击后提示"本版本已离线化"
- FSL 安装日志显示"安装完成（内置 FSL V6）"
- 首次启动弹出《关于 / 使用说明》，配置记录于 `%ProgramData%\GTA5OnlineTools\Config\Config.ini` 的 `[Dog] AboutShown`
- 启动 1.5 秒后自动检测一次新版本；发现新版本时弹出提示窗口，点"跳过此版本"会记录到 `[Dog] SkipVersion`

---

## v1.1.0 更新检测（2026-09-30）

### 新增文件

| 文件 | 说明 |
| --- | --- |
| `GTA5OnlineTools/Utils/UpdateHelper.cs` | 更新检测核心：只访问 GitHub 官方接口，API 失败自动回退 Release 订阅源 |
| `GTA5OnlineTools/Utils/UpdateCheckResult.cs` | 检测结果模型 + 状态枚举（Failed / UpToDate / UpdateAvailable / LocalNewer） |
| `GTA5OnlineTools/Windows/UpdateWindow.xaml` / `.xaml.cs` | "发现新版本"窗口：版本对比、更新内容、打开发布页面、复制下载链接、跳过此版本 |

### 修改文件

| 文件 | 改动 |
| --- | --- |
| `GTA5OnlineTools/Views/OptionsView.xaml` / `.xaml.cs` | 新增"软件更新"区：检查更新按钮、线上最新版本、状态文字、发布页面可点击链接 |
| `GTA5OnlineTools/Views/AboutView.xaml` / `.xaml.cs` | 新增"检查更新"按钮与当前版本号显示 |
| `GTA5OnlineTools/MainWindow.xaml.cs` | 启动后自动检测一次新版本（失败静默忽略），发现新版把状态栏改为"发现新版本 x.y.z" |
| `GTA5OnlineTools/MainWindow.xaml` | 默认标题由"GTA5线上小助手-非官方"改为"GTA5线上小助手-DOG版" |
| `GTA5OnlineTools/Properties/AssemblyInfo.cs` | 版本号 `1.0.0.0` → `1.1.0.0` |
| `GTA5OnlineTools/Views/HomeView.xaml`、`GTA5OnlineTools/Windows/NotificationWindow.xaml` | 文案更正：不再声称"不检查更新"，改为"只访问 GitHub 官方接口" |

### 联网范围（唯一允许的地址）

- 主通道：`https://api.github.com/repos/dogkka/GTA5OnlineTools-DOG/releases/latest`
- 备用通道：`https://github.com/dogkka/GTA5OnlineTools-DOG/releases.atom`
- 原项目的 `sstaticstp.1007890.xyz` / `antfcc0.1007890.xyz` / `blog.1007890.xyz` 仍保持禁用，`HttpHelper` 未被恢复

### 版本比较规则

- tag 支持 `v1.2.3`、`1.2.3.4`、`release-2026.10.1` 等写法，统一补齐为 4 段版本号后比较（避免 `1.0.0` 与 `1.0.0.0` 被当成不同版本）
- 本地版本高于线上时不会提示更新（方便开发版）

### 本次验证结果

- `dotnet build` 通过，0 错误
- 独立测试程序直连 GitHub：线上 `v1.0.0` → 解析为 `1.0.0.0`，附件识别为 `GTA5OnlineTools-DOG-v1.0.0.exe`（88.89MB）
- 真机运行程序（临时版本 0.9.0.0）：主界面正常加载、自动检测命中 `UpdateAvailable` 并弹出"发现新版本"窗口，无崩溃日志

---

## 菜单增强版 2.0.0（2026-10-01）

`GTA5Shared/Files/YimMenu/NewBase.dll` 由「官方原版 + 中文词典」升级为 **DOG 增强版 2.0.0**：

### 基线

- YimMenuV2 官方 enhanced 分支 `39a0f22`（2026-09-16）
- 社区中文维护版 `legeling/YimMenuV2-zh-cn` 全量简体中文本地化（运行时翻译表 + CJK 字体）
- 内置版本号：`v2.0.0-zh-cn-dev`

### 新增功能（16 项，全部中文化）

| 分类 | 功能 |
| --- | --- |
| 载具 | 瞬间刹车、引擎保持运转、转向灯、飞天模式、水上行驶、载具跳跃、彩虹车漆 |
| 玩家 | 爆胎、锁车门、遥控载具 |
| 个人 | 被动模式、自动开火（Triggerbot）、快速重生 |
| 世界 | 全城断电、重力调节、人形雨 |
| 自动驾驶 | 自动驶向导航点 / 随机漫游（带接管检测） |
| 武器整活 | 传送枪、修理枪、删除枪 |
| 玩家载具 | 砸碎车窗、破坏引擎、掉头翻转、传送进车 |
| 其他 | 玩家列表距离显示、清洁角色、防护日志面板（累计拦截 + 历史记录）、Tunables 编辑器、自定义车牌 |
| 第四轮 | 保镖小队、送护卫队、生成火车、载具改装操作包（拆改/全改/引爆）、静音警报、载具隐形、防撞车身、保持贴地、无摔伤、玩家命令右键「对全部玩家执行」 |

### 新增防护

- **脚本事件防护**：拦截 30+ 类恶意脚本事件（崩溃攻击、声音轰炸、假通知、假横幅、强制传送、
  室内踢出、踢出载具、损毁个人载具等），带中文通知提示（5 秒节流）
- 总开关位于：设置 → 游戏 → 防护

### 技术说明

- MSVC 构建需 `/utf-8`（中文源码）；版本注入 `VERSION` = 2.0.0
- 源码：分支 [`yimmenu-v2`](https://github.com/dogkka/GTA5OnlineTools-DOG/tree/yimmenu-v2)（每功能独立 commit）
- DLL SHA256（b0ed7fee 版）：`7579af1473480b3bc4ca6c34d2ca2682effa2b629c2f80b28eb76a0333340ac4`


---

## 软件版本 v2.0.0（2026-10-01）

**主版本发布：内置菜单全面升级为 DOG 增强版**

### 软件侧
- 程序版本号 `1.1.0.0` → **`2.0.0.0`**（关于页、标题栏、启动弹窗、更新检测全部自动同步）
- 新增 Release 自动构建：推送 `v2.0.0` 标签后由 GitHub Actions 自动打包单文件 exe 并发布

### 菜单侧（NewBase.dll，SHA256 见 README）
- 增强功能 43 项：载具 16 项、玩家操作 15 项、个人 10 项、世界 4 项（详见 CHANGES 上文各轮记录）
- 脚本事件防护：30+ 类恶意事件拦截 + 总开关 + 中文通知 + 防护日志面板
- 全量简体中文本地化（含 CJK 字体）
- 菜单源码：分支 [yimmenu-v2](https://github.com/dogkka/GTA5OnlineTools-DOG/tree/yimmenu-v2)


---

## 软件版本 v2.1.0（2026-10-01）

**小版本更新：功能扩充 + UI 修复**

### 菜单（NewBase.dll）
- 新增：脚本事件攻击包（送赏金/假封号/踢出载具/CEO突袭/强制任务/假交易/室内踢）
- 新增：恶搞扩展（循环赏金、强制小游戏 ×9、假钱通知 ×3、钱雨）
- 新增：陨石雨、黑洞、烟花秀、喷气式喷火器、天基炮
- 新增：武器 7 件套（无后坐力/无扩散/无限射程/大弹匣/伤害/射速/射程）、夜视/热成像
- 新增：载具调校滑条（重量/动力/极速/刹车/抓地 + 极速直设）、预设车辆扩充到 16 辆（含科幻款）
- 新增：载具列表预览模式（点击即换车预览）
- 修复：现代主题 UI 错位（字体图集溢出 + 1.65 倍缩放）、通知多行截断、瞬间刹车行为
- 菜单版本号 2.1.0

### 软件侧
- 程序版本号 2.0.0.0 → **2.1.0.0**


---

## 软件版本 v2.1.1（2026-10-01）

**UI 布局修复补丁**

- 修复菜单大面积横向溢出问题（功能组永不换行导致窗口无限撑宽）
- 全部功能页面重新整理：车辆/个人/武器/世界/玩家页大组拆分、改为一行一项的竖排布局
- 修复现代主题 UI 错位（与横向溢出同根因）
- 官方抢劫页（KortzCenter 等）超宽分组一并修复


---

## 软件版本 v2.1.2（2026-10-01）

**整活扩展版**

- 新增玩家恶搞：小丑军团 / 鲨鱼上岸 / 雪人军团 / 死亡保龄球 / 烟花绑身 / 天降钢琴 / 礼物雨 / 加笼子 / 集装箱围墙 / 天降保险箱 / 恶搞车牌 / 强制车色
- 新增世界整活：动物大迁徙 / 跳跳世界 / 个人蹦床 / 海水消失 / 天降杂物（马桶/集装箱/保龄球/跑车/奶牛）/ 核爆
- 新增：ESP 血条与方框 / 命令执行器 / 变身系统（12 种模型）/ 补给空投
- 菜单版本号 2.1.2


---

## 软件版本 v2.1.3（2026-10-01）

**防护溯源 + 网络增强版**

### 防护升级
- 静默拦截全部补上日志与通知：赏金骚扰 / 假短信 / CEO 踢出 / 室内控制攻击
- **防护溯源升级到 RID 级**：防护日志每条显示「时间 玩家(ID:RID) 事件 #事件hash」
- 最近攻击者 Top5 排行，每行带**「复制RID」按钮**（一键复制用于拉黑/对账）
- 补齐最后事件缺口：SetSkydiveCompleted（强制标记挑战）纳入拦截

### 网络与体验
- 新增「一键满员模式」（联机 → 匹配增强），含人数伪装快捷切换与当前设置播报
- 新增「环境自检面板」（设置 → 游戏 → 环境自检）
- 版本失配优雅处理（游戏版本与菜单不匹配时给出清晰提示而非崩溃）
- 新增 Lua 脚本 lobby_booster.lua（满员模式脚本版，随包内置）

### 菜单
- 菜单版本号 2.1.3


### v2.1.3 修正（代码审查）
- 修复：龙卷风 / UFO 目击 两个功能未注册（菜单显示"未知"）——审查发现并修复
- 玩家"骚扰"分组改为竖排（不再横向撑宽）
- NewBase.dll 重新编译（含修复）
