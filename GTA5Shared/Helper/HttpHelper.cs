namespace GTA5Shared.Helper;

/// <summary>
/// [DOG 离线版] 本文件已彻底禁用联网功能：
/// 所有 HTTP 请求直接返回空字符串，不再访问任何第三方服务器。
/// </summary>
public static class HttpHelper
{
    [DllImport("dnsapi.dll")]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool DnsFlushResolverCache();

    public static Task<string> DownloadString(string url)
    {
        return Task.FromResult(string.Empty);
    }

    public static Task<string> PostAsync(string url, string golt_commit_hash, string yimv2_file_hash)
    {
        return Task.FromResult(string.Empty);
    }

    /// <summary>
    /// 刷新DNS缓存
    /// </summary>
    public static void FlushDNSCache()
    {
        DnsFlushResolverCache();
    }
}
