using GTA5Shared.Helper;
using System.Reflection;
using System.Text;

namespace GTA5OnlineTools.Utils;

/// <summary>
/// [DOG] 环境自检：一键检查「游戏 / FSL / 菜单文件 / 脚本 / 数据模式 / 呼出键」等关键环节。
/// 面向不同用户习惯设计：纯只读检查、不修改任何文件；结果可一键复制，直接发群求助。
/// </summary>
public static class HealthCheckHelper
{
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

    /// <summary>运行全部检查（只读，不抛异常）</summary>
    public static List<Item> RunAll()
    {
        var items = new List<Item>();
        try { CheckVersion(items); } catch { }
        List<(string game, string platform, string path)> installs = new();
        try { installs = FindGameInstalls(); CheckGameInstall(items, installs); } catch { }
        try { CheckGameProcess(items); } catch { }
        try { CheckFSL(items, installs); } catch { }
        try { CheckDataMode(items); } catch { }
        try { CheckEnhanced(items); } catch { }
        try { CheckLegacy(items); } catch { }
        try { CheckTranslation(items); } catch { }
        try { CheckLogs(items); } catch { }
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
        sb.AppendLine("把以上内容发到 QQ 群 1121769473 即可获得帮助。");
        return sb.ToString();
    }

    //////////////////////////////////////////////////////////////////

    private static void CheckVersion(List<Item> items)
    {
        var ver = Assembly.GetExecutingAssembly().GetName().Version?.ToString(3) ?? "未知";
        items.Add(new Item
        {
            Icon = "ℹ️",
            Title = "工具版本",
            Detail = $"v{ver}（完全离线运行）"
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

    private static void CheckGameInstall(List<Item> items, List<(string game, string platform, string path)> installs)
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

    private static void CheckFSL(List<Item> items, List<(string game, string platform, string path)> installs)
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
        foreach (var (game, platform, path) in installs)
        {
            var winmm = Path.Combine(path, "WINMM.dll");
            if (File.Exists(winmm))
            {
                var len = new FileInfo(winmm).Length;
                sb.AppendLine($"{game}-{platform}：已安装（{len:N0} 字节）");
            }
            else
            {
                sb.AppendLine($"{game}-{platform}：未安装");
                missing.Add($"{game}-{platform}");
            }
        }
        items.Add(new Item
        {
            Icon = missing.Count == 0 ? "✅" : "❌",
            Title = "FSL 安装",
            Detail = sb.ToString().TrimEnd(),
            Hint = missing.Count == 0 ? "" : "未安装：" + string.Join("、", missing) + " → 点右侧按钮直接安装（等同 FSL 管理内的一键安装）",
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

    private static void CheckEnhanced(List<Item> items)
    {
        var dll = FileHelper.File_YimMenu_DLL_V2;
        var font = FileHelper.File_AppData_YimMenu_V2_Font;
        if (!File.Exists(dll))
        {
            items.Add(new Item
            {
                Icon = "❌",
                Title = "增强版菜单文件",
                Detail = "未找到（NewBase.dll 未释放）",
                Hint = "重新打开本工具即可自动释放；仍失败请检查杀软拦截"
            });
            return;
        }
        var hash8 = Sha256Prefix(dll, 8);
        var fontOk = File.Exists(font);
        items.Add(new Item
        {
            Icon = fontOk ? "✅" : "⚠️",
            Title = "增强版菜单文件",
            Detail = $"NewBase.dll 已就绪（SHA256 前 8 位 {hash8}）" + (fontOk ? "；字体已就绪" : "；字体缺失（中文字形可能异常）"),
            Hint = fontOk ? "" : "点右侧按钮重新释放；若反复失败请检查杀软是否拦截了 font.ttf",
            ActionKey = fontOk ? "" : "fix_v2",
            ActionLabel = "重新释放文件"
        });
    }

    private static void CheckLegacy(List<Item> items)
    {
        var sb = new StringBuilder();
        var problems = new List<string>();

        var dll = FileHelper.File_YimMenu_DLL_X;
        sb.AppendLine(File.Exists(dll) ? "菜单 DLL：已就绪" : "菜单 DLL：缺失");
        if (!File.Exists(dll)) problems.Add("菜单 DLL 缺失（重开本工具自动释放）");

        var scriptsDir = FileHelper.Dir_AppData_YimMenu_Scripts;
        var umFound = "";
        if (Directory.Exists(scriptsDir))
        {
            var ums = Directory.GetFiles(scriptsDir, "Ultimate_Menu*.lua");
            if (ums.Length > 0) umFound = Path.GetFileName(ums[0]);
        }
        var animOk = File.Exists(Path.Combine(scriptsDir, "DOG_InjectFx.lua"));

        sb.AppendLine(umFound.Length > 0 ? $"玩法脚本：已安装（{umFound}）" : "玩法脚本：未安装");
        sb.AppendLine(animOk ? "注入动画：已安装" : "注入动画：未安装");
        if (umFound.Length == 0) problems.Add("玩法脚本未装（点「安装脚本包」或注入一次自动补齐）");
        if (!animOk) problems.Add("注入动画缺失（一键补齐会自动装）");

        var keyName = "INSERT（默认）";
        if (File.Exists(FileHelper.File_AppData_YimMenu_Settings))
        {
            try
            {
                var json = File.ReadAllText(FileHelper.File_AppData_YimMenu_Settings);
                var m = System.Text.RegularExpressions.Regex.Match(json, "\"menu_toggle\"\\s*:\\s*(\\d+)");
                if (m.Success)
                {
                    var vk = int.Parse(m.Groups[1].Value);
                    keyName = vk switch
                    {
                        0x2D => "INSERT",
                        0x76 => "F7",
                        >= 0x70 and <= 0x7B => $"F{vk - 0x6F}",
                        0x0D => "Enter",
                        0x20 => "Space",
                        _ => $"VK 0x{vk:X2}"
                    };
                }
                else
                {
                    keyName = "INSERT（默认）";
                }
            }
            catch { }
        }
        sb.AppendLine($"呼出键：{keyName}（可在菜单 设置 → 热键 内修改）");

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

    private static string Sha256Prefix(string path, int chars)
    {
        try
        {
            using var sha = System.Security.Cryptography.SHA256.Create();
            using var fs = File.OpenRead(path);
            var hash = sha.ComputeHash(fs);
            var hex = Convert.ToHexString(hash);
            return hex[..Math.Min(chars, hex.Length)];
        }
        catch
        {
            return "未知";
        }
    }

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
                    if (!File.Exists(FileHelper.File_YimMenu_DLL_X))
                    {
                        FileHelper.ExtractResFile(FileHelper.Res_YimMenu_YimMenu_X, FileHelper.File_YimMenu_DLL_X);
                        parts.Add("传承版菜单 DLL 已重新释放");
                    }
                    return (true, parts.Count > 0 ? string.Join("；", parts) : "传承版组件已就绪，无需修复");
                }

                case "fix_v2":
                {
                    var parts = new List<string>();
                    if (!File.Exists(FileHelper.File_YimMenu_DLL_V2))
                    {
                        FileHelper.ExtractResFile(FileHelper.Res_YimMenu_YimMenu_V2, FileHelper.File_YimMenu_DLL_V2);
                        parts.Add("NewBase.dll 已重新释放");
                    }
                    if (!File.Exists(FileHelper.File_AppData_YimMenu_V2_Font))
                    {
                        Directory.CreateDirectory(FileHelper.Dir_AppData_YimMenu_V2);
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
