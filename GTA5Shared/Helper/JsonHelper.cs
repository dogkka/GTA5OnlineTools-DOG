namespace GTA5Shared.Helper;

public static class JsonHelper
{
    /// <summary>
    /// 反序列化配置
    /// </summary>
    private static readonly JsonSerializerOptions OptionsDese = new()
    {
        IncludeFields = true,
        NumberHandling = JsonNumberHandling.AllowNamedFloatingPointLiterals,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull
    };

    /// <summary>
    /// 序列化配置
    /// </summary>
    private static readonly JsonSerializerOptions OptionsSeri = new()
    {
        WriteIndented = true,
        IncludeFields = true,
        NumberHandling = JsonNumberHandling.AllowNamedFloatingPointLiterals,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
        Encoder = JavaScriptEncoder.Create(UnicodeRanges.All)
    };

    /// <summary>
    /// 反序列化，将json字符串转换成json类
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="result"></param>
    /// <returns></returns>
    public static T JsonDese<T>(string result)
    {
        return JsonSerializer.Deserialize<T>(result, OptionsDese);
    }

    /// <summary>
    /// 序列化，将json类转换成json字符串
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="jsonClass"></param>
    /// <returns></returns>
    public static string JsonSeri<T>(T jsonClass)
    {
        return JsonSerializer.Serialize(jsonClass, OptionsSeri);
    }

    /// <summary>
    /// 利用序列化和反序列号深拷贝
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="jsonClass"></param>
    /// <returns></returns>
    public static T DeepClone<T>(T jsonClass)
    {
        return JsonDese<T>(JsonSeri(jsonClass));
    }

    /// <summary>
    /// 读取Json文件
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="savePath"></param>
    /// <param name="jsonClass"></param>
    public static T ReadFile<T>(string savePath)
    {
        return JsonDese<T>(File.ReadAllText(savePath));
    }

    /// <summary>
    /// 写入Json文件
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="savePath"></param>
    /// <param name="jsonClass"></param>
    public static void WriteFile<T>(string savePath, T jsonClass)
    {
        File.WriteAllText(savePath, JsonSeri(jsonClass));
    }
}
