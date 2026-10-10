using GTA5Shared.Helper;

namespace GTA5OnlineTools.Utils;

/// <summary>
/// [DOG] FSL 一键安装（仅补缺失）：为注册表检测到的所有游戏安装拷贝内置 FSL V6（WINMM.dll）。
/// 与 FSL 管理页安装行为一致（同一来源文件），已存在的条目不动，供「环境自检」一键修复使用。
/// </summary>
public static class FslInstallHelper
{
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
                return (true, "FSL 已全部就绪，无需安装。");

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
}
