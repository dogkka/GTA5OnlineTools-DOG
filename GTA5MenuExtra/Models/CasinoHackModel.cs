using CommunityToolkit.Mvvm.ComponentModel;

namespace GTA5MenuExtra.Models;

public partial class CasinoHackModel : ObservableObject
{
    /// <summary>
    /// 黑杰克（21点） 庄家底牌
    /// </summary>
    [ObservableProperty]
    private string blackjackContent;

    /// <summary>
    /// 黑杰克（21点） 下一张牌
    /// </summary>
    [ObservableProperty]
    private string blackjackNextContent;

    /// <summary>
    /// 三张扑克 您的牌1
    /// </summary>
    [ObservableProperty]
    private string poker1Content;

    /// <summary>
    /// 三张扑克 您的牌2
    /// </summary>
    [ObservableProperty]
    private string poker2Content;

    /// <summary>
    /// 三张扑克 您的牌3
    /// </summary>
    [ObservableProperty]
    private string poker3Content;
}
