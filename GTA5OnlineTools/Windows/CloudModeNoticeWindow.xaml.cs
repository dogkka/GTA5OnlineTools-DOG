namespace GTA5OnlineTools.Windows;

/// <summary>
/// [DOG] 云端数据模式使用须知：勾选确认后才允许切换
/// </summary>
public partial class CloudModeNoticeWindow
{
    /// <summary>
    /// 用户是否已勾选同意并确认
    /// </summary>
    public bool Confirmed { get; private set; }

    public CloudModeNoticeWindow()
    {
        InitializeComponent();
    }

    private void CheckBox_Agree_Changed(object sender, RoutedEventArgs e)
    {
        if (Button_Confirm != null)
            Button_Confirm.IsEnabled = CheckBox_Agree.IsChecked == true;
    }

    private void Button_Cancel_Click(object sender, RoutedEventArgs e)
    {
        Confirmed = false;
        // [DOG] 必须设置 DialogResult：仅 Close() 时 ShowDialog() 返回 null，调用方会误判为未确认
        DialogResult = false;
    }

    private void Button_Confirm_Click(object sender, RoutedEventArgs e)
    {
        if (CheckBox_Agree.IsChecked != true)
            return;

        Confirmed = true;
        // [DOG] 同上：设置 DialogResult 才会让 ShowDialog() 返回 true，并自动关闭窗口
        DialogResult = true;
    }
}
