using CommunityToolkit.Mvvm.Input;
using GTA5Core;
using GTA5OnlineTools.Utils;
using GTA5Shared.Helper;

namespace GTA5OnlineTools.Views;

/// <summary>
/// ToolsView.xaml 的交互逻辑
/// </summary>
public partial class ToolsView : UserControl
{
    public ToolsView()
    {
        InitializeComponent();
        MainWindow.WindowClosingEvent += MainWindow_WindowClosingEvent;
    }

    private void MainWindow_WindowClosingEvent()
    {

    }

    /// <summary>
    /// 工具按钮点击
    /// </summary>
    /// <param name="name"></param>
    [RelayCommand]
    private void ToolsButtonClick(string name)
    {


        switch (name)
        {
            #region 分组1
            case "CurrentDirectory":
                CurrentDirectoryClick();
                break;
            case "ReleaseDirectory":
                ReleaseDirectoryClick();
                break;
            case "InitCPDPath":
                InitCPDPathClick();
                break;
            case "ManualGC":
                ManualGCClick();
                break;
            #endregion
            /////////////////////////
            #region 分组2
            case "RestartApp":
                RestartAppClick();
                break;
            case "ReNameAppCN":
                ReNameAppCNClick();
                break;
            case "ReNameAppEN":
                ReNameAppENClick();
                break;
            #endregion
            /////////////////////////
            #region 分组3
            case "IpTest":
                IpTest();
                break;
            case "RefreshDNSCache":
                RefreshDNSCacheClick();
                break;
            case "EditHosts":
                EditHostsClick();
                break;
                #endregion
        }
    }

    ////////////////////////////////////////////////////////////////////////

    #region 分组1
    /// <summary>
    /// 程序当前目录
    /// </summary>
    private void CurrentDirectoryClick()
    {
        ProcessHelper.OpenDir(FileUtil.Dir_MainApp);
    }

    /// <summary>
    /// 程序释放目录
    /// </summary>
    private void ReleaseDirectoryClick()
    {
        ProcessHelper.OpenDir(FileHelper.Dir_Base);
    }

    /// <summary>
    /// 初始化配置文件夹
    /// </summary>
    private async void InitCPDPathClick()
    {
        try
        {
            if (MessageBox.Show("你确定要初始化配置文件吗？将恢复小助手全部配置文件为默认版本，对于修复崩溃问题很有帮助\n\n" +
                $"程序会自动重置此文件夹：\n{FileHelper.Dir_Base}",
                "初始化配置文件", MessageBoxButton.YesNo, MessageBoxImage.Warning) == MessageBoxResult.Yes)
            {
                ProcessHelper.CloseThirdProcess();
                await Task.Delay(100);
                FileHelper.ClearDirectory(FileHelper.Dir_Base);
                await Task.Delay(100);

                App.AppMainMutex.Dispose();
                ProcessHelper.OpenProcess(FileUtil.File_MainApp);
                Application.Current.Shutdown();
            }
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
    }


    /// <summary>
    /// GC垃圾回收
    /// </summary>
    private void ManualGCClick()
    {
        GC.Collect();
        GC.WaitForPendingFinalizers();
        NotifierHelper.Show(NotifierType.Notification, "执行GC垃圾回收成功");
    }

    #endregion

    ////////////////////////////////////////////////////////////////////////

    #region 分组2
    /// <summary>
    /// 重启程序
    /// </summary>
    private void RestartAppClick()
    {
        ProcessHelper.CloseThirdProcess();
        App.AppMainMutex.Dispose();
        ProcessHelper.OpenProcess(FileUtil.File_MainApp);
        Application.Current.Shutdown();
    }

    /// <summary>
    /// 重命名小助手为中文
    /// </summary>
    private async void ReNameAppCNClick()
    {
        try
        {
            var fileName = Path.GetFileName(FileUtil.File_MainApp);
            var name = $"{CoreUtil.MainAppWindowName}{CoreUtil.ClientVersion}.exe";
            if (fileName != name)
            {
                var fullPath = FileUtil.GetCurrFullPath(name);
                FileUtil.FileReName(FileUtil.File_MainApp, fullPath);
                await Task.Delay(100);

                ProcessHelper.CloseThirdProcess();
                App.AppMainMutex.Dispose();
                ProcessHelper.OpenProcess(fullPath);
                Application.Current.Shutdown();
            }
            else
            {
                NotifierHelper.Show(NotifierType.Notification, "程序文件名已经符合中文命名标准，无需继续重命名");
            }
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
    }

    /// <summary>
    /// 重命名小助手为英文
    /// </summary>
    private async void ReNameAppENClick()
    {
        try
        {
            var fileName = Path.GetFileName(FileUtil.File_MainApp);
            if (fileName != "GTA5OnlineTools.exe")
            {
                FileUtil.FileReName(FileUtil.File_MainApp, "GTA5OnlineTools.exe");
                await Task.Delay(100);

                ProcessHelper.CloseThirdProcess();
                App.AppMainMutex.Dispose();
                ProcessHelper.OpenProcess(FileUtil.GetCurrFullPath("GTA5OnlineTools.exe"));
                Application.Current.Shutdown();
            }
            else
            {
                NotifierHelper.Show(NotifierType.Notification, "程序文件名已经符合英文命名标准，无需继续重命名");
            }
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
    }

    #endregion

    ////////////////////////////////////////////////////////////////////////

    #region 分组3
    /// <summary>
    /// 运行网络诊断
    /// </summary>
    private void IpTest()
    {
        FileHelper.ExtractResFile(FileHelper.Res_Cache_CloudflareSpeedTest, FileHelper.File_Cache_CloudflareSpeedTest);
        ProcessHelper.OpenProcess(FileHelper.File_Cache_CloudflareSpeedTest);
    }

    /// <summary>
    /// 刷新DNS缓存
    /// </summary>
    private void RefreshDNSCacheClick()
    {
        HttpHelper.FlushDNSCache();
        NotifierHelper.Show(NotifierType.Success, "成功刷新DNS解析程序缓存");
    }

    /// <summary>
    /// 编辑Hosts文件
    /// </summary>
    private void EditHostsClick()
    {
        ProcessHelper.Notepad2EditTextFile(@"C:\windows\system32\drivers\etc\hosts");
    }
    #endregion
}
