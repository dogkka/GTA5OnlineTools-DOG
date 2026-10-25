using GTA5OnlineTools.Utils;

using GTA5Shared.Helper;

namespace GTA5OnlineTools.Views;

/// <summary>
/// OptionsView.xaml 的交互逻辑
/// </summary>
public partial class OptionsView : UserControl
{
    public OptionsView()
    {
        InitializeComponent();
        MainWindow.WindowClosingEvent += MainWindow_WindowClosingEvent;


        TextBlock_Computer.Text = $"{Environment.UserName}";
        TextBlock_Runtime.Text = $"{RuntimeInformation.FrameworkDescription}";
        TextBlock_Admin.Text = $"{CoreUtil.GetAdminState()}";
        TextBlock_Version.Text = $"{CoreUtil.ClientVersion}";
        TextBlock_Build.Text = $"{CoreUtil.BuildDate}";
    }

    /// <summary>
    /// 主窗口关闭事件
    /// </summary>
    private void MainWindow_WindowClosingEvent()
    {
        SaveConfig();
    }

    /// <summary>
    /// 保存配置文件
    /// </summary>
    private void SaveConfig()
    {
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

    private void RadioButton_ClickAudio_Click(object sender, RoutedEventArgs e)
    {

    }
}
