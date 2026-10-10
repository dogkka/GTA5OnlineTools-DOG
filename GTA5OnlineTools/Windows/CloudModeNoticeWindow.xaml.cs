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
        Close();
    }

    private void Button_Confirm_Click(object sender, RoutedEventArgs e)
    {
        if (CheckBox_Agree.IsChecked != true)
            return;

        Confirmed = true;
        Close();
    }
}
