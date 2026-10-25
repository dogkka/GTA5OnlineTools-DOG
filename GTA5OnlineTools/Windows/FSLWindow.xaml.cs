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
        AppendLogger("开始下载...");

        try
        {
            string download_url = fsl_v2_url;
            using (var response = await _httpClient.GetAsync(download_url, HttpCompletionOption.ResponseHeadersRead))
            {
                response.EnsureSuccessStatusCode();
                var totalBytes = response.Content.Headers.ContentLength;

                using (var fileStream = new FileStream(tempPath, FileMode.Create, FileAccess.Write, FileShare.None))
                {
                    var buffer = new byte[81920];
                    var receivedBytes = 0L;
                    using (var stream = await response.Content.ReadAsStreamAsync())
                    {
                        int bytesRead;
                        while ((bytesRead = await stream.ReadAsync(buffer, 0, buffer.Length)) > 0)
                        {
                            await fileStream.WriteAsync(buffer, 0, bytesRead);
                            receivedBytes += bytesRead;

                            // 更新进度
                            ProgressBar_Download.Value = receivedBytes;
                            if (totalBytes.HasValue)
                            {
                                ProgressBar_Download.Maximum = totalBytes.Value;
                                TaskbarItemInfo.ProgressValue = receivedBytes / totalBytes.Value;
                                TextBlock_Percentage.Text = $"{CoreUtil.GetFileForamtSize(receivedBytes)} / {CoreUtil.GetFileForamtSize(totalBytes.Value)}";
                            }
                        }
                    }
                }

                AppendLogger("下载完成,下次启动GTA时FSL将自动注入");

            }
        }
        catch (Exception ex)
        {
            AppendLogger($"下载失败,请尝试使用 工具-运行网络诊断 修复。错误信息: {ex.Message}");
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
