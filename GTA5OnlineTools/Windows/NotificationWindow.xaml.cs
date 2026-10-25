using GTA5OnlineTools.Models;
using GTA5OnlineTools.Utils;
using GTA5Shared.Helper;
using System.Security.Cryptography;

namespace GTA5OnlineTools.Windows;

/// <summary>
/// UpdateWindow.xaml 的交互逻辑
/// </summary>
public partial class NotificationWindow
{
    public string currentHash = string.Empty;
    public NotificationWindow()
    {
        InitializeComponent();
    }

    static string CalculateHash(string input)
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
    /// 更新窗口加载事件
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private async void Window_Notification_Loaded(object sender, RoutedEventArgs e)
    {
        try
        {
            var remote_notification = await HttpHelper.DownloadString("https://sstaticstp.1007890.xyz/golt_notification.txt");
            TextBlock_LatestNotification.Text = remote_notification;
            currentHash = CalculateHash(remote_notification);
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
    }

    private void Button_Dismiss_Click(object sender, RoutedEventArgs e)
    {
        if (string.IsNullOrEmpty(currentHash))
        {
            NotifierHelper.Show(NotifierType.Warning, "当前通知内容为空，请等待加载！");
            return;
        }
        IniHelper.WriteValue("Notification", "Hash", $"{currentHash}");
        
        this.Close();
    }

}