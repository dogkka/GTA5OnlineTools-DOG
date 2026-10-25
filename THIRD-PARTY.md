# 第三方组件与许可

本仓库只发布**小助手**（免费版）。K9 产品线（控制台、服务端、K9 菜单核心）不在本仓库，闭源。

## 许可一览

| 组件 | 许可证 | 上游 / 来源 | 本仓库是否含源码 |
|---|---|---|---|
| 小助手本体（GTA5OnlineTools DOG 二次开发） | MIT | 原项目 [sch-lda/GTA5OnlineTools](https://github.com/sch-lda/GTA5OnlineTools)（作者 CrazyZhang666） | 未公开（只随 Release 发包） |
| 内置增强版菜单 `NewBase.dll` | GPL-2.0（YimMenuV2 衍生） | [YimMenu/YimMenuV2](https://github.com/YimMenu/YimMenuV2) | **是，`open-source` 分支**（含汉化/优化/新增功能说明） |
| 内置传承版菜单 `YimMenu.dll` | GPL-2.0 | [Mr-X-GTA/YimMenu](https://github.com/Mr-X-GTA/YimMenu) | 否（见上游） |
| FSL V6（`WINMM.dll`） | 作者发布，未见公开许可证 | [UnknownCheats 616977](https://www.unknowncheats.me/forum/grand-theft-auto-v/616977-fsl-local-gtao-saves.html) | 否（作者未公开源码） |
| 传承版 Lua 包 `UltimateMenu_V3_1.73_Legacy.lua` | 社区脚本，随包分发 | Ultimate Menu 社区维护版 | 否（传承版相关文件不上传） |
| `Vendor/GameOverlay`、`Vendor/VirtualizingWrapPanel`、`Vendor/WinFormLib`、`MetroSkin` | 见各目录源码头部 | 各自上游 | 是 |

## 说明

1. 增强版菜单 `NewBase.dll` 的源码已在 `open-source` 分支公开（GPL-2.0），并附 `CHANGES-VS-UPSTREAM.md` 说明汉化/优化/新增内容。
2. 传承版 `YimMenu.dll` 为独立维护线，按项目决定**不公开**其源码与相关文件；本仓库也不放传承版的 DLL / Lua / 文案。
3. FSL 为作者在 UnknownCheats 发布的成品，本仓库只按原样随小助手分发，不声明其为开源软件。
4. 如果你要再分发本仓库的 Release，请自行遵守上述各组件许可证（尤其是 GPL-2.0 的源码提供义务）。
