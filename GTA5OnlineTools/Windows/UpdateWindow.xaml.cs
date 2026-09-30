using GTA5OnlineTools.Utils;

using GTA5Shared.Helper;

namespace GTA5OnlineTools.Windows;

/// <summary>
/// [DOG] 发现新版本时的提示窗口
/// 检测结果由 UpdateHelper 传入，本窗口只负责显示和打开下载地址
/// </summary>
public partial class UpdateWindow
{
    private readonly UpdateCheckResult _result;

    public UpdateWindow(UpdateCheckResult result)
    {
        InitializeComponent();

        _result = result;

        LoadResult();
    }

    /// <summary>
    /// 把检测结果显示到界面上
    /// </summary>
    private void LoadResult()
    {
        if (_result == null)
            return;

        TextBlock_Title.Text = $"发现新版本 {_result.RemoteVersionText}";
        TextBlock_Local.Text = _result.LocalVersionText;
        TextBlock_Remote.Text = _result.RemoteVersionText;
        TextBlock_Date.Text = _result.PublishedText;
        TextBlock_Asset.Text = $"{_result.DownloadNameText}（{_result.DownloadSizeText}）";
        TextBlock_Notes.Text = _result.NotesText;

        // 没拿到直链的话就只留“打开发布页面”
        Button_Copy.IsEnabled = !string.IsNullOrWhiteSpace(_result.DownloadUrl);
        Button_Skip.ToolTip = "下次启动不再提示这个版本，之后可以在“选项”页手动检查更新";
    }

    /// <summary>
    /// 打开发布页面
    /// </summary>
    private void Button_Page_Click(object sender, RoutedEventArgs e)
    {
        var url = string.IsNullOrWhiteSpace(_result?.ReleasePage) ? UpdateHelper.LatestPageUrl : _result.ReleasePage;

        TextBlock_Status.Text = $"已用默认浏览器打开：{url}";
        ProcessHelper.OpenLink(url);
    }

    /// <summary>
    /// 复制下载链接（浏览器打不开时可以贴到下载工具里）
    /// </summary>
    private void Button_Copy_Click(object sender, RoutedEventArgs e)
    {
        var url = string.IsNullOrWhiteSpace(_result?.DownloadUrl) ? _result?.ReleasePage : _result.DownloadUrl;

        if (string.IsNullOrWhiteSpace(url))
        {
            NotifierHelper.Show(NotifierType.Warning, "没有拿到下载链接，请点“打开发布页面”手动下载");
            return;
        }

        try
        {
            Clipboard.SetText(url);
            TextBlock_Status.Text = $"下载链接已复制到剪贴板：{url}";
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
    }

    /// <summary>
    /// 跳过这个版本
    /// </summary>
    private void Button_Skip_Click(object sender, RoutedEventArgs e)
    {
        UpdateHelper.SkipVersion(_result);
        TextBlock_Status.Text = $"已跳过 {_result?.RemoteVersionText}，之后可以在“选项”页手动检查更新";

        this.Close();
    }

    private void Button_Close_Click(object sender, RoutedEventArgs e)
    {
        this.Close();
    }
}
