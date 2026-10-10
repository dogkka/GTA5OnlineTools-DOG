using GTA5Shared.Helper;

using System.Security.Cryptography;

namespace GTA5OnlineTools.Utils;

/// <summary>
/// [DOG] FSL 安装/检测助手（版本感知，静态读取，不加载 DLL）。
/// - 内置 FSL = 官方 v6 hotfix 1（2026-09-19 UC 审核通过，SHA256 b02d704b…）；
/// - 已装同款 → 跳过（装过无需重复安装）；
/// - 已装其他版本 FSL → 调用方弹确认框，确认后先备份旧 DLL + 本地存档，再覆盖；
/// - 非 FSL 的 WINMM.dll（其他模组）→ 拒绝覆盖。
/// 供「FSL 管理」与「环境自检」共用。
/// </summary>
public static class FslInstallHelper
{
    /// <summary>游戏目录 WINMM.dll 的四种状态</summary>
    public enum FslInstallKind
    {
        Missing,  // 未安装
        Bundled,  // 与内置同款（字节一致）
        OtherFsl, // 其他版本 FSL（含 LawnchairGetVersion 导出名）
        Foreign   // 非 FSL 的 WINMM.dll（可能来自其他模组）
    }

    private static string _embeddedSha256;

    /// <summary>内置 FSL（官方 V6 hotfix 1）SHA256（惰性计算，必要时先释放内置文件）</summary>
    public static string EmbeddedSha256
    {
        get
        {
            if (_embeddedSha256 == null)
            {
                if (!File.Exists(FileHelper.File_FSL_WINMM))
                    FileHelper.ExtractResFile(FileHelper.Res_FSL_WINMM, FileHelper.File_FSL_WINMM);

                using var sha = SHA256.Create();
                using var fs = new FileStream(FileHelper.File_FSL_WINMM, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
                _embeddedSha256 = Convert.ToHexString(sha.ComputeHash(fs));
            }

            return _embeddedSha256;
        }
    }

    /// <summary>
    /// 判定指定游戏目录里 WINMM.dll 的安装状态（只读，不修改任何文件；兼容文件被游戏占用）。
    /// </summary>
    /// <returns>(状态, 文件 SHA256 前 8 位；未安装时为空串)</returns>
    public static (FslInstallKind kind, string hash8) Inspect(string gameDir)
    {
        var dst = Path.Combine(gameDir, "WINMM.dll");
        if (!File.Exists(dst))
            return (FslInstallKind.Missing, "");

        // 读取失败（被杀软 / 进程短暂独占等）时降级：跳过哈希比对，仅按导出名判定
        string hash = null;
        try
        {
            using var sha = SHA256.Create();
            using var fs = new FileStream(dst, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
            hash = Convert.ToHexString(sha.ComputeHash(fs));
        }
        catch
        {
            hash = null;
        }

        if (hash != null && hash.Equals(EmbeddedSha256, StringComparison.OrdinalIgnoreCase))
            return (FslInstallKind.Bundled, hash[..8]);

        var hash8 = hash != null ? hash[..8] : "未知";

        if (ContainsFslExportMark(dst))
            return (FslInstallKind.OtherFsl, hash8);

        return (FslInstallKind.Foreign, hash8);
    }

    /// <summary>
    /// 用内置 FSL 覆盖安装到指定游戏目录；oldHash8 非空时先备份旧 WINMM.dll 到 ProgramData\GTA5OnlineTools\Backups。
    /// </summary>
    public static (bool ok, string message) ApplyBundled(string gameDir, string oldHash8 = "")
    {
        try
        {
            if (!File.Exists(FileHelper.File_FSL_WINMM))
                FileHelper.ExtractResFile(FileHelper.Res_FSL_WINMM, FileHelper.File_FSL_WINMM);

            var dst = Path.Combine(gameDir, "WINMM.dll");

            var backupNote = "";
            if (File.Exists(dst) && !string.IsNullOrEmpty(oldHash8))
            {
                var backupDir = Path.Combine(FileHelper.Dir_Base, "Backups");
                Directory.CreateDirectory(backupDir);

                var bak = Path.Combine(backupDir, $"FSL旧DLL备份-{DateTime.Now:yyyyMMdd-HHmmss}-{oldHash8}.dll");
                File.Copy(dst, bak, true);
                backupNote = $"，旧 DLL 已备份：{Path.GetFileName(bak)}";
            }

            File.Copy(FileHelper.File_FSL_WINMM, dst, true);
            return (true, $"安装完成（内置官方 FSL V6 hotfix 1）{backupNote} → {dst}");
        }
        catch (Exception ex)
        {
            return (false, $"安装失败：{ex.Message}（请确认游戏已关闭、目录有写入权限）");
        }
    }

    /// <summary>
    /// [DOG] 一键安装（仅补缺失）：为注册表检测到的所有游戏安装拷贝内置 FSL V6（WINMM.dll）。
    /// 已存在的条目一律不动（可能是用户自装的其他版本，误覆盖有丢档风险）；
    /// 需要更新/覆盖请走「FSL 管理」页（带确认与备份）。
    /// </summary>
    public static (bool ok, string message) InstallMissing()
    {
        try
        {
            var installs = HealthCheckHelper.FindGameInstalls();
            if (installs.Count == 0)
                return (false, "未检测到 GTA5 安装路径。请先运行一次游戏（Steam / Epic 均可）再试。");

            if (!File.Exists(FileHelper.File_FSL_WINMM))
                FileHelper.ExtractResFile(FileHelper.Res_FSL_WINMM, FileHelper.File_FSL_WINMM);

            var done = new List<string>();
            var failed = new List<string>();
            foreach (var (game, platform, path) in installs)
            {
                var dest = Path.Combine(path, "WINMM.dll");
                if (File.Exists(dest))
                    continue;

                try
                {
                    File.Copy(FileHelper.File_FSL_WINMM, dest, true);
                    done.Add($"{game}-{platform}");
                }
                catch (Exception ex)
                {
                    failed.Add($"{game}-{platform}（{ex.Message}）");
                }
            }

            if (done.Count == 0 && failed.Count == 0)
                return (true, "FSL 已全部就绪，无需安装（装过 FSL 无需重复安装）。");

            var msg = "";
            if (done.Count > 0)
                msg += $"已为 {string.Join("、", done)} 安装 FSL V6（下次启动游戏自动加载）";
            if (failed.Count > 0)
                msg += (msg.Length > 0 ? "\n" : "") + $"失败：{string.Join("；", failed)}\n请关闭游戏后重试（或确认游戏目录可写）";

            return (failed.Count == 0, msg);
        }
        catch (Exception ex)
        {
            return (false, "安装 FSL 失败：" + ex.Message);
        }
    }

    /// <summary>
    /// 文件内是否含 FSL 家族导出名（"LawnchairGetVersion"，FSL v2+ 暴露的开发者 API）。
    /// 纯静态字节扫描，不加载 DLL；兼容被游戏占用 / 加壳的构建。
    /// </summary>
    private static bool ContainsFslExportMark(string path)
    {
        try
        {
            byte[] data;
            using (var fs = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
            using (var ms = new MemoryStream())
            {
                fs.CopyTo(ms);
                data = ms.ToArray();
            }

            var needle = System.Text.Encoding.ASCII.GetBytes("LawnchairGetVersion");
            return data.AsSpan().IndexOf(needle) >= 0;
        }
        catch
        {
            return false;
        }
    }
}
