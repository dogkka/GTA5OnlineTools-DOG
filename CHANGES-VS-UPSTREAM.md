# 相对上游 YimMenuV2 的改动

本仓库是 **增强版菜单（NewBase）** 的开源源码，基于：

- 上游：[YimMenu/YimMenuV2](https://github.com/YimMenu/YimMenuV2)（GPL-2.0，基线 `39a0f22`）
- 中文本地化参考：[legeling/YimMenuV2-zh-cn](https://github.com/legeling/YimMenuV2-zh-cn)

## 我们做了什么

### 1. 全量中文化

- 菜单、通知、工具提示、防护日志全部中文
- 内置 CJK 字体，避免中文缺字/方块
- 术语统一（载具 / 武器 / 抢劫 / 任务）

### 2. 界面

- 4 种界面样式：经典 / 现代 / 现代（纵向）/ 模块化
- 菜单位置、缩放、呼出热键可自定义
- 注入成功动效，F7 / Ctrl+\ 呼出

### 3. 脚本事件防护

- 30+ 类恶意脚本事件拦截：崩溃攻击、声音轰炸、强制传送、室内踢出、假消息等
- 中文通知 + 防护日志，方便排查

### 4. 功能增强

- 载具 / 玩家 / 个人 / 世界等百余项实用与整活功能
- 任务辅助、传送、载具生成、玩家管理

### 5. 小细节

- 默认 F7 呼出（备用 Ctrl+\）
- 注入后日志窗口自动隐藏
- 载具生成落点 / 防重叠等体验优化

## 没有包含什么

- **传承版（Legacy）**：传承版是独立维护线，相关代码、DLL 和 Lua 脚本不在本仓库。
- **K9 商业线**：控制台、服务端、BE 通道不在本仓库。

## 构建

环境：Windows + VS2022（MSVC）+ CMake ≥ 3.20。

依赖（imgui / minhook / nlohmann_json / LuaJIT / AsyncLogger）由 CMake FetchContent 自动拉取；
网络受限时可预克隆到 `_deps/` 目录。

```powershell
cmake -S . -B build
cmake --build build --config Release
```
