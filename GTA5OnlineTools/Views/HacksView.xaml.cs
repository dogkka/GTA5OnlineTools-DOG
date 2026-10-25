using CommunityToolkit.Mvvm.Input;
using GTA5Core.Native;
using GTA5Inject;
using GTA5OnlineTools.Models;
using GTA5OnlineTools.Views.ReadMe;
using GTA5OnlineTools.Windows;
using GTA5Shared.Helper;

namespace GTA5OnlineTools.Views;

/// <summary>
/// HacksView.xaml 的交互逻辑
/// </summary>
public partial class HacksView : UserControl
{
    /// <summary>
    /// 数据模型绑定
    /// </summary>
    public HacksModel HacksModel { get; set; } = new();

    private readonly YimMenu YimMenu = new();
    private readonly YimMenuV2 YimMenuV2 = new();

    private OnlineLuaWindow OnlineLuaWindow = null;
    private FSLWindow FSLWindow = null;
    private YimDLWindow YimDLWindow = null;

    public HacksView()
    {
        HacksModel.IsYimMenuLangZhTw = IniHelper.ReadValue("Hacks", "IsYimMenuLangZhTw").Equals("True", StringComparison.OrdinalIgnoreCase); ;

        InitializeComponent();
        MainWindow.WindowClosingEvent += MainWindow_WindowClosingEvent;
    }

    private void MainWindow_WindowClosingEvent()
    {
        IniHelper.WriteValue("Hacks", "IsYimMenuLangZhTw", $"{HacksModel.IsYimMenuLangZhTw}");
    }

    /// <summary>
    /// 点击第三方辅助开关按钮
    /// </summary>
    /// <param name="hackName"></param>
    [RelayCommand]
    private void HacksClick(string hackName)
    {

        if (ProcessHelper.IsGTA5Run())
        {
            switch (hackName)
            {
                case "YimMenu_Local":
                    YimMenuClick_Local();
                    break;
                case "YimMenu_Online":
                    YimMenuClick_Online();
                    break;
                case "YimMenu_v2":
                    YimMenu_V2_Local_Click();
                    break;
                case "YimMenu_v2_Online":
                    YimMenu_V2_Online_Click();
                    break;
            }
        }
        else
        {
            NotifierHelper.Show(NotifierType.Warning, "未发现《GTA5》进程，请先运行《GTA5》游戏");
        }
    }

    /// <summary>
    /// 点击第三方辅助使用说明
    /// </summary>
    /// <param name="Name"></param>
    [RelayCommand]
    private void ReadMeClick(string Name)
    {


        switch (Name)
        {
            case "YimMenu":
                ShowReadMe(YimMenu);
                break;
            case "YimMenuV2":
                ShowReadMe(YimMenuV2);
                break;
        }
    }

    [RelayCommand]
    private void HacksFuncClick(string funcName)
    {


        switch (funcName)
        {
            case "OnlineLua":
                OnlineLuaClick();
                break;
            case "FSLmanage":
                FSLClick();
                break;
            case "YimDL":
                YimDLClick();
                break;
            case "ntf":
                NotificationClick();
                break;
        }
    }

    /// <summary>
    /// 点击第三方辅助配置文件路径
    /// </summary>
    /// <param name="funcName"></param>
    [RelayCommand]
    private void ExtraClick(string funcName)
    {
        try
        {


            switch (funcName)
            {
                ////////////////////////////////////
                #region Yimmenu功能
                //////////////
                case "YimMenuDirectory":
                    YimMenuDirectoryClick();
                    break;
                case "YimMenuV2Directory":
                    YimMenuV2DirectoryClick();
                    break;

                case "YimMenuScriptsDirectory":
                    YimMenuScriptsDirectoryClick();
                    break;
                case "YimMenuV2ScriptsDirectory":
                    YimMenuV2ScriptsDirectoryClick();
                    break;
                case "EditYimMenuConfig":
                    EditYimMenuConfigClick();
                    break;
                case "EditYimMenuV2Config":
                    EditYimMenuV2ConfigClick();
                    break;
                case "ViewYimMenuLogger":
                    ViewYimMenuLoggerClick();
                    break;
                case "ViewYimMenuV2Logger":
                    ViewYimMenuV2LoggerClick();
                    break;
                case "ResetYimMenuConfig":
                    ResetYimMenuConfigClick();
                    break;
                case "ResetYimMenuV2Config":
                    ResetYimMenuV2ConfigClick();
                    break;
                    #endregion
            }
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
    }

    /// <summary>
    /// 显示使用说明窗口
    /// </summary>
    /// <param name="userControl"></param>
    private void ShowReadMe(UserControl userControl)
    {
        var readMeWindow = new ReadMeWindow(userControl)
        {
            Owner = MainWindow.MainWindowInstance
        };
        readMeWindow.ShowDialog();
    }

    #region 第三方辅助功能开关事件

    /// <summary>
    /// YimMenu点击事件
    /// </summary>
    private async void YimMenuClick_Local()
    {
        try
        {
            // 释放Yimmenu官中语言文件
            FileHelper.CreateDirectory(FileHelper.Dir_AppData_YimMenu_Translations);

            // 是否使用繁体中文
            if (HacksModel.IsYimMenuLangZhTw)
            {
                FileHelper.ExtractResFile(FileHelper.Res_YimMenu_IndexTW, FileHelper.File_YimMenu_IndexTW);
                FileHelper.ExtractResFile(FileHelper.Res_YimMenu_ZHTW, FileHelper.File_YimMenu_ZHTW);
            }
            else
            {
                FileHelper.ExtractResFile(FileHelper.Res_YimMenu_IndexCN, FileHelper.File_YimMenu_IndexCN);
                FileHelper.ExtractResFile(FileHelper.Res_YimMenu_ZHCN, FileHelper.File_YimMenu_ZHCN);
            }
        }
        catch (Exception ex)
        {
            LoggerHelper.Error($"释放Yimmenu官中语言文件失败，异常信息：{ex.Message}");
        }

        await Task.Delay(100);

        // 由于玩家可能只使用YimMenu，GTA5Core模块不会初始化，这里要单独处理
        Process gta5Process;
        if (Memory.GTA5ProId == 0)
        {
            var pArray = Process.GetProcessesByName("GTA5");
            gta5Process = pArray.First();
        }
        else
        {
            gta5Process = Memory.GTA5Process;
        }

        var result = Injector.DLLInjector(gta5Process.Id, FileHelper.File_YimMenu_DLL_X, true);
        if (result.IsSuccess)
            NotifierHelper.Show(NotifierType.Success, "YimMenu菜单注入成功");
        else
            NotifierHelper.Show(NotifierType.Error, $"YimMenu菜单注入失败\n错误信息：{result.Content}");
    }
    private async void YimMenuClick_Online()
    {
        try
        {
            // 释放Yimmenu官中语言文件
            FileHelper.CreateDirectory(FileHelper.Dir_AppData_YimMenu_Translations);

            // 是否使用繁体中文
            if (HacksModel.IsYimMenuLangZhTw)
            {
                FileHelper.ExtractResFile(FileHelper.Res_YimMenu_IndexTW, FileHelper.File_YimMenu_IndexTW);
                FileHelper.ExtractResFile(FileHelper.Res_YimMenu_ZHTW, FileHelper.File_YimMenu_ZHTW);
            }
            else
            {
                FileHelper.ExtractResFile(FileHelper.Res_YimMenu_IndexCN, FileHelper.File_YimMenu_IndexCN);
                FileHelper.ExtractResFile(FileHelper.Res_YimMenu_ZHCN, FileHelper.File_YimMenu_ZHCN);
            }
        }
        catch (Exception ex)
        {
            LoggerHelper.Error($"释放Yimmenu官中语言文件失败，异常信息：{ex.Message}");
        }

        await Task.Delay(100);

        if (!File.Exists(FileHelper.File_YimMenu_DLL_V1_Online))
        {
            NotifierHelper.Show(NotifierType.Warning, "YimMenu V1在线版本不存在，请先到Yim管理菜单下载");
            return;
        }

        // 由于玩家可能只使用YimMenu，GTA5Core模块不会初始化，这里要单独处理
        Process gta5Process;
        if (Memory.GTA5ProId == 0)
        {
            var pArray = Process.GetProcessesByName("GTA5");
            gta5Process = pArray.First();
        }
        else
        {
            gta5Process = Memory.GTA5Process;
        }

        var result = Injector.DLLInjector(gta5Process.Id, FileHelper.File_YimMenu_DLL_V1_Online, true);
        if (result.IsSuccess)
            NotifierHelper.Show(NotifierType.Success, "YimMenu菜单注入成功");
        else
            NotifierHelper.Show(NotifierType.Error, $"YimMenu菜单注入失败\n错误信息：{result.Content}");
    }
    private static void YimMenu_V2_Local_Click()
    {
        if (!Directory.Exists(FileHelper.Dir_AppData_YimMenu_V2))
            Directory.CreateDirectory(FileHelper.Dir_AppData_YimMenu_V2);

        if (!File.Exists(FileHelper.File_AppData_YimMenu_V2_Font))
            FileHelper.ExtractResFile(FileHelper.Res_YimMenu_YimMenu_V2_Font, FileHelper.File_AppData_YimMenu_V2_Font);


        Process gta5Process;
        if (Memory.GTA5ProId == 0)
        {
            var pArray = Process.GetProcessesByName("GTA5_Enhanced");
            gta5Process = pArray.First();
        }
        else
        {
            gta5Process = Memory.GTA5Process;
        }

        var result = Injector.DLLInjector(gta5Process.Id, FileHelper.File_YimMenu_DLL_V2, true);
        if (result.IsSuccess)
            NotifierHelper.Show(NotifierType.Success, "YimMenu V2菜单注入成功");
        else
            NotifierHelper.Show(NotifierType.Error, $"YimMenu V2菜单注入失败\n错误信息：{result.Content}");
    }
    private static void YimMenu_V2_Online_Click()
    {
        if (!Directory.Exists(FileHelper.Dir_AppData_YimMenu_V2))
            Directory.CreateDirectory(FileHelper.Dir_AppData_YimMenu_V2);

        if (!File.Exists(FileHelper.File_AppData_YimMenu_V2_Font))
            FileHelper.ExtractResFile(FileHelper.Res_YimMenu_YimMenu_V2_Font, FileHelper.File_AppData_YimMenu_V2_Font);

        if (!File.Exists(FileHelper.File_YimMenu_DLL_V2_Online))
        {
            NotifierHelper.Show(NotifierType.Warning, "YimMenu V2在线版本不存在，请先到Yim管理菜单下载");
            return;
        }

        Process gta5Process;
        if (Memory.GTA5ProId == 0)
        {
            var pArray = Process.GetProcessesByName("GTA5_Enhanced");
            gta5Process = pArray.First();
        }
        else
        {
            gta5Process = Memory.GTA5Process;
        }

        var result = Injector.DLLInjector(gta5Process.Id, FileHelper.File_YimMenu_DLL_V2_Online, true);
        if (result.IsSuccess)
            NotifierHelper.Show(NotifierType.Success, "YimMenu V2菜单注入成功");
        else
            NotifierHelper.Show(NotifierType.Error, $"YimMenu V2菜单注入失败\n错误信息：{result.Content}");
    }

    #endregion

    #region 其他额外功能

    /// <summary>
    /// YimMenu配置目录
    /// </summary>
    private void YimMenuDirectoryClick()
    {
        ProcessHelper.OpenDir(FileHelper.Dir_AppData_YimMenu);
    }

    /// <summary>
    /// YimMenuV2配置目录
    /// </summary>
    private void YimMenuV2DirectoryClick()
    {
        ProcessHelper.OpenDir(FileHelper.Dir_AppData_YimMenu_V2);
    }
    /// <summary>
    /// YimMenu脚本目录
    /// </summary>
    private void YimMenuScriptsDirectoryClick()
    {
        ProcessHelper.OpenDir(FileHelper.Dir_AppData_YimMenu_Scripts);
    }

    /// <summary>
    /// YimMenuV2脚本目录
    /// </summary>
    private void YimMenuV2ScriptsDirectoryClick()
    {
        ProcessHelper.OpenDir(Path.Combine(FileHelper.Dir_AppData_YimMenu_V2, "scripts"));
    }

    /// <summary>
    /// YimMenu配置文件
    /// </summary>
    private void EditYimMenuConfigClick()
    {
        ProcessHelper.Notepad2EditTextFile(FileHelper.File_AppData_YimMenu_Settings);
    }

    /// <summary>
    /// YimMenuV2配置文件
    /// </summary>
    private void EditYimMenuV2ConfigClick()
    {
        ProcessHelper.Notepad2EditTextFile(FileHelper.File_AppData_YimMenu_V2_Settings);
    }

    /// <summary>
    /// YimMenu错误日志
    /// </summary>
    private void ViewYimMenuLoggerClick()
    {
        ProcessHelper.Notepad2EditTextFile(FileHelper.File_AppData_YimMenu_Logger);
    }

    /// <summary>
    /// YimMenuV2错误日志
    /// </summary>
    private void ViewYimMenuV2LoggerClick()
    {
        ProcessHelper.Notepad2EditTextFile(FileHelper.File_AppData_YimMenu_V2_Logger);
    }

    /// <summary>
    /// 重置YimMenu配置文件
    /// </summary>
    private void ResetYimMenuConfigClick()
    {
        if (FileHelper.IsOccupied(FileHelper.File_YimMenu_DLL) || FileHelper.IsOccupied(FileHelper.File_YimMenu_DLL_X))
        {
            NotifierHelper.Show(NotifierType.Warning, "YimMenu被占用，请先卸载YimMenu菜单后再执行操作");
            return;
        }

        if (MessageBox.Show($"你确定要重置YimMenu配置文件吗？\n\n将清空「{FileHelper.Dir_AppData_YimMenu}」文件夹，如有重要文件请提前备份",
            "重置YimMenu配置文件", MessageBoxButton.YesNo, MessageBoxImage.Warning) == MessageBoxResult.Yes)
        {
            FileHelper.ClearDirectory(FileHelper.Dir_AppData_YimMenu);

            NotifierHelper.Show(NotifierType.Success, "重置YimMenu配置文件成功");
        }
    }

    /// <summary>
    /// 重置YimMenu配置文件
    /// </summary>
    private void ResetYimMenuV2ConfigClick()
    {
        if (FileHelper.IsOccupied(FileHelper.File_YimMenu_DLL_V2))
        {
            NotifierHelper.Show(NotifierType.Warning, "YimMenuV2被占用，请先卸载YimMenu菜单后再执行操作");
            return;
        }

        if (MessageBox.Show($"你确定要重置YimMenuV2配置文件吗？\n\n将清空「{FileHelper.Dir_AppData_YimMenu_V2}」文件夹，如有重要文件请提前备份",
            "重置YimMenu配置文件", MessageBoxButton.YesNo, MessageBoxImage.Warning) == MessageBoxResult.Yes)
        {
            FileHelper.ClearDirectory(FileHelper.Dir_AppData_YimMenu_V2);

            NotifierHelper.Show(NotifierType.Success, "重置YimMenuV2配置文件成功");
        }
    }

    #endregion

    /// <summary>
    /// 打开在线下载Lua脚本窗口
    /// </summary>
    private void OnlineLuaClick()
    {
        if (OnlineLuaWindow == null)
        {
            OnlineLuaWindow = new OnlineLuaWindow();
            OnlineLuaWindow.Show();
        }
        else
        {
            if (OnlineLuaWindow.IsVisible)
            {
                if (!OnlineLuaWindow.Topmost)
                {
                    OnlineLuaWindow.Topmost = true;
                    OnlineLuaWindow.Topmost = false;
                }

                OnlineLuaWindow.WindowState = WindowState.Normal;
            }
            else
            {
                OnlineLuaWindow = null;
                OnlineLuaWindow = new OnlineLuaWindow();
                OnlineLuaWindow.Show();
            }
        }
    }

    /// <summary>
    /// 打开FSL下载窗口
    /// </summary>
    private void FSLClick()
    {
        if (FSLWindow == null)
        {
            FSLWindow = new FSLWindow();
            FSLWindow.Show();
        }
        else
        {
            if (FSLWindow.IsVisible)
            {
                if (!FSLWindow.Topmost)
                {
                    FSLWindow.Topmost = true;
                    FSLWindow.Topmost = false;
                }

                FSLWindow.WindowState = WindowState.Normal;
            }
            else
            {
                FSLWindow = null;
                FSLWindow = new FSLWindow();
                FSLWindow.Show();
            }
        }
    }

    /// <summary>
    /// 打开YimDL下载窗口
    /// </summary>
    private void YimDLClick()
    {
        if (YimDLWindow == null)
        {
            YimDLWindow = new YimDLWindow();
            YimDLWindow.Show();
        }
        else
        {
            if (YimDLWindow.IsVisible)
            {
                if (!YimDLWindow.Topmost)
                {
                    YimDLWindow.Topmost = true;
                    YimDLWindow.Topmost = false;
                }

                YimDLWindow.WindowState = WindowState.Normal;
            }
            else
            {
                YimDLWindow = null;
                YimDLWindow = new YimDLWindow();
                YimDLWindow.Show();
            }
        }
    }
    private void NotificationClick()
    {
        var NotificationWindow = new NotificationWindow
        {
            Owner = MainWindow.MainWindowInstance
        };
        NotificationWindow.ShowDialog();
    }

}
