using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace GTA5OnlineTools.Utils;

/// <summary>
/// [DOG] 剪贴板安全写入（纯 Win32 路径，带短重试）。
/// 背景：
///  - 其他程序占用剪贴板时，WPF 的 Clipboard API 会抛「OpenClipboard 失败 0x800401D0」，
///    且个别系统上每次失败前会内部阻塞数百毫秒到 1 秒，叠加多轮重试可卡 5~10 秒（用户实测）；
///  - 经实测：纯 Win32（OpenClipboard + SetClipboardData）微秒级完成、极其可靠，
///    在别人"打开"着剪贴板时也能正常写入。
/// 因此本实现直接走 Win32 路径 + 短重试；最终失败返回 false，由调用方给出友好提示（不再弹异常框）。
/// </summary>
public static class ClipboardHelper
{
    /// <summary>尝试写入剪贴板文本；成功 true（通常 &lt; 5ms）/ 重试后仍失败 false。</summary>
    public static async Task<bool> TrySetTextAsync(string text, int retries = 12, int delayMs = 50)
    {
        for (var i = 0; i < retries; i++)
        {
            if (TrySetTextOnce(text))
                return true;
            await Task.Delay(delayMs);
        }
        return false;
    }

    //////////////////////////////////////////////////////////////////

    private const uint CF_TEXT = 1;
    private const uint CF_UNICODETEXT = 13;
    private const uint GMEM_MOVEABLE = 0x0002;

    [DllImport("user32.dll", SetLastError = true)]
    private static extern bool OpenClipboard(IntPtr hWndNewOwner);

    [DllImport("user32.dll", SetLastError = true)]
    private static extern bool EmptyClipboard();

    [DllImport("user32.dll", SetLastError = true)]
    private static extern IntPtr SetClipboardData(uint uFormat, IntPtr hMem);

    [DllImport("user32.dll", SetLastError = true)]
    private static extern bool CloseClipboard();

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern IntPtr GlobalAlloc(uint uFlags, UIntPtr dwBytes);

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern IntPtr GlobalLock(IntPtr hMem);

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern bool GlobalUnlock(IntPtr hMem);

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern IntPtr GlobalFree(IntPtr hMem);

    /// <summary>单次尝试：写入 CF_UNICODETEXT（并附带 CF_TEXT 兼容旧程序）。微秒级。</summary>
    private static bool TrySetTextOnce(string text)
    {
        IntPtr hAnsi = IntPtr.Zero;
        IntPtr hUnicode = IntPtr.Zero;
        try
        {
            if (!OpenClipboard(IntPtr.Zero))
                return false;
            try
            {
                EmptyClipboard();

                // CF_TEXT（ANSI，兼容老旧程序；best-effort，失败不致命）
                hAnsi = AllocAndCopy(Encoding.Default.GetBytes(text + "\0"));
                if (hAnsi != IntPtr.Zero)
                {
                    if (SetClipboardData(CF_TEXT, hAnsi) == IntPtr.Zero)
                    {
                        GlobalFree(hAnsi);
                    }
                    hAnsi = IntPtr.Zero;
                }

                // CF_UNICODETEXT（主格式）
                hUnicode = AllocAndCopy(Encoding.Unicode.GetBytes(text + "\0"));
                if (hUnicode == IntPtr.Zero)
                    return false;
                if (SetClipboardData(CF_UNICODETEXT, hUnicode) == IntPtr.Zero)
                    return false;

                hUnicode = IntPtr.Zero; // 所有权已移交系统，不能再释放
                return true;
            }
            finally
            {
                CloseClipboard();
            }
        }
        catch
        {
            return false;
        }
        finally
        {
            if (hAnsi != IntPtr.Zero)
                GlobalFree(hAnsi);
            if (hUnicode != IntPtr.Zero)
                GlobalFree(hUnicode);
        }
    }

    /// <summary>分配可移动内存块并拷入数据，返回句柄（失败返回 0）</summary>
    private static IntPtr AllocAndCopy(byte[] data)
    {
        var handle = GlobalAlloc(GMEM_MOVEABLE, (UIntPtr)(uint)data.Length);
        if (handle == IntPtr.Zero)
            return IntPtr.Zero;

        var ptr = GlobalLock(handle);
        if (ptr == IntPtr.Zero)
        {
            GlobalFree(handle);
            return IntPtr.Zero;
        }

        Marshal.Copy(data, 0, ptr, data.Length);
        GlobalUnlock(handle);
        return handle;
    }
}
