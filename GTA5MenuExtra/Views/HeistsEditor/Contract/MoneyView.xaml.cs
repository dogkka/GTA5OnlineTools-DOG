using GTA5Core.Features;
using GTA5Shared.Helper;

namespace GTA5MenuExtra.Views.HeistsEditor.Contract;

/// <summary>
/// MoneyView.xaml 的交互逻辑
/// </summary>
public partial class MoneyView : UserControl
{
    /*private const int fixer_ratio = 262145 + 31084;     // -2108119120  joaat("FIXER_FINALE_LEADER_CASH_REWARD")     Global_262145.f_31955
    private const int tuner_ratio = 262145 + 30338;     // -920277662   joaat("TUNER_ROBBERY_LEADER_CASH_REWARD0")   Global_262145.f_31249[0]*/

    public MoneyView()
    {
        InitializeComponent();
    }

    private void Button_Read_Click(object sender, RoutedEventArgs e)
    {


        TextBox_FIXER_Value.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("FIXER_FINALE_LEADER_CASH_REWARD"))).ToString();

        TextBox_TUNER_Value1.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("TUNER_ROBBERY_LEADER_CASH_REWARD0"))).ToString();
        TextBox_TUNER_Value2.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("TUNER_ROBBERY_LEADER_CASH_REWARD1"))).ToString();
        TextBox_TUNER_Value3.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("TUNER_ROBBERY_LEADER_CASH_REWARD2"))).ToString();
        TextBox_TUNER_Value4.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("TUNER_ROBBERY_LEADER_CASH_REWARD3"))).ToString();
        TextBox_TUNER_Value5.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("TUNER_ROBBERY_LEADER_CASH_REWARD4"))).ToString();
        TextBox_TUNER_Value6.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("TUNER_ROBBERY_LEADER_CASH_REWARD5"))).ToString();
        TextBox_TUNER_Value7.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("TUNER_ROBBERY_LEADER_CASH_REWARD6"))).ToString();
        TextBox_TUNER_Value8.Text = Globals.Get_Global_Value<int>(Tunables.Index(RAGE.JOAAT("TUNER_ROBBERY_LEADER_CASH_REWARD7"))).ToString();

        NotifierHelper.Show(NotifierType.Success, "读取 事所合约 玩家分红数据 成功");
    }

    private void Button_Write_Click(object sender, RoutedEventArgs e)
    {


        if (!int.TryParse(TextBox_FIXER_Value.Text, out int fixer) ||

            !int.TryParse(TextBox_TUNER_Value1.Text, out int tuner1) ||
            !int.TryParse(TextBox_TUNER_Value2.Text, out int tuner2) ||
            !int.TryParse(TextBox_TUNER_Value3.Text, out int tuner3) ||
            !int.TryParse(TextBox_TUNER_Value4.Text, out int tuner4) ||
            !int.TryParse(TextBox_TUNER_Value5.Text, out int tuner5) ||
            !int.TryParse(TextBox_TUNER_Value6.Text, out int tuner6) ||
            !int.TryParse(TextBox_TUNER_Value7.Text, out int tuner7) ||
            !int.TryParse(TextBox_TUNER_Value8.Text, out int tuner8))
        {
            NotifierHelper.Show(NotifierType.Warning, "部分数据不合法，请检查后重新写入");
            return;
        }

        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("FIXER_FINALE_LEADER_CASH_REWARD")), fixer);

        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("TUNER_ROBBERY_LEADER_CASH_REWARD0")), tuner1);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("TUNER_ROBBERY_LEADER_CASH_REWARD1")), tuner2);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("TUNER_ROBBERY_LEADER_CASH_REWARD2")), tuner3);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("TUNER_ROBBERY_LEADER_CASH_REWARD3")), tuner4);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("TUNER_ROBBERY_LEADER_CASH_REWARD4")), tuner5);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("TUNER_ROBBERY_LEADER_CASH_REWARD5")), tuner6);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("TUNER_ROBBERY_LEADER_CASH_REWARD6")), tuner7);
        Globals.Set_Global_Value(Tunables.Index(RAGE.JOAAT("TUNER_ROBBERY_LEADER_CASH_REWARD7")), tuner8);

        NotifierHelper.Show(NotifierType.Success, "写入 事所合约 玩家分红数据 成功");
    }
}
