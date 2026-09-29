# GTA5 线上小助手 · DOG 二次开发版

> 完全离线 · 内置简体中文 YimMenuV2 · 内置 FSL V6 · 教程界面 · 联系二维码
>
> 二次开发：**DOG** ｜ 基于原作者 **CrazyZhang** 的《GTA5 线上小助手》（MIT 许可）二次开发

## ⬇ 直接下载（免安装，推荐给不使用源码的用户）

**[点此下载 GTA5线上小助手-DOG版.exe（约 89 MB，免安装单文件）](https://github.com/dogkka/GTA5OnlineTools-DOG/releases/latest)**

- 单文件、免安装、**内置 .NET 运行时**，换电脑也能直接跑
- 下载后双击运行（程序需要管理员权限，会正常弹 UAC 请求）
- 校验：Release 附件里附带 `SHA256.txt`，下载后可自行核对

## 三步使用

1. **关闭反作弊**：Rockstar 启动器 → 设置 → 我的已安装游戏 → Grand Theft Auto V Enhanced → 取消勾选 **BattlEye**
   （Steam 启动项加 `-nobattleye` 更稳）
2. **装 FSL**：程序 `YimMenu` 页 → `FSL管理` → 点 **安装FSL-Steam**（使用内置 FSL V6，离线安装）
3. **注入菜单**：游戏进到 **主菜单** → `YimMenu` 页 → `YimMenu V2` 卡片 → 点 **内嵌版** → 游戏里按 `INSERT`（或 `Ctrl+\`）呼出中文菜单

## 本版特色

- **完全离线**：不检查公告、不检查更新、不访问任何第三方服务器
- **内置中文菜单 DLL**：自编译简体中文 YimMenuV2（官方源码 + 3769 条中文词典 + 中文字形）
- **内置 FSL V6**：FSL 管理一键离线安装，不再下载旧版 v3
- **教程式界面**：主页含 软件介绍 / 三步教程 / 名词解释 / 故障排查；首次启动弹出《关于 / 使用说明》
- **"联系"按钮**：顶部导航最右侧，扫码加作者微信

## 源码与文档

- 二改说明与构建方法：[README-DOG.md](README-DOG.md)
- 文件级改动清单：[CHANGES-DOG.md](CHANGES-DOG.md)
- 原作者项目：https://github.com/CrazyZhang666/GTA5OnlineTools
- 上游基础仓库（sch 分支）：https://github.com/sch-lda/GTA5OnlineTools

## 许可

- 原项目：MIT License（见 [LICENSE.txt](LICENSE.txt)）
- 内置的 YimMenuV2 中文版 DLL：基于 GPL-2.0 源码编译
- 内置 FSL V6：来自 UnknownCheats 帖子 616977（作者发布）
