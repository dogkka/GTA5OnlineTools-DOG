using GTA5OnlineTools.Utils;
using GTA5Shared.Helper;

namespace GTA5OnlineTools.Windows;

/// <summary>
/// [DOG] 联系窗口：作者微信 / QQ 交流群（仅群号，无二维码）/ 打赏
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
    /// [DOG] 复制 QQ 群号（按钮 Tag 携带群号：主群 / 备用1 / 备用2）
    /// </summary>
    private async void Button_CopyGroup_Click(object sender, RoutedEventArgs e)
    {
        var number = (sender as System.Windows.Controls.Button)?.Tag as string;
        if (string.IsNullOrEmpty(number))
            return;

        if (await ClipboardHelper.TrySetTextAsync(number))
            NotifierHelper.Show(NotifierType.Success, $"QQ 群号 {number} 已复制到剪贴板（手机 QQ 搜索加入）");
        else
            NotifierHelper.Show(NotifierType.Warning, "剪贴板被其他程序占用，复制失败，请稍后再点一次。");
    }
}
