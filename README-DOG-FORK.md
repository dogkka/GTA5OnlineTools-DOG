# YimMenuV2 · DOG 增强版（源码分支说明）

本分支（`yimmenu-v2`）是 [GTA5OnlineTools-DOG](https://github.com/dogkka/GTA5OnlineTools-DOG)
内置中文菜单 DLL 的源码，用于持续开发与构建 `GTA5Shared/Files/YimMenu/NewBase.dll`。

## 基线

| 项 | 值 |
| --- | --- |
| 上游 | [YimMenu/YimMenuV2](https://github.com/YimMenu/YimMenuV2) `enhanced` 分支 |
| 上游基线 | `39a0f22`（2026-09-16） |
| 中文本地化 | [legeling/YimMenuV2-zh-cn](https://github.com/legeling/YimMenuV2-zh-cn)（运行时翻译表 + CJK 字体） |
| 内置版本号 | `v2.0.0-zh-cn-dev`（`VERSION` 文件） |

## 增强内容（相对上游）

### 新功能（16 项）

| 分类 | 功能 |
| --- | --- |
| 载具 | 瞬间刹车、引擎保持运转、转向灯、飞天模式、载具跳跃、水上行驶、彩虹车漆、自动驾驶、无碰撞、防爆胎 |
| 玩家 | 爆胎、锁车门、遥控载具、砸碎车窗、破坏引擎、掉头翻转、传送进车、玩家列表距离显示 |
| 个人 | 被动模式、自动开火（Triggerbot）、快速重生、传送枪、修理枪、删除枪、清洁角色 |
| 世界 | 全城断电、重力调节、人形雨 |

### 脚本事件防护

- 拦截 30+ 类恶意脚本事件（崩溃攻击、声音轰炸、假通知、强制传送、室内踢出、踢出载具、
  损毁个人载具等），带中文通知（5 秒节流）
- 总开关：设置 → 游戏 → 防护 → 脚本事件防护
- 防护日志面板：累计拦截计数 + 最近 100 条拦截历史（玩家 / 事件 / 时间）+ 清空
- 实现：`src/game/hooks/Network/HandleScriptedGameEvent.cpp` +
  `src/game/features/protections/ScriptEventProtection.cpp`

## 构建

环境：Windows + VS2022（MSVC）+ CMake ≥ 3.20 + Ninja（可选）

依赖（imgui / minhook / nlohmann_json / LuaJIT / AsyncLogger）由 CMake FetchContent 拉取。
网络受限时可预克隆到本地并通过 `FETCHCONTENT_SOURCE_DIR_*` 注入：

```powershell
git clone --depth 1 https://github.com/ocornut/imgui.git            _deps/imgui
git clone --depth 1 https://github.com/TsudaKageyu/minhook.git       _deps/minhook
git clone --depth 1 https://github.com/nlohmann/json.git             _deps/json
git clone --depth 1 https://github.com/Yimura/AsyncLogger.git        _deps/async-logger
git clone --depth 1 https://github.com/WohlSoft/LuaJIT.git           _deps/luajit

cmake -S . -B build -G "Visual Studio 17 2022" -A x64 `
  -DFETCHCONTENT_SOURCE_DIR_IMGUI=...\imgui `
  -DFETCHCONTENT_SOURCE_DIR_MINHOOK=...\minhook `
  -DFETCHCONTENT_SOURCE_DIR_JSON=...\json `
  -DFETCHCONTENT_SOURCE_DIR_ASYNCLOGGER=...\async-logger `
  -DFETCHCONTENT_SOURCE_DIR_LUAJIT=...\luajit
cmake --build build --config RelWithDebInfo --target YimMenuV2 --
```

注意：

- 中文源码需要 MSVC `/utf-8`（已配置于 `CMakeLists.txt`）
- 依赖仓库需 checkout 到本项目锁定版本（见 `cmake/*.cmake` 中的 `GIT_TAG`）
- 产物：`build/RelWithDebInfo/YimMenuV2.dll`

## 开发约定

- 一个功能一个 commit；新功能放 `src/game/features/<区域>/`，菜单注册同步修改
  `src/game/frontend/submenus/<页>/`
- 防护类改动放 `src/game/hooks/`，必须带开关
- DLL 更新到主仓库的步骤：替换 `GTA5Shared/Files/YimMenu/NewBase.dll`，
  并同步更新 `README-DOG.md` / `CHANGES-DOG.md` 中的 SHA256
