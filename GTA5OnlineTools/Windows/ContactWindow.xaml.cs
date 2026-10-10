using GTA5Shared.Helper;

namespace GTA5OnlineTools.Windows;

/// <summary>
/// [DOG] 联系窗口：作者微信 / QQ 交流群 二维码
/// </summary>
public partial class ContactWindow
{
    public ContactWindow(int initialTab = 0)
    {
        InitializeComponent();

        if (initialTab >= 0 && initialTab < TabControl_Contact.Items.Count)
            TabControl_Contact.SelectedIndex = initialTab;
    }

    private void Button_Close_Click(object sender, RoutedEventArgs e)
    {
        this.Close();
    }

    /// <summary>
    /// [DOG] 复制 QQ 群号
    /// </summary>
    private void Button_CopyQQGroup_Click(object sender, RoutedEventArgs e)
    {
        try
        {
            Clipboard.SetText("1121769473");
            NotifierHelper.Show(NotifierType.Success, "QQ 交流群号 1121769473 已复制到剪贴板");
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
    }
}
