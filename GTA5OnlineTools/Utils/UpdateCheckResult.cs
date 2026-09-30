namespace GTA5OnlineTools.Utils;

/// <summary>
/// 更新检测状态
/// </summary>
public enum UpdateCheckState
{
    /// <summary>
    /// 检测失败（网络不通、接口被拦等）
    /// </summary>
    Failed,

    /// <summary>
    /// 已经是最新版本
    /// </summary>
    UpToDate,

    /// <summary>
    /// 线上有新版本
    /// </summary>
    UpdateAvailable,

    /// <summary>
    /// 本地版本比线上还新（开发版）
    /// </summary>
    LocalNewer
}

/// <summary>
/// 更新检测结果
/// </summary>
public class UpdateCheckResult
{
    /// <summary>
    /// 检测状态
    /// </summary>
    public UpdateCheckState State { get; set; } = UpdateCheckState.Failed;

    /// <summary>
    /// 本地版本号
    /// </summary>
    public Version LocalVersion { get; set; }

    /// <summary>
    /// 线上版本号
    /// </summary>
    public Version RemoteVersion { get; set; }

    /// <summary>
    /// 线上版本原始 tag，例如 v1.1.0
    /// </summary>
    public string RemoteTag { get; set; }

    /// <summary>
    /// 发布标题
    /// </summary>
    public string ReleaseName { get; set; }

    /// <summary>
    /// 发布说明（更新内容）
    /// </summary>
    public string ReleaseNotes { get; set; }

    /// <summary>
    /// 发布页面地址
    /// </summary>
    public string ReleasePage { get; set; }

    /// <summary>
    /// 安装包直链（可能为空）
    /// </summary>
    public string DownloadUrl { get; set; }

    /// <summary>
    /// 安装包文件名
    /// </summary>
    public string DownloadName { get; set; }

    /// <summary>
    /// 安装包大小（字节）
    /// </summary>
    public long DownloadSize { get; set; }

    /// <summary>
    /// 发布时间
    /// </summary>
    public DateTime? PublishedAt { get; set; }

    /// <summary>
    /// 本次检测时间
    /// </summary>
    public DateTime CheckTime { get; set; }

    /// <summary>
    /// 失败原因
    /// </summary>
    public string ErrorMessage { get; set; }

    /// <summary>
    /// 是否发现新版本
    /// </summary>
    public bool IsUpdateAvailable => State == UpdateCheckState.UpdateAvailable;

    /// <summary>
    /// 界面上显示的本地版本
    /// </summary>
    public string LocalVersionText => LocalVersion?.ToString() ?? "-";

    /// <summary>
    /// 界面上显示的线上版本
    /// </summary>
    public string RemoteVersionText => RemoteVersion?.ToString()
        ?? (string.IsNullOrWhiteSpace(RemoteTag) ? "-" : RemoteTag);

    /// <summary>
    /// 界面上显示的发布时间
    /// </summary>
    public string PublishedText => PublishedAt.HasValue
        ? PublishedAt.Value.ToLocalTime().ToString("yyyy-MM-dd HH:mm")
        : "未知";

    /// <summary>
    /// 界面上显示的安装包大小
    /// </summary>
    public string DownloadSizeText => DownloadSize > 0
        ? CoreUtil.GetFileForamtSize(DownloadSize)
        : "未知";

    /// <summary>
    /// 界面上显示的安装包文件名
    /// </summary>
    public string DownloadNameText => string.IsNullOrWhiteSpace(DownloadName) ? "未知" : DownloadName;

    /// <summary>
    /// 界面上显示的更新说明
    /// </summary>
    public string NotesText => string.IsNullOrWhiteSpace(ReleaseNotes)
        ? "（这次发布没有填写更新说明，可以打开发布页面查看）"
        : ReleaseNotes.Trim();
}
