# CHANGES-DOG（二次开发改动清单）

对比上游 `sch-lda/GTA5OnlineTools` 的改动，按文件列出。

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
