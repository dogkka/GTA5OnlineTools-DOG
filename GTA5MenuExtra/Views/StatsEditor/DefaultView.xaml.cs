using GTA5Core.Features;
using GTA5Core.GTA.Stats;

namespace GTA5MenuExtra.Views.StatsEditor;

/// <summary>
/// DefaultView.xaml 的交互逻辑
/// </summary>
public partial class DefaultView : UserControl
{
    public DefaultView()
    {
        InitializeComponent();

        // STAT列表
        foreach (var item in StatData.StatClasses)
        {
            ListBox_STATClass.Items.Add(item.Name);
        }
        ListBox_STATClass.SelectedIndex = 0;
    }

    private void AppendLogger(string log)
    {
        this.Dispatcher.Invoke(() =>
        {
            TextBox_Logger.AppendText($"[{DateTime.Now:T}] {log}\n");
            TextBox_Logger.ScrollToEnd();
        });
    }

    private void AppendLogger(string statName, int value)
    {
        TextBox_Logger.AppendText($"${statName}\n");
        TextBox_Logger.AppendText($"{value}\n");
    }

    private void Button_ExecuteAutoScript_Click(object sender, RoutedEventArgs e)
    {


        var item = ListBox_STATClass.SelectedItem;
        if (item != null)
            STAT_SET_VALUE(item.ToString());
    }

    private void ListBox_STATClass_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (ListBox_STATClass.SelectedItem is string statClassName)
        {
            var result = StatData.StatClasses.Find(t => t.Name == statClassName);
            if (result == null)
                return;

            TextBox_Logger.Clear();
            TextBox_Logger.AppendText("INT32\n");

            for (var i = 0; i < result.StatInfos.Count; i++)
            {
                var hash = result.StatInfos[i].Hash;
                var value = result.StatInfos[i].Value;

                AppendLogger(hash, value);
            }
        }
    }

    private void STAT_SET_VALUE(string statClassName)
    {
        TextBox_Logger.Clear();

        Task.Run(async () =>
        {
            try
            {
                var result = StatData.StatClasses.Find(t => t.Name == statClassName);
                if (result != null)
                {
                    AppendLogger($"正在执行 {result.Name} STAT代码");

                    var count = result.StatInfos.Count;
                    for (int i = 0; i < count; i++)
                    {
                        AppendLogger($"正在执行 第 {i + 1}/{count} 条代码");

                        await STATS.STAT_SET_INT(result.StatInfos[i].Hash, result.StatInfos[i].Value);
                    }

                    AppendLogger($"{result.Name} STAT代码执行完毕");
                }
            }
            catch (Exception ex)
            {
                AppendLogger($"错误：{ex.Message}");
            }
        });
    }
}
