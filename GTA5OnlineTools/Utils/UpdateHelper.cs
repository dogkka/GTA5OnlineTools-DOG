using GTA5Shared.Helper;

using System.Globalization;
using System.Net;
using System.Net.Http;
using System.Text.Json;
using System.Text.RegularExpressions;

namespace GTA5OnlineTools.Utils;

/// <summary>
/// [DOG] 更新检测
/// 只访问本二改仓库的 GitHub 官方地址（api.github.com / github.com），
/// 原作者的公告服务器、哈希服务器、网络版下载保持禁用状态，不会被恢复。
/// </summary>
public static class UpdateHelper
{
    /// <summary>
    /// 本二改仓库作者
    /// </summary>
    public const string RepoOwner = "dogkka";

    /// <summary>
    /// 本二改仓库名称
    /// </summary>
    public const string RepoName = "GTA5OnlineTools-DOG";

    /// <summary>
    /// 最新 Release 接口
    /// </summary>
    private const string ApiLatestUrl = "https://api.github.com/repos/" + RepoOwner + "/" + RepoName + "/releases/latest";

    /// <summary>
    /// 备用通道：Release 订阅源（接口被限流或者 api 被墙时使用）
    /// </summary>
    private const string AtomFeedUrl = "https://github.com/" + RepoOwner + "/" + RepoName + "/releases.atom";

    /// <summary>
    /// 仓库主页
    /// </summary>
    public static readonly string RepoUrl = "https://github.com/" + RepoOwner + "/" + RepoName;

    /// <summary>
    /// 最新发布页面（用浏览器打开）
    /// </summary>
    public static readonly string LatestPageUrl = RepoUrl + "/releases/latest";

    /// <summary>
    /// 最后一次的检测结果
    /// </summary>
    public static UpdateCheckResult LastResult { get; private set; }

    private static readonly HttpClient _httpClient = CreateHttpClient();

    //////////////////////////////////////////////////////////////////

    /// <summary>
    /// 执行一次更新检测（内部吞掉全部异常，绝不因为检测失败让程序崩溃）
    /// </summary>
    public static async Task<UpdateCheckResult> CheckAsync()
    {
        var result = new UpdateCheckResult
        {
            CheckTime = DateTime.Now,
            LocalVersion = NormalizeVersion(CoreUtil.ClientVersion) ?? new Version(1, 0, 0, 0),
            ReleasePage = LatestPageUrl
        };

        try
        {
            // 优先走官方 API，失败再走 Release 订阅源
            var remote = await TryGetFromApiAsync() ?? await TryGetFromFeedAsync();

            if (remote == null)
            {
                result.State = UpdateCheckState.Failed;
                result.ErrorMessage = "无法连接 GitHub 更新接口，请检查网络或代理后重试";
                LoggerHelper.Warn("更新检测失败：GitHub 接口无响应");
                LastResult = result;
                return result;
            }

            result.RemoteTag = remote.Tag;
            result.ReleaseName = remote.Name;
            result.ReleaseNotes = remote.Notes;
            result.PublishedAt = remote.PublishedAt;
            result.ReleasePage = string.IsNullOrWhiteSpace(remote.PageUrl) ? LatestPageUrl : remote.PageUrl;
            result.DownloadUrl = remote.AssetUrl;
            result.DownloadName = remote.AssetName;
            result.DownloadSize = remote.AssetSize;
            result.RemoteVersion = ParseVersion(remote.Tag);

            if (result.RemoteVersion == null)
            {
                // 版本号写得不规范（比如 tag 是别的写法），只要有发布就提示用户去页面看
                result.State = UpdateCheckState.UpdateAvailable;
                LoggerHelper.Warn($"更新检测：无法解析版本号 [{remote.Tag}]，直接提示有更新");
                LastResult = result;
                return result;
            }

            var compare = result.RemoteVersion.CompareTo(result.LocalVersion);

            if (compare > 0)
                result.State = UpdateCheckState.UpdateAvailable;
            else if (compare == 0)
                result.State = UpdateCheckState.UpToDate;
            else
                result.State = UpdateCheckState.LocalNewer;

            LoggerHelper.Info($"更新检测完成：本地 {result.LocalVersion}，线上 {result.RemoteVersion}（{result.State}）");
        }
        catch (Exception ex)
        {
            result.State = UpdateCheckState.Failed;
            result.ErrorMessage = "更新检测出错：" + ex.Message;
            LoggerHelper.Error("更新检测出错", ex);
        }

        LastResult = result;
        return result;
    }

    /// <summary>
    /// 检查更新并按结果提示用户（手动检测 / 启动时自动检测都走这里）
    /// </summary>
    /// <param name="report">把状态文字显示到界面上的回调，可为 null</param>
    /// <param name="fromStartup">是否启动时的自动检测（会跳过用户点过“跳过此版本”的版本）</param>
    public static async Task<UpdateCheckResult> CheckAndPromptAsync(Action<string> report = null, bool fromStartup = false)
    {
        Report(report, "正在检查更新…");

        var result = await CheckAsync();

        switch (result.State)
        {
            case UpdateCheckState.UpdateAvailable:
                Report(report, $"发现新版本 {result.RemoteVersionText}（当前 {result.LocalVersionText}）");

                if (fromStartup && IsSkipped(result))
                {
                    // 用户点过“跳过此版本”，启动时就不再打扰
                    LoggerHelper.Info($"更新检测：{result.RemoteVersionText} 已被用户跳过");
                    break;
                }

                ShowUpdateWindow(result);
                break;

            case UpdateCheckState.UpToDate:
                Report(report, $"已是最新版本（{result.LocalVersionText}）");

                if (!fromStartup)
                    NotifierHelper.Show(NotifierType.Success, $"已经是最新版本 {result.LocalVersionText}");
                break;

            case UpdateCheckState.LocalNewer:
                Report(report, $"本地版本高于线上（本地 {result.LocalVersionText} / 线上 {result.RemoteVersionText}）");

                if (!fromStartup)
                    NotifierHelper.Show(NotifierType.Information, "本地版本比线上更新，可能是还没发布的开发版");
                break;

            default:
                Report(report, "检查更新失败，请稍后重试");

                if (!fromStartup)
                    NotifierHelper.Show(NotifierType.Error, result.ErrorMessage ?? "更新检测失败");
                break;
        }

        return result;
    }

    /// <summary>
    /// 弹出“发现新版本”窗口
    /// </summary>
    public static void ShowUpdateWindow(UpdateCheckResult result)
    {
        if (result == null)
            return;

        try
        {
            var window = new GTA5OnlineTools.Windows.UpdateWindow(result);

            var owner = MainWindow.MainWindowInstance;
            if (owner != null && owner.IsLoaded)
                window.Owner = owner;

            window.ShowDialog();
        }
        catch (Exception ex)
        {
            LoggerHelper.Error("显示更新窗口失败", ex);
        }
    }

    //////////////////////////////////////////////////////////////////
    // 跳过版本相关（记录在 %ProgramData%\GTA5OnlineTools\Config\Config.ini）
    //////////////////////////////////////////////////////////////////

    /// <summary>
    /// 用户点过“跳过此版本”之后记录的版本号
    /// </summary>
    public static string SkippedVersion => IniHelper.ReadValue("Dog", "SkipVersion");

    /// <summary>
    /// 判断这个版本是不是被用户跳过过
    /// </summary>
    public static bool IsSkipped(UpdateCheckResult result)
    {
        if (result == null)
            return false;

        var skipped = SkippedVersion;
        if (string.IsNullOrWhiteSpace(skipped))
            return false;

        return skipped == result.RemoteVersionText || skipped == result.RemoteTag;
    }

    /// <summary>
    /// 记录用户跳过的版本
    /// </summary>
    public static void SkipVersion(UpdateCheckResult result)
    {
        if (result == null)
            return;

        var version = string.IsNullOrWhiteSpace(result.RemoteVersionText) ? result.RemoteTag : result.RemoteVersionText;
        if (string.IsNullOrWhiteSpace(version))
            return;

        IniHelper.WriteValue("Dog", "SkipVersion", version);
        LoggerHelper.Info($"更新检测：用户跳过版本 {version}");
    }

    /// <summary>
    /// 清除跳过记录
    /// </summary>
    public static void ClearSkippedVersion()
    {
        IniHelper.WriteValue("Dog", "SkipVersion", string.Empty);
    }

    //////////////////////////////////////////////////////////////////
    // 网络请求（只允许本仓库的 GitHub 地址）
    //////////////////////////////////////////////////////////////////

    private static HttpClient CreateHttpClient()
    {
        var handler = new HttpClientHandler
        {
            // 跟随系统代理，挂了加速器 / 代理也能正常检测
            AutomaticDecompression = DecompressionMethods.GZip | DecompressionMethods.Deflate,
            UseProxy = true,
            Proxy = WebRequest.DefaultWebProxy
        };

        if (handler.Proxy != null)
            handler.Proxy.Credentials = CredentialCache.DefaultCredentials;

        var client = new HttpClient(handler)
        {
            Timeout = TimeSpan.FromSeconds(20)
        };

        client.DefaultRequestHeaders.UserAgent.ParseAdd("GTA5OnlineTools-DOG");
        client.DefaultRequestHeaders.Accept.ParseAdd("application/vnd.github+json");

        return client;
    }

    private static async Task<string> GetStringAsync(string url)
    {
        try
        {
            using var response = await _httpClient.GetAsync(url);

            if (!response.IsSuccessStatusCode)
            {
                LoggerHelper.Warn($"更新检测：{url} 返回 {(int)response.StatusCode}");
                return string.Empty;
            }

            return await response.Content.ReadAsStringAsync();
        }
        catch (Exception ex)
        {
            LoggerHelper.Warn($"更新检测：请求 {url} 失败 - {ex.Message}");
            return string.Empty;
        }
    }

    /// <summary>
    /// 解析 GitHub 官方 API 的返回内容
    /// </summary>
    private static async Task<RemoteRelease> TryGetFromApiAsync()
    {
        var json = await GetStringAsync(ApiLatestUrl);
        if (string.IsNullOrWhiteSpace(json))
            return null;

        using var document = JsonDocument.Parse(json);

        var root = document.RootElement;
        if (root.ValueKind != JsonValueKind.Object)
            return null;

        var release = new RemoteRelease
        {
            Tag = ReadString(root, "tag_name"),
            Name = ReadString(root, "name"),
            Notes = ReadString(root, "body"),
            PageUrl = ReadString(root, "html_url"),
            PublishedAt = ReadTime(root, "published_at")
        };

        if (string.IsNullOrWhiteSpace(release.Tag))
            return null;

        // 附件里优先挑 DOG 版的 exe，方便“复制下载链接”
        if (root.TryGetProperty("assets", out var assets) && assets.ValueKind == JsonValueKind.Array)
        {
            string backupName = null, backupUrl = null;
            long backupSize = 0;

            foreach (var asset in assets.EnumerateArray())
            {
                var name = ReadString(asset, "name");
                var url = ReadString(asset, "browser_download_url");

                if (string.IsNullOrWhiteSpace(name) || string.IsNullOrWhiteSpace(url))
                    continue;

                if (!name.EndsWith(".exe", StringComparison.OrdinalIgnoreCase))
                    continue;

                var size = asset.TryGetProperty("size", out var sizeElement) && sizeElement.TryGetInt64(out var value) ? value : 0;

                if (name.Contains("DOG", StringComparison.OrdinalIgnoreCase))
                {
                    release.AssetName = name;
                    release.AssetUrl = url;
                    release.AssetSize = size;
                    break;
                }

                if (backupUrl == null)
                {
                    backupName = name;
                    backupUrl = url;
                    backupSize = size;
                }
            }

            if (string.IsNullOrEmpty(release.AssetUrl))
            {
                release.AssetName = backupName;
                release.AssetUrl = backupUrl;
                release.AssetSize = backupSize;
            }
        }

        return release;
    }

    /// <summary>
    /// 解析 Release 订阅源（备用通道，只能拿到版本号和页面地址）
    /// </summary>
    private static async Task<RemoteRelease> TryGetFromFeedAsync()
    {
        var xml = await GetStringAsync(AtomFeedUrl);
        if (string.IsNullOrWhiteSpace(xml))
            return null;

        var entry = Regex.Match(xml, "<entry>(.*?)</entry>", RegexOptions.Singleline);
        if (!entry.Success)
            return null;

        var content = entry.Groups[1].Value;

        var title = Unescape(Regex.Match(content, "<title>(.*?)</title>", RegexOptions.Singleline).Groups[1].Value)?.Trim();
        var link = Regex.Match(content, "href=\"([^\"]+)\"", RegexOptions.Singleline).Groups[1].Value;
        var id = Regex.Match(content, "<id>(.*?)</id>", RegexOptions.Singleline).Groups[1].Value;
        var updated = Regex.Match(content, "<updated>(.*?)</updated>", RegexOptions.Singleline).Groups[1].Value;

        // 订阅源里的 title 是“发布标题”，真正的 tag 在链接或者 id 的最后一段
        var tag = ExtractTagFromUrl(link) ?? ExtractTagFromUrl(id) ?? title;

        if (string.IsNullOrWhiteSpace(tag))
            return null;

        return new RemoteRelease
        {
            Tag = tag,
            Name = string.IsNullOrWhiteSpace(title) ? tag : title,
            PageUrl = string.IsNullOrWhiteSpace(link) ? LatestPageUrl : link,
            PublishedAt = ParseTime(updated),
            Notes = "（备用通道只拿到了版本号，详细更新内容请打开发布页面查看）"
        };
    }

    /// <summary>
    /// 从形如 .../releases/tag/v1.0.0 的地址里取出 v1.0.0
    /// </summary>
    private static string ExtractTagFromUrl(string url)
    {
        if (string.IsNullOrWhiteSpace(url))
            return null;

        var index = url.LastIndexOf('/');
        if (index < 0 || index >= url.Length - 1)
            return null;

        var tag = url[(index + 1)..].Trim();

        return string.IsNullOrWhiteSpace(tag) || ParseVersion(tag) == null ? null : tag;
    }

    //////////////////////////////////////////////////////////////////
    // 小工具
    //////////////////////////////////////////////////////////////////

    private static string ReadString(JsonElement element, string propertyName)
    {
        return element.TryGetProperty(propertyName, out var value) && value.ValueKind == JsonValueKind.String
            ? value.GetString()
            : null;
    }

    private static DateTime? ReadTime(JsonElement element, string propertyName)
    {
        return ParseTime(ReadString(element, propertyName));
    }

    private static DateTime? ParseTime(string text)
    {
        if (string.IsNullOrWhiteSpace(text))
            return null;

        return DateTime.TryParse(text, CultureInfo.InvariantCulture,
            DateTimeStyles.AdjustToUniversal | DateTimeStyles.AssumeUniversal, out var time)
            ? time
            : null;
    }

    private static string Unescape(string text)
    {
        if (string.IsNullOrEmpty(text))
            return text;

        return text
            .Replace("&lt;", "<")
            .Replace("&gt;", ">")
            .Replace("&quot;", "\"")
            .Replace("&#39;", "'")
            .Replace("&amp;", "&");
    }

    /// <summary>
    /// 把 tag 解析成 4 段版本号，例如 v1.2.3 → 1.2.3.0
    /// </summary>
    public static Version ParseVersion(string tag)
    {
        if (string.IsNullOrWhiteSpace(tag))
            return null;

        // 必须至少是“主版本.次版本”，免得把标题里的单个数字（比如 GTA5）当成版本号
        var match = Regex.Match(tag, @"\d+\.\d+(\.\d+){0,2}");
        if (!match.Success)
            return null;

        return Version.TryParse(match.Value, out var version) ? NormalizeVersion(version) : null;
    }

    /// <summary>
    /// 版本号统一补齐成 4 段，避免 1.0.0 和 1.0.0.0 被当成不同版本
    /// </summary>
    private static Version NormalizeVersion(Version version)
    {
        if (version == null)
            return null;

        return new Version(
            Math.Max(version.Major, 0),
            Math.Max(version.Minor, 0),
            Math.Max(version.Build, 0),
            Math.Max(version.Revision, 0));
    }

    /// <summary>
    /// 把状态文字安全地显示到界面上（回调可能来自后台线程）
    /// </summary>
    private static void Report(Action<string> report, string text)
    {
        if (report == null)
            return;

        try
        {
            var dispatcher = Application.Current?.Dispatcher;

            if (dispatcher != null && !dispatcher.CheckAccess())
                dispatcher.Invoke(() => report(text));
            else
                report(text);
        }
        catch { }
    }

    /// <summary>
    /// 线上发布信息
    /// </summary>
    private sealed class RemoteRelease
    {
        public string Tag { get; set; }
        public string Name { get; set; }
        public string Notes { get; set; }
        public string PageUrl { get; set; }
        public DateTime? PublishedAt { get; set; }
        public string AssetName { get; set; }
        public string AssetUrl { get; set; }
        public long AssetSize { get; set; }
    }
}
