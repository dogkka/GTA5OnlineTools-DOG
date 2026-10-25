using CommunityToolkit.Mvvm.ComponentModel;

namespace GTA5Menu.Models;

public partial class SelfStateModel : ObservableObject
{
    /// <summary>
    /// 热键状态 补满全部武器弹药
    /// </summary>
    [ObservableProperty]
    private bool isHotKeyFillAllAmmo = false;

    /// <summary>
    /// 热键状态 坐标向前微调
    /// </summary>
    [ObservableProperty]
    private bool isHotKeyMovingFoward = false;

    /// <summary>
    /// 热键状态 传送到导航点
    /// </summary>
    [ObservableProperty]
    private bool isHotKeyToWaypoint = true;

    /// <summary>
    /// 热键状态 传送到目标点
    /// </summary>
    [ObservableProperty]
    private bool isHotKeyToObjective = true;

    /// <summary>
    /// 热键状态 补满血量和护甲
    /// </summary>
    [ObservableProperty]
    private bool isHotKeyFillHealthArmor = true;

    /// <summary>
    /// 热键状态 消除警星
    /// </summary>
    [ObservableProperty]
    private bool isHotKeyClearWanted = true;

    /// <summary>
    /// 热键状态 玩家无碰撞体积（穿墙）
    /// </summary>
    [ObservableProperty]
    private bool isHotKeyNoCollision = false;

    /// <summary>
    /// 热键状态 传送到准星位置
    /// </summary>
    [ObservableProperty]
    private bool isHotKeyToCrossHair = false;
}
