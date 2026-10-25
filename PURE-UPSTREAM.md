# 纯净上游工具本体（MIT）

本分支是 [sch-lda/GTA5OnlineTools](https://github.com/sch-lda/GTA5OnlineTools) 的**原始工具源码**
（原作者 CrazyZhang666 / sch-lda，MIT 许可），**不包含 DOG 的任何二次开发改动**。

- 没有 DOG 的联系方式、二维码、打赏入口
- 没有 DOG 的离线化 / FSL 安装 / 环境自检 / 注入动效等改造
- 没有内置菜单 DLL（`YimMenu.dll` / `NewBase.dll` 等 GPL 二进制已剔除）

## 用途

- 给想维护免费版工具本体的开发者一个干净起点；
- 菜单请自行使用上游 [YimMenuV2](https://github.com/YimMenu/YimMenuV2)（GPL-2.0）或
  [Mr-X-GTA/YimMenu](https://github.com/Mr-X-GTA/YimMenu)（GPL-2.0），并遵守其许可证。

## 构建

环境：Windows + Visual Studio 2022 + .NET 6 SDK（WPF）。

```powershell
dotnet restore GTA5OnlineTools.sln
dotnet build GTA5OnlineTools.sln -c Release
```

许可证：MIT，见 `LICENSE.txt`。
