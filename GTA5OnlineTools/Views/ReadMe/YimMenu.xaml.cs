using GTA5Shared.Helper;

namespace GTA5OnlineTools.Views.ReadMe;

/// <summary>
/// YimMenu.xaml 的交互逻辑
/// </summary>
public partial class YimMenu : UserControl
{
    public YimMenu()
    {
        InitializeComponent();
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
