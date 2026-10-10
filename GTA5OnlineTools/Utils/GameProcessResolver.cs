namespace GTA5OnlineTools.Utils;

/// <summary>
/// [DOG] 游戏进程解析：统一「传承版 / 增强版」的进程检测与失败指引。
/// 面向真实用户场景设计：
///  - 点错卡片（机器上只装了另一个版本）→ 直接告诉你该点哪张卡片；
///  - 开了 BattlEye（GTA5_Enhanced_BE 启动器 / BE 变体名）→ 给出关闭指引；
///  - 游戏还没进入主菜单 / 根本没启动 → 给出可操作的等待提示。
/// 覆盖的进程名（含 BE 变体）：GTA5 / GTA5_BE / GTA5_Enhanced / GTA5_Enhanced_BE。
/// </summary>
public static class GameProcessResolver
{
    private static readonly string[] LegacyNames = { "GTA5" };
    private static readonly string[] EnhancedNames = { "GTA5_Enhanced" };
    private static readonly string[] LegacyBENames = { "GTA5_BE" };
    private static readonly string[] EnhancedBENames = { "GTA5_Enhanced_BE" };

    /// <summary>
    /// 解析目标游戏进程。
    /// </summary>
    /// <param name="wantEnhanced">true = 增强版（YimMenu V2 卡片）；false = 传承版（YimMenu 卡片）</param>
    /// <returns>(是否找到, 进程对象, 失败时的人类可读指引)</returns>
    public static (bool ok, System.Diagnostics.Process proc, string message) Resolve(bool wantEnhanced)
    {
        var proc = FirstRunning(wantEnhanced ? EnhancedNames : LegacyNames);
        if (proc != null)
            return (true, proc, "");

        var self = wantEnhanced ? "增强版" : "传承版";
        var other = wantEnhanced ? "传承版" : "增强版";
        var otherCard = wantEnhanced ? "「YimMenu」" : "「YimMenu V2」";
        var selfExe = wantEnhanced ? "GTA5_Enhanced.exe" : "GTA5.exe";

        // 仅检测到 BE 变体名 → 不注入，给关 BE 指引
        var be = FirstRunning(wantEnhanced ? EnhancedBENames : LegacyBENames);
        if (be != null)
        {
            return (false, null,
                $"检测到《{self}》相关进程以 BattlEye 形式运行（{be.ProcessName}.exe）：\n" +
                "本工具需要关闭 BattlEye 才能注入。请到 R星启动器 → 设置 → 我的已安装游戏 → 关闭 BattlEye（Steam 可加 -nobattleye），\n" +
                "完全退出游戏后重新进入，再点「内嵌版」。");
        }

        // 另一版本的进程在跑 → 说明点错卡片了
        if (FirstRunning(wantEnhanced ? LegacyNames : EnhancedNames) != null
            || FirstRunning(wantEnhanced ? LegacyBENames : EnhancedBENames) != null)
        {
            return (false, null,
                $"检测到你正在运行的是《{other}》。\n" +
                $"{self}菜单请点击{otherCard}卡片的「内嵌版」；\n" +
                $"如果你要使用{self}菜单，请先启动《{self}》游戏（{selfExe}）。");
        }

        // 谁都没跑 → 没启动 / 还在启动器或加载阶段
        return (false, null,
            "未检测到游戏进程（传承版 GTA5.exe / 增强版 GTA5_Enhanced.exe）。\n" +
            "请先启动游戏，并等进入主菜单后再点「内嵌版」；\n" +
            "刚点完启动的话，启动器与加载阶段还没有游戏进程，等一会儿再试。");
    }

    /// <summary>是否有任意 GTA5 进程在运行（含 BE 变体名）</summary>
    public static bool AnyGameRunning()
    {
        return FirstRunning(LegacyNames) != null
            || FirstRunning(EnhancedNames) != null
            || FirstRunning(LegacyBENames) != null
            || FirstRunning(EnhancedBENames) != null;
    }

    private static System.Diagnostics.Process FirstRunning(string[] names)
    {
        foreach (var name in names)
        {
            var arr = System.Diagnostics.Process.GetProcessesByName(name);
            if (arr.Length > 0)
                return arr[0];
        }
        return null;
    }
}
