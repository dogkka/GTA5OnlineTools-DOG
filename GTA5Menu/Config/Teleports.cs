namespace GTA5Menu.Config;

public class Teleports
{
    public List<CustomLocationsItem> CustomLocations { get; set; }
}

public class CustomLocationsItem
{
    public string Name { get; set; }
    public float X { get; set; }
    public float Y { get; set; }
    public float Z { get; set; }
    public float Pitch { get; set; }
    public float Yaw { get; set; }
    public float Roll { get; set; }
}
