using GTA5OnlineTools.Utils;

using GTA5Shared.Helper;

namespace GTA5OnlineTools.Views;

/// <summary>
/// OptionsView.xaml 的交互逻辑
/// </summary>
public partial class OptionsView : UserControl
{
    /// <summary>
    /// 是否正在检查更新，防止连点
    /// </summary>
    private bool _updateChecking;

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
    /// [DOG] 手动检查更新（只访问本二改仓库的 GitHub 接口）
    /// </summary>
    private async void Button_CheckUpdate_Click(object sender, RoutedEventArgs e)
    {
        if (_updateChecking)
            return;

        _updateChecking = true;
        Button_CheckUpdate.IsEnabled = false;

        try
        {
            var result = await UpdateHelper.CheckAndPromptAsync(text => TextBlock_UpdateState.Text = $"状态：{text}");

            TextBlock_Latest.Text = result.RemoteVersionText;
        }
        catch (Exception ex)
        {
            TextBlock_UpdateState.Text = "状态：检查更新失败";
            NotifierHelper.ShowException(ex);
        }
        finally
        {
            Button_CheckUpdate.IsEnabled = true;
            _updateChecking = false;
        }
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
