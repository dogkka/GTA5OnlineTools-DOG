using GTA5Core.Native;
using GTA5Core.Offsets;

namespace GTA5Overlay;

public static class Core
{
    private static int _windowWidth = 100;
    private static int _windowHeight = 100;

    /// <summary>
    /// 设置窗口数据
    /// </summary>
    /// <param name="width"></param>
    /// <param name="height"></param>
    public static void SetWindowData(int width, int height)
    {
        _windowWidth = width;
        _windowHeight = height;
    }

    /// <summary>
    /// 判断屏幕坐标是否有效
    /// </summary>
    /// <param name="vector"></param>
    /// <returns></returns>
    public static bool IsNullVector2(Vector2 vector)
    {
        return vector == Vector2.Zero;
    }

    /// <summary>
    /// 获取相机矩阵数据
    /// </summary>
    /// <returns></returns>
    public static float[] GetCameraMatrix()
    {
        return Memory.ReadMatrix<float>(Pointers.ViewPortPTR + 0xC0, 16);
    }

    /// <summary>
    /// 获取骨骼矩阵数据 + 0x60
    /// </summary>
    /// <param name="offset"></param>
    /// <returns></returns>
    public static float[] GetBoneMatrix(long offset)
    {
        return Memory.ReadMatrix<float>(offset + 0x60, 16);
    }

    /// <summary>
    /// 世界坐标转屏幕坐标
    /// </summary>
    /// <param name="posV3"></param>
    /// <returns></returns>
    public static Vector2 WorldToScreen(Vector3 posV3)
    {
        Vector2 screenV2;
        Vector3 cameraV3;

        var viewMatrix = GetCameraMatrix();

        cameraV3.Z = viewMatrix[2] * posV3.X + viewMatrix[6] * posV3.Y + viewMatrix[10] * posV3.Z + viewMatrix[14];
        if (cameraV3.Z < 0.001f)
            return Vector2.Zero;

        cameraV3.X = _windowWidth / 2;
        cameraV3.Y = _windowHeight / 2;
        cameraV3.Z = 1 / cameraV3.Z;

        screenV2.X = viewMatrix[0] * posV3.X + viewMatrix[4] * posV3.Y + viewMatrix[8] * posV3.Z + viewMatrix[12];
        screenV2.Y = viewMatrix[1] * posV3.X + viewMatrix[5] * posV3.Y + viewMatrix[9] * posV3.Z + viewMatrix[13];

        screenV2.X = cameraV3.X + cameraV3.X * screenV2.X * cameraV3.Z;
        screenV2.Y = cameraV3.Y - cameraV3.Y * screenV2.Y * cameraV3.Z;

        return screenV2;
    }

    /// <summary>
    /// 获取方框高度
    /// </summary>
    /// <param name="posV3"></param>
    /// <param name="dist">高度扩展 1.0f</param>
    /// <param name="ratio">长宽比 2.0f</param>
    /// <returns></returns>
    public static Vector2 GetBoxSize(Vector3 posV3, float dist = 1.0f, float ratio = 2.0f)
    {
        Vector2 boxV2;
        Vector3 cameraV3;

        var viewMatrix = GetCameraMatrix();

        cameraV3.Z = viewMatrix[2] * posV3.X + viewMatrix[6] * posV3.Y + viewMatrix[10] * posV3.Z + viewMatrix[14];
        if (cameraV3.Z < 0.001f)
            return Vector2.Zero;

        cameraV3.Y = _windowHeight / 2;
        cameraV3.Z = 1 / cameraV3.Z;

        boxV2.X = viewMatrix[1] * posV3.X + viewMatrix[5] * posV3.Y + viewMatrix[9] * (posV3.Z + dist) + viewMatrix[13];
        boxV2.Y = viewMatrix[1] * posV3.X + viewMatrix[5] * posV3.Y + viewMatrix[9] * (posV3.Z - dist) + viewMatrix[13];

        boxV2.X = cameraV3.Y - cameraV3.Y * boxV2.X * cameraV3.Z;
        boxV2.Y = cameraV3.Y - cameraV3.Y * boxV2.Y * cameraV3.Z;

        boxV2.Y = Math.Abs(boxV2.X - boxV2.Y);
        boxV2.X = boxV2.Y / ratio;

        return boxV2;
    }

    /// <summary>
    /// 获取骨骼世界坐标
    /// </summary>
    /// <param name="pCPed"></param>
    /// <param name="boneMatrix"></param>
    /// <param name="boneId"></param>
    /// <returns></returns>
    public static Vector3 GetBonePosition(long pCPed, float[] boneMatrix, int boneId)
    {
        var bone_offset_pos = Memory.Read<Vector3>(pCPed + 0x410 + boneId * 0x10);

        Vector3 bone_pos;
        bone_pos.X = boneMatrix[0] * bone_offset_pos.X + boneMatrix[4] * bone_offset_pos.Y + boneMatrix[8] * bone_offset_pos.Z + boneMatrix[12];
        bone_pos.Y = boneMatrix[1] * bone_offset_pos.X + boneMatrix[5] * bone_offset_pos.Y + boneMatrix[9] * bone_offset_pos.Z + boneMatrix[13];
        bone_pos.Z = boneMatrix[2] * bone_offset_pos.X + boneMatrix[6] * bone_offset_pos.Y + boneMatrix[10] * bone_offset_pos.Z + boneMatrix[14];

        return bone_pos;
    }

    /// <summary>
    /// 鼠标角度
    /// </summary>
    /// <param name="cameraV3"></param>
    /// <param name="targetV3"></param>
    /// <returns></returns>
    public static Vector3 GetCCameraViewAngles(Vector3 cameraV3, Vector3 targetV3)
    {
        var distance = (float)Math.Sqrt(Math.Pow(cameraV3.X - targetV3.X, 2) + Math.Pow(cameraV3.Y - targetV3.Y, 2) + Math.Pow(cameraV3.Z - targetV3.Z, 2));

        return new Vector3
        {
            X = (targetV3.X - cameraV3.X) / distance,
            Y = (targetV3.Y - cameraV3.Y) / distance,
            Z = (targetV3.Z - cameraV3.Z) / distance
        };
    }
}
