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
