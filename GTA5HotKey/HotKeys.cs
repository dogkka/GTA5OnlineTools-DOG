using System;
using System.Collections.Generic;
using System.Threading;

namespace GTA5HotKey;

public static class HotKeys
{
    /// <summary>
    /// 热键字典集合
    /// </summary>
    private static readonly Dictionary<Keys, KeyInfo> HotKeyDirts = new();

    /// <summary>
    /// 按键弹起事件
    /// </summary>
    public static event Action<Keys> KeyUpEvent;
    /// <summary>
    /// 按键按下事件
    /// </summary>
    public static event Action<Keys> KeyDownEvent;

    /// <summary>
    /// 线程锁，防止溢出异常（Destination array is not long enough to copy all the items in the collection. Check array index and length.）
    /// </summary>
    private static object Lock = new object();

    /// <summary>
    /// 初始化
    /// </summary>
    static HotKeys()
    {
        new Thread(UpdateHotKeyThread)
        {
            Name = "UpdateHotKeyThread",
            IsBackground = true
        }.Start();
    }

    /// <summary>
    /// 增加快捷按键（自动过滤重复按键）
    /// </summary>
    /// <param name="key"></param>
    public static void AddKey(Keys key)
    {
        lock (Lock)
        {
            if (!HotKeyDirts.ContainsKey(key))
                HotKeyDirts.Add(key, new KeyInfo() { Key = key });
        }
    }

    /// <summary>
    /// 移除快捷键
    /// </summary>
    /// <param name="key"></param>
    public static void RemoveKey(Keys key)
    {
        lock (Lock)
        {
            if (HotKeyDirts.ContainsKey(key))
                HotKeyDirts.Remove(key);
        }
    }

    /// <summary>
    /// 清空快捷键
    /// </summary>
    public static void ClearKeys()
    {
        lock (Lock)
        {
            HotKeyDirts.Clear();
        }
    }

    /// <summary>
    /// 按键按下
    /// </summary>
    /// <param name="key"></param>
    private static void OnKeyDown(Keys key)
    {
        KeyDownEvent?.Invoke(key);
    }

    /// <summary>
    /// 按键弹起
    /// </summary>
    /// <param name="key"></param>
    private static void OnKeyUp(Keys key)
    {
        KeyUpEvent?.Invoke(key);
    }

    /// <summary>
    /// 按键监听线程
    /// </summary>
    /// <param name="sender"></param>
    private static void UpdateHotKeyThread()
    {
        while (true)
        {
            if (HotKeyDirts.Count == 0)
                goto SLEEP;

            // 防止枚举异常
            var keyInfos = new List<KeyInfo>(HotKeyDirts.Values);
            if (keyInfos.Count == 0)
                goto SLEEP;

            foreach (var key in keyInfos)
            {
                if (KeyHelper.IsKeyPressed(key.Key))
                {
                    if (!key.IsKeyDown)
                    {
                        key.IsKeyDown = true;
                        OnKeyDown(key.Key);
                    }
                }
                else
                {
                    if (key.IsKeyDown)
                    {
                        key.IsKeyDown = false;
                        OnKeyUp(key.Key);
                    }
                }
            }

        SLEEP:
            Thread.Sleep(20);
        }
    }
}
