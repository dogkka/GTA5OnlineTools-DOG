using CommunityToolkit.Mvvm.Input;
using GTA5OnlineTools.Data;
using GTA5OnlineTools.Models;
using GTA5OnlineTools.Utils;
using GTA5OnlineTools.Views;
using GTA5Shared.Helper;
using System.Security.Cryptography;
using System.Windows.Input;
using System.Windows.Media;
using GTA5OnlineTools.Windows;

namespace GTA5OnlineTools;

/// <summary>
/// MainWindow.xaml 的交互逻辑
/// </summary>
public partial class MainWindow
{
    /// <summary>
    /// 主窗口数据模型
    /// </summary>
    public MainModel MainModel { get; set; } = new();

    ///////////////////////////////////////////////////////////////

    /// <summary>
    /// 导航字典
    /// </summary>
    private readonly Dictionary<string, UserControl> NavDictionary = new();

    ///////////////////////////////////////////////////////////////

    /// <summary>
    /// 主程序是否在运行，用于结束线程内循环
    /// </summary>
    public static bool IsAppRunning = true;

    /// <summary>
    /// 主窗口关闭事件
    /// </summary>
    public static event Action WindowClosingEvent;

    /// <summary>
    /// 用于向外暴露主窗口实例
    /// </summary>
    public static Window MainWindowInstance { get; private set; }

    /// <summary>
    /// 存储软件开始运行的时间
    /// </summary>
    private DateTime OriginDateTime;

    ///////////////////////////////////////////////////////////////

    public MainWindow()
    {
        InitializeComponent();

        CreateView();
    }

    /// <summary>
    /// 窗口加载完成事件
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void Window_Main_Loaded(object sender, RoutedEventArgs e)
    {
        // 向外暴露主窗口实例
        MainWindowInstance = this;

        // 首页导航
        Navigate(NavDictionary.First().Key);

        // 获取当前时间，存储到对于变量中
        OriginDateTime = DateTime.Now;

        ///////////////////////////////////////////////////////////////

        // 设置主窗口标题
        this.Title = CoreUtil.MainAppWindowName + CoreUtil.ClientVersion;

        MainModel.AppRunTime = "00:00:00";
        MainModel.AppVersion = CoreUtil.ClientVersion;

        MainModel.Status = "检查完整性...";

        // 更新主窗口UI线程
        new Thread(UpdateUiThread)
        {
            Name = "UpdateUiThread",
            IsBackground = true
        }.Start();

        // 检查GTA5是否运行线程
        new Thread(CheckGTA5IsRunThread)
        {
            Name = "CheckGTA5IsRunThread",
            IsBackground = true
        }.Start();

        // 检查更新线程
        new Thread(CheckUpdateThread)
        {
            Name = "CheckUpdateThread",
            IsBackground = true
        }.Start();
        
    }

    /// <summary>
    /// 窗口关闭事件
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void Window_Main_Closing(object sender, CancelEventArgs e)
    {
        // 终止线程内循环
        IsAppRunning = false;

        WindowClosingEvent();
        LoggerHelper.Info("调用主窗口关闭事件成功");

        GTA5View.ActionCloseAllGTA5Window();
        LoggerHelper.Info("关闭小助手功能窗口成功");

        ProcessHelper.CloseThirdProcess();
        LoggerHelper.Info("关闭第三方进程成功");

        Application.Current.Shutdown();
        LoggerHelper.Info("主程序关闭\n\n");
    }

    ///////////////////////////////////////////////////////////////

    /// <summary>
    /// 创建页面
    /// </summary>
    private void CreateView()
    {
        foreach (var item in ControlHelper.GetControls(StackPanel_NavMenu).Cast<RadioButton>())
        {
            var viewName = item.CommandParameter.ToString();

            if (NavDictionary.ContainsKey(viewName))
                continue;

            var viewType = Type.GetType($"GTA5OnlineTools.Views.{viewName}");
            if (viewType == null)
                continue;

            NavDictionary.Add(viewName, Activator.CreateInstance(viewType) as UserControl);
        }
    }

    /// <summary>
    /// View页面导航
    /// </summary>
    /// <param name="viewName"></param>
    [RelayCommand]
    private void Navigate(string viewName)
    {
        if (!NavDictionary.ContainsKey(viewName))
            return;

        if (ContentControl_NavRegion.Content == NavDictionary[viewName])
            return;

        ContentControl_NavRegion.Content = NavDictionary[viewName];
    }

    /// <summary>
    /// 更新主窗口UI线程
    /// </summary>
    private void UpdateUiThread()
    {
        while (IsAppRunning)
        {
            // 获取软件运行时间
            MainModel.AppRunTime = CoreUtil.ExecDateDiff(OriginDateTime, DateTime.Now);

            Thread.Sleep(1000);
        }
    }

    /// <summary>
    /// 检查GTA5是否运行线程
    /// </summary>
    private void CheckGTA5IsRunThread()
    {
        bool isExecute = true;

        while (IsAppRunning)
        {
            // 判断 GTA5 是否运行
            MainModel.IsGTA5Run = ProcessHelper.IsGTA5Run();

            if (MainModel.IsGTA5Run)
            {
                isExecute = false;
            }
            else
            {
                // 下列功能只会在GTA5停止运行时执行一次，直到下一次GTA5停止运行
                if (!isExecute)
                {
                    isExecute = true;
                    GTA5View.ActionCloseAllGTA5Window();
                }
            }

            Thread.Sleep(1000);
        }
    }

    static string CalculateSHA256_File(string filePath)
    {
        using (SHA256 sha256 = SHA256.Create())
        {
            using (FileStream stream = File.OpenRead(filePath))
            {
                byte[] hashBytes = sha256.ComputeHash(stream);
                return BitConverter.ToString(hashBytes).Replace("-", "").ToLower();
            }
        }
    }
    static string CalculateHash_String(string input)
    {
        // 选择哈希算法（这里使用SHA256）
        using (SHA256 sha256 = SHA256.Create())
        {
            // 将字符串转换为字节数组
            byte[] inputBytes = Encoding.UTF8.GetBytes(input);

            // 计算哈希值
            byte[] hashBytes = sha256.ComputeHash(inputBytes);

            // 将字节数组转换为十六进制字符串
            StringBuilder sb = new StringBuilder();
            foreach (byte b in hashBytes)
            {
                sb.Append(b.ToString("x2")); // "x2"表示两位小写的十六进制
            }

            return sb.ToString();
        }
    }

    /// <summary>
    /// 检查更新线程
    /// </summary>

    private async void CheckUpdateThread()
    {
        try
        {
            LoggerHelper.Info("开始检查新公告...");
            var remote_notification = await HttpHelper.DownloadString("https://sstaticstp.1007890.xyz/golt_notification.txt");
            var NotificationHash = CalculateHash_String(remote_notification);
            var LocalNotificationHash = IniHelper.ReadValue("Notification", "Hash");
            if (string.IsNullOrEmpty(LocalNotificationHash) || LocalNotificationHash != NotificationHash)
            {
                LoggerHelper.Info("发现未读公告，显示通知窗口...");
                this.Dispatcher.Invoke(() =>
                {
                    var NotificationWindow = new NotificationWindow
                    {
                        Owner = this
                    };
                    NotificationWindow.ShowDialog();
                });
            }
            else
            {
                LoggerHelper.Info("没有新公告");
            }

            var self_path = Environment.ProcessPath;
            LoggerHelper.Debug($"当前软件路径: {self_path}");
            string sha256 = "sha256:" + CalculateSHA256_File(self_path);
            LoggerHelper.Info($"当前软件SHA256: {sha256}");
            LoggerHelper.Info("获取云端构建信息...");

            var remote_digest = await HttpHelper.DownloadString("https://antfcc0.1007890.xyz/golt_hash");
            LoggerHelper.Info($"云端构建信息: {remote_digest}");
            if (string.IsNullOrEmpty(remote_digest))
            {
                MainModel.Status = "网络连接失败";
                return;
            }
            if (remote_digest == sha256)
            {
                LoggerHelper.Info("当前软件已是最新版本");
                MainModel.Status = "小助手已最新";
            }
            else
            {
                LoggerHelper.Warn("当前软件不是最新版本");
                Application.Current.Dispatcher.Invoke(() =>
                {
                    Status_Text.Foreground = new SolidColorBrush(Colors.Purple); // 使用Color结构
                    MainModel.Status = "小助手不是最新！";
                });
            }

            var YimmenuV2_path = FileHelper.File_YimMenu_DLL_V2_Online;

            if (File.Exists(YimmenuV2_path)) 
            {
                var currentHash_V2 = "sha256:" + CalculateSHA256_File(YimmenuV2_path);
                var remote_digest_V2 = await HttpHelper.DownloadString("https://antfcc0.1007890.xyz/yimv2_hash");
                if (string.IsNullOrEmpty(remote_digest_V2))
                {
                    MainModel.Status += "YimV2状态未知.";
                }
                if (remote_digest_V2 == currentHash_V2)
                {
                    MainModel.Status += "YimV2已最新.";
                }
                else
                {
                    Application.Current.Dispatcher.Invoke(() =>
                    {
                        Status_Text.Foreground = new SolidColorBrush(Colors.Red); // 使用Color结构
                    });
                    MainModel.Status += "YimV2需更新.";
                }

            }

            var YimmenuV1_path = FileHelper.File_YimMenu_DLL_V1_Online;

            if (File.Exists(YimmenuV1_path))
            {
                var currentHash_V1 = "sha256:" + CalculateSHA256_File(YimmenuV1_path);
                var remote_digest_V1 = await HttpHelper.DownloadString("https://antfcc0.1007890.xyz/yimv1_hash");
                if (string.IsNullOrEmpty(remote_digest_V1))
                {
                    MainModel.Status += "YimV1状态未知.";
                }
                if (remote_digest_V1 == currentHash_V1)
                {
                    MainModel.Status += "YimV1已最新.";
                }
                else
                {
                    Application.Current.Dispatcher.Invoke(() =>
                    {
                        Status_Text.Foreground = new SolidColorBrush(Colors.Red); // 使用Color结构
                    });

                    MainModel.Status += "YimV1需更新.";
                }
            }

        }
        catch (Exception ex)
        {
            LoggerHelper.Error("获取云端构建信息失败", ex);
            return;
        }

        return;
    }

    /// <summary>
    /// 超链接请求导航事件
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void Hyperlink_RequestNavigate(object sender, RequestNavigateEventArgs e)
    {
        LoggerHelper.Info($"打开链接: {e.Uri.OriginalString}");
        ProcessHelper.OpenLink(e.Uri.OriginalString);
        e.Handled = true;
    }
}
