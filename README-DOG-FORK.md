# YimMenuV2 · DOG 增强版（源码分支）

本分支（`yimmenu-v2`）是 [GTA5OnlineTools-DOG](https://github.com/dogkka/GTA5OnlineTools-DOG)
内置中文菜单 DLL（`NewBase.dll`）的源码，用于持续开发与构建。

## 基线

- 上游：[YimMenu/YimMenuV2](https://github.com/YimMenu/YimMenuV2) `enhanced` 分支（基线 `39a0f22`）
- 中文本地化参考：[legeling/YimMenuV2-zh-cn](https://github.com/legeling/YimMenuV2-zh-cn)（运行时翻译表 + CJK 字体）
- 当前版本号见仓库根目录 `VERSION` 文件

## 相对上游的主要内容

- DOG 增强功能包：载具 / 玩家 / 个人 / 世界等百余项实用与整活功能，全量中文化
- 脚本事件防护：拦截 30+ 类恶意脚本事件（崩溃攻击、声音轰炸、强制传送、室内踢出等），带中文通知与防护日志
- 菜单界面：4 种界面样式（经典 / 现代 / 现代（纵向）/ 模块化），支持菜单位置自定义
- 详细更新记录见主仓库的 Releases 页 / `CHANGES-DOG.md`

## 构建

环境：Windows + VS2022（MSVC）+ CMake ≥ 3.20

依赖（imgui / minhook / nlohmann_json / LuaJIT / AsyncLogger）由 CMake FetchContent 自动拉取；
网络受限时可预克隆到 `_deps/` 目录，并按下面的方式指定本地源码：

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
cmake --build build --config RelWithDebInfo --target YimMenuV2
```

说明：

- 中文源码需要 MSVC `/utf-8`（已在 `CMakeLists.txt` 配置）
- 依赖需 checkout 到本项目锁定版本（见 `cmake/*.cmake` 中的 `GIT_TAG`）
- 产物：`build/RelWithDebInfo/YimMenuV2.dll`

## 开发约定

- 一个功能一个 commit；功能放 `src/game/features/`，菜单注册同步修改 `src/game/frontend/submenus/`
- 防护类改动放 `src/game/hooks/`，必须带开关
- 更新到主仓库：替换 `GTA5Shared/Files/YimMenu/NewBase.dll`，并在 Releases 页补充更新说明
