using System;
using System.Collections.Generic;
using System.IO;
using System.Numerics;
using System.Text.Encodings.Web;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Text.Unicode;

namespace FormatConverter;

internal class Program
{
    static void Main(string[] args)
    {
        if (args.Length == 1)
        {
            if (!File.Exists(args[0]))
                return;

            Console.WriteLine($"文件路径为：{args[0]}");

            using var streamReader = new StreamReader(args[0]);
            var oldForamts = JsonDese<List<OldFormat>>(streamReader.ReadToEnd());

            var teleports = new Teleports
            {
                CustomLocations = new()
            };

            foreach (var info in oldForamts)
            {
                teleports.CustomLocations.Add(new()
                {
                    Name = info.Name,
                    X = info.Position.X,
                    Y = info.Position.Y,
                    Z = info.Position.Z,
                    Pitch = 0.0f,
                    Yaw = 0.0f,
                    Roll = 0.0f,
                });
            }

            File.WriteAllText(".\\Teleports.New.json", JsonSeri(teleports));

            Console.WriteLine("转换完成，按任意键关闭程序!");
            Console.ReadKey();
        }
    }

    /// <summary>
    /// 反序列化配置
    /// </summary>
    static readonly JsonSerializerOptions OptionsDese = new()
    {
        IncludeFields = true,
        NumberHandling = JsonNumberHandling.AllowNamedFloatingPointLiterals,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull
    };

    /// <summary>
    /// 序列化配置
    /// </summary>
    static readonly JsonSerializerOptions OptionsSeri = new()
    {
        WriteIndented = true,
        IncludeFields = true,
        NumberHandling = JsonNumberHandling.AllowNamedFloatingPointLiterals,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
        Encoder = JavaScriptEncoder.Create(UnicodeRanges.All)
    };

    public static T JsonDese<T>(string result)
    {
        return JsonSerializer.Deserialize<T>(result, OptionsDese);
    }

    public static string JsonSeri<T>(T jsonClass)
    {
        return JsonSerializer.Serialize(jsonClass, OptionsSeri);
    }
}

internal class OldFormat
{
    public string Name { get; set; }
    public Vector3 Position { get; set; }
}

internal class Teleports
{
    public List<CustomLocationsItem> CustomLocations { get; set; }
}

internal class CustomLocationsItem
{
    public string Name { get; set; }
    public float X { get; set; }
    public float Y { get; set; }
    public float Z { get; set; }
    public float Pitch { get; set; }
    public float Yaw { get; set; }
    public float Roll { get; set; }
}
