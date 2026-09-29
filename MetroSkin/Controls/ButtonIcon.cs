namespace MetroSkin.Controls;

public class ButtonIcon : Button
{
    /// <summary>
    /// Icon图标
    /// </summary>
    public string Icon
    {
        get { return (string)GetValue(IconProperty); }
        set { SetValue(IconProperty, value); }
    }
    public static readonly DependencyProperty IconProperty =
        DependencyProperty.Register("Icon", typeof(string), typeof(ButtonIcon), new PropertyMetadata(default));

    /// <summary>
    /// Title标题
    /// </summary>
    public string Title
    {
        get { return (string)GetValue(TitleProperty); }
        set { SetValue(TitleProperty, value); }
    }
    public static readonly DependencyProperty TitleProperty =
        DependencyProperty.Register("Title", typeof(string), typeof(ButtonIcon), new PropertyMetadata(default));

    /// <summary>
    /// Description描述
    /// </summary>
    public string Description
    {
        get { return (string)GetValue(DescriptionProperty); }
        set { SetValue(DescriptionProperty, value); }
    }
    public static readonly DependencyProperty DescriptionProperty =
        DependencyProperty.Register("Description", typeof(string), typeof(ButtonIcon), new PropertyMetadata(default));

    /// <summary>
    /// Star推荐星级
    /// </summary>
    public string Star
    {
        get { return (string)GetValue(StarProperty); }
        set { SetValue(StarProperty, value); }
    }
    public static readonly DependencyProperty StarProperty =
        DependencyProperty.Register("Star", typeof(string), typeof(ButtonIcon), new PropertyMetadata(default));
}
