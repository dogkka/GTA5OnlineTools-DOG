using CommunityToolkit.Mvvm.ComponentModel;

namespace GTA5Menu.Models;

public partial class TeleportInfoModel : ObservableObject
{
    [ObservableProperty]
    private string name;

    [ObservableProperty]
    private float x;

    [ObservableProperty]
    private float y;

    [ObservableProperty]
    private float z;
}
