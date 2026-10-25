namespace MetroSkin.Controls;

public class BlurImage : Control
{
    /// <summary>
    /// 模糊半径
    /// </summary>
    public double BlurRadius
    {
        get { return (double)GetValue(BlurRadiusProperty); }
        set { SetValue(BlurRadiusProperty, value); }
    }
    public static readonly DependencyProperty BlurRadiusProperty =
        DependencyProperty.Register("BlurRadius", typeof(double), typeof(BlurImage), new PropertyMetadata(25.0));

    /// <summary>
    /// 缩放大小
    /// </summary>
    public double ScaleSize
    {
        get { return (double)GetValue(ScaleSizeProperty); }
        set { SetValue(ScaleSizeProperty, value); }
    }
    public static readonly DependencyProperty ScaleSizeProperty =
        DependencyProperty.Register("ScaleSize", typeof(double), typeof(BlurImage), new PropertyMetadata(1.05));

    /// <summary>
    /// 遮罩层透明度
    /// </summary>
    public double ShadeOpacity
    {
        get { return (double)GetValue(ShadeOpacityProperty); }
        set { SetValue(ShadeOpacityProperty, value); }
    }
    public static readonly DependencyProperty ShadeOpacityProperty =
        DependencyProperty.Register("ShadeOpacity", typeof(double), typeof(BlurImage), new PropertyMetadata(0.1));

    /// <summary>
    /// 要模糊的图像
    /// </summary>
    public ImageSource Source
    {
        get { return (ImageSource)GetValue(SourceProperty); }
        set { SetValue(SourceProperty, value); }
    }
    public static readonly DependencyProperty SourceProperty =
        DependencyProperty.Register("Source", typeof(ImageSource), typeof(BlurImage), new PropertyMetadata(default));

    /// <summary>
    /// 图像缩放
    /// </summary>
    public Stretch Stretch
    {
        get { return (Stretch)GetValue(StretchProperty); }
        set { SetValue(StretchProperty, value); }
    }
    public static readonly DependencyProperty StretchProperty =
        DependencyProperty.Register("Stretch", typeof(Stretch), typeof(BlurImage), new PropertyMetadata(default));
}
