using GTA5Core.Features;
using GTA5Shared.Helper;

namespace GTA5MenuExtra.Views.HeistsEditor.Perico;

/// <summary>
/// MissionView.xaml 的交互逻辑
/// </summary>
public partial class MissionView : UserControl
{
    private readonly Dictionary<string, int> STAT_DIC = new();

    public MissionView()
    {
        InitializeComponent();

        TextBox_Logger.AppendText("INT32\n");
    }

    ////////////////////////////////////////////////////

    private void ClearLogger()
    {
        TextBox_Logger.Clear();
    }

    private void AppendLogger(string hash, int value)
    {
        TextBox_Logger.AppendText($"${hash}\n");
        TextBox_Logger.AppendText($"{value}\n");

        STAT_DIC.Add(hash, value);
    }

    private void AppendLogger(string log)
    {
        TextBox_Logger.AppendText($"[{DateTime.Now:T}] {log}\n");
        TextBox_Logger.ScrollToEnd();
    }

    private bool IsValidIndex(int index)
    {
        return index != -1 && index != 0;
    }

    ////////////////////////////////////////////////////

    private void STAT_Build()
    {
        ClearLogger();
        TextBox_Logger.AppendText("INT32\n");

        STAT_DIC.Clear();

        //////////////////////////

        // 下面这4句代码写入的时候要严格按照顺序来

        // 解锁所有兴趣点
        if (CheckBox_H4CNF_BS_GEN.IsChecked == true)
            AppendLogger("MPx_H4CNF_BS_GEN", 131071);

        // 解锁所有入侵点
        if (CheckBox_H4CNF_BS_ENTR.IsChecked == true)
            AppendLogger("MPx_H4CNF_BS_ENTR", 63);

        // 解锁团队支援
        if (CheckBox_H4CNF_BS_ABIL.IsChecked == true)
            AppendLogger("MPx_H4CNF_BS_ABIL", 63);

        // 解锁所有逃离点
        if (CheckBox_H4CNF_APPROACH.IsChecked == true)
            AppendLogger("MPx_H4CNF_APPROACH", -1);

        //////////////////////////

        // 任务难度
        var index = ListBox_H4_PROGRESS.SelectedIndex;
        switch (index)
        {
            case 1:
                AppendLogger("MPx_H4_PROGRESS", 126823);
                break;
            case 2:
                AppendLogger("MPx_H4_PROGRESS", 131055);
                break;
        }

        // 主要目标
        index = ListBox_H4CNF_TARGET.SelectedIndex;
        if (IsValidIndex(index))
            AppendLogger("MPx_H4CNF_TARGET", index - 1);

        // 次要目标
        index = ListBox_H4LOOT.SelectedIndex;

        // 现金
        if (index == 1)
        {
            AppendLogger("MPx_H4LOOT_CASH_I", -1);
            AppendLogger("MPx_H4LOOT_CASH_C", -1);
            AppendLogger("MPx_H4LOOT_CASH_I_SCOPED", -1);
            AppendLogger("MPx_H4LOOT_CASH_C_SCOPED", -1);
            AppendLogger("MPx_H4LOOT_CASH_V", 90000);
        }
        else
        {
            if (CheckBox_H4LOOT_Random.IsChecked == false)
            {
                AppendLogger("MPx_H4LOOT_CASH_I", 0);
                AppendLogger("MPx_H4LOOT_CASH_C", 0);
                AppendLogger("MPx_H4LOOT_CASH_I_SCOPED", 0);
                AppendLogger("MPx_H4LOOT_CASH_C_SCOPED", 0);
                AppendLogger("MPx_H4LOOT_CASH_V", 0);
            }
        }

        // 大麻
        if (index == 2)
        {
            AppendLogger("MPx_H4LOOT_WEED_I", -1);
            AppendLogger("MPx_H4LOOT_WEED_C", -1);
            AppendLogger("MPx_H4LOOT_WEED_I_SCOPED", -1);
            AppendLogger("MPx_H4LOOT_WEED_C_SCOPED", -1);
            AppendLogger("MPx_H4LOOT_WEED_V", 145000);
        }
        else
        {
            if (CheckBox_H4LOOT_Random.IsChecked == false)
            {
                AppendLogger("MPx_H4LOOT_WEED_I", 0);
                AppendLogger("MPx_H4LOOT_WEED_C", 0);
                AppendLogger("MPx_H4LOOT_WEED_I_SCOPED", 0);
                AppendLogger("MPx_H4LOOT_WEED_C_SCOPED", 0);
                AppendLogger("MPx_H4LOOT_WEED_V", 0);
            }
        }

        // 可可
        if (index == 3)
        {
            AppendLogger("MPx_H4LOOT_COKE_I", -1);
            AppendLogger("MPx_H4LOOT_COKE_C", -1);
            AppendLogger("MPx_H4LOOT_COKE_I_SCOPED", -1);
            AppendLogger("MPx_H4LOOT_COKE_C_SCOPED", -1);
            AppendLogger("MPx_H4LOOT_COKE_V", 220000);
        }
        else
        {
            if (CheckBox_H4LOOT_Random.IsChecked == false)
            {
                AppendLogger("MPx_H4LOOT_COKE_I", 0);
                AppendLogger("MPx_H4LOOT_COKE_C", 0);
                AppendLogger("MPx_H4LOOT_COKE_I_SCOPED", 0);
                AppendLogger("MPx_H4LOOT_COKE_C_SCOPED", 0);
                AppendLogger("MPx_H4LOOT_COKE_V", 0);
            }
        }

        // 黄金
        if (index == 4)
        {
            AppendLogger("MPx_H4LOOT_GOLD_I", -1);
            AppendLogger("MPx_H4LOOT_GOLD_C", -1);
            AppendLogger("MPx_H4LOOT_GOLD_I_SCOPED", -1);
            AppendLogger("MPx_H4LOOT_GOLD_C_SCOPED", -1);
            AppendLogger("MPx_H4LOOT_GOLD_V", 320000);
        }
        else
        {
            if (CheckBox_H4LOOT_Random.IsChecked == false)
            {
                AppendLogger("MPx_H4LOOT_GOLD_I", 0);
                AppendLogger("MPx_H4LOOT_GOLD_C", 0);
                AppendLogger("MPx_H4LOOT_GOLD_I_SCOPED", 0);
                AppendLogger("MPx_H4LOOT_GOLD_C_SCOPED", 0);
                AppendLogger("MPx_H4LOOT_GOLD_V", 0);
            }
        }

        // 次要目标
        index = ListBox_H4LOOT_PAINT.SelectedIndex;

        // 画作
        if (index == 1)
        {
            AppendLogger("MPx_H4LOOT_PAINT", -1);
            AppendLogger("MPx_H4LOOT_PAINT_SCOPED", -1);
            AppendLogger("MPx_H4LOOT_PAINT_V", 180000);
        }
        else
        {
            if (CheckBox_H4LOOT_Random.IsChecked == false)
            {
                AppendLogger("MPx_H4LOOT_PAINT", 0);
                AppendLogger("MPx_H4LOOT_PAINT_SCOPED", 0);
                AppendLogger("MPx_H4LOOT_PAINT_V", 0);
            }
        }

        // 接近载具
        index = ListBox_H4_MISSIONS.SelectedIndex;
        switch (index)
        {
            case 1:
                AppendLogger("MPx_H4_MISSIONS", 65283);
                break;
            case 2:
                AppendLogger("MPx_H4_MISSIONS", 65413);
                break;
            case 3:
                AppendLogger("MPx_H4_MISSIONS", 65289);
                break;
            case 4:
                AppendLogger("MPx_H4_MISSIONS", 65425);
                break;
            case 5:
                AppendLogger("MPx_H4_MISSIONS", 65313);
                break;
            case 6:
                AppendLogger("MPx_H4_MISSIONS", 65345);
                break;
            case 7:
                AppendLogger("MPx_H4_MISSIONS", 65535);
                break;
        }

        // 武器装备
        index = ListBox_H4CNF_WEAPONS.SelectedIndex;
        if (IsValidIndex(index))
            AppendLogger("MPx_H4CNF_WEAPONS", index);

        // 运货卡车位置
        index = ListBox_H4CNF_TROJAN.SelectedIndex;
        if (IsValidIndex(index))
            AppendLogger("MPx_H4CNF_TROJAN", index);

        // 武器
        if (CheckBox_H4CNF_WEP_DISRP.IsChecked == true)
            AppendLogger("MPx_H4CNF_WEP_DISRP", 3);

        // 防弹衣
        if (CheckBox_H4CNF_ARM_DISRP.IsChecked == true)
            AppendLogger("MPx_H4CNF_ARM_DISRP", 3);

        // 空中支援
        if (CheckBox_H4CNF_HEL_DISRP.IsChecked == true)
            AppendLogger("MPx_H4CNF_HEL_DISRP", 3);

        // 解锁抓钩
        if (CheckBox_H4CNF_GRAPPEL.IsChecked == true)
            AppendLogger("MPx_H4CNF_GRAPPEL", -1);

        // 解锁保安制服
        if (CheckBox_H4CNF_UNIFORM.IsChecked == true)
            AppendLogger("MPx_H4CNF_UNIFORM", -1);

        // 解锁螺栓切割器
        if (CheckBox_H4CNF_BOLTCUT.IsChecked == true)
            AppendLogger("MPx_H4CNF_BOLTCUT", -1);

        // 任务通过状态
        if (CheckBox_H4_PLAYTHROUGH_STATUS.IsChecked == true)
            AppendLogger("MPx_H4_PLAYTHROUGH_STATUS", 10);
    }

    private async Task STAT_Run()
    {
        if (STAT_DIC.Count == 0)
        {
            NotifierHelper.Show(NotifierType.Warning, "STAT代码不能为空，操作取消");
            return;
        }

        ClearLogger();

        AppendLogger("正在执行STAT代码中...");

        var index = 1;
        foreach (var item in STAT_DIC)
        {
            AppendLogger($"正在执行 第 {index++}/{STAT_DIC.Count} 条代码");

            await STATS.STAT_SET_INT(item.Key, item.Value);
        }

        AppendLogger("STAT代码执行完毕");
    }

    private void Button_STAT_Build_Click(object sender, RoutedEventArgs e)
    {


        Button_STAT_Run.IsEnabled = false;
        Button_STAT_Build.IsEnabled = false;
        STAT_Build();
        Button_STAT_Run.IsEnabled = true;
        Button_STAT_Build.IsEnabled = true;
    }

    private async void Button_STAT_Run_Click(object sender, RoutedEventArgs e)
    {


        Button_STAT_Run.IsEnabled = false;
        Button_STAT_Build.IsEnabled = false;
        await STAT_Run();
        Button_STAT_Run.IsEnabled = true;
        Button_STAT_Build.IsEnabled = true;
    }
}
