using System.Diagnostics;

namespace AppLauncher;

public class Program
{
    static void Main(string[] args)
    {
        foreach (string file in Directory.GetFiles(AppDomain.CurrentDomain.BaseDirectory))
        {
            var fileInfo = new FileInfo(file);

            if (fileInfo.Name.Contains("旧版本小助手请手动删除") &&
                fileInfo.Extension.Equals(".exe"))
            {
                File.Delete(file);
            }

            if (fileInfo.Name.StartsWith("GTA5线上小助手 支持1.67 完全免费") &&
                fileInfo.Extension.Equals(".exe"))
            {
                OpenProcess(file);
            }
        }
    }

    static void OpenProcess(string path)
    {
        Process.Start(new ProcessStartInfo(path) { UseShellExecute = true });
    }
}
