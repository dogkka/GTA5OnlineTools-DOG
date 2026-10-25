using GTA5Core.Features;
using GTA5Shared.Helper;

namespace GTA5MenuExtra.Views.HeistsEditor.Apartment;

/// <summary>
/// MoneyView.xaml 的交互逻辑
/// </summary>
public partial class MoneyView : UserControl
{
    private const int apart_ratio = 2685444 + 6403;     // +1 +2 +3 +4
    /* private const int apart_money = 262145 + 9171; //HEIST_FLEECA_JOB_CASH_REWARD*/

    public MoneyView()
    {
        InitializeComponent();
    }

    private void Button_Read_Click(object sender, RoutedEventArgs e)
    {


        TextBox_Apart_Player1.Text = Globals.Get_Global_Value<int>(apart_ratio).ToString();

        TextBox_Apart_Fleeca.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST_FLEECA_JOB_CASH_REWARD"))).ToString();
        TextBox_Apart_PrisonBreak.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST_PRISON_BREAK_CASH_REWARD"))).ToString();
        TextBox_Apart_HumaneLabs.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST_HUMANE_LABS_RAID_CASH_REWARD"))).ToString();
        TextBox_Apart_SeriesA.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST_SERIES_A_FUNDING_CASH_REWARD"))).ToString();
        TextBox_Apart_PacificStandard.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("HEIST_PACIFIC_STANDARD_JOB_CASH_REWARD"))).ToString();

        NotifierHelper.Show(NotifierType.Success, "读取 公寓抢劫 玩家分红数据 成功");
    }

    private void Button_Write_Click(object sender, RoutedEventArgs e)
    {


        if (!int.TryParse(TextBox_Apart_Player1.Text, out int player1) ||

            !int.TryParse(TextBox_Apart_Fleeca.Text, out int apart1) ||
            !int.TryParse(TextBox_Apart_PrisonBreak.Text, out int apart2) ||
            !int.TryParse(TextBox_Apart_HumaneLabs.Text, out int apart3) ||
            !int.TryParse(TextBox_Apart_SeriesA.Text, out int apart4) ||
            !int.TryParse(TextBox_Apart_PacificStandard.Text, out int apart5))
        {
            NotifierHelper.Show(NotifierType.Warning, "部分数据不合法，请检查后重新写入");
            return;
        }

        Globals.Set_Global_Value(apart_ratio, player1);

        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST_FLEECA_JOB_CASH_REWARD")), apart1);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST_PRISON_BREAK_CASH_REWARD")), apart2);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST_HUMANE_LABS_RAID_CASH_REWARD")), apart3);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST_SERIES_A_FUNDING_CASH_REWARD")), apart4);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("HEIST_PACIFIC_STANDARD_JOB_CASH_REWARD")), apart5);

        NotifierHelper.Show(NotifierType.Success, "写入 公寓抢劫 玩家分红数据 成功");
    }
}
