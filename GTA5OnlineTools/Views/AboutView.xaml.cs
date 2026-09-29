using GTA5Shared.Helper;

namespace GTA5OnlineTools.Views;

/// <summary>
/// AboutView.xaml 的交互逻辑
/// </summary>
public partial class AboutView : UserControl
{
    public AboutView()
    {
        InitializeComponent();
        MainWindow.WindowClosingEvent += MainWindow_WindowClosingEvent;
    }

    private void MainWindow_WindowClosingEvent()
    {

    }

    /// <summary>
    /// [DOG] 打开使用说明窗口
    /// </summary>
    private void Button_About_Click(object sender, RoutedEventArgs e)
    {
        new Windows.NotificationWindow
        {
            Owner = MainWindow.MainWindowInstance
        }.ShowDialog();
    }

    /// <summary>
    /// 超链接请求导航事件
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void Hyperlink_RequestNavigate(object sender, RequestNavigateEventArgs e)
    {
        ProcessHelper.OpenLink(e.Uri.OriginalString);
        e.Handled = true;
    }
}
