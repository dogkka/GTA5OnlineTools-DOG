using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace GTA5OnlineTools.Utils;

/// <summary>
/// [DOG] 隐藏游戏进程附带的控制台窗口。
/// 背景：菜单注入成功后，菜单会创建控制台窗口（日志窗口）；新手用户误关该窗口会导致
/// 游戏进程一并退出。工具在注入成功后自动将该窗口隐藏——仅隐藏、不关闭，日志照常写入。
/// </summary>
public static class ConsoleWindowHelper
{
    private delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);

    [DllImport("user32.dll")]
    private static extern bool EnumWindows(EnumWindowsProc lpEnumFunc, IntPtr lParam);

    [DllImport("user32.dll")]
    private static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);

    [DllImport("user32.dll", CharSet = CharSet.Unicode)]
    private static extern int GetClassName(IntPtr hWnd, StringBuilder lpClassName, int nMaxCount);

    [DllImport("user32.dll", CharSet = CharSet.Unicode)]
    private static extern int GetWindowText(IntPtr hWnd, StringBuilder lpString, int nMaxCount);

    [DllImport("user32.dll")]
    private static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

    [DllImport("user32.dll")]
    private static extern bool IsWindowVisible(IntPtr hWnd);

    private const int SW_HIDE = 0;

    /// <summary>菜单控制台可能出现的标题（用于 Windows Terminal 托管场景的兜底匹配）</summary>
    private static readonly string[] ConsoleTitles = { "YimMenuV2", "YimMenu" };

    /// <summary>隐藏指定进程的控制台窗口（含 Windows Terminal 托管兜底），返回隐藏数量</summary>
    private static int HideForProcess(int pid)
    {
        var hidden = 0;
        try
        {
            EnumWindows((hWnd, _) =>
            {
                var sb = new StringBuilder(128);
                GetClassName(hWnd, sb, sb.Capacity);
                var className = sb.ToString();

                GetWindowThreadProcessId(hWnd, out var windowPid);
                if (windowPid == (uint)pid && className == "ConsoleWindowClass")
                {
                    if (IsWindowVisible(hWnd))
                    {
                        ShowWindow(hWnd, SW_HIDE);
                        hidden++;
                    }
                    return true;
                }

                // 兜底：个别系统把控制台交给 Windows Terminal 托管（窗口归属 WindowsTerminal.exe），
                // 通过「类名 + 标题」定位（标题由菜单 SetConsoleTitle 设置）
                if (className == "CASCADIA_HOSTING_WINDOW_CLASS")
                {
                    var tb = new StringBuilder(256);
                    GetWindowText(hWnd, tb, tb.Capacity);
                    var title = tb.ToString();
                    foreach (var t in ConsoleTitles)
                    {
                        if (title.Contains(t))
                        {
                            if (IsWindowVisible(hWnd))
                            {
                                ShowWindow(hWnd, SW_HIDE);
                                hidden++;
                            }
                            break;
                        }
                    }
                }
                return true;
            }, IntPtr.Zero);
        }
        catch
        {
            // 静默：隐藏失败不影响已完成的注入流程
        }
        return hidden;
    }

    /// <summary>
    /// 注入成功后调用：短轮询数秒，发现游戏进程的控制台窗口即隐藏。
    /// （菜单创建窗口可能略滞后于注入返回，故做轮询兜底）
    /// </summary>
    public static async Task HideGameConsoleAfterInjectAsync(int gamePid, int totalMs = 6000, int intervalMs = 400)
    {
        try
        {
            for (var waited = 0; waited < totalMs; waited += intervalMs)
            {
                if (HideForProcess(gamePid) > 0)
                {
                    // 窗口可能在创建瞬间被替换，稍候再补一轮
                    await Task.Delay(intervalMs);
                    HideForProcess(gamePid);
                    return;
                }
                await Task.Delay(intervalMs);
            }
        }
        catch
        {
            // 静默失败
        }
    }
}
