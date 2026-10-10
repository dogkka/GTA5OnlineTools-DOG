using GTA5Shared.Helper;

namespace GTA5OnlineTools.Utils;

/// <summary>
/// [DOG] 一键卸载：清理本程序在所有预设位置生成的配置、缓存、日志与已释放的菜单文件。
/// 刻意不触碰：FSL 本地存档（%appdata%\FSL）、游戏存档、游戏目录内的 FSL
/// （游戏目录中的 FSL 请在「FSL 管理」中移除）。
/// </summary>
public static class UninstallHelper
{
    /// <summary>
    /// 执行清理。
    /// </summary>
    /// <returns>(是否全部成功, 清理报告行列表)</returns>
    public static (bool allOk, List<string> report) RunCleanup()
    {
        var report = new List<string>();
        var allOk = true;

        // 先解锁日志文件（NLog 默认保持文件句柄打开），否则日志目录无法删除
        LoggerHelper.ShutdownForUninstall();

        allOk &= TryDeleteTarget(FileHelper.Dir_Base, "程序配置 / 缓存 / 日志 / 菜单文件", report);
        allOk &= TryDeleteTarget(FileHelper.Dir_AppData_YimMenu, "传承版菜单配置与日志", report);
        allOk &= TryDeleteTarget(FileHelper.Dir_AppData_YimMenu_V2, "增强版菜单配置与缓存", report);

        // 桌面快捷方式（无BE启动，正常为 .lnk，生成失败时为 .bat）
        try
        {
            var desktop = Environment.GetFolderPath(Environment.SpecialFolder.DesktopDirectory);

            foreach (var name in new[] { "GTA5 无BE启动.lnk", "GTA5 无BE启动.bat" })
            {
                var shortcut = Path.Combine(desktop, name);

                if (File.Exists(shortcut))
                {
                    File.SetAttributes(shortcut, FileAttributes.Normal);
                    File.Delete(shortcut);
                    report.Add($"已删除：桌面快捷方式「{name}」");
                }
            }
        }
        catch (Exception ex)
        {
            allOk = false;
            report.Add($"桌面快捷方式删除失败：{ex.Message}");
        }

        return (allOk, report);
    }

    /// <summary>
    /// 删除单个目标目录：先整体删除（带重试），失败则逐文件尽力删除并报告被占用项。
    /// </summary>
    private static bool TryDeleteTarget(string path, string label, List<string> report)
    {
        try
        {
            if (string.IsNullOrWhiteSpace(path))
            {
                report.Add($"{label}：路径为空，跳过");
                return true;
            }

            path = Path.GetFullPath(path).TrimEnd(Path.DirectorySeparatorChar);

            if (!Directory.Exists(path))
            {
                report.Add($"{label}：不存在，跳过");
                return true;
            }

            for (var attempt = 0; attempt < 3; attempt++)
            {
                try
                {
                    Directory.Delete(path, true);
                    report.Add($"已删除：{label}");
                    return true;
                }
                catch
                {
                    Thread.Sleep(250);
                }
            }

            // 整体删除失败：逐文件/逐目录尽力删除，收集被占用项
            var failed = new List<string>();
            DeleteBestEffort(path, failed);

            if (failed.Count == 0 && !Directory.Exists(path))
            {
                report.Add($"已删除：{label}");
                return true;
            }

            report.Add($"{label}：以下文件被占用未能删除（请关闭游戏 / 菜单后重试）：");
            foreach (var item in failed.Take(8))
                report.Add("　- " + item);
            if (failed.Count > 8)
                report.Add($"　… 其余 {failed.Count - 8} 项");

            return false;
        }
        catch (Exception ex)
        {
            report.Add($"{label} 清理失败：{ex.Message}");
            return false;
        }
    }

    /// <summary>
    /// 尽力删除目录内容：先删文件，再自深到浅删目录，无法删除的记入 failed。
    /// </summary>
    private static void DeleteBestEffort(string root, List<string> failed)
    {
        if (!Directory.Exists(root))
            return;

        foreach (var file in Directory.EnumerateFiles(root, "*", SearchOption.AllDirectories))
        {
            try
            {
                File.SetAttributes(file, FileAttributes.Normal);
                File.Delete(file);
            }
            catch
            {
                failed.Add(file);
            }
        }

        // 自深到浅删除空目录（仍包含被占用文件的目录会自然失败）
        foreach (var dir in Directory.EnumerateDirectories(root, "*", SearchOption.AllDirectories)
                     .OrderByDescending(d => d.Length))
        {
            try
            {
                Directory.Delete(dir, true);
            }
            catch
            {
                // 目录内若有未能删除的文件，这里会失败；保留给上层报告
            }
        }

        try
        {
            Directory.Delete(root, true);
        }
        catch
        {
            if (Directory.Exists(root))
                failed.Add(root);
        }
    }
}
