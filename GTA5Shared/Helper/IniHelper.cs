namespace GTA5Shared.Helper;

public static class IniHelper
{
    /// <summary>
    /// 默认配置文件路径
    /// </summary>
    private static string IniPath { get; }

    [DllImport("kernel32", CharSet = CharSet.Unicode)]
    private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);

    [DllImport("kernel32", CharSet = CharSet.Unicode)]
    private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);

    static IniHelper()
    {
        IniPath = Path.Combine(FileHelper.Dir_Config, "Config.ini");
    }

    /// <summary>
    /// 读取节点值
    /// </summary>
    /// <param name="section"></param>
    /// <param name="key"></param>
    /// <param name="intPath"></param>
    /// <returns></returns>
    public static string ReadValue(string section, string key, string intPath)
    {
        var temp = new StringBuilder(1024);
        _ = GetPrivateProfileString(section, key, string.Empty, temp, temp.Capacity, intPath);
        return temp.ToString();
    }

    /// <summary>
    /// 读取节点值
    /// </summary>
    /// <param name="section"></param>
    /// <param name="key"></param>
    /// <returns></returns>
    public static string ReadValue(string section, string key)
    {
        return ReadValue(section, key, IniPath);
    }

    /// <summary>
    /// 写入节点值
    /// </summary>
    /// <param name="section"></param>
    /// <param name="key"></param>
    /// <param name="value"></param>
    /// <param name="intPath"></param>
    public static void WriteValue(string section, string key, string value, string intPath)
    {
        WritePrivateProfileString(section, key, value, intPath);
    }

    /// <summary>
    /// 写入节点值
    /// </summary>
    /// <param name="section"></param>
    /// <param name="key"></param>
    /// <param name="value"></param>
    public static void WriteValue(string section, string key, string value)
    {
        WriteValue(section, key, value, IniPath);
    }
}
