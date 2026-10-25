using GTA5Core.Features;
using GTA5Shared.Helper;

namespace GTA5MenuExtra;

/// <summary>
/// OutfitsEditorWindow.xaml 的交互逻辑
/// </summary>
public partial class OutfitsEditorWindow
{
    public OutfitsEditorWindow()
    {
        InitializeComponent();
    }

    private void Window_OutfitsEditor_Loaded(object sender, RoutedEventArgs e)
    {
        ComboBox_OutfitIndex.SelectedIndex = 0;
    }

    private void Window_OutfitsEditor_Closing(object sender, CancelEventArgs e)
    {

    }

    ///////////////////////////////////////////////////////////////////////

    private void Hyperlink_RequestNavigate(object sender, RequestNavigateEventArgs e)
    {
        ProcessHelper.OpenLink(e.Uri.OriginalString);
        e.Handled = true;
    }

    ///////////////////////////////////////////////////////////////////////

    private void ReadOutfitsData()
    {
        if (ComboBox_OutfitIndex is null)
            return;

        var index = ComboBox_OutfitIndex.SelectedIndex;
        if (index == -1)
            return;

        Outfits.OutfitIndex = index;

        TextBox_OutfitName.Text = Outfits.GetOutfitNameByIndex();

        TextBox_HEAD.Text = Outfits.HEAD.ToString();
        TextBox_HEAD_TEX.Text = Outfits.HEAD_TEX.ToString();

        TextBox_MASKS.Text = Outfits.MASKS.ToString();
        TextBox_MASKS_TEX.Text = Outfits.MASKS_TEX.ToString();

        TextBox_HAIR.Text = Outfits.HAIR.ToString();
        TextBox_HAIR_TEX.Text = Outfits.HAIR_TEX.ToString();

        TextBox_TORSOS.Text = Outfits.TORSOS.ToString();
        TextBox_TORSOS_TEX.Text = Outfits.TORSOS_TEX.ToString();

        TextBox_LEGS.Text = Outfits.LEGS.ToString();
        TextBox_LEGS_TEX.Text = Outfits.LEGS_TEX.ToString();

        TextBox_BAGS.Text = Outfits.BAGS.ToString();
        TextBox_BAGS_TEX.Text = Outfits.BAGS_TEX.ToString();

        TextBox_SHOES.Text = Outfits.SHOES.ToString();
        TextBox_SHOES_TEX.Text = Outfits.SHOES_TEX.ToString();

        TextBox_ACCESSORIES.Text = Outfits.ACCESSORIES.ToString();
        TextBox_ACCESSORIES_TEX.Text = Outfits.ACCESSORIES_TEX.ToString();

        TextBox_UNDERSHIRTS.Text = Outfits.UNDERSHIRTS.ToString();
        TextBox_UNDERSHIRTS_TEX.Text = Outfits.UNDERSHIRTS_TEX.ToString();

        TextBox_ARMORS.Text = Outfits.ARMORS.ToString();
        TextBox_ARMORS_TEX.Text = Outfits.ARMORS_TEX.ToString();

        TextBox_DECALS.Text = Outfits.DECALS.ToString();
        TextBox_DECALS_TEX.Text = Outfits.DECALS_TEX.ToString();

        TextBox_TOPS.Text = Outfits.TOPS.ToString();
        TextBox_TOPS_TEX.Text = Outfits.TOPS_TEX.ToString();

        ///////////////////////////////////////////////////////////////////////

        TextBox_HATS.Text = Outfits.HATS.ToString();
        TextBox_HATS_TEX.Text = Outfits.HATS_TEX.ToString();

        TextBox_GLASSES.Text = Outfits.GLASSES.ToString();
        TextBox_GLASSES_TEX.Text = Outfits.GLASSES_TEX.ToString();

        TextBox_EARS.Text = Outfits.EARS.ToString();
        TextBox_EARS_TEX.Text = Outfits.EARS_TEX.ToString();

        TextBox_WATCHES.Text = Outfits.WATCHES.ToString();
        TextBox_WATCHES_TEX.Text = Outfits.WATCHES_TEX.ToString();

        TextBox_BRACELETS.Text = Outfits.BRACELETS.ToString();
        TextBox_BRACELETS_TEX.Text = Outfits.BRACELETS_TEX.ToString();
    }

    private void WriteOutfitsData()
    {
        if (ComboBox_OutfitIndex is null)
            return;

        var index = ComboBox_OutfitIndex.SelectedIndex;
        if (index == -1)
            return;

        Outfits.OutfitIndex = index;

        Outfits.SetOutfitNameByIndex(TextBox_OutfitName.Text);

        Outfits.HEAD = Convert.ToInt32(TextBox_HEAD.Text);
        Outfits.HEAD_TEX = Convert.ToInt32(TextBox_HEAD_TEX.Text);

        Outfits.MASKS = Convert.ToInt32(TextBox_MASKS.Text);
        Outfits.MASKS_TEX = Convert.ToInt32(TextBox_MASKS_TEX.Text);

        Outfits.HAIR = Convert.ToInt32(TextBox_HAIR.Text);
        Outfits.HAIR_TEX = Convert.ToInt32(TextBox_HAIR_TEX.Text);

        Outfits.TORSOS = Convert.ToInt32(TextBox_TORSOS.Text);
        Outfits.TORSOS_TEX = Convert.ToInt32(TextBox_TORSOS_TEX.Text);

        Outfits.LEGS = Convert.ToInt32(TextBox_LEGS.Text);
        Outfits.LEGS_TEX = Convert.ToInt32(TextBox_LEGS_TEX.Text);

        Outfits.BAGS = Convert.ToInt32(TextBox_BAGS.Text);
        Outfits.BAGS_TEX = Convert.ToInt32(TextBox_BAGS_TEX.Text);

        Outfits.SHOES = Convert.ToInt32(TextBox_SHOES.Text);
        Outfits.SHOES_TEX = Convert.ToInt32(TextBox_SHOES_TEX.Text);

        Outfits.ACCESSORIES = Convert.ToInt32(TextBox_ACCESSORIES.Text);
        Outfits.ACCESSORIES_TEX = Convert.ToInt32(TextBox_ACCESSORIES_TEX.Text);

        Outfits.UNDERSHIRTS = Convert.ToInt32(TextBox_UNDERSHIRTS.Text);
        Outfits.UNDERSHIRTS_TEX = Convert.ToInt32(TextBox_UNDERSHIRTS_TEX.Text);

        Outfits.ARMORS = Convert.ToInt32(TextBox_ARMORS.Text);
        Outfits.ARMORS_TEX = Convert.ToInt32(TextBox_ARMORS_TEX.Text);

        Outfits.DECALS = Convert.ToInt32(TextBox_DECALS.Text);
        Outfits.DECALS_TEX = Convert.ToInt32(TextBox_DECALS_TEX.Text);

        Outfits.TOPS = Convert.ToInt32(TextBox_TOPS.Text);
        Outfits.TOPS_TEX = Convert.ToInt32(TextBox_TOPS_TEX.Text);

        ///////////////////////////////////////////////////////////////////////

        Outfits.HATS = Convert.ToInt32(TextBox_HATS.Text);
        Outfits.HATS_TEX = Convert.ToInt32(TextBox_HATS_TEX.Text);

        Outfits.GLASSES = Convert.ToInt32(TextBox_GLASSES.Text);
        Outfits.GLASSES_TEX = Convert.ToInt32(TextBox_GLASSES_TEX.Text);

        Outfits.EARS = Convert.ToInt32(TextBox_EARS.Text);
        Outfits.EARS_TEX = Convert.ToInt32(TextBox_EARS_TEX.Text);

        Outfits.WATCHES = Convert.ToInt32(TextBox_WATCHES.Text);
        Outfits.WATCHES_TEX = Convert.ToInt32(TextBox_WATCHES_TEX.Text);

        Outfits.BRACELETS = Convert.ToInt32(TextBox_BRACELETS.Text);
        Outfits.BRACELETS_TEX = Convert.ToInt32(TextBox_BRACELETS_TEX.Text);
    }

    ///////////////////////////////////////////////////////////////////////

    private void ComboBox_OutfitIndex_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        ReadOutfitsData();
    }

    private void Button_Read_Click(object sender, RoutedEventArgs e)
    {


        ReadOutfitsData();

        NotifierHelper.Show(NotifierType.Success, $"读取 槽位{Outfits.OutfitIndex} 角色服装数据 成功");
    }

    private void Button_Write_Click(object sender, RoutedEventArgs e)
    {


        try
        {
            WriteOutfitsData();

            NotifierHelper.Show(NotifierType.Success, $"写入 槽位{Outfits.OutfitIndex} 角色服装数据 成功");
        }
        catch
        {
            NotifierHelper.Show(NotifierType.Warning, "部分数据不合法，请检查后重新写入");
        }
    }
}
