using GTA5OnlineTools.Windows;
using GTA5OnlineTools.Utils;
using GTA5Shared.Helper;
using System.Windows.Input;
using System.Windows.Media;

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
        Loaded += (_, _) => RefreshDataModeUI();
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

    /// <summary>
    /// [DOG] 打赏支持：打开联系窗口的打赏页
    /// </summary>
    private void Border_Reward_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
        new ContactWindow(2) { Owner = MainWindow.MainWindowInstance }.ShowDialog();
    }

    /// <summary>
    /// [DOG] 环境自检：一键体检（游戏 / FSL / 脚本 / 数据模式 / 呼出键）
    /// </summary>
    private void Border_HealthCheck_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
        new HealthCheckWindow { Owner = MainWindow.MainWindowInstance }.ShowDialog();
    }

    /// <summary>
    /// [DOG] 刷新主页数据模式显示（本地数据 / 云端数据）
    /// </summary>
    private void RefreshDataModeUI()
    {
        try
        {
            var mode = FslModeHelper.GetCurrentMode();

            TextBlock_ModeStatus.Text = mode switch
            {
                true => "当前：本地数据模式",
                false => "当前：云端数据模式",
                _ => "当前：未检测到 FSL 配置"
            };

            TextBlock_ModeStatus.Foreground = new SolidColorBrush(mode switch
            {
                true => Color.FromRgb(0x21, 0xA3, 0x66),
                false => Color.FromRgb(0xFE, 0x78, 0x0A),
                _ => Color.FromRgb(0x80, 0x8A, 0x93)
            });

            // [DOG] 未初始化时给出可操作引导
            TextBlock_ModeDesc.Text = mode is null
                ? "还没有可用数据模式：完成「关闭 BattlEye → 一键安装 FSL → 进一次游戏」后，回来点卡片二选一（也可点下方「环境自检」一键排查）。"
                : "「本地数据 / 云端数据」对传承版与增强版均适用：两种模式都在「关闭 BattlEye + FSL」下进入线上，区别只有存档去向；切换后需重启游戏生效。";

            var localActive = mode == true;
            var cloudActive = mode == false;

            Card_ModeLocal.Background = new SolidColorBrush(localActive ? Color.FromRgb(0xE9, 0xF7, 0xF0) : Colors.White);
            Card_ModeLocal.BorderBrush = new SolidColorBrush(localActive ? Color.FromRgb(0x21, 0xA3, 0x66) : Color.FromRgb(0xD5, 0xDA, 0xE0));
            Badge_ModeLocal.Text = localActive ? "✔ 当前使用中" : "点击切换";

            Card_ModeCloud.Background = new SolidColorBrush(cloudActive ? Color.FromRgb(0xFF, 0xF1, 0xE2) : Colors.White);
            Card_ModeCloud.BorderBrush = new SolidColorBrush(cloudActive ? Color.FromRgb(0xFE, 0x78, 0x0A) : Color.FromRgb(0xD5, 0xDA, 0xE0));
            Badge_ModeCloud.Text = cloudActive ? "✔ 当前使用中" : "点击切换";
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
    }

    /// <summary>
    /// [DOG] 切换数据模式：云端模式需先阅读使用须知并勾选确认
    /// </summary>
    private void SwitchDataMode(bool local)
    {
        try
        {
            var current = FslModeHelper.GetCurrentMode();
            if (current == local)
            {
                NotifierHelper.Show(NotifierType.Information, $"当前已是{FslModeHelper.Describe(local)}。");
                return;
            }

            // [DOG] 后端兜底：游戏运行中切换数据模式，FSL 需重启游戏才生效，先提醒
            if (ProcessHelper.IsGTA5Run())
            {
                var result = MessageBox.Show(
                    "检测到 GTA5 正在运行。\n\n" +
                    "数据模式修改会在 FSL 下次加载（重新启动游戏）后才生效；\n" +
                    "游戏运行中切换，当前对局仍按旧模式写入数据。\n\n" +
                    "建议：先退出游戏，再切换模式。\n\n" +
                    "是否仍要继续切换？",
                    "游戏运行中 — 数据模式切换",
                    MessageBoxButton.YesNo,
                    MessageBoxImage.Warning);

                if (result != MessageBoxResult.Yes)
                    return;
            }

            if (!local)
            {
                var notice = new CloudModeNoticeWindow { Owner = MainWindow.MainWindowInstance };
                if (notice.ShowDialog() != true || !notice.Confirmed)
                    return;
            }

            var (ok, message) = FslModeHelper.Apply(local);
            NotifierHelper.Show(ok ? NotifierType.Success : NotifierType.Error, message);

            if (ok)
                RefreshDataModeUI();
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
    }

    private void Card_ModeLocal_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
        SwitchDataMode(true);
    }

    private void Card_ModeCloud_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
        SwitchDataMode(false);
    }
}
