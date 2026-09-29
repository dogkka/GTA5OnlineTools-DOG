using System;
using System.Runtime.InteropServices;

namespace GTA5HotKey;

public static class KeyHelper
{
    [DllImport("user32.dll")]
    public static extern int GetAsyncKeyState(Keys key);

    [DllImport("user32.dll", EntryPoint = "keybd_event")]
    public static extern void Keybd_Event(Keys bVk, uint bScan, uint dwFlags, uint dwExtraInfo);

    [DllImport("user32.dll")]
    public static extern uint MapVirtualKey(Keys uCode, uint uMapType);

    public const int KEY_PRESSED = 0x8000;

    /// <summary>
    /// 判断按键是否按下
    /// </summary>
    /// <param name="key"></param>
    /// <returns></returns>
    public static bool IsKeyPressed(Keys key)
    {
        return Convert.ToBoolean(GetAsyncKeyState(key) & KEY_PRESSED);
    }
}
