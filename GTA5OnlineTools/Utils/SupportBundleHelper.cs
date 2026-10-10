using GTA5Shared.Helper;
using System.IO.Compression;
using System.Text;

namespace GTA5OnlineTools.Utils;

/// <summary>
/// [DOG] 反馈包：一键收集「自检结果 + 日志 + 关键配置」打包到桌面，方便发群求助。
/// 只读收集，不修改游戏/菜单的任何文件。
/// </summary>
public static class SupportBundleHelper
{
    private const long MaxLogBytes = 8 * 1024 * 1024; // 单个日志最多收录 8MB（取尾部）

    /// <summary>生成反馈包</summary>
    /// <returns>(是否成功, 结果消息, 文件路径)</returns>
    public static (bool ok, string message, string path) Collect()
    {
        try
        {
            var desktop = Environment.GetFolderPath(Environment.SpecialFolder.DesktopDirectory);
            var fileName = $"DOG小助手-反馈包-{DateTime.Now:yyyyMMdd-HHmmss}.zip";
            var zipPath = Path.Combine(desktop, fileName);

            var added = new List<string>();

            using (var zip = ZipFile.Open(zipPath, ZipArchiveMode.Create))
            {
                // 1) 自检结果 + 系统信息
                var info = new StringBuilder();
                info.AppendLine("== DOG 小助手 反馈包 ==");
                info.AppendLine("生成时间：" + DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss"));
                info.AppendLine("系统：" + Environment.OSVersion.VersionString + " / 64位: " + Environment.Is64BitOperatingSystem);
                info.AppendLine();
                info.AppendLine(HealthCheckHelper.ToText(HealthCheckHelper.RunAll()));
                AddText(zip, "自检结果.txt", info.ToString());
                added.Add("自检结果.txt");

                // 2) 工具日志（NLog / Crash）
                AddLatestFiles(zip, FileHelper.Dir_Log_NLog, "日志/工具", 5, "*.log", added);
                AddLatestFiles(zip, FileHelper.Dir_Log_Crash, "日志/崩溃", 3, "*.log", added);

                // 3) 菜单日志（cout.log）
                AddSingleLog(zip, FileHelper.File_AppData_YimMenu_Logger, "日志/传承版-cout.log", added);
                AddSingleLog(zip, FileHelper.File_AppData_YimMenu_V2_Logger, "日志/增强版-cout.log", added);

                // 4) 关键配置（不含游戏存档本体）
                AddSingleFile(zip, FslModeHelper.SettingsPath, "配置/FSL-settings.ini", added);
                AddSingleFile(zip, FileHelper.File_AppData_YimMenu_Settings, "配置/YimMenu-settings.json", added);
                AddSingleFile(zip, FileHelper.File_AppData_YimMenu_V2_Settings, "配置/YimMenuV2-settings.json", added);

                // 5) 脚本清单（只列文件名与大小，不打包脚本本体）
                var list = new StringBuilder();
                if (Directory.Exists(FileHelper.Dir_AppData_YimMenu_Scripts))
                {
                    foreach (var f in Directory.GetFiles(FileHelper.Dir_AppData_YimMenu_Scripts))
                        list.AppendLine($"{Path.GetFileName(f)} | {new FileInfo(f).Length} 字节 | {File.GetLastWriteTime(f):yyyy-MM-dd HH:mm}");
                }
                AddText(zip, "配置/脚本清单.txt", "传承版脚本目录：\n" + (list.Length == 0 ? "（空）" : list.ToString()));
                added.Add("配置/脚本清单.txt");
            }

            var size = new FileInfo(zipPath).Length;
            return (true, $"反馈包已生成（{size:N0} 字节，共 {added.Count} 项）：\n{zipPath}\n把这个压缩包发到 QQ 群即可。", zipPath);
        }
        catch (Exception ex)
        {
            return (false, $"生成反馈包失败：{ex.Message}", "");
        }
    }

    //////////////////////////////////////////////////////////////////

    private static void AddText(ZipArchive zip, string entryName, string content)
    {
        var entry = zip.CreateEntry(entryName, CompressionLevel.Optimal);
        using var writer = new StreamWriter(entry.Open(), new UTF8Encoding(false));
        writer.Write(content);
    }

    private static void AddSingleFile(ZipArchive zip, string path, string entryName, List<string> added)
    {
        try
        {
            if (!File.Exists(path)) return;
            if (new FileInfo(path).Length > MaxLogBytes)
            {
                AddTail(zip, path, entryName);
            }
            else
            {
                zip.CreateEntryFromFile(path, entryName, CompressionLevel.Optimal);
            }
            added.Add(entryName);
        }
        catch { }
    }

    private static void AddSingleLog(ZipArchive zip, string path, string entryName, List<string> added)
    {
        AddSingleFile(zip, path, entryName, added);
    }

    private static void AddTail(ZipArchive zip, string path, string entryName)
    {
        using var fs = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
        var start = Math.Max(0, fs.Length - MaxLogBytes);
        fs.Seek(start, SeekOrigin.Begin);
        var buf = new byte[fs.Length - start];
        var read = 0;
        while (read < buf.Length)
        {
            var n = fs.Read(buf, read, buf.Length - read);
            if (n <= 0) break;
            read += n;
        }
        var entry = zip.CreateEntry(entryName, CompressionLevel.Optimal);
        using var s = entry.Open();
        s.Write(new UTF8Encoding(false).GetBytes($"[日志过大，仅收录末尾 {read:N0} 字节]\n"));
        s.Write(buf, 0, read);
    }

    private static void AddLatestFiles(ZipArchive zip, string dir, string entryPrefix, int count, string pattern, List<string> added)
    {
        try
        {
            if (!Directory.Exists(dir)) return;
            var files = new DirectoryInfo(dir).GetFiles(pattern)
                .OrderByDescending(f => f.LastWriteTime)
                .Take(count);
            foreach (var f in files)
            {
                var name = $"{entryPrefix}/{f.Name}";
                AddSingleFile(zip, f.FullName, name, added);
            }
        }
        catch { }
    }
}
