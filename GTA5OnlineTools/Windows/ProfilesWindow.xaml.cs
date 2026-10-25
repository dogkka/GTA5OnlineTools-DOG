using GTA5Shared.Helper;

namespace GTA5OnlineTools.Windows;

/// <summary>
/// ProfilesWindow.xaml 的交互逻辑
/// </summary>
public partial class ProfilesWindow
{
    public ProfilesWindow()
    {
        InitializeComponent();
    }

    private void Window_Profiles_Loaded(object sender, RoutedEventArgs e)
    {

    }

    private void Window_Profiles_Closing(object sender, CancelEventArgs e)
    {

    }

    /////////////////////////////////////////////////////

    private void ClearLogger()
    {
        TextBox_Logger.Clear();
    }

    private void AppendLogger(string log = "")
    {
        TextBox_Logger.AppendText($"{log}\n");
        TextBox_Logger.ScrollToEnd();
    }

    /////////////////////////////////////////////////////

    private async void Button_ReplaceStroyModeProfiles_Click(object sender, RoutedEventArgs e)
    {


        ClearLogger();

        var path = Path.Combine(FileHelper.Dir_MyDocuments, "Rockstar Games\\GTA V\\Profiles");
        if (!Directory.Exists(path))
        {
            AppendLogger("GTA5故事模式存档路径不存在，操作取消");
            return;
        }

        try
        {
            var dirs = Directory.GetDirectories(path);

            if (dirs.Length == 0)
            {
                AppendLogger($"GTA5故事模式存档为空，操作取消");
                return;
            }

            AppendLogger($"已发现 {dirs.Length}个 GTA5故事模式存档路径");
            AppendLogger();

            foreach (var dir in dirs)
            {
                var profileDir = new DirectoryInfo(dir);

                var profileFile = Path.Combine(profileDir.FullName, "SGTA50000");
                FileHelper.ExtractResFile(FileHelper.Res_Other_SGTA50000, profileFile);

                AppendLogger($"替换GTA5故事模式存档成功 {profileFile}");
                await Task.Delay(1);
            }

            AppendLogger();
            AppendLogger($"替换GTA5故事模式存档成功，操作结束");
        }
        catch (Exception ex)
        {
            AppendLogger();
            AppendLogger($"发生了未知的异常，操作结束。异常信息：{ex.Message}");
        }
    }
    private async void Button_ReplaceStroyModeProfiles_Enhanced_Click(object sender, RoutedEventArgs e)
    {


        ClearLogger();

        var path = Path.Combine(FileHelper.Dir_MyDocuments, "Rockstar Games\\GTAV Enhanced\\Profiles");
        if (!Directory.Exists(path))
        {
            AppendLogger("GTA5故事模式存档路径不存在，操作取消");
            return;
        }

        try
        {
            var dirs = Directory.GetDirectories(path);

            if (dirs.Length == 0)
            {
                AppendLogger($"GTA5故事模式存档为空，操作取消");
                return;
            }

            AppendLogger($"已发现 {dirs.Length}个 GTA5故事模式存档路径");
            AppendLogger();

            foreach (var dir in dirs)
            {
                var profileDir = new DirectoryInfo(dir);

                var profileFile = Path.Combine(profileDir.FullName, "SGTA50000");
                FileHelper.ExtractResFile(FileHelper.Res_Other_SGTA50000, profileFile);

                AppendLogger($"替换GTA5故事模式存档成功 {profileFile}");
                await Task.Delay(1);
            }

            AppendLogger();
            AppendLogger($"替换GTA5故事模式存档成功，操作结束");
        }
        catch (Exception ex)
        {
            AppendLogger();
            AppendLogger($"发生了未知的异常，操作结束。异常信息：{ex.Message}");
        }
    }

}
