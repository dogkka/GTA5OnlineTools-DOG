namespace MetroSkin.Controls;

public class TextKeyValue : Control
{
    /// <summary>
    /// 字体图标
    /// </summary>
    public string Icon
    {
        get { return (string)GetValue(IconProperty); }
        set { SetValue(IconProperty, value); }
    }
    public static readonly DependencyProperty IconProperty =
        DependencyProperty.Register("Icon", typeof(string), typeof(TextKeyValue), new PropertyMetadata(default));

    /// <summary>
    /// Key字符串
    /// </summary>
    public string Key
    {
        get { return (string)GetValue(KeyProperty); }
        set { SetValue(KeyProperty, value); }
    }
    public static readonly DependencyProperty KeyProperty =
        DependencyProperty.Register("Key", typeof(string), typeof(TextKeyValue), new PropertyMetadata(default));

    /// <summary>
    /// Value字符串
    /// </summary>
    public string Value
    {
        get { return (string)GetValue(ValueProperty); }
        set { SetValue(ValueProperty, value); }
    }
    public static readonly DependencyProperty ValueProperty =
        DependencyProperty.Register("Value", typeof(string), typeof(TextKeyValue), new PropertyMetadata(default));
}
