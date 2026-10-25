using GTA5Shared.Helper;

namespace GTA5OnlineTools.Windows;

/// <summary>
/// StartupWindow.xaml 的交互逻辑
/// </summary>
public partial class StartupWindow
{
    public StartupWindow()
    {
        InitializeComponent();
    }

    private void Window_Startup_Loaded(object sender, RoutedEventArgs e)
    {

    }

    private void Window_Startup_Closing(object sender, CancelEventArgs e)
    {

    }

    ///////////////////////////////////////////////////////////////////////

    private void Button_StringToBase64_Click(object sender, RoutedEventArgs e)
    {


        var bytes = Encoding.UTF8.GetBytes(TextBox_Password.Text.Trim());
        TextBox_Password.Text = Convert.ToBase64String(bytes);
    }

    private void Button_BuildStartup_Click(object sender, RoutedEventArgs e)
    {


        var password = TextBox_Password.Text.Trim();
        BuildStartup(password);
    }

    private void BuildStartup(string password)
    {
        var builder = new StringBuilder();

        builder.AppendLine("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
        builder.AppendLine($"<!--{password}-->");
        builder.AppendLine("<CDataFileMgr__ContentsOfDataFileXml>");
        builder.AppendLine("\t<disabledFiles />");
        builder.AppendLine("\t<includedXmlFiles itemType=\"CDataFileMgr__DataFileArray\" />");
        builder.AppendLine("\t<includedDataFiles />");
        builder.AppendLine("\t<dataFiles itemType=\"CDataFileMgr__DataFile\">");
        builder.AppendLine("\t\t<Item>");
        builder.AppendLine("\t\t\t<filename>platform:/data/cdimages/scaleform_platform_pc.rpf</filename>");
        builder.AppendLine("\t\t\t<fileType>RPF_FILE</fileType>");
        builder.AppendLine("\t\t</Item>");
        builder.AppendLine("\t\t<Item>");
        builder.AppendLine("\t\t<filename>platform:/data/cdimages/scaleform_frontend.rpf</filename>");
        builder.AppendLine("\t\t\t<fileType>RPF_FILE_PRE_INSTALL</fileType>");
        builder.AppendLine("\t\t</Item>");
        builder.AppendLine("\t</dataFiles>");
        builder.AppendLine("\t<contentChangeSets itemType=\"CDataFileMgr__ContentChangeSet\" />");
        builder.AppendLine("\t<dataFiles itemType=\"CDataFileMgr__DataFile\" />");
        builder.AppendLine("\t<patchFiles />");
        builder.AppendLine("</CDataFileMgr__ContentsOfDataFileXml>");
        builder.AppendLine($"<!--{password}-->");

        var fileDialog = new SaveFileDialog
        {
            Title = "请放到 Grand Theft Auto V \\ x64 \\ data 文件夹下",
            RestoreDirectory = true,
            FileName = "startup.meta",
            Filter = "元数据文件|*.meta"
        };

        if (fileDialog.ShowDialog() == true)
        {
            File.WriteAllText(fileDialog.FileName, builder.ToString());

            NotifierHelper.Show(NotifierType.Success, "导出 startup.meta 文件成功");
        }
    }
}
