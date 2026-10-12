using GTA5Shared.Helper;
using System.Reflection;
using System.Text;

namespace GTA5OnlineTools.Utils;

/// <summary>
/// [DOG] 环境自检：一键检查「运行环境 / 游戏 / FSL / 菜单文件 / 玩法脚本 / 数据模式 / 呼出键」等关键环节，
/// 并在顶部给出「能否进游戏注入」的结论 —— 关键文件齐全 + 前置满足 = 可以进游戏注入。
/// 纯只读检查（仅目录可写性探针会临时建删一个探针文件）；结果可一键复制，直接发群求助。
/// </summary>
public static class HealthCheckHelper
{
    // ===== 内置版本期望哈希（随菜单 / 脚本包版本更新） =====
    /// <summary>传承版菜单 YimMenu.dll（Mr-X 源 + DOG 自建线增强 2026-10-12：掉线自动归队 / 任务守卫）</summary>
    private const string LegacyDllSha256 = "8BA02D8B1A6C6E540F08438DC234E4F2049EECA3CD98FEBF104985F51106826C";
    /// <summary>增强版菜单 NewBase.dll（2.1.42 · 赌场必中/防锁 + 转盘修复与必中 + 保存载具/速度表/修复载具/武器持久化）</summary>
    private const string EnhancedDllSha256 = "C8AA7401395AEA53E45417A7E98ACF2406EA3FA02123C89BA702EC037E04AFB1";
    /// <summary>传承版注入动画 DOG_InjectFx.lua</summary>
    private const string InjectFxSha256 = "76D1B7B74366D47E356D064436B8DE6B82B2F4ECD5F59CA64BD93A8904C5CC2C";

    public class Item
    {
        public string Icon { get; set; } = "ℹ️";
        public string Title { get; set; } = "";
        public string Detail { get; set; } = "";
        public string Hint { get; set; } = "";
        public string ActionKey { get; set; } = "";
        public string ActionLabel { get; set; } = "";

        public string IconColor => Icon switch
        {
            "✅" => "#21A366",
            "⚠️" => "#E6A23C",
            "❌" => "#C0392B",
            _ => "#409EFF"
        };
    }

    /// <summary>检查过程中的关键状态（用于汇总「能否注入」结论）</summary>
    private class Context
    {
        public bool Elevated;
        public bool LegacyInstallSeen;
        public bool LegacyGameOk;
        public bool LegacyFslOk;
        public bool EnhancedInstallSeen;
        public bool EnhancedGameOk;
        public bool EnhancedFslOk;
        public bool LegacyDllExists;
        public bool LegacyDllFresh;
        public bool LegacyScriptsOk;
        public bool LegacyAnimOk;
        public bool EnhancedDllExists;
        public bool EnhancedDllFresh;
        public bool EnhancedFontOk;
        public bool ScriptsDirWritable = true;
        public bool BaseDirWritable = true;
    }

    /// <summary>运行全部检查（只读，不抛异常）</summary>
    public static List<Item> RunAll()
    {
        var ctx = new Context();
        var items = new List<Item>();
        List<(string game, string platform, string path)> installs = new();

        try { CheckRuntime(items, ctx); } catch { }
        try { installs = FindGameInstalls(); CheckGameInstall(items, ctx, installs); } catch { }
        try { CheckGameProcess(items); } catch { }
        try { CheckFSL(items, ctx, installs); } catch { }
        try { CheckDataMode(items); } catch { }
        try { CheckEnhanced(items, ctx); } catch { }
        try { CheckLegacy(items, ctx); } catch { }
        try { CheckWritable(items, ctx); } catch { }
        try { CheckTranslation(items); } catch { }
        try { CheckLogs(items); } catch { }
        try { InsertReadinessSummary(items, ctx); } catch { }

        return items;
    }

    /// <summary>打包为可复制的纯文本</summary>
    public static string ToText(List<Item> items)
    {
        var sb = new StringBuilder();
        sb.AppendLine("DOG 小助手 · 环境自检结果");
        sb.AppendLine("时间：" + DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss"));
        sb.AppendLine("--------------------------------");
        foreach (var it in items)
        {
            sb.Append(it.Icon).Append(' ').Append(it.Title);
            if (!string.IsNullOrEmpty(it.Detail))
            {
                sb.AppendLine();
                sb.Append("     ").AppendLine(it.Detail);
            }
            else
            {
                sb.AppendLine();
            }
            if (!string.IsNullOrEmpty(it.Hint))
                sb.AppendLine("     → " + it.Hint);
        }
        sb.AppendLine("--------------------------------");
        sb.AppendLine("把以上内容发到 QQ 群（备用2群：1129626000）即可获得帮助。");
        return sb.ToString();
    }

    //////////////////////////////////////////////////////////////////

    private static void CheckRuntime(List<Item> items, Context ctx)
    {
        var ver = Assembly.GetExecutingAssembly().GetName().Version?.ToString(3) ?? "未知";
        bool elevated = false;
        try
        {
            using var identity = System.Security.Principal.WindowsIdentity.GetCurrent();
            elevated = new System.Security.Principal.WindowsPrincipal(identity)
                .IsInRole(System.Security.Principal.WindowsBuiltInRole.Administrator);
        }
        catch { }
        ctx.Elevated = elevated;

        items.Add(new Item
        {
            Icon = elevated ? "✅" : "❌",
            Title = "运行环境",
            Detail = $"工具版本 v{ver}（完全离线运行）；管理员权限：{(elevated ? "有（注入必需）" : "无")}",
            Hint = elevated ? "" : "关闭本工具 → 右键 → 以管理员身份运行（注入游戏需要管理员权限）"
        });
    }

    /// <summary>从注册表检测全部 GTA5 安装（增强版/传承版 × Steam/Epic）</summary>
    public static List<(string game, string platform, string path)> FindGameInstalls()
    {
        var list = new List<(string, string, string)>();
        var combos = new (string key, string game, string platform, string value)[]
        {
            (@"SOFTWARE\WOW6432Node\Rockstar Games\Grand Theft Auto V", "传承版", "Steam", "InstallFolderSteam"),
            (@"SOFTWARE\WOW6432Node\Rockstar Games\Grand Theft Auto V", "传承版", "Epic", "InstallFolderEpic"),
            (@"SOFTWARE\WOW6432Node\Rockstar Games\GTA V Enhanced", "增强版", "Steam", "InstallFolderSteam"),
            (@"SOFTWARE\WOW6432Node\Rockstar Games\GTAV Enhanced", "增强版", "Epic", "InstallFolderEpic"),
        };
        foreach (var c in combos)
        {
            using var key = Microsoft.Win32.Registry.LocalMachine.OpenSubKey(c.key);
            var v = key?.GetValue(c.value)?.ToString();
            if (!string.IsNullOrEmpty(v) && Directory.Exists(v))
                list.Add((c.game, c.platform, v));
        }
        return list;
    }

    private static void CheckGameInstall(List<Item> items, Context ctx, List<(string game, string platform, string path)> installs)
    {
        if (installs.Count == 0)
        {
            items.Add(new Item
            {
                Icon = "⚠️",
                Title = "游戏安装",
                Detail = "未检测到 GTA5 安装路径（注册表无记录）",
                Hint = "请先运行一次游戏（Steam / Epic 均可），再回到本页重新检查"
            });
            return;
        }

        var sb = new StringBuilder();
        var exeMissing = new List<string>();
        foreach (var (game, platform, path) in installs)
        {
            var exe = Path.Combine(path, game == "增强版" ? "GTA5_Enhanced.exe" : "GTA5.exe");
            var exeOk = File.Exists(exe);
            if (game == "增强版")
            {
                ctx.EnhancedInstallSeen = true;
                if (exeOk) ctx.EnhancedGameOk = true;
            }
            else
            {
                ctx.LegacyInstallSeen = true;
                if (exeOk) ctx.LegacyGameOk = true;
            }
            if (!exeOk) exeMissing.Add($"{game}-{platform}");
            sb.AppendLine($"{game}-{platform} → {path}{(exeOk ? "" : "（未找到游戏主程序！）")}");
        }
        items.Add(new Item
        {
            Icon = exeMissing.Count == 0 ? "✅" : "⚠️",
            Title = "游戏安装",
            Detail = sb.ToString().TrimEnd(),
            Hint = exeMissing.Count == 0 ? "" : "以下安装疑似不完整：" + string.Join("、", exeMissing) + "，建议先验证游戏文件完整性"
        });
    }

    private static void CheckGameProcess(List<Item> items)
    {
        var running = new List<string>();
        if (System.Diagnostics.Process.GetProcessesByName("GTA5_Enhanced").Length > 0) running.Add("增强版（GTA5_Enhanced.exe）");
        if (System.Diagnostics.Process.GetProcessesByName("GTA5_Enhanced_BE").Length > 0) running.Add("增强版 BE 启动器（GTA5_Enhanced_BE.exe）");
        if (System.Diagnostics.Process.GetProcessesByName("GTA5").Length > 0) running.Add("传承版（GTA5.exe）");
        if (System.Diagnostics.Process.GetProcessesByName("GTA5_BE").Length > 0) running.Add("传承版 BE 变体（GTA5_BE.exe）");

        var hasBE = running.Any(r => r.Contains("BE"));
        items.Add(new Item
        {
            Icon = running.Count == 0 ? "ℹ️" : (hasBE ? "⚠️" : "✅"),
            Title = "游戏进程",
            Detail = running.Count > 0 ? "正在运行：" + string.Join("、", running) : "未运行（正常；注入时需把游戏开到主菜单）",
            Hint = hasBE ? "检测到 BE 相关进程：本工具需在关闭 BattlEye 的普通模式下注入（游戏目录里的 GTA5_Enhanced_BE.exe 是 BE 启动器壳，实际游戏进程应为 GTA5_Enhanced.exe）" : ""
        });
    }

    private static void CheckFSL(List<Item> items, Context ctx, List<(string game, string platform, string path)> installs)
    {
        if (installs.Count == 0)
        {
            items.Add(new Item
            {
                Icon = "ℹ️",
                Title = "FSL 安装",
                Detail = "跳过（未检测到游戏安装路径）",
                Hint = "先运行一次游戏，再到 YimMenu 页 → FSL 管理 → 一键安装FSL"
            });
            return;
        }

        var sb = new StringBuilder();
        var missing = new List<string>();
        var legacyCount = 0;
        var legacyMissingCount = 0;
        var enhancedCount = 0;
        var enhancedMissingCount = 0;
        foreach (var (game, platform, path) in installs)
        {
            if (game == "增强版") enhancedCount++;
            else legacyCount++;
            var winmm = Path.Combine(path, "WINMM.dll");
            if (File.Exists(winmm))
            {
                var (kind, hash8) = FslInstallHelper.Inspect(path);
                sb.AppendLine(kind switch
                {
                    FslInstallHelper.FslInstallKind.Bundled => $"{game}-{platform}：已安装（内置同款官方 V6 hotfix 1）",
                    FslInstallHelper.FslInstallKind.OtherFsl => $"{game}-{platform}：已安装其他版本 FSL（{hash8}…，能正常游戏就无需重装/更新）",
                    FslInstallHelper.FslInstallKind.Foreign => $"{game}-{platform}：⚠ WINMM.dll 不是 FSL（{hash8}…，可能来自其他模组）",
                    _ => $"{game}-{platform}：已安装"
                });
            }
            else
            {
                sb.AppendLine($"{game}-{platform}：未安装");
                missing.Add($"{game}-{platform}");
                if (game == "增强版") enhancedMissingCount++;
                else legacyMissingCount++;
            }
        }
        ctx.LegacyFslOk = legacyCount > 0 && legacyMissingCount == 0;
        ctx.EnhancedFslOk = enhancedCount > 0 && enhancedMissingCount == 0;
        items.Add(new Item
        {
            Icon = missing.Count == 0 ? "✅" : "❌",
            Title = "FSL 安装",
            Detail = sb.ToString().TrimEnd(),
            Hint = missing.Count == 0
                ? "装过 FSL 的用户无需重复安装，直接进游戏点「内嵌版」注入即可"
                : "未安装：" + string.Join("、", missing) + " → 点右侧按钮直接安装（等同 FSL 管理内的一键安装；已存在的文件不会被动）",
            ActionKey = missing.Count == 0 ? "" : "fix_fsl",
            ActionLabel = "一键安装 FSL"
        });
    }

    private static void CheckDataMode(List<Item> items)
    {
        var mode = FslModeHelper.GetCurrentMode();
        items.Add(new Item
        {
            Icon = mode is null ? "ℹ️" : "✅",
            Title = "数据模式（存档去向）",
            Detail = mode switch
            {
                true => "本地数据模式（存档只存本机，不上传服务器）",
                false => "云端数据模式（存档照常上传官方服务器）",
                _ => "未初始化（安装 FSL 并进过一次游戏后自动生成）"
            },
            Hint = mode is null ? "到 FSL 管理 / 主页切换模式；未装 FSL 时先安装 FSL" : ""
        });
    }

    private static void CheckEnhanced(List<Item> items, Context ctx)
    {
        var dll = FileHelper.File_YimMenu_DLL_V2;
        var font = FileHelper.File_AppData_YimMenu_V2_Font;

        if (!File.Exists(dll))
        {
            ctx.EnhancedDllExists = false;
            items.Add(new Item
            {
                Icon = "❌",
                Title = "增强版菜单文件",
                Detail = "未找到（NewBase.dll 未释放）",
                Hint = "点右侧按钮释放；仍失败请检查杀软拦截",
                ActionKey = "fix_v2",
                ActionLabel = "重新释放文件"
            });
            return;
        }

        ctx.EnhancedDllExists = true;
        ctx.EnhancedDllFresh = HashEquals(dll, EnhancedDllSha256);
        ctx.EnhancedFontOk = File.Exists(font);

        var hash8 = Sha256Prefix(dll, 8);
        var ok = ctx.EnhancedDllFresh && ctx.EnhancedFontOk;
        items.Add(new Item
        {
            Icon = ok ? "✅" : "⚠️",
            Title = "增强版菜单文件",
            Detail = $"NewBase.dll 已就绪（SHA256 前 8 位 {hash8}{(ctx.EnhancedDllFresh ? "，版本校验一致" : "，与内置最新版不一致")}）" +
                     (ctx.EnhancedFontOk ? "；字体已就绪" : "；字体缺失（中文字形可能异常）"),
            Hint = ok ? "" : (ctx.EnhancedDllFresh ? "点右侧按钮重新释放字体" : "版本不一致（可能为旧版）：点右侧按钮更新为最新版"),
            ActionKey = ok ? "" : "fix_v2",
            ActionLabel = ctx.EnhancedDllFresh ? "重新释放文件" : "更新为最新版"
        });
    }

    private static void CheckLegacy(List<Item> items, Context ctx)
    {
        var sb = new StringBuilder();
        var problems = new List<string>();

        // 菜单 DLL（存在 + 版本校验）
        var dll = FileHelper.File_YimMenu_DLL_X;
        ctx.LegacyDllExists = File.Exists(dll);
        ctx.LegacyDllFresh = ctx.LegacyDllExists && HashEquals(dll, LegacyDllSha256);
        if (!ctx.LegacyDllExists)
        {
            sb.AppendLine("菜单 DLL：缺失");
            problems.Add("菜单 DLL 缺失（重开本工具或点修复自动释放）");
        }
        else if (!ctx.LegacyDllFresh)
        {
            sb.AppendLine("菜单 DLL：存在但版本不一致（可能为旧版，建议重新释放）");
            problems.Add("菜单 DLL 非最新内置版（点修复重新释放）");
        }
        else
        {
            sb.AppendLine("菜单 DLL：已就绪（版本校验一致）");
        }

        // 玩法脚本（存在 + 版本识别）
        var scriptsDir = FileHelper.Dir_AppData_YimMenu_Scripts;
        var umFile = "";
        if (Directory.Exists(scriptsDir))
        {
            var ums = Directory.GetFiles(scriptsDir, "Ultimate_Menu*.lua");
            if (ums.Length > 0) umFile = ums[0];
        }
        ctx.LegacyScriptsOk = umFile.Length > 0;
        if (umFile.Length > 0)
            sb.AppendLine($"玩法脚本：已安装（{Path.GetFileName(umFile)} · {DescribeUmVersion(Sha256Full(umFile))}）");
        else
        {
            sb.AppendLine("玩法脚本：未安装");
            problems.Add("玩法脚本未装（点「安装脚本包」或注入一次自动补齐）");
        }

        // 注入动画（存在 + 版本校验）
        var animPath = Path.Combine(scriptsDir, "DOG_InjectFx.lua");
        var animExists = File.Exists(animPath);
        var animFresh = animExists && HashEquals(animPath, InjectFxSha256);
        ctx.LegacyAnimOk = animExists && animFresh;
        if (!animExists)
        {
            sb.AppendLine("注入动画：未安装");
            problems.Add("注入动画缺失（一键补齐会自动装）");
        }
        else if (!animFresh)
        {
            sb.AppendLine("注入动画：存在但非最新内置版");
            problems.Add("注入动画非最新版（点修复重新部署）");
        }
        else
        {
            sb.AppendLine("注入动画：已就绪（版本校验一致）");
        }

        // 呼出键
        sb.AppendLine($"呼出键：{ReadToggleKeyDescription()}");

        items.Add(new Item
        {
            Icon = problems.Count == 0 ? "✅" : "⚠️",
            Title = "传承版组件",
            Detail = sb.ToString().TrimEnd(),
            Hint = problems.Count == 0 ? "" : string.Join("；", problems),
            ActionKey = problems.Count == 0 ? "" : "fix_legacy",
            ActionLabel = "一键补齐（脚本 / 动画 / 按键）"
        });
    }

    private static void CheckWritable(List<Item> items, Context ctx)
    {
        var targets = new (string name, string dir, string tag)[]
        {
            ("脚本目录", FileHelper.Dir_AppData_YimMenu_Scripts, "scripts"),
            ("工具数据目录", FileHelper.Dir_Base, "base"),
        };
        var problems = new List<string>();
        var sb = new StringBuilder();
        foreach (var (name, dir, tag) in targets)
        {
            var ok = false;
            try
            {
                Directory.CreateDirectory(dir);
                var probe = Path.Combine(dir, $".dog_write_probe_{Environment.TickCount64}");
                File.WriteAllText(probe, "probe");
                File.Delete(probe);
                ok = true;
            }
            catch { }

            if (tag == "scripts") ctx.ScriptsDirWritable = ok;
            else ctx.BaseDirWritable = ok;

            sb.AppendLine($"{name}：{(ok ? "可写" : "不可写")}（{dir}）");
            if (!ok) problems.Add($"{name}不可写");
        }
        items.Add(new Item
        {
            Icon = problems.Count == 0 ? "✅" : "❌",
            Title = "目录写入权限",
            Detail = sb.ToString().TrimEnd(),
            Hint = problems.Count == 0 ? "" : string.Join("、", problems) + "：多为杀软 / 安全软件拦截，请把本工具及其数据目录加入白名单后重试"
        });
    }

    private static void CheckTranslation(List<Item> items)
    {
        var index = FileHelper.File_YimMenu_IndexCN;
        var zhcn = FileHelper.File_YimMenu_ZHCN;
        var ok = File.Exists(index) && File.Exists(zhcn);
        items.Add(new Item
        {
            Icon = ok ? "✅" : "ℹ️",
            Title = "汉化文件（传承版菜单）",
            Detail = ok ? "已就绪（点一次传承版「内嵌版」注入即自动更新）" : "未生成（点一次传承版「内嵌版」注入即自动生成）"
        });
    }

    private static void CheckLogs(List<Item> items)
    {
        var cout1 = FileHelper.File_AppData_YimMenu_Logger;
        var cout2 = FileHelper.File_AppData_YimMenu_V2_Logger;
        var sb = new StringBuilder();
        sb.AppendLine("传承版 cout.log：" + (File.Exists(cout1) ? "有" : "暂无"));
        sb.AppendLine("增强版 cout.log：" + (File.Exists(cout2) ? "有" : "暂无"));
        items.Add(new Item
        {
            Icon = "ℹ️",
            Title = "日志",
            Detail = sb.ToString().TrimEnd(),
            Hint = "遇到问题：点下方「生成反馈包」，把生成的压缩包发到群里最快"
        });
    }

    /// <summary>汇总「能否进游戏注入」结论（插到列表第 2 位，紧跟在运行环境之后）</summary>
    private static void InsertReadinessSummary(List<Item> items, Context ctx)
    {
        // ---- 按版本独立判定（未安装的版本给中性提示，不算缺失项） ----
        var legacyBlockers = new List<string>();
        var enhancedBlockers = new List<string>();

        string legacyLine;
        if (!ctx.LegacyInstallSeen)
        {
            legacyLine = "【传承版】未检测到安装（如需使用，请先安装传承版游戏）。";
        }
        else
        {
            if (!ctx.LegacyGameOk) legacyBlockers.Add("游戏主程序");
            if (!ctx.LegacyFslOk) legacyBlockers.Add("FSL");
            if (!ctx.Elevated) legacyBlockers.Add("管理员权限");
            if (!ctx.LegacyDllExists) legacyBlockers.Add("菜单 DLL");
            else if (!ctx.LegacyDllFresh) legacyBlockers.Add("菜单需重新释放");
            if (!ctx.LegacyScriptsOk) legacyBlockers.Add("玩法脚本");
            if (!ctx.LegacyAnimOk) legacyBlockers.Add("注入动画");
            if (!ctx.ScriptsDirWritable) legacyBlockers.Add("脚本目录权限");
            if (!ctx.BaseDirWritable) legacyBlockers.Add("工具目录权限");
            legacyLine = legacyBlockers.Count == 0
                ? "【传承版】就绪，可以注入。"
                : $"【传承版】还差：{string.Join("、", legacyBlockers)}。";
        }

        string enhancedLine;
        if (!ctx.EnhancedInstallSeen)
        {
            enhancedLine = "【增强版】未检测到安装（如需使用，请先安装增强版游戏）。";
        }
        else
        {
            if (!ctx.EnhancedGameOk) enhancedBlockers.Add("游戏主程序");
            if (!ctx.EnhancedFslOk) enhancedBlockers.Add("FSL");
            if (!ctx.Elevated) enhancedBlockers.Add("管理员权限");
            if (!ctx.EnhancedDllExists) enhancedBlockers.Add("增强版菜单");
            else if (!ctx.EnhancedDllFresh) enhancedBlockers.Add("增强版菜单需重新释放");
            if (!ctx.EnhancedFontOk) enhancedBlockers.Add("菜单字体");
            if (!ctx.BaseDirWritable) enhancedBlockers.Add("工具目录权限");
            enhancedLine = enhancedBlockers.Count == 0
                ? "【增强版】就绪，可以注入。"
                : $"【增强版】还差：{string.Join("、", enhancedBlockers)}。";
        }

        var anySeen = ctx.LegacyInstallSeen || ctx.EnhancedInstallSeen;
        var anyBlockers = legacyBlockers.Count > 0 || enhancedBlockers.Count > 0;
        var readyCount = (ctx.LegacyInstallSeen && legacyBlockers.Count == 0 ? 1 : 0)
                       + (ctx.EnhancedInstallSeen && enhancedBlockers.Count == 0 ? 1 : 0);

        string icon;
        string detail;
        string hint;
        if (!anySeen)
        {
            icon = "⚠️";
            detail = "未检测到游戏安装：请先安装并运行一次游戏（Steam / Epic 均可），再回到本页重新检查。";
            hint = "装好游戏后，安装 FSL 并检查菜单文件是否齐全";
        }
        else if (!anyBlockers)
        {
            icon = "✅";
            detail = "关键文件全部就绪：\n" + legacyLine + "\n" + enhancedLine + "\n" +
                     "下一步：关闭 BattlEye → 启动游戏到主菜单 → 回来点对应卡片的「内嵌版」。";
            hint = "";
        }
        else
        {
            icon = readyCount > 0 ? "⚠️" : "❌";
            detail = legacyLine + "\n" + enhancedLine;
            hint = "按下方对应条目处理；可一键修复的直接点按钮";
        }

        items.Insert(1, new Item
        {
            Icon = icon,
            Title = "能否进游戏注入？",
            Detail = detail,
            Hint = hint
        });
    }

    //////////////////////////////////////////////////////////////////

    private static string ReadToggleKeyDescription()
    {
        if (!File.Exists(FileHelper.File_AppData_YimMenu_Settings))
            return "未生成（首次注入会自动写入 F7）";
        try
        {
            var json = File.ReadAllText(FileHelper.File_AppData_YimMenu_Settings);
            var m = System.Text.RegularExpressions.Regex.Match(json, "\"menu_toggle\"\\s*:\\s*(\\d+)");
            if (!m.Success)
                return "配置未含呼出键（首次注入会自动补写）";
            var vk = int.Parse(m.Groups[1].Value);
            var keyName = vk switch
            {
                0x2D => "INSERT",
                0x76 => "F7",
                >= 0x70 and <= 0x7B => $"F{vk - 0x6F}",
                0x0D => "Enter",
                0x20 => "Space",
                _ => $"VK 0x{vk:X2}"
            };
            return keyName + "（可在菜单 设置 → 热键 内修改）";
        }
        catch
        {
            return "读取失败";
        }
    }

    /// <summary>识别玩法脚本版本（按已知哈希）</summary>
    private static string DescribeUmVersion(string fullSha)
    {
        return fullSha.ToUpperInvariant() switch
        {
            "EB379F9A34DA9B6B048DBFD3E3EBAC7A8062BB259B842FD2D244E82173EA8289" => "最新汉化版 v3",
            "468A79F9EB7B1FFC2C1110F64ABAE8C452809BD629F84E3CCA5835608EDB0E5F" => "汉化版（中间版，首次注入会自动升级）",
            "5E28B9225BD3BD49A53452E536A93539EACFC427379EE237AFC148A1739C2326" => "汉化版（首版，首次注入会自动升级）",
            "1A8A9EA7A7272F797A8CAEFF2BB45C25E7977BF87D368AFF4E48D101EDE3A9F7" => "英文原版（首次注入会自动升级为汉化版）",
            "A08DEBF5336C2468038DDF386EA4A8E4BBFE2BD884FC3ADEBF778B8E3E15AE30" => "英文原版（换行变体，首次注入会自动升级为汉化版）",
            "" => "未知",
            _ => "未知版本（可点「安装脚本包」重新部署为内置版）"
        };
    }

    private static string Sha256Full(string path)
    {
        try
        {
            using var sha = System.Security.Cryptography.SHA256.Create();
            using var fs = File.OpenRead(path);
            return Convert.ToHexString(sha.ComputeHash(fs));
        }
        catch
        {
            return "";
        }
    }

    private static bool HashEquals(string path, string expectedHex)
    {
        var hex = Sha256Full(path);
        return hex.Length > 0 && hex.Equals(expectedHex, StringComparison.OrdinalIgnoreCase);
    }

    private static string Sha256Prefix(string path, int chars)
    {
        var hex = Sha256Full(path);
        if (hex.Length == 0) return "未知";
        return hex[..Math.Min(chars, hex.Length)];
    }

    //////////////////////////////////////////////////////////////////

    /// <summary>执行修复动作（供自检窗口「一键修复」按钮调用）</summary>
    /// <returns>(是否成功, 结果消息)</returns>
    public static (bool ok, string message) RunFix(string key)
    {
        try
        {
            switch (key)
            {
                case "fix_fsl":
                    return FslInstallHelper.InstallMissing();

                case "fix_legacy":
                {
                    var parts = new List<string>();
                    var (changed, msg) = FileHelper.EnsureLegacyLuaPack();
                    if (changed)
                        parts.Add(msg);

                    // 菜单 DLL：缺失或版本不一致 → 重新释放为内置版
                    var dll = FileHelper.File_YimMenu_DLL_X;
                    if (!File.Exists(dll) || !HashEquals(dll, LegacyDllSha256))
                    {
                        FileHelper.CreateDirectory(FileHelper.Dir_YimMenu);
                        FileHelper.ExtractResFile(FileHelper.Res_YimMenu_YimMenu_X, dll);
                        parts.Add("传承版菜单 DLL 已重新释放");
                    }

                    // 注入动画：缺失或版本不一致 → 重新部署
                    var animPath = Path.Combine(FileHelper.Dir_AppData_YimMenu_Scripts, "DOG_InjectFx.lua");
                    if (!File.Exists(animPath) || !HashEquals(animPath, InjectFxSha256))
                    {
                        FileHelper.CreateDirectory(FileHelper.Dir_AppData_YimMenu_Scripts);
                        FileHelper.ExtractResFile(FileHelper.Res_Lua_DogInjectFx, animPath);
                        parts.Add("注入动画已重新部署");
                    }

                    return (true, parts.Count > 0 ? string.Join("；", parts) : "传承版组件已就绪，无需修复");
                }

                case "fix_v2":
                {
                    var parts = new List<string>();

                    // 菜单 DLL：缺失或版本不一致 → 重新释放为内置版
                    var dll = FileHelper.File_YimMenu_DLL_V2;
                    if (!File.Exists(dll) || !HashEquals(dll, EnhancedDllSha256))
                    {
                        FileHelper.CreateDirectory(FileHelper.Dir_YimMenu);
                        FileHelper.ExtractResFile(FileHelper.Res_YimMenu_YimMenu_V2, dll);
                        parts.Add("NewBase.dll 已重新释放");
                    }

                    if (!File.Exists(FileHelper.File_AppData_YimMenu_V2_Font))
                    {
                        FileHelper.CreateDirectory(FileHelper.Dir_AppData_YimMenu_V2);
                        FileHelper.ExtractResFile(FileHelper.Res_YimMenu_YimMenu_V2_Font, FileHelper.File_AppData_YimMenu_V2_Font);
                        parts.Add("中文字体已重新释放");
                    }
                    return (true, parts.Count > 0 ? string.Join("；", parts) : "增强版组件已就绪，无需修复");
                }

                default:
                    return (false, "未知的修复动作");
            }
        }
        catch (Exception ex)
        {
            return (false, "修复失败：" + ex.Message);
        }
    }
}
