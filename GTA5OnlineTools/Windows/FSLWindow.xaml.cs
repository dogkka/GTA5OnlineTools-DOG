using GTA5OnlineTools.Data;
using GTA5OnlineTools.Utils;

using GTA5Shared.Helper;
using System.Net.Http;

namespace GTA5OnlineTools.Windows;

/// <summary>
/// FSLWindow.xaml 的交互逻辑
/// </summary>
public partial class FSLWindow
{
    private string tempPath = string.Empty;

    private const string fsl_v2_url = "https://sstaticstp.1007890.xyz/WINMM.dll";

    private string GTA5_InstallPath_Steam = string.Empty;
    private string GTA5_InstallPath_Epic = string.Empty;
    private string GTA5_InstallPath_Steam_enhanced = string.Empty;
    private string GTA5_InstallPath_Epic_enhanced = string.Empty;
    private HttpClient _httpClient = new();

    public FSLWindow()
    {
        InitializeComponent();
    }

    private void Window_FSL_Loaded(object sender, RoutedEventArgs e)
    {
        Button_GTA_STEAM_Dir.IsEnabled = false;
        Button_GTA_EPIC_Dir.IsEnabled = false;
        Button_StartDownload_Steam.IsEnabled = false;
        Button_StartDownload_Epic.IsEnabled = false;
        Button_RM_FSL_Steam.IsEnabled = false;
        Button_RM_FSL_Epic.IsEnabled = false;

        string registryPath_legacy = @"SOFTWARE\WOW6432Node\Rockstar Games\Grand Theft Auto V";
        string registryPath_enhanced_steam = @"SOFTWARE\WOW6432Node\Rockstar Games\GTA V Enhanced";
        string registryPath_enhanced_epic = @"SOFTWARE\WOW6432Node\Rockstar Games\GTAV Enhanced";

        string valueName_steam = "InstallFolderSteam";
        string valueName_epic = "InstallFolderEpic";
        bool isfound = false;
        
        using (RegistryKey key = Registry.LocalMachine.OpenSubKey(registryPath_legacy))
        {
            if (key != null)
            {
                object value_steam = key.GetValue(valueName_steam);
                if (value_steam != null)
                {
                    GTA5_InstallPath_Steam = value_steam.ToString();
                    if (Directory.Exists(GTA5_InstallPath_Steam))
                    {
                        isfound = true;
                        Button_GTA_STEAM_Dir.IsEnabled = true;
                        Button_StartDownload_Steam.IsEnabled = true;
                        Button_RM_FSL_Steam.IsEnabled = true;
                        AppendLogger($"已从注册表获取GTA5传承版-Steam安装路径：{GTA5_InstallPath_Steam}");
                    }
                }
                object value_epic = key.GetValue(valueName_epic);
                if (value_epic != null)
                {
                    GTA5_InstallPath_Epic = value_epic.ToString();
                    if (Directory.Exists(GTA5_InstallPath_Epic))
                    {
                        isfound = true;
                        Button_GTA_EPIC_Dir.IsEnabled = true;
                        Button_StartDownload_Epic.IsEnabled = true;
                        Button_RM_FSL_Epic.IsEnabled = true;
                        AppendLogger($"已从注册表获取GTA5传承版-Epic安装路径：{GTA5_InstallPath_Epic}");
                    }
                }
            }
        }
        using (RegistryKey key = Registry.LocalMachine.OpenSubKey(registryPath_enhanced_steam))
        {
            if (key != null)
            {
                object value_steam = key.GetValue(valueName_steam);
                if (value_steam != null)
                {
                    GTA5_InstallPath_Steam_enhanced = value_steam.ToString();
                    if (Directory.Exists(GTA5_InstallPath_Steam_enhanced))
                    {
                        isfound = true;
                        Button_GTA_STEAM_Dir.IsEnabled = true;
                        Button_StartDownload_Steam.IsEnabled = true;
                        Button_RM_FSL_Steam.IsEnabled = true;
                        AppendLogger($"已从注册表获取GTA5增强版-Steam安装路径：{GTA5_InstallPath_Steam_enhanced}");
                    }
                }
            }
        }
        using (RegistryKey key = Registry.LocalMachine.OpenSubKey(registryPath_enhanced_epic))
        {
            if (key != null)
            {
                object value_epic = key.GetValue(valueName_epic);
                if (value_epic != null)
                {
                    GTA5_InstallPath_Epic_enhanced = value_epic.ToString();
                    if (Directory.Exists(GTA5_InstallPath_Epic_enhanced))
                    {
                        isfound = true;
                        Button_GTA_EPIC_Dir.IsEnabled = true;
                        Button_StartDownload_Epic.IsEnabled = true;
                        Button_RM_FSL_Epic.IsEnabled = true;
                        AppendLogger($"已从注册表获取GTA5增强版-Epic安装路径：{GTA5_InstallPath_Epic_enhanced}");
                    }
                }
            }
        }

        if (!isfound)
        {
            AppendLogger("未找到GTA5增强版或传承版的安装路径,如果您安装后从未启动过游戏,请先运行一次GTA5再进入此页面");
        }

        UpdateDetectedText();
    }

    /// <summary>
    /// [DOG] 刷新“已检测到”状态文本
    /// </summary>
    private void UpdateDetectedText()
    {
        var parts = new List<string>();
        if (!string.IsNullOrEmpty(GTA5_InstallPath_Steam_enhanced)) parts.Add("增强版-Steam");
        if (!string.IsNullOrEmpty(GTA5_InstallPath_Epic_enhanced)) parts.Add("增强版-Epic");
        if (!string.IsNullOrEmpty(GTA5_InstallPath_Steam)) parts.Add("传承版-Steam");
        if (!string.IsNullOrEmpty(GTA5_InstallPath_Epic)) parts.Add("传承版-Epic");

        if (parts.Count > 0)
        {
            TextBlock_Detected.Text = "已自动检测到：" + string.Join(" / ", parts);
            TextBlock_Detected.Foreground = System.Windows.Media.Brushes.SeaGreen;
        }
        else
        {
            TextBlock_Detected.Text = "未检测到游戏，请先运行一次游戏再打开本页";
            TextBlock_Detected.Foreground = System.Windows.Media.Brushes.OrangeRed;
        }
    }

    /// <summary>
    /// [DOG] 优先目标：增强版优先（Steam+Epic 都处理）；没有增强版再处理传承版
    /// </summary>
    private List<string> GetPreferredTargets()
    {
        var list = new List<string>();
        if (!string.IsNullOrEmpty(GTA5_InstallPath_Steam_enhanced)) list.Add(GTA5_InstallPath_Steam_enhanced);
        if (!string.IsNullOrEmpty(GTA5_InstallPath_Epic_enhanced)) list.Add(GTA5_InstallPath_Epic_enhanced);
        if (list.Count == 0)
        {
            if (!string.IsNullOrEmpty(GTA5_InstallPath_Steam)) list.Add(GTA5_InstallPath_Steam);
            if (!string.IsNullOrEmpty(GTA5_InstallPath_Epic)) list.Add(GTA5_InstallPath_Epic);
        }
        return list;
    }

    /// <summary>
    /// [DOG] 一键安装 FSL（自动检测平台与版本）
    /// </summary>
    private async void Button_AutoInstall_Click(object sender, RoutedEventArgs e)
    {
        var targets = GetPreferredTargets();
        if (targets.Count == 0)
        {
            AppendLogger("未检测到游戏安装路径：请先运行一次游戏，再打开本页面");
            return;
        }

        Button_AutoInstall.IsEnabled = false;
        try
        {
            foreach (var dir in targets)
            {
                AppendLogger("检测到安装目录：" + dir);

                RemoveFile(dir, true);

                var dst = Path.Combine(dir, "WINMM.dll");
                await Task.Run(() => File.Copy(FileHelper.File_FSL_WINMM, dst, true));

                ProgressBar_Download.Value = ProgressBar_Download.Maximum;
                TaskbarItemInfo.ProgressValue = 1;
                TextBlock_Percentage.Text = CoreUtil.GetFileForamtSize(new FileInfo(dst).Length);
                AppendLogger("安装完成（内置 FSL V6）：" + dst);
            }

            AppendLogger("√ 一键安装完成，下次启动 GTA 时 FSL 将自动加载");
        }
        catch (Exception ex)
        {
            AppendLogger("安装失败：" + ex.Message + "（请确认游戏已关闭、目录有写入权限）");
        }
        finally
        {
            Button_AutoInstall.IsEnabled = true;
        }
    }

    /// <summary>
    /// [DOG] 查找优先使用的游戏目录（无BE启动/快捷方式用）
    /// </summary>
    private string FindPreferredGameDir()
    {
        var targets = GetPreferredTargets();
        return targets.Count > 0 ? targets[0] : null;
    }

    /// <summary>
    /// [DOG] 以无 BattlEye 模式启动游戏（解决 Epic 平台“需要启动BE”弹窗）
    /// </summary>
    private void Button_LaunchNoBE_Click(object sender, RoutedEventArgs e)
    {
        var dir = FindPreferredGameDir();
        if (dir == null)
        {
            AppendLogger("未检测到游戏目录：请先运行一次游戏，再打开本页面");
            return;
        }

        var launcher = Path.Combine(dir, "PlayGTAV.exe");
        if (!File.Exists(launcher))
            launcher = Path.Combine(dir, "GTA5_Enhanced.exe");

        if (!File.Exists(launcher))
        {
            AppendLogger("未找到启动器（PlayGTAV.exe），请确认游戏目录完整");
            return;
        }

        try
        {
            Process.Start(new System.Diagnostics.ProcessStartInfo(launcher, "-nobattleye")
            {
                WorkingDirectory = dir,
                UseShellExecute = true
            });

            AppendLogger("已以无BattlEye模式启动游戏：" + launcher + " -nobattleye");
            AppendLogger("提示：进游戏后按 INSERT 呼出菜单；若仍提示需要BE，请先在 Rockstar 启动器设置中关闭 BattlEye");
        }
        catch (Exception ex)
        {
            AppendLogger("启动失败：" + ex.Message);
        }
    }

    /// <summary>
    /// [DOG] 生成“无BE启动”桌面快捷方式（Epic 用户双击即可，无需 Epic 启动器）
    /// </summary>
    private void Button_CreateShortcut_Click(object sender, RoutedEventArgs e)
    {
        var dir = FindPreferredGameDir();
        if (dir == null)
        {
            AppendLogger("未检测到游戏目录：请先运行一次游戏，再打开本页面");
            return;
        }

        var launcher = Path.Combine(dir, "PlayGTAV.exe");
        if (!File.Exists(launcher))
            launcher = Path.Combine(dir, "GTA5_Enhanced.exe");

        if (!File.Exists(launcher))
        {
            AppendLogger("未找到启动器（PlayGTAV.exe），请确认游戏目录完整");
            return;
        }

        var desktop = Environment.GetFolderPath(Environment.SpecialFolder.DesktopDirectory);
        var lnkPath = Path.Combine(desktop, "GTA5 无BE启动.lnk");

        try
        {
            Type shellType = Type.GetTypeFromProgID("WScript.Shell");
            dynamic shell = Activator.CreateInstance(shellType);
            var lnk = shell.CreateShortcut(lnkPath);
            lnk.TargetPath = launcher;
            lnk.Arguments = "-nobattleye";
            lnk.WorkingDirectory = dir;
            lnk.Description = "以无 BattlEye 模式启动 GTA5（小助手生成）";
            lnk.Save();

            AppendLogger("√ 已生成桌面快捷方式：" + lnkPath);
            AppendLogger("以后双击桌面的「GTA5 无BE启动」即可直接进入游戏（不需要 Epic/Steam）");
        }
        catch (Exception ex)
        {
            try
            {
                var batPath = Path.Combine(desktop, "GTA5 无BE启动.bat");
                File.WriteAllText(batPath,
                    "@echo off\r\ncd /d \"" + dir + "\"\r\nstart \"\" \"" + launcher + "\" -nobattleye\r\n");
                AppendLogger("快捷方式生成失败（" + ex.Message + "），已改为生成批处理：" + batPath);
            }
            catch (Exception ex2)
            {
                AppendLogger("生成失败：" + ex2.Message);
            }
        }
    }

    private void AppendLogger(string log)
    {
        TextBox_Logger.AppendText($"{log}\n");
        TextBox_Logger.ScrollToEnd();
    }

    private async void Button_StartDownload_Steam_Click(object sender, RoutedEventArgs e)
    {
        if (!string.IsNullOrEmpty(GTA5_InstallPath_Steam_enhanced))
        {
            RemoveFile(GTA5_InstallPath_Steam_enhanced, true);
            AppendLogger("开始为GTA5增强版-Steam 下载FSL");
            await StartDownload(GTA5_InstallPath_Steam_enhanced, true);
        }
        if (!string.IsNullOrEmpty(GTA5_InstallPath_Steam))
        {
            RemoveFile(GTA5_InstallPath_Steam, true);
            AppendLogger("开始为GTA5传承版-Steam 下载FSL");
            await StartDownload(GTA5_InstallPath_Steam, false);
        }

        Button_RM_FSL_Steam.IsEnabled = true;
    }

    private async void Button_StartDownload_Epic_Click(object sender, RoutedEventArgs e)
    {
        if (!string.IsNullOrEmpty(GTA5_InstallPath_Epic_enhanced))
        {
            RemoveFile(GTA5_InstallPath_Epic_enhanced, true);
            AppendLogger("开始为GTA5增强版-Epic 下载FSL");
            await StartDownload(GTA5_InstallPath_Epic_enhanced, true);
        }
        if (!string.IsNullOrEmpty(GTA5_InstallPath_Epic))
        {
            RemoveFile(GTA5_InstallPath_Epic, true);
            AppendLogger("开始为GTA5传承版-Epic 下载FSL");
            await StartDownload(GTA5_InstallPath_Epic, false);
        }

        Button_RM_FSL_Epic.IsEnabled = true;
    }

    private async Task StartDownload(string installPath, bool isEnhanced)
    {
        Button_StartDownload_Steam.IsEnabled = false;
        Button_StartDownload_Epic.IsEnabled = false;
        Button_RM_FSL_Steam.IsEnabled = false;
        Button_RM_FSL_Epic.IsEnabled = false;

        ResetUIState();

        tempPath = Path.Combine(installPath, "WINMM.dll");
        AppendLogger("开始安装内置 FSL V6 ...");

        try
        {
            await Task.Run(() =>
            {
                File.Copy(FileHelper.File_FSL_WINMM, tempPath, true);
            });

            ProgressBar_Download.Maximum = 100;
            ProgressBar_Download.Value = 100;
            TaskbarItemInfo.ProgressValue = 1;
            TextBlock_Percentage.Text = CoreUtil.GetFileForamtSize(new FileInfo(tempPath).Length);
            AppendLogger("安装完成（内置 FSL V6），下次启动 GTA 时 FSL 将自动加载");
        }
        catch (Exception ex)
        {
            AppendLogger($"安装失败，请确认小助手数据目录完整。错误信息: {ex.Message}");
        }
        finally
        {
            ResetButtons(installPath);
        }
    }

    private void ResetButtons(string installPath)
    {
        if (installPath == GTA5_InstallPath_Steam)
        {
            Button_GTA_STEAM_Dir.IsEnabled = true;
            Button_StartDownload_Steam.IsEnabled = true;
            Button_RM_FSL_Steam.IsEnabled = true;
        }
        else if (installPath == GTA5_InstallPath_Epic)
        {
            Button_GTA_EPIC_Dir.IsEnabled = true;
            Button_StartDownload_Epic.IsEnabled = true;
            Button_RM_FSL_Epic.IsEnabled = true;
        }
    }

    private void ResetUIState()
    {
        ProgressBar_Download.Maximum = 1024;
        ProgressBar_Download.Value = 0;

        TaskbarItemInfo.ProgressValue = 0;

        TextBlock_Percentage.Text = "0KB / 0MB";
    }

    private void Button_RM_FSL_Steam_Click(object sender, RoutedEventArgs e)
    {
        AppendLogger("尝试移除传承版FSL");
        RemoveFile(GTA5_InstallPath_Steam, false);
        AppendLogger("尝试移除增强版FSL");
        RemoveFile(GTA5_InstallPath_Steam_enhanced, false);
    }

    private void Button_RM_FSL_Epic_Click(object sender, RoutedEventArgs e)
    {
        AppendLogger("尝试移除传承版FSL");
        RemoveFile(GTA5_InstallPath_Epic, false);
        AppendLogger("尝试移除增强版FSL");
        RemoveFile(GTA5_InstallPath_Epic_enhanced, false);
    }

    private void RemoveFile(string installPath, bool is_upgrade)
    {
        string filePath = Path.Combine(installPath, "version.dll");
        if (File.Exists(filePath))
        {
            if (!FileHelper.IsOccupied(filePath))
            {
                File.Delete(filePath);
                AppendLogger("已删除旧版FSL: version.dll");
            }
            else
            {
                AppendLogger("version.dll被占用,无法删除,请先关闭GTA5.");
            }
        }

        if (is_upgrade)
            return;

        filePath = Path.Combine(installPath, "WINMM.dll");
        if (File.Exists(filePath))
        {
            if (!FileHelper.IsOccupied(filePath))
            {
                File.Delete(filePath);
                AppendLogger("已删除WINMM.dll");
            }
            else
            {
                AppendLogger("WINMM.dll被占用,无法删除请先关闭GTA5.");
            }
        }
        else
        {
            AppendLogger("WINMM.dll不存在,无需移除");
        }
    }

    private void Button_GTA_STEAM_Dir_Click(object sender, RoutedEventArgs e)
    {
        ProcessHelper.OpenDir(GTA5_InstallPath_Steam);
        ProcessHelper.OpenDir(GTA5_InstallPath_Steam_enhanced);
    }
    private void Button_FSL_Dir_Click(object sender, RoutedEventArgs e)
    {
        ProcessHelper.OpenDir(FileHelper.Dir_AppData_FSL);
    }
    private void Button_GTA_EPIC_Dir_Click(object sender, RoutedEventArgs e)
    {
        ProcessHelper.OpenDir(GTA5_InstallPath_Epic);
        ProcessHelper.OpenDir(GTA5_InstallPath_Epic_enhanced);
    }
}
