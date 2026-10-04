# 二次开发说明

本仓库是 [sch-lda/GTA5OnlineTools](https://github.com/sch-lda/GTA5OnlineTools)
（原作者 [CrazyZhang666](https://github.com/CrazyZhang666/GTA5OnlineTools)，MIT 许可）的二次开发版，作者：**DOG**。

## 主要改动

- 完全离线化：移除原项目全部第三方域名请求，只保留本仓库 GitHub 接口的更新检测
- 内置简体中文 YimMenuV2 增强版菜单（`GTA5Shared/Files/YimMenu/NewBase.dll`，自编译；源码见 `yimmenu-v2` 分支）
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
- 原项目为 MIT 许可（见 `LICENSE.txt`）；内置的 YimMenuV2 中文版 DLL 基于 GPL-2.0 源码编译。
