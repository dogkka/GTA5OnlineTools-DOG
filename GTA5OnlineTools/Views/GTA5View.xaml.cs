using CommunityToolkit.Mvvm.Input;
using GTA5Menu;
using GTA5OnlineTools.Data;
using GTA5OnlineTools.Windows;
using GTA5Shared.Helper;

namespace GTA5OnlineTools.Views;

/// <summary>
/// GTA5View.xaml 的交互逻辑
/// </summary>
public partial class GTA5View : UserControl
{
    /// <summary>
    /// 导航字典
    /// </summary>
    private readonly Dictionary<string, NavWindow> NavDictionary = new();

    /// <summary>
    /// 关闭全部GTA5窗口委托
    /// </summary>
    public static Action ActionCloseAllGTA5Window;

    /////////////////////////////////////////////////

    public GTA5View()
    {
        InitializeComponent();
        MainWindow.WindowClosingEvent += MainWindow_WindowClosingEvent;

        ActionCloseAllGTA5Window = CloseAllGTA5Window;

        /////////////////////////////////////////////////

        NavDictionary.Add("ProfilesWindow", new()
        {
            Type = typeof(ProfilesWindow),
            Window = null
        });
    }

    private void MainWindow_WindowClosingEvent()
    {

    }

    [RelayCommand]
    private void GTA5ViewClick(string viewName)
    {

    }

    [RelayCommand]
    private void GTA5FuncClick(string funcName)
    {
        switch (funcName)
        {
            case "StoryProfiles":
                StoryProfilesWindowClick();
                break;
        }
    }

    ///////////////////////////////////////////////////////////////////

    /// <summary>
    /// 自动处理窗口显示、隐藏和创建
    /// </summary>
    /// <param name="windowName"></param>
    private void AutoOpenWindow(string windowName)
    {
        if (!NavDictionary.ContainsKey(windowName))
            return;

        // 首次创建
        if (NavDictionary[windowName].Window == null)
            goto RE_CREATE;

        // 窗口隐藏
        if (NavDictionary[windowName].Window.Visibility == Visibility.Hidden)
        {
            NavDictionary[windowName].Window.Show();
            return;
        }

        // 窗口最小化
        if (NavDictionary[windowName].Window.WindowState == WindowState.Minimized)
        {
            NavDictionary[windowName].Window.WindowState = WindowState.Normal;
            return;
        }

        // 窗口不在最前面（为false代表窗口已被关闭）
        if (NavDictionary[windowName].Window.IsVisible)
        {
            NavDictionary[windowName].Window.Topmost = true;
            NavDictionary[windowName].Window.Topmost = false;
            return;
        }

        // 窗口已被关闭，重新创建
        NavDictionary[windowName].Window.Close();
        NavDictionary[windowName].Window = null;

    RE_CREATE:
        NavDictionary[windowName].Window = Activator.CreateInstance(NavDictionary[windowName].Type) as Window;
        NavDictionary[windowName].Window.Show();
    }

    /// <summary>
    /// 关闭窗口
    /// </summary>
    /// <param name="windowName"></param>
    private void AutoCloseWindow(string windowName)
    {
        if (NavDictionary[windowName].Window != null)
        {
            NavDictionary[windowName].Window.Close();
            NavDictionary[windowName].Window = null;
        }
    }

    ///////////////////////////////////////////////////////////////////

    /// <summary>
    /// 关闭全部GTA5窗口
    /// </summary>
    private void CloseAllGTA5Window()
    {

    }

    ///////////////////////////////////////////////////////////////////

    /// <summary>
    /// 替换故事模式完美存档
    /// </summary>
    private void StoryProfilesWindowClick()
    {
        AutoOpenWindow("ProfilesWindow");
    }
}
