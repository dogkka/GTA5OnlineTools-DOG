using GTA5Core.Features;
using GTA5Core.GTA.Stats;

namespace GTA5MenuExtra.Views.HeistsEditor.Perico;

/// <summary>
/// DefaultView.xaml 的交互逻辑
/// </summary>
public partial class DefaultView : UserControl
{
    public DefaultView()
    {
        InitializeComponent();
    }

    ////////////////////////////////////////////////////

    private void ClearLogger()
    {
        TextBox_Logger.Clear();
    }

    private void AppendLogger(string log)
    {
        TextBox_Logger.AppendText($"[{DateTime.Now:T}] {log}\n");
        TextBox_Logger.ScrollToEnd();
    }

    ////////////////////////////////////////////////////

    private async void Button_STAT_Run_Click(object sender, RoutedEventArgs e)
    {


        Button_STAT_Run.IsEnabled = false;
        await STAT_Run();
        Button_STAT_Run.IsEnabled = true;
    }

    private async Task STAT_Run()
    {
        ClearLogger();

        var selectedIndex = ListBox_STATClass.SelectedIndex;
        AppendLogger($"当前选中索引 {selectedIndex}\n");

        AppendLogger("正在执行STAT代码中...");

        int index;
        int count;

        switch (selectedIndex)
        {
            case 0:
                {
                    index = 1;
                    count = StatData.H4CNF_1.Count;

                    foreach (var item in StatData.H4CNF_1)
                    {
                        AppendLogger($"正在执行 第 {index++}/{count} 条代码");

                        await STATS.STAT_SET_INT(item.Hash, item.Value);
                    }
                }
                break;
            case 1:
                {
                    index = 1;
                    count = StatData.H4CNF_2.Count;

                    foreach (var item in StatData.H4CNF_2)
                    {
                        AppendLogger($"正在执行 第 {index++}/{count} 条代码");

                        await STATS.STAT_SET_INT(item.Hash, item.Value);
                    }
                }
                break;
            case 2:
                {
                    index = 1;
                    count = StatData.H4CNF_3.Count;

                    foreach (var item in StatData.H4CNF_3)
                    {
                        AppendLogger($"正在执行 第 {index++}/{count} 条代码");

                        await STATS.STAT_SET_INT(item.Hash, item.Value);
                    }
                }
                break;
            case 3:
                {
                    index = 1;
                    count = StatData.H4CNF_4.Count;

                    foreach (var item in StatData.H4CNF_4)
                    {
                        AppendLogger($"正在执行 第 {index++}/{count} 条代码");

                        await STATS.STAT_SET_INT(item.Hash, item.Value);
                    }
                }
                break;
        }

        AppendLogger("STAT代码执行完毕");
    }
}
