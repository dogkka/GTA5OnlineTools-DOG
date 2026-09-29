using CommunityToolkit.Mvvm.Input;
using GTA5OnlineTools.Models;
using GTA5OnlineTools.Utils;
using GTA5Shared.Helper;

namespace GTA5OnlineTools;

/// <summary>
/// LoadWindow.xaml 的交互逻辑
/// </summary>
public partial class LoadWindow
{
    /// <summary>
    /// 数据模型绑定
    /// </summary>
    public LoadModel LoadModel { get; set; } = new();

    public LoadWindow()
    {
        InitializeComponent();
    }

    private void Window_Load_Loaded(object sender, RoutedEventArgs e)
    {
        Task.Run(() =>
        {
            try
            {
                // 关闭第三方进程
                ProcessHelper.CloseThirdProcess();

                LoadModel.LoadState = "正在初始化工具...";
                LoadModel.IsShowLoading = true;

                LoggerHelper.Info("开始初始化程序...");
                LoggerHelper.Info($"当前程序版本号 {CoreUtil.ClientVersion}");
                LoggerHelper.Info($"当前程序最后编译时间 {CoreUtil.BuildDate}");

                // 客户端程序版本号
                LoadModel.VersionInfo = CoreUtil.ClientVersion;
                // 最后编译时间
                LoadModel.BuildDate = CoreUtil.BuildDate;

                /////////////////////////////////////////////////////////////////////

                LoadModel.LoadState = "正在初始化配置文件...";
                LoggerHelper.Info("正在初始化配置文件...");

                // 创建指定文件夹，用于释放必要文件和更新软件（如果已存在则不会创建）
                FileHelper.CreateDirectory(FileHelper.Dir_Cache);
                FileHelper.CreateDirectory(FileHelper.Dir_YimMenu);
                FileHelper.CreateDirectory(FileHelper.Dir_FSL);
                FileHelper.CreateDirectory(FileHelper.Dir_Config);
                FileHelper.CreateDirectory(FileHelper.Dir_Logger);

                FileHelper.CreateDirectory(FileHelper.Dir_Log_NLog);
                FileHelper.CreateDirectory(FileHelper.Dir_Log_Crash);

                // 清空缓存文件夹
                FileHelper.ClearDirectory(FileHelper.Dir_Cache);


                /////////////////////////////////////////////////////////////////////
                FileHelper.ExtractResFile(FileHelper.Res_Cache_Notepad2, FileHelper.File_Cache_Notepad2);

                // 释放内置 FSL V6（WINMM.dll）
                try
                {
                    FileHelper.ExtractResFile(FileHelper.Res_FSL_WINMM, FileHelper.File_FSL_WINMM);
                    LoggerHelper.Info("释放内置 FSL V6 成功");
                }
                catch (Exception ex)
                {
                    LoggerHelper.Warn("释放内置 FSL V6 失败：" + ex.Message);
                }

                // 判断YimMenu.dll文件是否存在 是否被占用
                if (!FileHelper.IsOccupied(FileHelper.File_YimMenu_DLL))
                {
                    FileHelper.ExtractResFile(FileHelper.Res_YimMenu_YimMenu, FileHelper.File_YimMenu_DLL);
                    LoggerHelper.Info("释放YimMenu.dll文件成功");
                }
                else
                {
                    LoggerHelper.Warn("YimMenu.dll文件正在被占用，跳过释放");
                }

                // 判断YimMenu.dll Mr_X 文件 是否被占用
                if (!FileHelper.IsOccupied(FileHelper.File_YimMenu_DLL_X))
                {
                    FileHelper.ExtractResFile(FileHelper.Res_YimMenu_YimMenu_X, FileHelper.File_YimMenu_DLL_X);
                    LoggerHelper.Info("释放YimMenu.dll2文件成功");
                }
                else
                {
                    LoggerHelper.Warn("YimMenu.dll2文件正在被占用，跳过释放");
                }

                // 检查目录是否存在，如果不存在则创建
                if (!Directory.Exists(FileHelper.Dir_AppData_YimMenu_V2))
                {
                    Directory.CreateDirectory(FileHelper.Dir_AppData_YimMenu_V2);
                }
                FileHelper.ExtractResFile(FileHelper.Res_YimMenu_YimMenu_V2_Font, FileHelper.File_AppData_YimMenu_V2_Font);

                if (!File.Exists(FileHelper.File_AppData_YimMenu_V2_Font)) { 
                    LoggerHelper.Error("字体释放失败"); 
                }

                // 判断NewBase.dll 文件 是否被占用
                if (!FileHelper.IsOccupied(FileHelper.File_YimMenu_DLL_V2))
                {
                    FileHelper.ExtractResFile(FileHelper.Res_YimMenu_YimMenu_V2, FileHelper.File_YimMenu_DLL_V2);
                    LoggerHelper.Info("释放Yim V2文件成功");
                }
                else
                {
                    LoggerHelper.Warn("NewBase.dll文件正在被占用，跳过释放");
                }

                /////////////////////////////////////////////////////////////////////
                // [DOG 离线版] 移除原项目的域名过期警告，直接进入主界面
                this.Dispatcher.Invoke(() =>
                {
                    var mainWindow = new MainWindow();
                    // 转移主程序控制权
                    Application.Current.MainWindow = mainWindow;
                    // 显示主窗口
                    mainWindow.Show();
                    // 关闭初始化窗口
                    this.Close();
                });
            }
            catch (Exception ex)
            {
                LoadModel.LoadState = $"初始化错误，发生了未知异常！\n{ex.Message}";
                LoadModel.IsShowLoading = false;
                LoadModel.IsInitError = true;
                LoggerHelper.Error("初始化错误，发生了未知异常", ex);
            }
        });
    }

    [RelayCommand]
    private void ButtonClick(string name)
    {
        switch (name)
        {
            case "OpenDefaultPath":
                ProcessHelper.OpenProcess(FileHelper.Dir_Base);
                break;
            case "ExitAPP":
                Application.Current.Shutdown();
                break;
        }
    }
}
