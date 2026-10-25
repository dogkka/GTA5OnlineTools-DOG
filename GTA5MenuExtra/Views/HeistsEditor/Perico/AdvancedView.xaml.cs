using GTA5Core.Features;
using GTA5Shared.Helper;

namespace GTA5MenuExtra.Views.HeistsEditor.Perico;

/// <summary>
/// AdvancedView.xaml 的交互逻辑
/// </summary>
public partial class AdvancedView : UserControl
{
    public AdvancedView()
    {
        InitializeComponent();
    }

    private async void STAT_SET_INT(string hash, int value)
    {
        await STATS.STAT_SET_INT(hash, value);
    }

    private void Hyperlink_RequestNavigate(object sender, RequestNavigateEventArgs e)
    {
        ProcessHelper.OpenLink(e.Uri.OriginalString);
        e.Handled = true;
    }

    private void Button_Reset_H4_Click(object sender, RoutedEventArgs e)
    {


        STAT_SET_INT("MPx_H4_MISSIONS", 0);
        STAT_SET_INT("MPx_H4_PROGRESS", 0);
        STAT_SET_INT("MPx_H4_PLAYTHROUGH_STATUS", 0);
        STAT_SET_INT("MPx_H4CNF_APPROACH", 0);
        STAT_SET_INT("MPx_H4CNF_BS_ENTR", 0);
        STAT_SET_INT("MPx_H4CNF_BS_GEN", 0);
    }
}
