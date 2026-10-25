namespace GTA5Shared.Helper;

public static class ControlHelper
{
    /// <summary>
    /// 获取子控件集合
    /// </summary>
    /// <param name="parent"></param>
    /// <returns></returns>
    public static IList<Control> GetControls(this DependencyObject parent)
    {
        var result = new List<Control>();

        for (int x = 0; x < VisualTreeHelper.GetChildrenCount(parent); x++)
        {
            var child = VisualTreeHelper.GetChild(parent, x);

            if (child is Control instance)
                result.Add(instance);

            result.AddRange(child.GetControls());
        }

        return result;
    }
}
