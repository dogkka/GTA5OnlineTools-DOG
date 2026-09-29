namespace GTA5Core.Native;

public static class ProcessMgr
{
    /// <summary>
    /// 暂停进程
    /// </summary>
    public static void SuspendProcess()
    {
        _ = Win32.NtSuspendProcess(Memory.GTA5ProHandle);
    }

    /// <summary>
    /// 恢复进程
    /// </summary>
    public static void ResumeProcess()
    {
        _ = Win32.NtResumeProcess(Memory.GTA5ProHandle);
    }
}
