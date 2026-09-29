using CommunityToolkit.Mvvm.ComponentModel;

namespace GTA5OnlineTools.Models;

public partial class HacksModel : ObservableObject
{
    /// <summary>
    /// YimMenu是否使用繁体中文
    /// </summary>
    [ObservableProperty]
    private bool isYimMenuLangZhTw;
}
