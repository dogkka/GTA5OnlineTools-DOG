using GTA5Shared.Helper;

using GTA5OnlineTools.Utils;

namespace GTA5OnlineTools.Views;

/// <summary>
/// AboutView.xaml 的交互逻辑
/// </summary>
public partial class AboutView : UserControl
{
    /// <summary>
    /// 是否正在检查更新，防止连点
    /// </summary>
    private bool _updateChecking;

    public AboutView()
    {
        InitializeComponent();
        MainWindow.WindowClosingEvent += MainWindow_WindowClosingEvent;

        TextBlock_AboutVersion.Text = $"当前版本：{CoreUtil.ClientVersion}";
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
    /// [DOG] 检查更新（只访问本二改仓库的 GitHub 接口）
    /// </summary>
    private async void Button_CheckUpdate_Click(object sender, RoutedEventArgs e)
    {
        if (_updateChecking)
            return;

        _updateChecking = true;
        Button_AboutCheckUpdate.IsEnabled = false;

        try
        {
            await UpdateHelper.CheckAndPromptAsync();
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
        finally
        {
            Button_AboutCheckUpdate.IsEnabled = true;
            _updateChecking = false;
        }
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
