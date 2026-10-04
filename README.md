# GTA5 线上小助手 · DOG 二次开发版

基于原作者 **CrazyZhang** 的《GTA5 线上小助手》（MIT 许可）二次开发，作者：**DOG**。

- 完全离线运行，只保留向本仓库 GitHub 接口查询新版本
- 内置简体中文 YimMenuV2 增强版菜单：百余项增强功能、30+ 类脚本事件防护、全量中文化
- 内置 FSL V6，一键离线安装
- 教程式界面与《关于 / 使用说明》弹窗，顶部导航带"联系"按钮
- 免安装单文件 exe，已内置 .NET 运行时，换电脑直接跑

## 下载

到 **[Releases](https://github.com/dogkka/GTA5OnlineTools-DOG/releases/latest)** 页下载最新版 exe。

更新方式：下载新的 exe 覆盖旧文件即可，配置和网上存档都不会丢。

## 三步使用

1. **关闭反作弊**：Rockstar 启动器 → 设置 → 我的已安装游戏 → Grand Theft Auto V Enhanced → 取消勾选 **BattlEye**
   （Steam 启动项加 `-nobattleye` 更稳）
2. **装 FSL**：程序 `YimMenu` 页 → `FSL管理` → 点 **安装FSL-Steam**（使用内置 FSL V6，离线安装）
3. **注入菜单**：游戏进到 **主菜单** → `YimMenu` 页 → 点 **内嵌版** → 游戏里按 `INSERT`（或 `Ctrl+\`）呼出中文菜单

## 更新检测

启动 1.5 秒后自动检查一次新版本；也可以在 **选项 → 检查更新** 或 **关于 → 检查更新** 手动检查。

检测只访问本仓库的 GitHub 接口，不碰任何第三方服务器。发现新版本会弹出提示（含更新内容），支持一键打开发布页面、复制下载链接、跳过此版本。

## 文档

- 二改说明与构建方法：[README-DOG.md](README-DOG.md)
- 版本更新记录：[CHANGES-DOG.md](CHANGES-DOG.md)

## 许可

- 原项目：MIT License（见 [LICENSE.txt](LICENSE.txt)）
- 内置的 YimMenuV2 中文版 DLL：基于 GPL-2.0 源码编译（源码见 `yimmenu-v2` 分支）
- 内置 FSL V6：来自 UnknownCheats 帖子 616977（作者发布）
