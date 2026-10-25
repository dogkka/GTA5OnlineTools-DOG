# GTA5 线上小助手 · DOG 二次开发版

基于原作者 **CrazyZhang** 的《GTA5 线上小助手》（MIT 许可）二次开发，作者：**DOG**。

> **本仓库为纯发布页**：仅保留本 README 与最新版 Release，历史版本与源代码不再保留。

- 完全离线运行，只保留向本仓库 GitHub 接口查询新版本
- 内置简体中文 YimMenuV2 增强版菜单：百余项增强功能、30+ 类脚本事件防护、全量中文化
- 内置 FSL V6，一键离线安装
- 教程式界面与《关于 / 使用说明》弹窗，顶部导航带“联系”按钮
- 免安装单文件 exe，已内置 .NET 运行时，换电脑直接跑

## 小助手 与 K9：两条独立产品线

**小助手和 K9 已经分开，是两条独立的产品线，不再合并。** 本仓库只发布小助手，K9 不在本仓库发布。

| 对比项 | 小助手（本仓库） | K9 |
| --- | --- | --- |
| 定位 | 免费、公益 | 独立产品线 |
| 维护频率 | **较低**，只做必要的版本跟进 | 持续维护、快速迭代 |
| BattlEye（BE） | **关闭 BE 使用** | **可以直接开 BE 使用** |
| 匹配环境 | 与同样关 BE 的玩家在同一个池 | 与正常玩家（绿色玩家）在同一个池 |
| 典型场景 | 自己玩、独自刷内容 | 带绿色玩家一起做任务、一起进线上战局 |

### 核心区别：开不开 BE，决定了你在哪个“池子”里

Rockstar 在 GTA 线上引入 BattlEye 之后，匹配是按 BE 状态分流的：**开 BE 的玩家只会和同样开 BE 的玩家进到同一个战局，关 BE 的玩家也只会遇到同样关 BE 的玩家。**

所以两条线的差别不只是功能多少，而是**你身边的玩家是谁**：

- 小助手走的是关 BE 路线 —— 身边天然全是关 BE 的人，正常玩家（绿色玩家）不会出现在你的战局里；
- K9 走的是开 BE 路线 —— 你和正常玩家在同一个匹配池里，才能稳定组队、一起做任务、带朋友玩。

很多人问“为什么有的工具和绿色玩家凑不到一块儿”：不是功能强弱的问题，是根本不在一个匹配池。这就是两条产品线最本质的分界。

## 下载

到 **[Releases](https://github.com/dogkka/GTA5OnlineTools-DOG/releases/latest)** 页下载最新版 exe。

更新方式：下载新的 exe 覆盖旧文件即可，配置和网上存档都不会丢。

Releases 里同时保留上一版：最新版万一有问题，可以先退回上一版用。

## 数据

![Stars](https://img.shields.io/github/stars/dogkka/GTA5OnlineTools-DOG?style=flat-square&label=Stars)
![Downloads](https://img.shields.io/github/downloads/dogkka/GTA5OnlineTools-DOG/total?style=flat-square&label=Downloads)
![Last commit](https://img.shields.io/github/last-commit/dogkka/GTA5OnlineTools-DOG?style=flat-square&label=Last%20update)

**下载量趋势**（每日快照累积）

![Downloads over time](assets/downloads.svg)

**Star 增长**

[![Star History Chart](https://api.star-history.com/svg?repos=dogkka/GTA5OnlineTools-DOG&type=Date)](https://star-history.com/#dogkka/GTA5OnlineTools-DOG&Date)

> GitHub 官方只提供"当前累计下载量"，没有历史曲线。下载量趋势图中的数据由仓库内
> `scripts/downloads_chart.py` 每日抓取并追加到 `data/downloads.json`，图表会自动更新。

## 三步使用

1. **关闭反作弊**：Rockstar 启动器 → 设置 → 我的已安装游戏 → Grand Theft Auto V Enhanced → 取消勾选 **BattlEye**
   （Steam：启动项加 `-nobattleye`；**Epic 平台**：用程序内 FSL 管理里的「无BE模式启动游戏」或生成的桌面快捷方式）
2. **装 FSL**：程序 `YimMenu` 页 → `FSL管理` → 点 **一键安装FSL（自动检测）**（自动识别 Steam / Epic 与增强版 / 传承版；内置 FSL V6，离线安装）
3. **注入菜单**：游戏进到 **主菜单** → `YimMenu` 页 → 点 **内嵌版** → 游戏里按 `F7`（默认键，可自定义；备用 `Ctrl+\`）呼出中文菜单

## 更新检测

启动 1.5 秒后自动检查一次新版本；也可以在 **选项 → 检查更新** 或 **关于 → 检查更新** 手动检查。

检测只访问本仓库的 GitHub 接口，不碰任何第三方服务器。发现新版本会弹出提示（含更新内容），支持一键打开发布页面、复制下载链接、跳过此版本。

## 交流反馈

使用问题、反馈建议、更新通知都在群里；**主群、备用 1 群、备用 2 群均已满员，请直接加「备用 3 群」**：

<table>
  <tr>
    <th align="center">主群（已满）</th>
    <th align="center">备用 1 群（已满）</th>
    <th align="center">备用 2 群（已满）</th>
    <th align="center">备用 3 群（新群 · 推荐加入）</th>
  </tr>
  <tr>
    <td align="center"><b>472770575</b><br /><img src="qq-group.jpg" alt="主群二维码" width="200" /></td>
    <td align="center"><b>1121769473</b><br /><img src="qq-group-backup.jpg" alt="备用 1 群二维码" width="200" /></td>
    <td align="center"><b>1129626000</b><br /><img src="qq-group-2.jpg" alt="备用 2 群二维码" width="200" /></td>
    <td align="center"><b>1128353048</b><br /><img src="qq-group-3.png" alt="备用 3 群二维码" width="200" /></td>
  </tr>
</table>

（群进满速度很快；也可以直接用群号搜索加群，已满的群无需尝试。）

**防诈骗提醒**：本项目完全免费、公益性质。如果你遇到收费，那就是被骗了，请务必联系卖软件给你的那个人退款；官方发布渠道只有本仓库的 Releases 页面。

## 参与贡献

- 欢迎有能力的开发者**一起参与共同开发**——提交 Pull Request 或 Issue 都欢迎；
- 如果你只是路过，也欢迎**顺手点一个 Star** 支持一下，这对我们是最大的鼓励。

## 许可

- 原项目：MIT License（原作者 CrazyZhang / sch-lda，详见同目录发行说明）
- 内置的 YimMenuV2 中文版 DLL：基于 GPL-2.0 源码编译
- 内置 FSL V6：来自 UnknownCheats 帖子 616977（作者发布）
