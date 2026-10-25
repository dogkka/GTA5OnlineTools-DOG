using GTA5Core.Features;

namespace GTA5MenuExtra.Views.HeistsEditor.Casino;

/// <summary>
/// AdvancedView.xaml 的交互逻辑
/// </summary>
public partial class AdvancedView : UserControl
{
    public AdvancedView()
    {
        InitializeComponent();
    }

    ////////////////////////////////////////////////////

    private async void STAT_SET_INT(string hash, int value)
    {
        await STATS.STAT_SET_INT(hash, value);
    }

    private void SetPanelBITSET1(string hash, int value)
    {
        STAT_SET_INT("MPx_H3OPT_BITSET1", 0);
        STAT_SET_INT(hash, value);
        STAT_SET_INT("MPx_H3OPT_BITSET1", -1);
    }

    private void SetPanelBITSET0(string hash, int value)
    {
        STAT_SET_INT("MPx_H3OPT_BITSET0", 0);
        STAT_SET_INT(hash, value);
        STAT_SET_INT("MPx_H3OPT_BITSET0", -1);
    }

    ////////////////////////////////////////////////////

    private void Button_H3OPT_ACCESSPOINTS_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET1("MPx_H3OPT_ACCESSPOINTS", -1);
    }

    private void Button_H3OPT_H3OPT_POI_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET1("MPx_H3OPT_POI", -1);
    }

    private void Button_H3OPT_ACCESSPOINTS_0_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET1("MPx_H3OPT_ACCESSPOINTS", 0);
    }

    private void Button_H3OPT_H3OPT_POI_0_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET1("MPx_H3OPT_POI", 0);
    }

    ////////////////////////////////////////////////////

    private void Button_Reset_H3OPT_Click(object sender, RoutedEventArgs e)
    {


        STAT_SET_INT("MPx_H3OPT_BITSET1", 0);
        STAT_SET_INT("MPx_H3OPT_BITSET0", 0);
        STAT_SET_INT("MPx_H3OPT_ACCESSPOINTS", 0);
        STAT_SET_INT("MPx_H3OPT_POI", 0);
        STAT_SET_INT("MPx_H3OPT_BITSET1", -1);
        STAT_SET_INT("MPx_H3OPT_BITSET0", -1);
    }

    private void Button_CAS_HEIST_FLOW_Click(object sender, RoutedEventArgs e)
    {


        STAT_SET_INT("MPx_CAS_HEIST_FLOW", -1610744257);
    }

    ////////////////////////////////////////////////////

    private void Button_H3OPT_APPROACH_1_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET1("MPx_H3OPT_APPROACH", 1);
    }

    private void Button_H3OPT_APPROACH_2_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET1("MPx_H3OPT_APPROACH", 2);
    }

    private void Button_H3OPT_APPROACH_3_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET1("MPx_H3OPT_APPROACH", 3);
    }

    ////////////////////////////////////////////////////

    private void Button_H3OPT_CREWWEAP_1_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET0("MPx_H3OPT_CREWWEAP", 1);
    }

    private void Button_H3OPT_CREWWEAP_2_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET0("MPx_H3OPT_CREWWEAP", 2);
    }

    private void Button_H3OPT_CREWWEAP_3_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET0("MPx_H3OPT_CREWWEAP", 3);
    }

    private void Button_H3OPT_CREWWEAP_4_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET0("MPx_H3OPT_CREWWEAP", 4);
    }

    private void Button_H3OPT_CREWWEAP_5_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET0("MPx_H3OPT_CREWWEAP", 5);
    }

    ////////////////////////////////////////////////////

    private void Button_H3OPT_CREWDRIVER_1_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET0("MPx_H3OPT_CREWDRIVER", 1);
    }

    private void Button_H3OPT_CREWDRIVER_2_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET0("MPx_H3OPT_CREWDRIVER", 2);
    }

    private void Button_H3OPT_CREWDRIVER_3_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET0("MPx_H3OPT_CREWDRIVER", 3);
    }

    private void Button_H3OPT_CREWDRIVER_4_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET0("MPx_H3OPT_CREWDRIVER", 4);
    }

    private void Button_H3OPT_CREWDRIVER_5_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET0("MPx_H3OPT_CREWDRIVER", 5);
    }

    ////////////////////////////////////////////////////

    private void Button_H3OPT_WEAPS_0_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET0("MPx_H3OPT_WEAPS", 0);
    }

    private void Button_H3OPT_WEAPS_1_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET0("MPx_H3OPT_WEAPS", 1);
    }

    ////////////////////////////////////////////////////

    private void Button_H3OPT_VEH_0_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET0("MPx_H3OPT_VEHS", 0);
    }

    private void Button_H3OPT_VEH_1_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET0("MPx_H3OPT_VEHS", 1);
    }

    private void Button_H3OPT_VEH_2_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET0("MPx_H3OPT_VEHS", 2);
    }

    private void Button_H3OPT_VEH_3_Click(object sender, RoutedEventArgs e)
    {


        SetPanelBITSET0("MPx_H3OPT_VEHS", 3);
    }
}
