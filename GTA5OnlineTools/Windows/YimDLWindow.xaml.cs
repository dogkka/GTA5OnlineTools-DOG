using GTA5OnlineTools.Data;
using GTA5OnlineTools.Models;
using GTA5OnlineTools.Utils;
using GTA5Shared.Helper;
using System.Net.Http;
using System.Runtime.Intrinsics.Arm;
using System.Security.Cryptography;
using System.Windows.Media;

namespace GTA5OnlineTools.Windows;

public partial class YimDLWindow
{
    private HttpClient _httpClient = new();
    private string YimmenuV2_url = "https://blog.1007890.xyz/https://github.com/sch-lda/yctest2/releases/download/CI/YimMenuV2.dll";
    private string YimmenuV1_url = "https://blog.1007890.xyz/https://github.com/sch-lda/yctest2/releases/download/CI/YimMenu.dll";

    public YimDLWindow()
    {
        InitializeComponent();
    }

    static string CalculateSHA256(string filePath)
    {
        using (SHA256 sha256 = SHA256.Create())
        {
            using (FileStream stream = File.OpenRead(filePath))
            {
                byte[] hashBytes = sha256.ComputeHash(stream);
                return BitConverter.ToString(hashBytes).Replace("-", "").ToLower();
            }
        }
    }

    private async void Window_YimDL_Loaded(object sender, RoutedEventArgs e)
    {
        TextBox_Logger.AppendText("内嵌版指构建小助手时打包进小助手的Yimmenu，随小助手更新。\n网络版指独立于小助手程序的Yimmenu，可单独更新Yimmenu而不用重新下载小助手。网络版dll文件命名时有_net后缀\n正在检查更新...\n");
        var YimmenuV2_path = FileHelper.File_YimMenu_DLL_V2_Online;

        if (!File.Exists(YimmenuV2_path))
        {
            TextBox_Logger.AppendText("YimMenuV2 不存在,开始下载...\n");

            bool is_succeed = await StartDownload(YimmenuV2_path, YimmenuV2_url);

            if (!is_succeed)
            {
                TextBox_Logger.AppendText("YimMenuV2 下载失败,请尝试使用 工具-运行网络诊断 修复。\n");
                return;
            }

            TextBox_Logger.AppendText("YimMenuV2 下载完成。\n");
        }

        var currentHash_V2 = "sha256:" + CalculateSHA256(YimmenuV2_path);
        var remote_digest_V2 = await HttpHelper.DownloadString("https://antfcc0.1007890.xyz/yimv2_hash");
        if (string.IsNullOrEmpty(remote_digest_V2))
        {
            TextBox_Logger.AppendText("YimMenuV2 云端版本获取失败，请检查互联网连接。检查更新失败\n");
            return;

        }
        if (remote_digest_V2 == currentHash_V2)
        {
            TextBox_Logger.AppendText("YimMenuV2 与云端一致。\n");
        }
        else
        {
            TextBox_Logger.AppendText("YimMenuV2 与云端不一致,开始重新下载...\n");
            if (!FileHelper.IsOccupied(YimmenuV2_path))
            {
                await StartDownload(YimmenuV2_path, YimmenuV2_url);
                TextBox_Logger.AppendText("YimMenuV2 重新下载完成。\n");
            }
            else
            {
                TextBox_Logger.AppendText("YimMenuV2 正在被占用,关闭GTA后重试。\n");
                return;
            }
        }

        var YimmenuV1_path = FileHelper.File_YimMenu_DLL_V1_Online;

        if (!File.Exists(YimmenuV1_path))
        {
            TextBox_Logger.AppendText("YimMenuV1 不存在,开始下载...\n");

            bool is_succeed = await StartDownload(YimmenuV1_path, YimmenuV1_url);
            if (!is_succeed) 
            {
                TextBox_Logger.AppendText("YimMenuV1 下载失败,请尝试使用 工具-运行网络诊断 修复。\n");
                return;
            }

            TextBox_Logger.AppendText("YimMenuV1 下载完成。\n");
        }

        var currentHash_V1 = "sha256:" + CalculateSHA256(YimmenuV1_path);
        var remote_digest_V1 = await HttpHelper.DownloadString("https://antfcc0.1007890.xyz/yimv1_hash");
        if (string.IsNullOrEmpty(remote_digest_V1))
        {
            TextBox_Logger.AppendText("YimMenuV1 云端版本获取失败，请检查互联网连接。检查更新失败\n");
            return;
        }
        if (remote_digest_V1 == currentHash_V1)
        {
            TextBox_Logger.AppendText("YimMenuV1 与云端一致。\n");
        }
        else
        {
            TextBox_Logger.AppendText("YimMenuV1 与云端不一致,开始重新下载...\n");
            if (!FileHelper.IsOccupied(YimmenuV1_path))
            {
                await StartDownload(YimmenuV1_path, YimmenuV1_url);
                TextBox_Logger.AppendText("YimMenuV1 重新下载完成。\n");
            }
            else
            {
                TextBox_Logger.AppendText("YimMenuV1 正在被占用,关闭GTA后重试。\n");
                return;
            }
        }
        TextBox_Logger.AppendText("检查更新结束。\n");
    }

    private void AppendLogger(string log)
    {
        TextBox_Logger.AppendText($"{log}\n");
        TextBox_Logger.ScrollToEnd();
    }


    private async Task<bool> StartDownload(string FilePath, string url)
    {

        ResetUIState();
        _httpClient.DefaultRequestHeaders.UserAgent.ParseAdd("Downloader/1.1.2");

        try
        {
            using (var response = await _httpClient.GetAsync(url, HttpCompletionOption.ResponseHeadersRead))
            {
                response.EnsureSuccessStatusCode();
                var totalBytes = response.Content.Headers.ContentLength;

                using (var fileStream = new FileStream(FilePath, FileMode.Create, FileAccess.Write, FileShare.None))
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

            }
            return true;
        }
        catch (Exception ex)
        {
            AppendLogger($"下载失败,请尝试使用 工具-运行网络诊断 修复。错误信息: {ex.Message}");
            return false;
        }
        finally
        {
            // ResetButtons(FilePath);
        }
    }

    private void ResetUIState()
    {
        ProgressBar_Download.Maximum = 1024;
        ProgressBar_Download.Value = 0;

        TaskbarItemInfo.ProgressValue = 0;

        TextBlock_Percentage.Text = "0KB / 0MB";
    }

    private void Button_Extract_Yimmenu_Dir_Click(object sender, RoutedEventArgs e)
    {
        var filePath = Path.Combine(FileHelper.Dir_Base, "YimMenu");
        ProcessHelper.OpenDir(filePath);

    }
}
