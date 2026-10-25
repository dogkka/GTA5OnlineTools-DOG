using GTA5Core.Features;
using GTA5Shared.Helper;

namespace GTA5MenuExtra.Views.HeistsEditor.Casino;

/// <summary>
/// MoneyView.xaml 的交互逻辑
/// </summary>
public partial class MoneyView : UserControl
{
    private const int player_ratio = 1964849 + 1497 + 736 + 92;
    /*private const int player_money = 262145 + 28327;     // -1638885821

    private const int ai_ratio = 262145 + 28338;
    private const int lester_ratio = 262145 + 28313;     // joaat("CH_LESTER_CUT")*/

    public MoneyView()
    {
        InitializeComponent();
    }

    private void Button_Read_Click(object sender, RoutedEventArgs e)
    {


        TextBox_Casino_Player1.Text = Globals.Get_Global_Value<int>(player_ratio + 1).ToString();
        TextBox_Casino_Player2.Text = Globals.Get_Global_Value<int>(player_ratio + 2).ToString();
        TextBox_Casino_Player3.Text = Globals.Get_Global_Value<int>(player_ratio + 3).ToString();
        TextBox_Casino_Player4.Text = Globals.Get_Global_Value<int>(player_ratio + 4).ToString();

        TextBox_Casino_Lester.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("CH_LESTER_CUT"))).ToString();

        TextBox_CasinoPotential_Money.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("CH_VAULT_MAX_TAKE_CASH"))).ToString();
        TextBox_CasinoPotential_Artwork.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("CH_VAULT_MAX_TAKE_ART"))).ToString();
        TextBox_CasinoPotential_Gold.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("CH_VAULT_MAX_TAKE_GOLD"))).ToString();
        TextBox_CasinoPotential_Diamonds.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("CH_VAULT_MAX_TAKE_DIAMONDS"))).ToString();

        TextBox_CasinoAI_1.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST3_PREPBOARD_GUNMEN_KARL_CUT"))).ToString();
        TextBox_CasinoAI_2.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST3_PREPBOARD_GUNMEN_GUSTAVO_CUT"))).ToString();
        TextBox_CasinoAI_3.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST3_PREPBOARD_GUNMEN_CHARLIE_CUT"))).ToString();
        TextBox_CasinoAI_4.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST3_PREPBOARD_GUNMEN_CHESTER_CUT"))).ToString();
        TextBox_CasinoAI_5.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST3_PREPBOARD_GUNMEN_PATRICK_CUT"))).ToString();

        TextBox_CasinoAI_6.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST3_DRIVERS_KARIM_CUT"))).ToString();
        TextBox_CasinoAI_7.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST3_DRIVERS_TALIANA_CUT"))).ToString();
        TextBox_CasinoAI_8.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST3_DRIVERS_EDDIE_CUT"))).ToString();
        TextBox_CasinoAI_9.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST3_DRIVERS_ZACH_CUT"))).ToString();
        TextBox_CasinoAI_10.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST3_DRIVERS_CHESTER_CUT"))).ToString();

        TextBox_CasinoAI_11.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST3_HACKERS_RICKIE_CUT"))).ToString();
        TextBox_CasinoAI_12.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST3_HACKERS_CHRISTIAN_CUT"))).ToString();
        TextBox_CasinoAI_13.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST3_HACKERS_YOHAN_CUT"))).ToString();
        TextBox_CasinoAI_14.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST3_HACKERS_AVI_CUT"))).ToString();
        TextBox_CasinoAI_15.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST3_HACKERS_PAIGE_CUT"))).ToString();

        NotifierHelper.Show(NotifierType.Success, "读取 赌场抢劫 玩家分红数据 成功");
    }

    private void Button_Write_Click(object sender, RoutedEventArgs e)
    {


        if (!int.TryParse(TextBox_Casino_Player1.Text, out int player1) ||
            !int.TryParse(TextBox_Casino_Player2.Text, out int player2) ||
            !int.TryParse(TextBox_Casino_Player3.Text, out int player3) ||
            !int.TryParse(TextBox_Casino_Player4.Text, out int player4) ||

            !int.TryParse(TextBox_Casino_Lester.Text, out int lester) ||

            !int.TryParse(TextBox_CasinoPotential_Money.Text, out int money) ||
            !int.TryParse(TextBox_CasinoPotential_Artwork.Text, out int artwork) ||
            !int.TryParse(TextBox_CasinoPotential_Gold.Text, out int gold) ||
            !int.TryParse(TextBox_CasinoPotential_Diamonds.Text, out int diamonds) ||

            !int.TryParse(TextBox_CasinoAI_1.Text, out int ai1) ||
            !int.TryParse(TextBox_CasinoAI_2.Text, out int ai2) ||
            !int.TryParse(TextBox_CasinoAI_3.Text, out int ai3) ||
            !int.TryParse(TextBox_CasinoAI_4.Text, out int ai4) ||
            !int.TryParse(TextBox_CasinoAI_5.Text, out int ai5) ||

            !int.TryParse(TextBox_CasinoAI_6.Text, out int ai6) ||
            !int.TryParse(TextBox_CasinoAI_7.Text, out int ai7) ||
            !int.TryParse(TextBox_CasinoAI_8.Text, out int ai8) ||
            !int.TryParse(TextBox_CasinoAI_9.Text, out int ai9) ||
            !int.TryParse(TextBox_CasinoAI_10.Text, out int ai10) ||

            !int.TryParse(TextBox_CasinoAI_11.Text, out int ai11) ||
            !int.TryParse(TextBox_CasinoAI_12.Text, out int ai12) ||
            !int.TryParse(TextBox_CasinoAI_13.Text, out int ai13) ||
            !int.TryParse(TextBox_CasinoAI_14.Text, out int ai14) ||
            !int.TryParse(TextBox_CasinoAI_15.Text, out int ai15))
        {
            NotifierHelper.Show(NotifierType.Warning, "部分数据不合法，请检查后重新写入");
            return;
        }

        Globals.Set_Global_Value(player_ratio + 1, player1);
        Globals.Set_Global_Value(player_ratio + 2, player2);
        Globals.Set_Global_Value(player_ratio + 3, player3);
        Globals.Set_Global_Value(player_ratio + 4, player4);

        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("CH_LESTER_CUT")), lester);

        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("CH_VAULT_MAX_TAKE_CASH")), money);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("CH_VAULT_MAX_TAKE_ART")), artwork);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("CH_VAULT_MAX_TAKE_GOLD")), gold);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("CH_VAULT_MAX_TAKE_DIAMONDS")), diamonds);

        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST3_PREPBOARD_GUNMEN_KARL_CUT")), ai1);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST3_PREPBOARD_GUNMEN_GUSTAVO_CUT")), ai2);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST3_PREPBOARD_GUNMEN_CHARLIE_CUT")), ai3);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST3_PREPBOARD_GUNMEN_CHESTER_CUT")), ai4);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST3_PREPBOARD_GUNMEN_PATRICK_CUT")), ai5);

        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST3_DRIVERS_KARIM_CUT")), ai6);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST3_DRIVERS_TALIANA_CUT")), ai7);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST3_DRIVERS_EDDIE_CUT")), ai8);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST3_DRIVERS_ZACH_CUT")), ai9);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST3_DRIVERS_CHESTER_CUT")), ai10);

        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST3_HACKERS_RICKIE_CUT")), ai11);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST3_HACKERS_CHRISTIAN_CUT")), ai12);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST3_HACKERS_YOHAN_CUT")), ai13);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST3_HACKERS_AVI_CUT")), ai14);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST3_HACKERS_PAIGE_CUT")), ai15);

        NotifierHelper.Show(NotifierType.Success, "写入 赌场抢劫 玩家分红数据 成功");
    }
}
