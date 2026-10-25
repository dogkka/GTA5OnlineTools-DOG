using GTA5Core.Features;
using GTA5Core.GTA.Onlines;
using GTA5Core.GTA.Stats;

namespace GTA5MenuExtra.Views.StatsEditor;

/// <summary>
/// UserfulView.xaml 的交互逻辑
/// </summary>
public partial class UserfulView : UserControl
{
    public UserfulView()
    {
        InitializeComponent();
    }

    private async void STAT_SET_INT(string hash, int value)
    {
        await STATS.STAT_SET_INT(hash, value);
    }

    private void STAT_SET_INT(List<StatData.StatInfo> statInfos)
    {
        foreach (var item in StatData.MP_CHAR_ARMOUR)
        {
            STAT_SET_INT(item.Hash, item.Value);
        }
    }

    ////////////////////////////////////////////////////

    private void Button_MP_CHAR_ARMOUR_Click(object sender, RoutedEventArgs e)
    {


        STAT_SET_INT(StatData.MP_CHAR_ARMOUR);
    }

    private void Button_NO_BOUGHT_Click(object sender, RoutedEventArgs e)
    {


        STAT_SET_INT(StatData.NO_BOUGHT);
    }

    private void Button_SCRIPT_INCREASE_Click(object sender, RoutedEventArgs e)
    {


        STAT_SET_INT(StatData.SCRIPT_INCREASE);
    }

    private void Button_CHAR_ABILITY_Click(object sender, RoutedEventArgs e)
    {


        STAT_SET_INT(StatData.CHAR_ABILITY);
    }

    ////////////////////////////////////////////////////

    private void Button_CHAR_ABILITY0_Click(object sender, RoutedEventArgs e)
    {


        STAT_SET_INT(StatData.CHAR_ABILITY0);
    }

    private void Button_PLAYER_MENTAL_STATE_Click(object sender, RoutedEventArgs e)
    {


        STATS.STAT_SET_FLOAT("MPx_PLAYER_MENTAL_STATE", 0);
    }

    ////////////////////////////////////////////////////

    private void Button_GENDER_CHANGE_ON_Click(object sender, RoutedEventArgs e)
    {


        STAT_SET_INT(StatData.GENDER_CHANGE_ON);
    }

    private void Button_GENDER_CHANGE_OFF_Click(object sender, RoutedEventArgs e)
    {


        STAT_SET_INT(StatData.GENDER_CHANGE_OFF);
    }

    private void Button_FreeChangeAppearance_Click(object sender, RoutedEventArgs e)
    {


        Online.FreeChangeAppearance(true);
    }

    private void Button_ChangeAppearanceCooldown_Click(object sender, RoutedEventArgs e)
    {


        Online.ChangeAppearanceCooldown(true);
    }

    ////////////////////////////////////////////////////

    private void Button_ChangePlayerRank_Click(object sender, RoutedEventArgs e)
    {


        var rank = (int)Slider_PlayerRank.Value;
        STAT_SET_INT("MPx_CHAR_SET_RP_GIFT_ADMIN", Levels.PlayerRank[rank - 1]);
    }

    private void Button_ChangeClubRank_Click(object sender, RoutedEventArgs e)
    {


        var rank = (int)Slider_ClubRank.Value;
        STAT_SET_INT("MPx_CAR_CLUB_REP", Levels.ClubRank[rank - 1]);
    }

    ////////////////////////////////////////////////////

    private void Button_ChangeCrew1Rank_Click(object sender, RoutedEventArgs e)
    {


        var rank = (int)Slider_Crew1Rank.Value;
        STAT_SET_INT("MPPLY_CREW_LOCAL_XP_1", Levels.PlayerRank[rank - 1]);
    }

    private void Button_ChangeCrew2Rank_Click(object sender, RoutedEventArgs e)
    {


        var rank = (int)Slider_Crew2Rank.Value;
        STAT_SET_INT("MPPLY_CREW_LOCAL_XP_2", Levels.PlayerRank[rank - 1]);
    }

    private void Button_ChangeCrew3Rank_Click(object sender, RoutedEventArgs e)
    {


        var rank = (int)Slider_Crew3Rank.Value;
        STAT_SET_INT("MPPLY_CREW_LOCAL_XP_3", Levels.PlayerRank[rank - 1]);
    }

    private void Button_ChangeCrew4Rank_Click(object sender, RoutedEventArgs e)
    {


        var rank = (int)Slider_Crew4Rank.Value;
        STAT_SET_INT("MPPLY_CREW_LOCAL_XP_4", Levels.PlayerRank[rank - 1]);
    }

    private void Button_ChangeCrew5Rank_Click(object sender, RoutedEventArgs e)
    {


        var rank = (int)Slider_Crew5Rank.Value;
        STAT_SET_INT("MPPLY_CREW_LOCAL_XP_5", Levels.PlayerRank[rank - 1]);
    }
}
