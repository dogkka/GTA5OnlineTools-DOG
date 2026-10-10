# 二次开发说明

本仓库是 [sch-lda/GTA5OnlineTools](https://github.com/sch-lda/GTA5OnlineTools)
（原作者 [CrazyZhang666](https://github.com/CrazyZhang666/GTA5OnlineTools)，MIT 许可）的二次开发版，作者：**DOG**。

## 主要改动

- 完全离线化：移除原项目全部第三方域名请求，只保留本仓库 GitHub 接口的更新检测
- 内置简体中文 YimMenuV2 增强版菜单（`GTA5Shared/Files/YimMenu/NewBase.dll`，自编译；源码见 `yimmenu-v2` 分支）
- 内置传承版菜单：社区维护版 YimMenu（[Mr-X-GTA/YimMenu](https://github.com/Mr-X-GTA/YimMenu)，GPL-2.0，适配传承版 1.73 b3889；`GTA5Shared/Files/YimMenu/YimMenu.dll`）
- 传承版玩法脚本汉化：`UltimateMenu_V3_1.73_Legacy.lua` 为汉化版（词法级安全替换，映射脚本与清单见 `_lua_research` 工作区），旧英文版按哈希自动升级
- 内置传承版注入动画：`GTA5Shared/Files/Lua/DOG_InjectFx.lua`（随「安装脚本包」部署到 `%appdata%\YimMenu\scripts`，注入后自动播放「注入成功 · DOG」动画；增强版同款动画内置于菜单 DLL）
- 传承版体验补强：首次注入自动补齐「玩法脚本 + 注入动画」（已有则不覆盖）；默认呼出键 F7（首次自动写入 `%appdata%\YimMenu\settings.json`，菜单内可改）
- 内置「环境自检」+「反馈包」：主页一键体检（游戏 / FSL / 脚本 / 模式 / 呼出键），支持一键修复（补脚本 / 装 FSL / 释放菜单文件），一键打包日志配置到桌面；首启「三步上手」引导；切换数据模式前自动打包存档备份（`HealthCheckHelper.cs`、`SupportBundleHelper.cs`、`FslInstallHelper.cs`、`FslModeHelper.cs`）
- 内置 FSL V6：FSL 管理改为离线安装；"网络版 / 在线 Lua"入口禁用并给出提示
- 界面：主页教程化、首次启动《关于 / 使用说明》、顶部"联系"按钮

具体功能与修复记录见 [CHANGES-DOG.md](CHANGES-DOG.md) 和 Releases 页。

## 构建

环境：Windows + .NET 6 SDK（WPF）

```powershell
dotnet restore GTA5OnlineTools.sln
dotnet build GTA5OnlineTools.sln -c Release
```

发布免安装单文件（自包含，免装 .NET 运行时）：

```powershell
dotnet publish GTA5OnlineTools\GTA5OnlineTools.csproj -c Release -r win-x64 `
  --self-contained true -p:PublishSingleFile=true `
  -p:IncludeNativeLibrariesForSelfExtract=true -p:DebugType=none -o publish
```

内置菜单 DLL（NewBase.dll）的构建说明见 `yimmenu-v2` 分支的 `README-DOG-FORK.md`。

## 注意

- 仓库内含第三方二进制文件（FSL V6、YimMenu 系列 DLL 等）与作者微信二维码图片，再分发请自行评估相关许可。
- 原项目为 MIT 许可（见 `LICENSE.txt`）；内置的 YimMenuV2 中文版 DLL 基于 GPL-2.0 源码编译；内置的传承版 YimMenu DLL 来自 Mr-X-GTA/YimMenu（GPL-2.0，源码公开）。
