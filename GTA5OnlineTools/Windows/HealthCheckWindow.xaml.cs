using GTA5OnlineTools.Utils;
using GTA5Shared.Helper;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;

namespace GTA5OnlineTools.Windows;

/// <summary>
/// [DOG] 环境自检窗口：只读检查 + 复制结果 / 生成反馈包
/// </summary>
public partial class HealthCheckWindow
{
    private List<HealthCheckHelper.Item> _items = new();

    public HealthCheckWindow()
    {
        InitializeComponent();
        Loaded += (_, _) => Refresh();
    }

    private async void Refresh()
    {
        try
        {
            Button_Recheck.IsEnabled = false;
            Panel_Items.Children.Clear();
            Panel_Items.Children.Add(new TextBlock
            {
                Text = "正在检查…",
                FontSize = 13,
                Foreground = new SolidColorBrush(Color.FromRgb(0x99, 0x99, 0x99))
            });

            _items = await Task.Run(HealthCheckHelper.RunAll);
            RenderItems();
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
        finally
        {
            Button_Recheck.IsEnabled = true;
        }
    }

    private void RenderItems()
    {
        Panel_Items.Children.Clear();
        foreach (var item in _items)
        {
            var border = new Border
            {
                Background = new SolidColorBrush(Color.FromRgb(0xF7, 0xF8, 0xFA)),
                CornerRadius = new CornerRadius(6),
                Padding = new Thickness(10, 8, 10, 8),
                Margin = new Thickness(0, 0, 0, 8)
            };

            var panel = new StackPanel();

            var title = new TextBlock { FontSize = 13.5, FontWeight = FontWeights.Bold };
            title.Inlines.Add(new Run(item.Icon)
            {
                Foreground = new SolidColorBrush((Color)ColorConverter.ConvertFromString(item.IconColor))
            });
            title.Inlines.Add(new Run(" " + item.Title));
            panel.Children.Add(title);

            if (!string.IsNullOrEmpty(item.Detail))
            {
                panel.Children.Add(new TextBlock
                {
                    Text = item.Detail,
                    FontSize = 12.5,
                    Foreground = new SolidColorBrush(Color.FromRgb(0x33, 0x33, 0x33)),
                    TextWrapping = TextWrapping.Wrap,
                    Margin = new Thickness(0, 4, 0, 0)
                });
            }

            if (!string.IsNullOrEmpty(item.Hint))
            {
                panel.Children.Add(new TextBlock
                {
                    Text = "→ " + item.Hint,
                    FontSize = 12,
                    Foreground = new SolidColorBrush(Color.FromRgb(0xE6, 0xA2, 0x3C)),
                    TextWrapping = TextWrapping.Wrap,
                    Margin = new Thickness(0, 4, 0, 0)
                });
            }

            if (!string.IsNullOrEmpty(item.ActionKey))
            {
                var action = new Button
                {
                    Content = string.IsNullOrEmpty(item.ActionLabel) ? "一键修复" : item.ActionLabel,
                    Padding = new Thickness(12, 4, 12, 4),
                    Margin = new Thickness(0, 7, 0, 0),
                    HorizontalAlignment = HorizontalAlignment.Right,
                    Cursor = Cursors.Hand,
                    Tag = item.ActionKey
                };
                action.Click += ActionButton_Click;
                panel.Children.Add(action);
            }

            border.Child = panel;
            Panel_Items.Children.Add(border);
        }
    }

    private void ActionButton_Click(object sender, RoutedEventArgs e)
    {
        try
        {
            if (sender is not Button btn || btn.Tag is not string key || string.IsNullOrEmpty(key))
                return;

            btn.IsEnabled = false;
            var (ok, message) = HealthCheckHelper.RunFix(key);
            NotifierHelper.Show(ok ? NotifierType.Success : NotifierType.Error, message);
            Refresh();
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
    }

    private void Button_Recheck_Click(object sender, RoutedEventArgs e)
    {
        Refresh();
    }

    private void Button_Copy_Click(object sender, RoutedEventArgs e)
    {
        try
        {
            Clipboard.SetText(HealthCheckHelper.ToText(_items));
            NotifierHelper.Show(NotifierType.Success, "自检结果已复制，可直接粘贴到 QQ 群求助。");
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
    }

    private void Button_Bundle_Click(object sender, RoutedEventArgs e)
    {
        try
        {
            var (ok, message, _) = SupportBundleHelper.Collect();
            NotifierHelper.Show(ok ? NotifierType.Success : NotifierType.Error, message);
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
    }

    private void Button_Close_Click(object sender, RoutedEventArgs e)
    {
        Close();
    }
}
