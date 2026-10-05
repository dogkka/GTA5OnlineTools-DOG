namespace GTA5Shared.Helper;

/// <summary>
/// [DOG] 使用教程：把内嵌的 HTML 教程释放到本地，用默认浏览器打开
/// </summary>
public static class TutorialHelper
{
    /// <summary>
    /// 打开使用教程（HTML 图文教程）
    /// </summary>
    public static void OpenTutorial()
    {
        try
        {
            var dir = Path.Combine(FileHelper.Dir_Cache, "Tutorial");
            Directory.CreateDirectory(dir);

            var htmlPath = Path.Combine(dir, "GTA5小助手-使用教程.html");

            var assembly = Assembly.GetExecutingAssembly();
            using (var stream = assembly.GetManifestResourceStream(FileHelper.Res_Tutorial))
            {
                if (stream == null)
                {
                    NotifierHelper.Show(NotifierType.Error, "内置教程资源缺失，请重新下载完整程序");
                    return;
                }

                using var fs = new FileStream(htmlPath, FileMode.Create, FileAccess.Write);
                stream.CopyTo(fs);
            }

            Process.Start(new ProcessStartInfo(htmlPath) { UseShellExecute = true });
        }
        catch (Exception ex)
        {
            NotifierHelper.ShowException(ex);
        }
    }
}
