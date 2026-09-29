using GameOverlay.Drawing;

namespace GTA5Overlay;

public static class Draw
{
    private static Graphics _gfx;

    private static int _windowWidth = 100;
    private static int _windowHeight = 100;

    // 视角宽和视角高
    private static float _gviewWidth = _windowWidth / 2;
    private static float _gviewHeight = _windowHeight / 2;

    public static void SetGraphicsIns(Graphics gfx)
    {
        _gfx = gfx;
    }

    /// <summary>
    /// 设置窗口数据
    /// </summary>
    /// <param name="width"></param>
    /// <param name="height"></param>
    public static void SetWindowData(int width, int height)
    {
        _windowWidth = width;
        _windowHeight = height;

        _gviewWidth = _windowWidth / 2;
        _gviewHeight = _windowHeight / 2;
    }

    /// <summary>
    /// 绘制十字准星
    /// </summary>
    /// <param name="brush"></param>
    /// <param name="length"></param>
    public static void DrawCrosshair(IBrush brush, float length)
    {
        _gfx.DrawLine(brush,
            _gviewWidth - length,
            _gviewHeight,
            _gviewWidth + length,
            _gviewHeight, 1.5f);
        _gfx.DrawLine(brush,
            _gviewWidth,
            _gviewHeight - length,
            _gviewWidth,
            _gviewHeight + length, 1.5f);

        //_gfx.DrawCircle(brush, _gviewWidth, _gviewHeight, _gviewHeight / 4, 1.0f);
    }

    /// <summary>
    /// 2D方框
    /// </summary>
    /// <param name="brush"></param>
    /// <param name="screenV2"></param>
    /// <param name="boxV2"></param>
    public static void Draw2DBox(IBrush brush, Vector2 screenV2, Vector2 boxV2)
    {
        _gfx.DrawRectangle(brush, Rectangle.Create(
            screenV2.X - boxV2.X / 2,
            screenV2.Y - boxV2.Y / 2,
            boxV2.X,
            boxV2.Y), 1.0f);
    }

    /// <summary>
    /// 2D射线
    /// </summary>
    /// <param name="brush"></param>
    /// <param name="screenV2"></param>
    /// <param name="boxV2"></param>
    public static void Draw2DLine(IBrush brush, Vector2 screenV2, Vector2 boxV2)
    {
        _gfx.DrawLine(brush,
            _windowWidth / 2,
            0,
            screenV2.X,
            screenV2.Y - boxV2.Y / 2, 1.0f);
    }

    /// <summary>
    /// 2DBox血条
    /// </summary>
    /// <param name="dBrush"></param>
    /// <param name="fBrush"></param>
    /// <param name="screenV2"></param>
    /// <param name="boxV2"></param>
    /// <param name="pedHPPercentage"></param>
    public static void Draw2DHealthBar(IBrush dBrush, IBrush fBrush, Vector2 screenV2, Vector2 boxV2, float pedHPPercentage)
    {
        _gfx.DrawRectangle(dBrush, Rectangle.Create(
            screenV2.X - boxV2.X / 2 - boxV2.X / 8,
            screenV2.Y + boxV2.Y / 2,
            boxV2.X / 10,
            boxV2.Y * -1.0f), 0.7f);

        _gfx.FillRectangle(fBrush, Rectangle.Create(
            screenV2.X - boxV2.X / 2 - boxV2.X / 8,
            screenV2.Y + boxV2.Y / 2,
            boxV2.X / 10,
            boxV2.Y * pedHPPercentage * -1.0f));
    }

    /// <summary>
    /// 3DBox血条
    /// </summary>
    /// <param name="dBrush"></param>
    /// <param name="fBrush"></param>
    /// <param name="screenV2"></param>
    /// <param name="boxV2"></param>
    /// <param name="hpPer"></param>
    public static void Draw3DHealthBar(IBrush dBrush, IBrush fBrush, Vector2 screenV2, Vector2 boxV2, float hpPer)
    {
        _gfx.DrawRectangle(dBrush, Rectangle.Create(
            screenV2.X - boxV2.X / 2,
            screenV2.Y + boxV2.Y / 2 + boxV2.X / 10,
            boxV2.X,
            boxV2.X / 10 / 2), 0.7f);

        _gfx.FillRectangle(fBrush, Rectangle.Create(
            screenV2.X - boxV2.X / 2,
            screenV2.Y + boxV2.Y / 2 + boxV2.X / 10,
            boxV2.X * hpPer,
            boxV2.X / 10 / 2));
    }

    /// <summary>
    /// 2DBox血量数字
    /// </summary>
    /// <param name="font"></param>
    /// <param name="brush"></param>
    /// <param name="screenV2"></param>
    /// <param name="boxV2"></param>
    /// <param name="health"></param>
    /// <param name="maxHealth"></param>
    /// <param name="index"></param>
    public static void Draw2DHealthText(Font font, IBrush brush, Vector2 screenV2, Vector2 boxV2, float health, float maxHealth, int index)
    {
        _gfx.DrawText(font, 10, brush,
            screenV2.X - boxV2.X / 2,
            screenV2.Y + boxV2.Y / 2 + boxV2.X / 8 - boxV2.X / 10,
            $"[{index}] HP : {health:0}/{maxHealth:0}");
    }

    /// <summary>
    /// 3DBox血量数字
    /// </summary>
    /// <param name="font"></param>
    /// <param name="brush"></param>
    /// <param name="screenV2"></param>
    /// <param name="boxV2"></param>
    /// <param name="pedHealth"></param>
    /// <param name="pedMaxHealth"></param>
    /// <param name="index"></param>
    public static void Draw3DHealthText(Font font, IBrush brush, Vector2 screenV2, Vector2 boxV2, float pedHealth, float pedMaxHealth, int index)
    {
        _gfx.DrawText(font, 10, brush,
            screenV2.X - boxV2.X / 2,
            screenV2.Y + boxV2.Y / 2 + boxV2.X / 10 + boxV2.X / 10 / 2 + boxV2.X / 8 - boxV2.X / 10,
            $"[{index}] HP : {pedHealth:0}/{pedMaxHealth:0}");
    }

    /// <summary>
    /// 2DBox玩家名称
    /// </summary>
    /// <param name="font"></param>
    /// <param name="brush"></param>
    /// <param name="screenV2"></param>
    /// <param name="boxV2"></param>
    /// <param name="name"></param>
    /// <param name="distance"></param>
    public static void Draw2DNameText(Font font, IBrush brush, Vector2 screenV2, Vector2 boxV2, string name, float distance)
    {
        _gfx.DrawText(font, 10, brush,
            screenV2.X + boxV2.X / 2 + boxV2.X / 8 - boxV2.X / 10,
            screenV2.Y - boxV2.Y / 2,
            $"[{distance:0m}] ID : {name}");
    }

    /// <summary>
    /// 3DBox玩家名称
    /// </summary>
    /// <param name="font"></param>
    /// <param name="brush"></param>
    /// <param name="screenV2"></param>
    /// <param name="boxV2"></param>
    /// <param name="name"></param>
    /// <param name="distance"></param>
    public static void Draw3DNameText(Font font, IBrush brush, Vector2 screenV2, Vector2 boxV2, string name, float distance)
    {
        _gfx.DrawText(font, 10, brush,
            screenV2.X + boxV2.X / 2 + boxV2.X / 10 + boxV2.X / 10 / 2 + boxV2.X / 8 - boxV2.X / 10,
            screenV2.Y - boxV2.Y / 2,
            $"[{distance:0m}] ID : {name}");
    }

    /// <summary>
    /// 绘制3D方框连线
    /// </summary>
    /// <param name="brush"></param>
    /// <param name="m_Position"></param>
    public static void DrawAABBLine(IBrush brush, Vector3 m_Position)
    {
        var aabb_0 = new Vector3(0.0f, 0.0f, 1.0f) + m_Position; // 0
        var aabb_0V2Pos = Core.WorldToScreen(aabb_0);

        if (!Core.IsNullVector2(aabb_0V2Pos))
        {
            _gfx.DrawLine(brush,
                _windowWidth / 2, 0,
                aabb_0V2Pos.X, aabb_0V2Pos.Y, 1.0f);
        }
    }

    /// <summary>
    /// 绘制3D方框
    /// </summary>
    /// <param name="brush"></param>
    /// <param name="m_position"></param>
    /// <param name="m_sinCos"></param>
    /// <param name="dist"></param>
    /// <param name="ratio"></param>
    public static void DrawAABBBox(IBrush brush, Vector3 m_position, Vector2 m_sinCos, float dist = 1.0f, float ratio = 2.0f)
    {
        var aabb = new AxisAlignedBox()
        {
            Min = new Vector3
            {
                X = dist / ratio * -1,
                Y = dist / ratio * -1,
                Z = dist * -1
            },
            Max = new Vector3
            {
                X = dist / ratio,
                Y = dist / ratio,
                Z = dist
            }
        };

        var aabb_0 = new Vector3(
            aabb.Min.X * m_sinCos.Y - aabb.Min.Y * m_sinCos.X,
            aabb.Min.X * m_sinCos.X + aabb.Min.Y * m_sinCos.Y,
            aabb.Min.Z) + m_position; // 0
        var aabb_1 = new Vector3(
            aabb.Min.X * m_sinCos.Y - aabb.Max.Y * m_sinCos.X,
            aabb.Min.X * m_sinCos.X + aabb.Max.Y * m_sinCos.Y,
            aabb.Min.Z) + m_position; // 1
        var aabb_2 = new Vector3(
            aabb.Min.X * m_sinCos.Y - aabb.Min.Y * m_sinCos.X,
            aabb.Min.X * m_sinCos.X + aabb.Min.Y * m_sinCos.Y,
            aabb.Max.Z) + m_position; // 2
        var aabb_3 = new Vector3(
            aabb.Min.X * m_sinCos.Y - aabb.Max.Y * m_sinCos.X,
            aabb.Min.X * m_sinCos.X + aabb.Max.Y * m_sinCos.Y,
            aabb.Max.Z) + m_position; // 3

        var aabb_4 = new Vector3(
            aabb.Max.X * m_sinCos.Y - aabb.Min.Y * m_sinCos.X,
            aabb.Max.X * m_sinCos.X + aabb.Min.Y * m_sinCos.Y,
            aabb.Min.Z) + m_position; // 4
        var aabb_5 = new Vector3(
            aabb.Max.X * m_sinCos.Y - aabb.Max.Y * m_sinCos.X,
            aabb.Max.X * m_sinCos.X + aabb.Max.Y * m_sinCos.Y,
            aabb.Min.Z) + m_position; // 5
        var aabb_6 = new Vector3(
            aabb.Max.X * m_sinCos.Y - aabb.Min.Y * m_sinCos.X,
            aabb.Max.X * m_sinCos.X + aabb.Min.Y * m_sinCos.Y,
            aabb.Max.Z) + m_position; // 6
        var aabb_7 = new Vector3(
            aabb.Max.X * m_sinCos.Y - aabb.Max.Y * m_sinCos.X,
            aabb.Max.X * m_sinCos.X + aabb.Max.Y * m_sinCos.Y,
            aabb.Max.Z) + m_position; // 7

        /// <summary>
        ///                    max
        ///         6 --------- 7
        ///       / |         / |
        ///     2 --------- 3   |
        ///     |   |       |   |
        ///     |   |       |   |
        ///     |   4 --------- 5
        ///     | /         | /
        ///     0 --------- 1
        ///    min
        /// </summary>

        var aabb_0V2Pos = Core.WorldToScreen(aabb_0);
        var aabb_1V2Pos = Core.WorldToScreen(aabb_1);
        var aabb_2V2Pos = Core.WorldToScreen(aabb_2);
        var aabb_3V2Pos = Core.WorldToScreen(aabb_3);
        var aabb_4V2Pos = Core.WorldToScreen(aabb_4);
        var aabb_5V2Pos = Core.WorldToScreen(aabb_5);
        var aabb_6V2Pos = Core.WorldToScreen(aabb_6);
        var aabb_7V2Pos = Core.WorldToScreen(aabb_7);

        if (!Core.IsNullVector2(aabb_0V2Pos) &&
            !Core.IsNullVector2(aabb_1V2Pos) &&
            !Core.IsNullVector2(aabb_2V2Pos) &&
            !Core.IsNullVector2(aabb_3V2Pos) &&
            !Core.IsNullVector2(aabb_4V2Pos) &&
            !Core.IsNullVector2(aabb_5V2Pos) &&
            !Core.IsNullVector2(aabb_6V2Pos) &&
            !Core.IsNullVector2(aabb_7V2Pos))
        {
            _gfx.DrawLine(brush, aabb_0V2Pos.X, aabb_0V2Pos.Y, aabb_1V2Pos.X, aabb_1V2Pos.Y, 1.0f);
            _gfx.DrawLine(brush, aabb_0V2Pos.X, aabb_0V2Pos.Y, aabb_2V2Pos.X, aabb_2V2Pos.Y, 1.0f);
            _gfx.DrawLine(brush, aabb_3V2Pos.X, aabb_3V2Pos.Y, aabb_1V2Pos.X, aabb_1V2Pos.Y, 1.0f);
            _gfx.DrawLine(brush, aabb_3V2Pos.X, aabb_3V2Pos.Y, aabb_2V2Pos.X, aabb_2V2Pos.Y, 1.0f);

            _gfx.DrawLine(brush, aabb_4V2Pos.X, aabb_4V2Pos.Y, aabb_5V2Pos.X, aabb_5V2Pos.Y, 1.0f);
            _gfx.DrawLine(brush, aabb_4V2Pos.X, aabb_4V2Pos.Y, aabb_6V2Pos.X, aabb_6V2Pos.Y, 1.0f);
            _gfx.DrawLine(brush, aabb_7V2Pos.X, aabb_7V2Pos.Y, aabb_5V2Pos.X, aabb_5V2Pos.Y, 1.0f);
            _gfx.DrawLine(brush, aabb_7V2Pos.X, aabb_7V2Pos.Y, aabb_6V2Pos.X, aabb_6V2Pos.Y, 1.0f);

            _gfx.DrawLine(brush, aabb_0V2Pos.X, aabb_0V2Pos.Y, aabb_4V2Pos.X, aabb_4V2Pos.Y, 1.0f);
            _gfx.DrawLine(brush, aabb_1V2Pos.X, aabb_1V2Pos.Y, aabb_5V2Pos.X, aabb_5V2Pos.Y, 1.0f);
            _gfx.DrawLine(brush, aabb_2V2Pos.X, aabb_2V2Pos.Y, aabb_6V2Pos.X, aabb_6V2Pos.Y, 1.0f);
            _gfx.DrawLine(brush, aabb_3V2Pos.X, aabb_3V2Pos.Y, aabb_7V2Pos.X, aabb_7V2Pos.Y, 1.0f);
        }
    }

    /// <summary>
    /// 绘制骨骼连线
    /// </summary>
    /// <param name="brush"></param>
    /// <param name="pCPed"></param>
    /// <param name="boneMatrix"></param>
    /// <param name="boneStartId"></param>
    /// <param name="boneEndId"></param>
    public static void DrawBone(IBrush brush, long pCPed, float[] boneMatrix, int boneStartId, int boneEndId)
    {
        var v2BoneStart = Core.WorldToScreen(Core.GetBonePosition(pCPed, boneMatrix, boneStartId));
        var v2BoneEnd = Core.WorldToScreen(Core.GetBonePosition(pCPed, boneMatrix, boneEndId));

        if (!Core.IsNullVector2(v2BoneStart) && !Core.IsNullVector2(v2BoneEnd))
            _gfx.DrawLine(brush, v2BoneStart.X, v2BoneStart.Y, v2BoneEnd.X, v2BoneEnd.Y, 1);
    }

    /// <summary>
    /// 绘制骨骼调试
    /// </summary>
    /// <param name="font"></param>
    /// <param name="brush"></param>
    /// <param name="pCPed"></param>
    /// <param name="boneMatrix"></param>
    /// <param name="boneId"></param>
    public static void DrawBoneDebug(Font font, IBrush brush, long pCPed, float[] boneMatrix, int boneId)
    {
        var v2Bone = Core.WorldToScreen(Core.GetBonePosition(pCPed, boneMatrix, boneId));

        if (!Core.IsNullVector2(v2Bone))
            _gfx.DrawText(font, 10, brush, v2Bone.X, v2Bone.Y, $"{boneId}");
    }
}

public struct AxisAlignedBox
{
    public Vector3 Min;
    public Vector3 Max;
}
