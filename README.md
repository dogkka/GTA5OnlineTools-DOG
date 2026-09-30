# GTA5 线上小助手 · DOG 二次开发版

> 离线运行（仅 GitHub 更新检测）· 内置简体中文 YimMenuV2 · 内置 FSL V6 · 教程界面 · 联系二维码
>
> 二次开发：**DOG** ｜ 基于原作者 **CrazyZhang** 的《GTA5 线上小助手》（MIT 许可）二次开发

## ⬇ 直接下载（免安装，推荐给不使用源码的用户）

**[点此下载 GTA5OnlineTools-DOG-v1.0.0.exe（约 89 MB，免安装单文件）](https://github.com/dogkka/GTA5OnlineTools-DOG/releases/latest)**

- 单文件、免安装、**内置 .NET 运行时**，换电脑也能直接跑
- 下载后双击运行（程序需要管理员权限，会正常弹 UAC 请求）
- 校验：Release 附件里附带 `SHA256.txt`，下载后可自行核对

## 三步使用

1. **关闭反作弊**：Rockstar 启动器 → 设置 → 我的已安装游戏 → Grand Theft Auto V Enhanced → 取消勾选 **BattlEye**
   （Steam 启动项加 `-nobattleye` 更稳）
2. **装 FSL**：程序 `YimMenu` 页 → `FSL管理` → 点 **安装FSL-Steam**（使用内置 FSL V6，离线安装）
3. **注入菜单**：游戏进到 **主菜单** → `YimMenu` 页 → `YimMenu V2` 卡片 → 点 **内嵌版** → 游戏里按 `INSERT`（或 `Ctrl+\`）呼出中文菜单

## 本版特色

- **离线运行**：不检查公告、不访问任何第三方服务器；只保留向本仓库 GitHub 官方接口查询新版本
- **内置中文菜单 DLL**：自编译简体中文 YimMenuV2（官方源码 + 3769 条中文词典 + 中文字形）
- **内置 FSL V6**：FSL 管理一键离线安装，不再下载旧版 v3
- **教程式界面**：主页含 软件介绍 / 三步教程 / 名词解释 / 故障排查；首次启动弹出《关于 / 使用说明》
- **"联系"按钮**：顶部导航最右侧，扫码加作者微信
- **更新检测**：启动自动查一次新版本，"选项"页 / "关于"页也能手动查，发现新版一键打开发布页面

## 版本与更新检测

程序启动 1.5 秒后会自动查一次新版本；也可以点顶部导航 **选项 → 检查更新**，或 **关于 → 检查更新** 手动查。

检测只访问下面两个地址（GitHub 官方），不碰任何第三方服务器：

- 主通道：`https://api.github.com/repos/dogkka/GTA5OnlineTools-DOG/releases/latest`
- 备用通道：`https://github.com/dogkka/GTA5OnlineTools-DOG/releases.atom`

发现新版本时会弹出提示窗口，显示当前版本 / 最新版本 / 发布时间 / 更新内容，可以直接：

- **打开发布页面**（默认浏览器）下载新的 exe
- **复制下载链接**（浏览器打不开时贴到下载工具里）
- **跳过此版本**（下次启动不再提示，之后仍可手动检查）

更新方式很简单：**下载新的 exe，覆盖掉旧的这个文件就行**，配置和网上存档都不会丢，也不用重新安装。

> 当前仓库源码版本为 **v1.1.0**（含更新检测）；免安装 exe 以 [Releases](https://github.com/dogkka/GTA5OnlineTools-DOG/releases/latest) 页面上的最新版本为准。

## 源码与文档

- 二改说明与构建方法：[README-DOG.md](README-DOG.md)
- 文件级改动清单：[CHANGES-DOG.md](CHANGES-DOG.md)
- 原作者项目：https://github.com/CrazyZhang666/GTA5OnlineTools
- 上游基础仓库（sch 分支）：https://github.com/sch-lda/GTA5OnlineTools

## 许可

- 原项目：MIT License（见 [LICENSE.txt](LICENSE.txt)）
- 内置的 YimMenuV2 中文版 DLL：基于 GPL-2.0 源码编译
- 内置 FSL V6：来自 UnknownCheats 帖子 616977（作者发布）
