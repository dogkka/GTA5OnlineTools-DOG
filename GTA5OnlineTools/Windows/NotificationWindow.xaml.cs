using GTA5Shared.Helper;
using System.Windows.Input;

namespace GTA5OnlineTools.Windows;

/// <summary>
/// [DOG 离线版] 启动弹窗：展示作者的其他项目，倒计时结束前无法关闭
/// </summary>
public partial class NotificationWindow
{
    private DispatcherTimer timer;
    private int countdown = 5;
    private bool canClose = false;

    public NotificationWindow()
    {
        InitializeComponent();

        Loaded += Window_Loaded;
        Closing += Window_Closing;
    }

    /// <summary>
    /// 倒计时：5 秒后按钮才可点击
    /// </summary>
    private void Window_Loaded(object sender, RoutedEventArgs e)
    {
        countdown = 5;
        Button_Dismiss.Content = $"请稍候 {countdown} 秒…";

        timer = new DispatcherTimer
        {
            Interval = TimeSpan.FromSeconds(1)
        };
        timer.Tick += Timer_Tick;
        timer.Start();
    }

    private void Timer_Tick(object sender, EventArgs e)
    {
        countdown--;

        if (countdown <= 0)
        {
            timer.Stop();
            canClose = true;
            Button_Dismiss.IsEnabled = true;
            Button_Dismiss.Content = "我知道了，开始使用";
        }
        else
        {
            Button_Dismiss.Content = $"请稍候 {countdown} 秒…";
        }
    }

    /// <summary>
    /// 倒计时结束前禁止关闭（点右上角 X 也不行）
    /// </summary>
    private void Window_Closing(object sender, CancelEventArgs e)
    {
        if (!canClose)
            e.Cancel = true;
    }

    private void Button_Dismiss_Click(object sender, RoutedEventArgs e)
    {
        timer?.Stop();
        canClose = true;
        this.Close();
    }

    /// <summary>
    /// [DOG] 广告位：可言AI
    /// </summary>
    private void Border_AdDogBot_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
        ProcessHelper.OpenLink("https://dogbot.top/");
    }

    /// <summary>
    /// [DOG] 广告位：自研机场节点
    /// </summary>
    private void Border_AdAirport_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
        ProcessHelper.OpenLink("https://1825428229.share.123pan.cn/123pan/ZpBWjv-4r8Jd?notoken=1&pwd=BVUf");
    }

    /// <summary>
    /// [DOG] 打赏支持：打开联系窗口的打赏页
    /// </summary>
    private void Border_Reward_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
        new ContactWindow(2) { Owner = this }.ShowDialog();
    }
}
