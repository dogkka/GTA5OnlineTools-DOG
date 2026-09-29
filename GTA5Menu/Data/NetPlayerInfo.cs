namespace GTA5Menu.Data;

public class NetPlayerInfo
{
    public int Index { get; set; }
    public string Avatar { get; set; }
    public bool IsHost { get; set; }
    public bool IsScriptHost { get; set; }

    public int Rank { get; set; }
    public long RockstarId { get; set; }
    public string PlayerName { get; set; }

    public long Money { get; set; }
    public long Bank { get; set; }
    public long Cash { get; set; }

    public float Health { get; set; }
    public float HealthMax { get; set; }
    public float Armor { get; set; }
    public bool GodMode { get; set; }
    public bool NoRagdoll { get; set; }

    public byte WantedLevel { get; set; }
    public float WalkSpeed { get; set; }
    public float RunSpeed { get; set; }
    public float SwimSpeed { get; set; }

    public float Distance { get; set; }
    public Vector3 Position { get; set; }

    public string ClanTag { get; set; }
    public string ClanName { get; set; }
    public string ClanMotto { get; set; }

    public string ClanTagUpper { get; set; }
    public string GodModeFlag { get; set; }

    public string RelayIP { get; set; }
    public string ExternalIP { get; set; }
    public string InternalIP { get; set; }
}
