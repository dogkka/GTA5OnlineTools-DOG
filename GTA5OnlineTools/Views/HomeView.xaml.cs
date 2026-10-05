using GTA5Shared.Helper;
using System.Windows.Input;

namespace GTA5OnlineTools.Views;

/// <summary>
/// HomeView.xaml 的交互逻辑
/// </summary>
public partial class HomeView : UserControl
{
    private readonly StringBuilder builder = new();

    public HomeView()
    {
        InitializeComponent();
        MainWindow.WindowClosingEvent += MainWindow_WindowClosingEvent;
    }

    private void MainWindow_WindowClosingEvent()
    {

    }

    /// <summary>
    /// 超链接请求导航事件
    /// </summary>
    private void Hyperlink_RequestNavigate(object sender, RequestNavigateEventArgs e)
    {
        ProcessHelper.OpenLink(e.Uri.OriginalString);
        e.Handled = true;
    }

    /// <summary>
    /// [DOG] 打开使用教程（HTML 图文教程）
    /// </summary>
    private void Border_OpenTutorial_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
        TutorialHelper.OpenTutorial();
    }
}
