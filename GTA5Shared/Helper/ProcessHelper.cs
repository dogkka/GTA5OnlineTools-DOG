namespace GTA5Shared.Helper;

public static class ProcessHelper
{
    /// <summary>
    /// 判断程序是否运行
    /// </summary>
    /// <param name="appName">程序名称</param>
    /// <returns>正在运行返回true，未运行返回false</returns>
    public static bool IsAppRun(string appName)
    {
        return Process.GetProcessesByName(appName).Length > 0;
    }

    /// <summary>
    /// 判断GTA5程序是否运行
    /// </summary>
    /// <returns>正在运行返回true，未运行返回false</returns>
    public static bool IsGTA5Run()
    {
        if (IsAppRun("GTA5") || IsAppRun("GTA5_Enhanced"))
            return true;
        return false;
    }

    /// <summary>
    /// 打开http链接
    /// </summary>
    /// <param name="url"></param>
    public static void OpenLink(string url)
    {
        if (!url.StartsWith("http"))
            return;

        Process.Start(new ProcessStartInfo(url) { UseShellExecute = true });
    }

    /// <summary>
    /// 打开文件夹路径
    /// </summary>
    /// <param name="dir"></param>
    public static void OpenDir(string dir)
    {
        if (!Directory.Exists(dir))
        {
            NotifierHelper.Show(NotifierType.Error, $"要打开的文件夹路径不存在\n{dir}");
            return;
        }

        try
        {
            Process.Start(new ProcessStartInfo(dir) { UseShellExecute = true });
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
    }

    /// <summary>
    /// 打开指定进程，可以附带运行参数
    /// </summary>
    /// <param name="path">本地文件夹路径</param>
    public static void OpenProcess(string path, string args = "")
    {
        if (!File.Exists(path))
        {
            NotifierHelper.Show(NotifierType.Error, $"要打开的文件路径不存在\n{path}");
            return;
        }

        try
        {
            Process.Start(path, args);
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
    }

    /// <summary>
    /// 以管理员权限打开指定程序
    /// </summary>
    /// <param name="path">程序路径</param>
    public static void OpenProcessWithWorkDir(string path)
    {
        if (!File.Exists(path))
        {
            NotifierHelper.Show(NotifierType.Error, $"要打开的文件路径不存在\n{path}");
            return;
        }

        try
        {
            var processInfo = new ProcessStartInfo
            {
                FileName = path,
                Verb = "runas",
                WorkingDirectory = Path.GetDirectoryName(path)
            };

            Process.Start(processInfo);
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
    }

    /// <summary>
    /// 使用Notepad2编辑文本文件
    /// </summary>
    /// <param name="args"></param>
    public static void Notepad2EditTextFile(string args)
    {
        if (!File.Exists(args))
        {
            NotifierHelper.Show(NotifierType.Error, $"使用Notepad2编辑文本文件路径不存在\n{args}");
            return;
        }

        OpenProcess(FileHelper.File_Cache_Notepad2, args);
    }

    /// <summary>
    /// 根据进程名字关闭指定程序
    /// </summary>
    /// <param name="processName">程序名字，不需要加.exe</param>
    public static void CloseProcess(string processName)
    {
        var pArray = Process.GetProcessesByName(processName);
        foreach (var process in pArray)
        {
            process.Kill();
        }
    }

    /// <summary>
    /// 关闭全部第三方exe进程
    /// </summary> 
    public static void CloseThirdProcess()
    {
        CloseProcess("Notepad2");
    }
}
