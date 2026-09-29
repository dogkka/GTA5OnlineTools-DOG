using GTA5OnlineTools.Data;
using GTA5OnlineTools.Utils;
using GTA5Shared.Helper;
using System.Net.Http;

namespace GTA5OnlineTools.Windows;

/// <summary>
/// OnlineLuaWindow.xaml 的交互逻辑
/// </summary>
public partial class OnlineLuaWindow
{
    private HttpClient _httpClient = new();

    private string tempPath = string.Empty;

    private const string yimMenu = $"https://blog.1007890.xyz/https://raw.githubusercontent.com/sch-lda/GTA5OnlineLua/refs/heads/main/Release//YimMenu.json";

    public ObservableCollection<LuaInfo> OnlineLuas { get; set; } = new();

    public OnlineLuaWindow()
    {
        InitializeComponent();
    }

    private void Window_OnlineLua_Loaded(object sender, RoutedEventArgs e)
    {
        Refresh_list();
    }

    private async void Window_OnlineLua_Closing(object sender, CancelEventArgs e)
    {
    }

    //////////////////////////////////////////////////////////

    private void ClearLogger()
    {
        TextBox_Logger.Clear();
    }

    private void AppendLogger(string log)
    {
        TextBox_Logger.AppendText($"{log}\n");
        TextBox_Logger.ScrollToEnd();
    }

    //////////////////////////////////////////////////////////


    private async void Refresh_list()
    {
        try
        {
            OnlineLuas.Clear();

            Button_StartDownload.IsEnabled = false;

            LoadingSpinner_Refush.IsLoading = true;

            string content;
            content = await HttpHelper.DownloadString(yimMenu);

            // 无网络情况下加载本地lua下载信息
            if (string.IsNullOrEmpty(content))
            {
                string local_index = "GTA5OnlineTools.Files.lua_index_yimmenu.json";

                var assembly = Assembly.GetExecutingAssembly();
                var stream = assembly.GetManifestResourceStream(local_index);

                using var reader = new StreamReader(stream);
                content = reader.ReadToEnd();

                stream.Close();

                ClearLogger();
                AppendLogger("Lua版本信息获取失败,但您下载的仍是最新版本.\n如果此问题持续存在,请尝试使用 工具-运行网络诊断 修复.");
                NotifierHelper.Show(NotifierType.Warning, "无法获取服务器Lua列表，返回结果为空");
            }

            var result = JsonHelper.JsonDese<List<LuaInfo>>(content);
            if (result != null)
            {
                foreach (var item in result)
                {
                    OnlineLuas.Add(item);
                }
            }

            ListBox_DownloadNode.Items.Clear();
            for (int i = 0; i < OnlineLuas.First().Download.Count; i++)
            {
                ListBox_DownloadNode.Items.Add($"节点{i + 1}");
            }
            ListBox_DownloadNode.SelectedIndex = 0;
        }
        catch (Exception ex)
        {
            AppendLogger($"获取服务器Lua列表异常：{ex.Message}");
            NotifierHelper.Show(NotifierType.Error, $"获取服务器Lua列表异常\n{ex.Message}");
        }
        finally
        {
            Button_StartDownload.IsEnabled = true;

            LoadingSpinner_Refush.IsLoading = false;
        }
    }
    private void Button_RefushList_Click(object sender, RoutedEventArgs e)
    {
        Refresh_list();
    }


    private async void Button_StartDownload_Click(object sender, RoutedEventArgs e)
    {
        var index = ListBox_DownloadAddress.SelectedIndex;
        if (index == -1)
        {
            AppendLogger("请选择要下载的内容，操作取消");
            NotifierHelper.Show(NotifierType.Warning, "请选择要下载的内容，操作取消");
            return;
        }

        var index2 = ListBox_DownloadNode.SelectedIndex;
        if (index2 == -1)
        {
            AppendLogger("请选择下载节点，操作取消");
            NotifierHelper.Show(NotifierType.Warning, "请选择下载节点，操作取消");
            return;
        }

        ClearLogger();

        StackPanel_ToggleOption.IsEnabled = false;
        Button_StartDownload.IsEnabled = false;

        ResetUIState();

        var adddress = OnlineLuas[index].Download[index2];

        AppendLogger($"Lua名称：{OnlineLuas[index].Name}");
        AppendLogger($"Lua大小: {OnlineLuas[index].Size}");

        FileHelper.CreateDirectory(FileHelper.Dir_AppData_YimMenu_Scripts);
        tempPath = Path.Combine(FileHelper.Dir_AppData_YimMenu_Scripts, "GTA5OnlineLua.zip");

        AppendLogger("开始下载...");
        _httpClient.DefaultRequestHeaders.UserAgent.ParseAdd("Downloader/1.1.2");
        try
        {
            using (var response = await _httpClient.GetAsync(adddress, HttpCompletionOption.ResponseHeadersRead))
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

            }


        }
        catch (Exception ex)
        {
            ResetUIState();

            StackPanel_ToggleOption.IsEnabled = true;
            Button_StartDownload.IsEnabled = true;

            AppendLogger($"下下载失败,请尝试使用 工具-运行网络诊断 修复。错误信息: {ex.Message}");
            NotifierHelper.Show(NotifierType.Error, $"下载失败\n{ex.Message}");
            return;
        }
        finally
        {
            StackPanel_ToggleOption.IsEnabled = true;
            Button_StartDownload.IsEnabled = true;
            try
            {
                AppendLogger("下载成功");
                AppendLogger("开始解压Lua中...");

                using var archive = ZipFile.OpenRead(tempPath);

                archive.ExtractToDirectory(FileHelper.Dir_AppData_YimMenu_Scripts, true);

                await Task.Delay(100);
                archive.Dispose();

                AppendLogger("解压成功");
                AppendLogger("开始删除临时文件中...");

                await Task.Delay(100);
                File.Delete(tempPath);

                AppendLogger("删除临时文件成功");
                AppendLogger("操作结束");
            }
            catch (Exception ex)
            {
                AppendLogger("解压时发生异常");
                AppendLogger($"异常信息：{ex.Message}");
            }
            finally
            {
                StackPanel_ToggleOption.IsEnabled = true;
                Button_StartDownload.IsEnabled = true;
            }
        }

    }

    //////////////////////////////////////////////////////////

    /// <summary>
    /// 重置UI显示
    /// </summary>
    private void ResetUIState()
    {
        ProgressBar_Download.Maximum = 1024;
        ProgressBar_Download.Value = 0;

        TaskbarItemInfo.ProgressValue = 0;

        TextBlock_Percentage.Text = "0KB / 0MB";
    }

    private void Button_ScriptDir_Click(object sender, RoutedEventArgs e)
    {


        ProcessHelper.OpenDir(FileHelper.Dir_AppData_YimMenu_Scripts);
    }

    private void Button_ClearScriptDir_Click(object sender, RoutedEventArgs e)
    {



        FileHelper.ClearDirectory(FileHelper.Dir_AppData_YimMenu_Scripts);
        NotifierHelper.Show(NotifierType.Success, "清空YimMenu Lua脚本文件夹成功");
    }
}
