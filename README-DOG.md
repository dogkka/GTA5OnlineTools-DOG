# GTA5 线上小助手 · DOG 二次开发版

本仓库是 [sch-lda/GTA5OnlineTools](https://github.com/sch-lda/GTA5OnlineTools)
（原作者 **CrazyZhang** 的《GTA5 线上小助手》，MIT 许可）的**二次开发版**。

- 二次开发：**DOG**
- 基础上游：https://github.com/sch-lda/GTA5OnlineTools
- 原作者项目：https://github.com/CrazyZhang666/GTA5OnlineTools

## 二改内容一览

1. **离线化（只保留 GitHub 更新检测）**
   - `GTA5Shared/Helper/HttpHelper.cs`：所有 HTTP 请求直接返回空，不再访问任何第三方服务器
   - 移除启动时的"检查公告 + 检查更新"线程
   - 禁用"网络版 YimMenu 下载"和"在线 Lua 下载"入口（点击提示"本版本已离线化"）
   - 移除原项目 2027-04-06 域名过期警告
   - 新增 **更新检测**：只访问本仓库的 `api.github.com` 接口（失败回退 `releases.atom`），不碰原项目的任何第三方域名
2. **内置中文菜单 DLL**：`GTA5Shared/Files/YimMenu/NewBase.dll` 替换为自编译的简体中文 YimMenuV2
   （官方源码 + 3769 条中文词典 + 中文字形，SHA256 `1aae7acd8fdd8af3f27f3dd5b8ac1e802fc852c83a04b2a8ecea686b52aa9b8d`）
3. **内置 FSL V6**：`GTA5Shared/Files/FSL/WINMM.dll`（2026-09-19 hotfix，SHA256 `b02d704b821e0ff499fdbb72b2b56d5e29b389dfd65b8fd6a9ab08126d8d5bfe`）
   FSL 管理窗口改为离线安装（不再下载旧版 v3）
4. **主页改教程风**：`Views/HomeView.xaml`（软件介绍 / 三步教程 / 名词解释 / 故障排查）
5. **启动弹窗重做**：`Windows/NotificationWindow.xaml` 改为《关于 / 使用说明》（含原作者署名与 DOG）
6. **新增"联系"按钮**：`Windows/ContactWindow.xaml`，点击显示微信二维码（`Assets/contact_wechat.jpg`）
7. **品牌**：主窗口标题改为 `GTA5线上小助手-DOG版 <版本号>`
8. **更新检测（v1.1.0）**：启动自动查一次新版本，"选项"页 / "关于"页也可手动查；
   发现新版本弹出提示窗口（版本对比、更新内容、打开发布页面、复制下载链接、跳过此版本）

详细文件级改动见 [CHANGES-DOG.md](CHANGES-DOG.md)。

## 构建

环境：Windows + .NET 6 SDK（WPF）

```powershell
dotnet restore GTA5OnlineTools.sln
dotnet build GTA5OnlineTools.sln -c Release
```

发布单文件（自包含，免装 .NET 运行时）：

```powershell
dotnet publish GTA5OnlineTools\GTA5OnlineTools.csproj -c Release -r win-x64 `
  --self-contained true -p:PublishSingleFile=true `
  -p:IncludeNativeLibrariesForSelfExtract=true -p:DebugType=none -o publish
```

## 使用

1. Rockstar 启动器 → 设置 → GTA V Enhanced → 取消勾选 BattlEye
2. 程序 `YimMenu` 页 → `FSL管理` → 安装FSL-Steam（内置 FSL V6）
3. 游戏进主菜单 → `YimMenu V2` 卡片 → **内嵌版** → 游戏里按 `INSERT` 呼出中文菜单

## 注意

- 本仓库包含第三方二进制文件（FSL V6、YimMenu 系列 DLL、Notepad2 等）以及作者微信二维码图片，
  如果需要公开仓库，请自行评估第三方文件的再分发许可，并考虑替换/移除二维码。
- 原项目为 **MIT** 许可（见 `LICENSE.txt`）；内置的 YimMenuV2 中文版 DLL 基于 GPL-2.0 源码编译。
