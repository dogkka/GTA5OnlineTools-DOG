using GTA5Core.Features;
using GTA5Shared.Helper;

namespace GTA5MenuExtra.Views.HeistsEditor.Perico;

/// <summary>
/// MoneyView.xaml 的交互逻辑
/// </summary>
public partial class MoneyView : UserControl
{
    /// <summary>
    /// 玩家分红
    /// </summary>
    private const int player_ratio = 1971648 + 831 + 56;
    /// <summary>
    /// 主要目标价值
    /// </summary>
    /*private const int target_money = 262145 + 29458;    // 132820683    joaat("IH_PRIMARY_TARGET_VALUE_TEQUILA")     Global_262145.f_30259*/
    /// <summary>
    /// 背包容量
    /// </summary>
    /*private const int bag_size = 262145 + 29211;        // 1859395035   Global_262145.f_30009*/

    public MoneyView()
    {
        InitializeComponent();
    }

    private void Button_Read_Click(object sender, RoutedEventArgs e)
    {


        TextBox_Cayo_Player1.Text = Globals.Get_Global_Value<int>(player_ratio + 1).ToString();
        TextBox_Cayo_Player2.Text = Globals.Get_Global_Value<int>(player_ratio + 2).ToString();
        TextBox_Cayo_Player3.Text = Globals.Get_Global_Value<int>(player_ratio + 3).ToString();
        TextBox_Cayo_Player4.Text = Globals.Get_Global_Value<int>(player_ratio + 4).ToString();

        TextBox_Cayo_Tequila.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("IH_PRIMARY_TARGET_VALUE_TEQUILA"))).ToString();
        TextBox_Cayo_RubyNecklace.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("IH_PRIMARY_TARGET_VALUE_PEARL_NECKLACE"))).ToString();
        TextBox_Cayo_BearerBonds.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("IH_PRIMARY_TARGET_VALUE_BEARER_BONDS"))).ToString();
        TextBox_Cayo_PinkDiamond.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("IH_PRIMARY_TARGET_VALUE_PINK_DIAMOND"))).ToString();
        TextBox_Cayo_MadrazoFiles.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("IH_PRIMARY_TARGET_VALUE_MADRAZO_FILES"))).ToString();
        TextBox_Cayo_BlackPanther.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("IH_PRIMARY_TARGET_VALUE_SAPPHIRE_PANTHER_STATUE"))).ToString();

        TextBox_Cayo_LocalBagSize.Text = Globals.Get_Global_Value<int>(Tunables.Index(1859395035)).ToString();

        TextBox_Cayo_FencingFee.Text = Globals.Get_Global_Value<float>(Tunables.Index(RAGE.JOAAT("IH_DEDUCTION_FENCING_FEE"))).ToString();
        TextBox_Cayo_PavelCut.Text = Globals.Get_Global_Value<float>(Tunables.Index(RAGE.JOAAT("IH_DEDUCTION_PAVEL_CUT"))).ToString();

        NotifierHelper.Show(NotifierType.Success, "读取 佩里克岛 玩家分红数据 成功");
    }

    private void Button_Write_Click(object sender, RoutedEventArgs e)
    {


        if (!int.TryParse(TextBox_Cayo_Player1.Text, out int player1) ||
            !int.TryParse(TextBox_Cayo_Player2.Text, out int player2) ||
            !int.TryParse(TextBox_Cayo_Player3.Text, out int player3) ||
            !int.TryParse(TextBox_Cayo_Player4.Text, out int player4) ||

            !int.TryParse(TextBox_Cayo_Tequila.Text, out int cayo1) ||
            !int.TryParse(TextBox_Cayo_RubyNecklace.Text, out int cayo2) ||
            !int.TryParse(TextBox_Cayo_BearerBonds.Text, out int cayo3) ||
            !int.TryParse(TextBox_Cayo_PinkDiamond.Text, out int cayo4) ||
            !int.TryParse(TextBox_Cayo_MadrazoFiles.Text, out int cayo5) ||
            !int.TryParse(TextBox_Cayo_BlackPanther.Text, out int cayo6) ||

            !int.TryParse(TextBox_Cayo_LocalBagSize.Text, out int bagsize) ||

            !float.TryParse(TextBox_Cayo_FencingFee.Text, out float fencingfee) ||
            !float.TryParse(TextBox_Cayo_PavelCut.Text, out float pavecut))
        {
            NotifierHelper.Show(NotifierType.Warning, "部分数据不合法，请检查后重新写入");
            return;
        }

        Globals.Set_Global_Value(player_ratio + 1, player1);
        Globals.Set_Global_Value(player_ratio + 2, player2);
        Globals.Set_Global_Value(player_ratio + 3, player3);
        Globals.Set_Global_Value(player_ratio + 4, player4);

        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("IH_PRIMARY_TARGET_VALUE_TEQUILA")), cayo1);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("IH_PRIMARY_TARGET_VALUE_PEARL_NECKLACE")), cayo2);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("IH_PRIMARY_TARGET_VALUE_BEARER_BONDS")), cayo3);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("IH_PRIMARY_TARGET_VALUE_PINK_DIAMOND")), cayo4);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("IH_PRIMARY_TARGET_VALUE_MADRAZO_FILES")), cayo5);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("IH_PRIMARY_TARGET_VALUE_SAPPHIRE_PANTHER_STATUE")), cayo6);

        Globals.Set_Global_Value(Tunables.Index(1859395035), bagsize);

        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("IH_DEDUCTION_FENCING_FEE")), fencingfee);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("IH_DEDUCTION_PAVEL_CUT")), pavecut);

        NotifierHelper.Show(NotifierType.Success, "写入 佩里克岛 玩家分红数据 成功");
    }
}
