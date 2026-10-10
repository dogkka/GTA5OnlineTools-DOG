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
    /// [DOG] 一键卸载：清理全部预设位置（配置 / 缓存 / 日志 / 已释放的菜单文件），
    /// 完成后提示删除程序文件并自动退出。
    /// </summary>
    private void Button_Uninstall_Click(object sender, RoutedEventArgs e)
    {
        var gameRunning = Process.GetProcessesByName("GTA5").Length > 0
                          || Process.GetProcessesByName("GTA5_Enhanced").Length > 0;

        var confirmText =
            "将清理本程序在所有预设位置生成的文件：\n\n" +
            $"① {FileHelper.Dir_Base}\n（配置 / 缓存 / 日志 / 已释放的菜单 DLL / FSL 文件）\n" +
            $"② {FileHelper.Dir_AppData_YimMenu}\n（传承版菜单配置与日志，含脚本与翻译文件）\n" +
            $"③ {FileHelper.Dir_AppData_YimMenu_V2}\n（增强版菜单配置与缓存）\n" +
            "④ 桌面快捷方式「GTA5 无BE启动」（如存在）\n\n" +
            "不会删除：FSL 本地存档（%appdata%\\FSL）、游戏存档、游戏目录内的 FSL。\n" +
            "如需移除游戏目录中的 FSL，请先到「FSL 管理」操作。\n\n" +
            "如有重要文件请提前备份。清理完成后本程序将自动退出，删除程序文件即可完成卸载。\n";

        if (gameRunning)
            confirmText += "\n⚠ 检测到游戏正在运行：部分文件可能被占用而无法删除，建议先关闭游戏。\n";

        confirmText += "\n确定要执行一键卸载清理吗？";

        if (MessageBox.Show(confirmText, "一键卸载 — 确认清理",
                MessageBoxButton.YesNo, MessageBoxImage.Warning) != MessageBoxResult.Yes)
            return;

        Button_Uninstall.IsEnabled = false;

        try
        {
            var (allOk, report) = UninstallHelper.RunCleanup();

            if (allOk)
            {
                var doneText = "配置与缓存已全部清理完成！\n\n" + string.Join("\n", report);
                doneText += "\n\n最后一步：删除本程序文件即可完成卸载。\n" +
                            "点击【确定】后将打开程序所在文件夹，随后本程序自动退出；\n" +
                            "直接删除该文件夹（或其中的 exe）即可。";

                MessageBox.Show(doneText, "一键卸载 — 清理完成",
                    MessageBoxButton.OK, MessageBoxImage.Information);

                // 打开程序所在文件夹，便于用户删除；随后退出，避免 exe 被占用无法删除
                try
                {
                    var exeDir = Path.GetDirectoryName(Environment.ProcessPath);
                    if (!string.IsNullOrEmpty(exeDir))
                        ProcessHelper.OpenDir(exeDir);
                }
                catch
                {
                    // 打开文件夹失败不影响卸载流程
                }

                Application.Current.Shutdown();
            }
            else
            {
                var partText = "清理完成，但有部分文件被占用未能删除（见下方列表）：\n\n"
                               + string.Join("\n", report)
                               + "\n\n建议关闭游戏 / 菜单后，重新点一次「一键卸载」重试；"
                               + "也可手动删除上述文件，然后删除本程序即可完成卸载。";

                MessageBox.Show(partText, "一键卸载 — 部分完成",
                    MessageBoxButton.OK, MessageBoxImage.Warning);

                Button_Uninstall.IsEnabled = true;
            }
        }
        catch (Exception ex)
        {
            Button_Uninstall.IsEnabled = true;
            NotifierHelper.ShowException(ex);
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
