using GTA5HotKey;

namespace GTA5Overlay;

public static class Setting
{
    public static bool VSync = true;
    public static int FPS = 300;

    // Player
    public static bool ESP_Player = true;
    public static bool ESP_Player_2D = true;
    public static bool ESP_Player_3D = false;
    public static bool ESP_Player_Box = true;
    public static bool ESP_Player_Line = true;
    public static bool ESP_Player_Bone = false;
    public static bool ESP_Player_HealthBar = true;
    public static bool ESP_Player_HealthText = false;
    public static bool ESP_Player_NameText = false;
    public static bool AimBot_Player_Enabled = false;

    // NPC
    public static bool ESP_NPC = true;
    public static bool ESP_NPC_2D = true;
    public static bool ESP_NPC_3D = false;
    public static bool ESP_NPC_Box = true;
    public static bool ESP_NPC_Line = true;
    public static bool ESP_NPC_Bone = false;
    public static bool ESP_NPC_HealthBar = true;
    public static bool ESP_NPC_HealthText = false;
    public static bool ESP_NPC_NameText = false;
    public static bool AimBot_NPC_Enabled = false;

    // Pickup
    public static bool ESP_Pickup = true;
    public static bool ESP_Pickup_2D = true;
    public static bool ESP_Pickup_3D = false;
    public static bool ESP_Pickup_Box = true;
    public static bool ESP_Pickup_Line = true;

    public static bool ESP_Crosshair = true;
    public static bool ESP_InfoText = true;

    public static int AimBot_BoneIndex = 0;
    public static float AimBot_Fov = 250.0f;
    public static Keys AimBot_Key = Keys.ControlKey;

    public static bool NoTopMostHide = false;

    public static void Reset()
    {
        VSync = true;
        FPS = 300;

        ESP_Player = true;
        ESP_Player_2D = true;
        ESP_Player_3D = false;
        ESP_Player_Box = true;
        ESP_Player_Line = true;
        ESP_Player_Bone = false;
        ESP_Player_HealthBar = true;
        ESP_Player_HealthText = false;
        ESP_Player_NameText = false;
        AimBot_Player_Enabled = false;

        ESP_NPC = true;
        ESP_NPC_2D = true;
        ESP_NPC_3D = false;
        ESP_NPC_Box = true;
        ESP_NPC_Line = true;
        ESP_NPC_Bone = false;
        ESP_NPC_HealthBar = true;
        ESP_NPC_HealthText = false;
        ESP_NPC_NameText = false;
        AimBot_NPC_Enabled = false;

        ESP_Pickup = true;
        ESP_Pickup_2D = true;
        ESP_Pickup_3D = false;
        ESP_Pickup_Box = true;
        ESP_Pickup_Line = true;

        ESP_Crosshair = true;
        ESP_InfoText = true;

        AimBot_BoneIndex = 0;
        AimBot_Fov = 250.0f;
        AimBot_Key = Keys.ControlKey;

        NoTopMostHide = false;
    }
}
