using GTA5Core.Features;
using GTA5Shared.Helper;

namespace GTA5MenuExtra.Views.HeistsEditor.Casino;

/// <summary>
/// MissionView.xaml 的交互逻辑
/// </summary>
public partial class MissionView : UserControl
{
    private readonly Dictionary<string, int> STAT_DIC = new();

    public MissionView()
    {
        InitializeComponent();
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

        //////////////////////////////////////////

        // 解锁所有侦察点
        if (Button_H3OPT_ACCESSPOINTS.IsChecked == true)
            AppendLogger("MPx_H3OPT_ACCESSPOINTS", -1);

        // 解锁所有兴趣点
        if (Button_H3OPT_H3OPT_POI.IsChecked == true)
            AppendLogger("MPx_H3OPT_POI", -1);

        // 抢劫方式
        var index = ListBox_H3OPT_APPROACH.SelectedIndex;
        if (IsValidIndex(index))
        {
            AppendLogger("MPx_H3OPT_APPROACH", index);

            // 困难模式
            if (CheckBox_H3_HARD_APPROACH.IsChecked == true)
                AppendLogger("MPx_H3_HARD_APPROACH", index);

            // 最近抢劫
            AppendLogger("MPx_H3_LAST_APPROACH", index == 3 ? new Random().Next(1, 3) : 3);
        }

        // 抢劫物品
        index = ListBox_H3OPT_TARGET.SelectedIndex;
        if (IsValidIndex(index))
        {
            AppendLogger("MPx_H3OPT_TARGET", index - 1);
        }

        /////////////////////////////

        // 赌场准备和工具
        var bitSet1 = 1;

        // security
        if (ListBox_H3OPT_KEYLEVELS.SelectedIndex != 0)
            bitSet1 += 1 << 1;

        // weapon
        bitSet1 += 1 << 2;

        // vehicle
        bitSet1 += 1 << 3;

        // hackingDevice
        bitSet1 += 1 << 4;

        // drone
        if (ListBox_H3OPT_APPROACH.SelectedIndex == 1)
            bitSet1 += 1 << 5;

        // vault laser
        if (ListBox_H3OPT_APPROACH.SelectedIndex == 1)
            bitSet1 += 1 << 6;

        // vault drill
        if (ListBox_H3OPT_APPROACH.SelectedIndex == 2)
            bitSet1 += 1 << 7;

        // vault explosives
        if (ListBox_H3OPT_APPROACH.SelectedIndex == 3)
            bitSet1 += 1 << 8;

        // breaching/thermal charges
        if (ListBox_H3OPT_APPROACH.SelectedIndex == 3)
            bitSet1 += 1 << 9;

        AppendLogger("MPx_H3OPT_BITSET1", bitSet1);

        /////////////////////////////

        // 门禁卡等级
        index = ListBox_H3OPT_KEYLEVELS.SelectedIndex;
        if (IsValidIndex(index))
        {
            AppendLogger("MPx_H3OPT_KEYLEVELS", index);
        }

        // shipment
        AppendLogger("MPx_H3OPT_DISRUPTSHIP", 3);

        // 枪手队员
        index = ListBox_H3OPT_CREWWEAP.SelectedIndex;
        if (IsValidIndex(index))
        {
            AppendLogger("MPx_H3OPT_CREWWEAP", index);
        }

        // 车手队员
        index = ListBox_H3OPT_CREWDRIVER.SelectedIndex;
        if (IsValidIndex(index))
        {
            AppendLogger("MPx_H3OPT_CREWDRIVER", index);
        }

        // 黑客队员
        index = ListBox_H3OPT_CREWHACKER.SelectedIndex;
        if (IsValidIndex(index))
        {
            AppendLogger("MPx_H3OPT_CREWHACKER", index);
        }

        // 武器类型
        index = ListBox_H3OPT_WEAPS.SelectedIndex;
        if (IsValidIndex(index))
        {
            AppendLogger("MPx_H3OPT_WEAPS", index - 1);
        }

        // 逃亡载具
        index = ListBox_H3OPT_VEHS.SelectedIndex;
        if (IsValidIndex(index))
        {
            AppendLogger("MPx_H3OPT_VEHS", index - 1);
        }

        /////////////////////////////

        // 选择队友支援
        var bitSet0 = 1;

        // patrol routes
        bitSet0 += 1 << 1;

        // duggan shipments
        bitSet0 += 1 << 2;

        // infiltration suits - stealth specific
        if (ListBox_H3OPT_APPROACH.SelectedIndex == 1)
            bitSet0 += 1 << 3;

        // power drills
        bitSet0 += 1 << 4;

        // emp - stealth specific
        if (ListBox_H3OPT_APPROACH.SelectedIndex == 1)
            bitSet0 += 1 << 5;

        // decoy
        bitSet0 += 1 << 6;

        // clean vehicle
        bitSet0 += 1 << 7;

        if (ListBox_H3OPT_APPROACH.SelectedIndex == 2)
        {
            // bugstars
            bitSet0 += 3 << 8;

            // sechs
            bitSet0 += 3 << 10;

            // sechs
            bitSet0 += 3 << 12;

            // noose exit disguise
            bitSet0 += 3 << 14;

            // noose
            bitSet0 += 1 << 16;

            // firefighter
            bitSet0 += 1 << 17;
        }

        // highroller
        bitSet0 += 1 << 18;

        // boring machine - aggressive specific
        if (ListBox_H3OPT_APPROACH.SelectedIndex == 3)
            bitSet0 += 1 << 19;

        // reinforced armor - aggressive specific
        if (ListBox_H3OPT_APPROACH.SelectedIndex == 3)
            bitSet0 += 1 << 20;

        AppendLogger("MPx_H3OPT_BITSET0", bitSet0);

        /////////////////////////////

        // 抢劫面具
        index = ListBox_H3OPT_MASKS.SelectedIndex;
        if (IsValidIndex(index))
        {
            AppendLogger("MPx_H3OPT_MASKS", index);
        }
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

    private void ListBox_H3OPT_APPROACH_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        ListBox_H3OPT_CREWWEAP_SelectionChanged(null, null);
    }

    private void ListBox_H3OPT_CREWWEAP_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (ListBox_H3OPT_WEAPS is null)
            return;

        var index = ListBox_H3OPT_WEAPS.SelectedIndex;
        for (int i = ListBox_H3OPT_WEAPS.Items.Count - 1; i > 0; i--)
        {
            ListBox_H3OPT_WEAPS.Items.RemoveAt(i);
        }
        ListBox_H3OPT_WEAPS.SelectedIndex = index;

        // 抢劫方式
        var APPROACH_INDEX = ListBox_H3OPT_APPROACH.SelectedIndex;
        // 枪手队员
        var WEAPS_INDEX = ListBox_H3OPT_CREWWEAP.SelectedIndex;

        switch (WEAPS_INDEX)
        {
            case 1:     // Karl  5%
                switch (APPROACH_INDEX)
                {
                    case 1:
                        ListBox_H3OPT_WEAPS.Items.Add("微型冲锋枪装备");
                        ListBox_H3OPT_WEAPS.Items.Add("冲锋手枪装备");
                        break;
                    case 2:
                        ListBox_H3OPT_WEAPS.Items.Add("微型冲锋枪装备");
                        ListBox_H3OPT_WEAPS.Items.Add("霰弹枪装备");
                        break;
                    case 3:
                        ListBox_H3OPT_WEAPS.Items.Add("霰弹枪装备");
                        ListBox_H3OPT_WEAPS.Items.Add("左轮手枪装备");
                        break;
                }
                break;
            case 2:     // Gustavo  9%
                switch (APPROACH_INDEX)
                {
                    case 1:
                        ListBox_H3OPT_WEAPS.Items.Add("步枪装备");
                        ListBox_H3OPT_WEAPS.Items.Add("霰弹枪装备");
                        break;
                    case 2:
                        ListBox_H3OPT_WEAPS.Items.Add("步枪装备");
                        ListBox_H3OPT_WEAPS.Items.Add("霰弹枪装备");
                        break;
                    case 3:
                        ListBox_H3OPT_WEAPS.Items.Add("步枪装备");
                        ListBox_H3OPT_WEAPS.Items.Add("霰弹枪装备");
                        break;
                }
                break;
            case 3:     // Charlie  7%
                switch (APPROACH_INDEX)
                {
                    case 1:
                        ListBox_H3OPT_WEAPS.Items.Add("冲锋枪装备");
                        ListBox_H3OPT_WEAPS.Items.Add("霰弹枪装备");
                        break;
                    case 2:
                        ListBox_H3OPT_WEAPS.Items.Add("冲锋手枪装备");
                        ListBox_H3OPT_WEAPS.Items.Add("霰弹枪装备");
                        break;
                    case 3:
                        ListBox_H3OPT_WEAPS.Items.Add("冲锋手枪装备");
                        ListBox_H3OPT_WEAPS.Items.Add("霰弹枪装备");
                        break;
                }
                break;
            case 4:     // Chester  10%
                switch (APPROACH_INDEX)
                {
                    case 1:
                        ListBox_H3OPT_WEAPS.Items.Add("MK2霰弹枪装备");
                        ListBox_H3OPT_WEAPS.Items.Add("MK2步枪装备");
                        break;
                    case 2:
                        ListBox_H3OPT_WEAPS.Items.Add("MK2冲锋枪装备");
                        ListBox_H3OPT_WEAPS.Items.Add("MK2步枪装备");
                        break;
                    case 3:
                        ListBox_H3OPT_WEAPS.Items.Add("MK2霰弹枪装备");
                        ListBox_H3OPT_WEAPS.Items.Add("MK2步枪装备");
                        break;
                }
                break;
            case 5:     // Patrick  8%
                switch (APPROACH_INDEX)
                {
                    case 1:
                        ListBox_H3OPT_WEAPS.Items.Add("作战自卫冲锋枪装备");
                        ListBox_H3OPT_WEAPS.Items.Add("步枪装备");
                        break;
                    case 2:
                        ListBox_H3OPT_WEAPS.Items.Add("霰弹枪装备");
                        ListBox_H3OPT_WEAPS.Items.Add("步枪装备");
                        break;
                    case 3:
                        ListBox_H3OPT_WEAPS.Items.Add("霰弹枪装备");
                        ListBox_H3OPT_WEAPS.Items.Add("战斗机枪装备");
                        break;
                }
                break;
        }
    }

    private void ListBox_H3OPT_CREWDRIVER_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (ListBox_H3OPT_VEHS is null)
            return;

        var index = ListBox_H3OPT_VEHS.SelectedIndex;
        for (int i = ListBox_H3OPT_VEHS.Items.Count - 1; i > 0; i--)
        {
            ListBox_H3OPT_VEHS.Items.RemoveAt(i);
        }
        ListBox_H3OPT_VEHS.SelectedIndex = index;

        // 车手队员
        index = ListBox_H3OPT_CREWDRIVER.SelectedIndex;
        switch (index)
        {
            case 1:     // Karim
                ListBox_H3OPT_VEHS.Items.Add("天威经典版");          // Issi Classic
                ListBox_H3OPT_VEHS.Items.Add("反社会");             // Asbo
                ListBox_H3OPT_VEHS.Items.Add("羽黑");               // Kanjo
                ListBox_H3OPT_VEHS.Items.Add("卫士经典版");          // Sentinel Classic
                break;
            case 2:     // Taliana
                ListBox_H3OPT_VEHS.Items.Add("随行者MK2");          // Retinue MK II
                ListBox_H3OPT_VEHS.Items.Add("漂移约塞米蒂");        // Drift Yosemite
                ListBox_H3OPT_VEHS.Items.Add("斯国一");            // Sugoi
                ListBox_H3OPT_VEHS.Items.Add("扼喉");              // Jugular
                break;
            case 3:     // Eddie
                ListBox_H3OPT_VEHS.Items.Add("王者经典版");        // Sultan Classic
                ListBox_H3OPT_VEHS.Items.Add("铁腕经典版");        // Gauntlet Classic
                ListBox_H3OPT_VEHS.Items.Add("爱利");             // Ellie
                ListBox_H3OPT_VEHS.Items.Add("科莫达");           // Komoda
                break;
            case 4:     // Zach
                ListBox_H3OPT_VEHS.Items.Add("曼切兹");           // Manchez
                ListBox_H3OPT_VEHS.Items.Add("斯特德");           // Stryder
                ListBox_H3OPT_VEHS.Items.Add("亵渎者");           // Defiler
                ListBox_H3OPT_VEHS.Items.Add("雷克卓");           // Lectro
                break;
            case 5:     // Chester
                ListBox_H3OPT_VEHS.Items.Add("炸吧");             // Zhaba
                ListBox_H3OPT_VEHS.Items.Add("流浪者");           // Vagrant
                ListBox_H3OPT_VEHS.Items.Add("不法之徒");         // Outlaw
                ListBox_H3OPT_VEHS.Items.Add("埃弗伦");           // Everon
                break;
        }
    }
}
