using GTA5Shared.Helper;
using System.Text.RegularExpressions;

namespace GTA5OnlineTools.Utils;

/// <summary>
/// [DOG] FSL 数据模式：本地数据 / 云端数据。
/// 两种模式均为「关闭 BE + FSL 进入线上」，区别仅在存档去向：
///  - 本地数据：存档只存本机（%appdata%\FSL），不进服务器；
///  - 云端数据：存档照常上传官方服务器（云同步）。
/// </summary>
public static class FslModeHelper
{
    /// <summary>FSL 配置文件路径</summary>
    public static string SettingsPath => Path.Combine(FileHelper.Dir_AppData_FSL, "settings.ini");

    /// <summary>是否已生成 FSL 配置（未生成 = 未装 FSL 或未进过游戏）</summary>
    public static bool ConfigExists => File.Exists(SettingsPath);

    /// <summary>
    /// 当前模式：null = 未生成配置；true = 本地数据；false = 云端数据。
    /// </summary>
    public static bool? GetCurrentMode()
    {
        if (!ConfigExists)
            return null;

        var text = File.ReadAllText(SettingsPath);
        return GetKey(text, "UseLocalSaves").Equals("true", StringComparison.OrdinalIgnoreCase);
    }

    /// <summary>
    /// 应用模式并写回配置（带写入回读校验）。
    /// [DOG] 切换前自动打包一份 %appdata%\FSL（存档 / 配置）到 ProgramData\GTA5OnlineTools\Backups（保留最近 10 份）；
    /// 备份失败则取消切换，防止丢档。
    /// </summary>
    /// <returns>(是否成功, 结果消息)</returns>
    public static (bool ok, string message) Apply(bool local)
    {
        try
        {
            if (!ConfigExists)
                return (false, "未检测到 FSL 配置。请先到「YimMenu → FSL 管理」安装 FSL 并进一次游戏，再切换模式。");

            var current = GetCurrentMode();
            if (current == local)
                return (true, $"当前已是{Describe(local)}，无需切换。");

            // [DOG] 防手滑：先备份再切换
            var (backupOk, backupMsg) = BackupSaves(current == true ? "本地到云端" : "云端到本地");
            if (!backupOk)
                return (false, $"存档备份失败，已取消切换（防止丢档）：{backupMsg}\n可关闭游戏 / 菜单后重试，或手动备份 %appdata%\\FSL。");

            var text = File.ReadAllText(SettingsPath);

            SetKey(ref text, "UseLocalSaves", local ? "true" : "false");
            SetKey(ref text, "BypassBattlEye", "true");
            SetKey(ref text, "BlockTelemetry", "true");
            SetKey(ref text, "EnableBackups", "true");

            if (local)
            {
                SetKey(ref text, "EnableMoneyCheatCodes", "true");
                SetKey(ref text, "UnlockGTAPlus", "true");
                SetKey(ref text, "UnlockCESP", "true");
            }
            else
            {
                // 官方文档：钱码/GTA+/CESP 依赖本地存档；云端模式下关闭，保持云端档案干净
                SetKey(ref text, "EnableMoneyCheatCodes", "false");
                SetKey(ref text, "UnlockGTAPlus", "false");
                SetKey(ref text, "UnlockCESP", "false");
            }

            File.WriteAllText(SettingsPath, text);

            // 写入回读校验（兜底：防止文件被占用/只读导致假成功）
            var check = GetKey(File.ReadAllText(SettingsPath), "UseLocalSaves");
            if (!check.Equals(local ? "true" : "false", StringComparison.OrdinalIgnoreCase))
                return (false, "配置写入校验失败，请检查 FSL 配置文件权限或稍后重试。");

            var message = local
                ? "已切换为本地数据模式（重启游戏生效）"
                : "已切换为云端数据模式（重启游戏生效）";
            if (backupMsg.Length > 0)
                message += $"\n切换前存档已自动备份：{backupMsg}";

            return (true, message);
        }
        catch (Exception ex)
        {
            return (false, $"切换失败：{ex.Message}");
        }
    }

    /// <summary>
    /// [DOG] 备份 %appdata%\FSL 到 ProgramData\GTA5OnlineTools\Backups（zip，保留最近 10 份）。
    /// </summary>
    /// <returns>(是否可继续, 成功时=zip 路径 / 失败时=错误信息；无内容可备份时 = (true, ""))</returns>
    public static (bool ok, string msg) BackupSaves(string tag)
    {
        try
        {
            var src = FileHelper.Dir_AppData_FSL;
            if (!Directory.Exists(src))
                return (true, "");

            var files = Directory.GetFiles(src, "*", SearchOption.AllDirectories);
            if (files.Length == 0)
                return (true, "");

            var backupDir = Path.Combine(FileHelper.Dir_Base, "Backups");
            Directory.CreateDirectory(backupDir);

            var zipPath = Path.Combine(backupDir, $"FSL备份-{DateTime.Now:yyyyMMdd-HHmmss}-{tag}.zip");
            using (var fs = new FileStream(zipPath, FileMode.Create, FileAccess.Write))
            using (var archive = new System.IO.Compression.ZipArchive(fs, System.IO.Compression.ZipArchiveMode.Create))
            {
                foreach (var f in files)
                {
                    var rel = Path.GetRelativePath(src, f).Replace('\\', '/');
                    archive.CreateEntryFromFile(f, rel, System.IO.Compression.CompressionLevel.Optimal);
                }
            }

            // 只保留最近 10 份
            try
            {
                var old = new DirectoryInfo(backupDir).GetFiles("FSL备份-*.zip")
                    .OrderByDescending(f => f.CreationTimeUtc)
                    .Skip(10);
                foreach (var f in old)
                    f.Delete();
            }
            catch { }

            return (true, zipPath);
        }
        catch (Exception ex)
        {
            return (false, ex.Message);
        }
    }

    /// <summary>模式名（用于展示）</summary>
    public static string Describe(bool? mode)
    {
        return mode switch
        {
            true => "本地数据模式",
            false => "云端数据模式",
            _ => "未检测到配置"
        };
    }

    /// <summary>读取键值（保持文件原样）</summary>
    private static string GetKey(string text, string key)
    {
        var match = Regex.Match(text, @"^(\s*)" + Regex.Escape(key) + @"(\s*=\s*)(.+?)(\s*)$", RegexOptions.Multiline);
        return match.Success ? match.Groups[3].Value.Trim() : string.Empty;
    }

    /// <summary>写入键值（键存在则原位替换，不存在则追加）</summary>
    private static void SetKey(ref string text, string key, string value)
    {
        var pattern = new Regex(@"^(\s*)" + Regex.Escape(key) + @"(\s*=\s*).*?(\s*)$", RegexOptions.Multiline);

        if (pattern.IsMatch(text))
            text = pattern.Replace(text, "${1}" + key + "${2}" + value + "${3}", 1);
        else
            text = text.TrimEnd() + Environment.NewLine + key + " = " + value + Environment.NewLine;
    }
}
