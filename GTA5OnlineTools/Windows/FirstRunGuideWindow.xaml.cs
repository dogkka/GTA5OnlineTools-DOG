namespace GTA5OnlineTools.Windows;

/// <summary>
/// [DOG] 新手引导：首次启动且未检测到 FSL 配置时提示一次（三步上手 + 直达 FSL 管理）
/// </summary>
public partial class FirstRunGuideWindow
{
    public FirstRunGuideWindow()
    {
        InitializeComponent();
    }

    private void Button_OpenFSL_Click(object sender, RoutedEventArgs e)
    {
        Close();
        new FSLWindow { Owner = MainWindow.MainWindowInstance }.ShowDialog();
    }

    private void Button_Later_Click(object sender, RoutedEventArgs e)
    {
        Close();
    }
}
