namespace GTA5OnlineTools.Utils;

public static class FileUtil
{
    /// <summary>
    /// 获取当前运行文件完整路径
    /// </summary>
    public static string File_MainApp { get; private set; }

    /// <summary>
    /// 获取当前文件目录，不加文件名及后缀
    /// </summary>
    public static string Dir_MainApp { get; private set; }

    static FileUtil()
    {
        File_MainApp = Environment.ProcessPath;
        Dir_MainApp = AppDomain.CurrentDomain.BaseDirectory;
    }

    /// <summary>
    /// 文件重命名
    /// </summary>
    public static void FileReName(string oldPath, string newPath)
    {
        var ReName = new FileInfo(oldPath);
        ReName.MoveTo(newPath);
    }

    /// <summary>
    /// 给文件名，得出当前目录完整路径，AppName带文件名后缀
    /// </summary>
    public static string GetCurrFullPath(string appName)
    {
        return Path.Combine(Dir_MainApp, appName);
    }
}
