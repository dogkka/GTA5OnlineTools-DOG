using GTA5Shared.Helper;

namespace GTA5OnlineTools.Windows;

/// <summary>
/// [DOG 离线版] 原来的“服务器公告”弹窗已改造为本地“关于 / 使用说明”，不联网。
/// </summary>
public partial class NotificationWindow
{
    public NotificationWindow()
    {
        InitializeComponent();
    }

    private void Button_Dismiss_Click(object sender, RoutedEventArgs e)
    {
        this.Close();
    }

    /// <summary>
    /// [DOG] 超链接：点击后用默认浏览器打开
    /// </summary>
    private void Hyperlink_RequestNavigate(object sender, RequestNavigateEventArgs e)
    {
        ProcessHelper.OpenLink(e.Uri.OriginalString);
        e.Handled = true;
    }
}
