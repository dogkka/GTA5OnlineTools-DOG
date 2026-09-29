using GTA5Core.Features;

namespace GTA5MenuExtra.Views.HeistsEditor.Apartment;

/// <summary>
/// MissionView.xaml 的交互逻辑
/// </summary>
public partial class MissionView : UserControl
{
    public MissionView()
    {
        InitializeComponent();
    }

    private async void STAT_SET_INT(string hash, int value)
    {
        await STATS.STAT_SET_INT(hash, value);
    }

    ////////////////////////////////////////////////////

    private void Button_HEIST_PLANNING_STAGE_Click(object sender, RoutedEventArgs e)
    {


        STAT_SET_INT("MPx_HEIST_PLANNING_STAGE", -1);
    }
}
