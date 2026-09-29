using GTA5Core.Features;

namespace GTA5MenuExtra.Views.HeistsEditor.Contract;

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

    private void Button_FIXER_GENERAL_BS_Click(object sender, RoutedEventArgs e)
    {


        STAT_SET_INT("MPx_FIXER_GENERAL_BS", -1);
        STAT_SET_INT("MPx_FIXER_STORY_BS", 4095);
    }

    private void Button_TUNER_CURRENT_Click(object sender, RoutedEventArgs e)
    {


        var index = ListBox_TUNER_CURRENT.SelectedIndex;
        if (index == -1)
            return;

        STAT_SET_INT("MPx_TUNER_CURRENT", index);
        STAT_SET_INT("MPx_TUNER_GEN_BS", 65535);
    }

    ////////////////////////////////////////////////////

    private void Button_Reset_FIXER_STORY_Click(object sender, RoutedEventArgs e)
    {


        STAT_SET_INT("MPx_FIXER_GENERAL_BS", -1);
        STAT_SET_INT("MPx_FIXER_STORY_BS", 0);
    }

    private void Button_Reset_TUNER_STORY_Click(object sender, RoutedEventArgs e)
    {


        STAT_SET_INT("MPx_TUNER_CURRENT", -1);
        STAT_SET_INT("MPx_TUNER_GEN_BS", 0);
    }
}
