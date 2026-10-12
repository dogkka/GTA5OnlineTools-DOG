namespace GTA5Shared.Helper;

public static class FileHelper
{
    public const string ResFiles = "GTA5Shared.Files";

    public const string Res_Cache_Notepad2 = $"{ResFiles}.Cache.Notepad2.exe";
    public const string Res_Cache_CloudflareSpeedTest = $"{ResFiles}.Cache.CloudflareSpeedTest.exe";

    public const string Res_YimMenu_YimMenu = $"{ResFiles}.YimMenu.YimMenu_uc.dll";
    public const string Res_YimMenu_YimMenu_X = $"{ResFiles}.YimMenu.YimMenu.dll";
    public const string Res_YimMenu_YimMenu_V2 = $"{ResFiles}.YimMenu.NewBase.dll";
    public const string Res_YimMenu_YimMenu_V2_Font = $"{ResFiles}.YimMenu.font.ttf";
    public const string Res_YimMenu_IndexCN = $"{ResFiles}.YimMenu.index_cn.json";
    public const string Res_YimMenu_IndexTW = $"{ResFiles}.YimMenu.index_tw.json";
    public const string Res_YimMenu_ZHCN = $"{ResFiles}.YimMenu.zh_CN.json";
    public const string Res_YimMenu_ZHTW = $"{ResFiles}.YimMenu.zh_TW.json";

    public const string Res_Other_SGTA50000 = $"{ResFiles}.Other.SGTA50000";
    public const string Res_FSL_WINMM = $"{ResFiles}.FSL.WINMM.dll";
    public const string Res_Lua_LuaPack = $"{ResFiles}.Lua.LuaPack.zip";
    public const string Res_Lua_UltimateMenuLegacy = $"{ResFiles}.Lua.UltimateMenu_V3_1.73_Legacy.lua";
    public const string Res_Lua_DogInjectFx = $"{ResFiles}.Lua.DOG_InjectFx.lua";
    public const string Res_Tutorial = $"{ResFiles}.Tutorial.html";

    //////////////////////////////////////////////////////////////////

    public static string Dir_CommonAppData { get; private set; }
    public static string Dir_MyDocuments { get; private set; }

    public static string Dir_Base { get; private set; }

    public static string Dir_YimMenu { get; private set; }
    public static string Dir_FSL { get; private set; }
    public static string Dir_Cache { get; private set; }
    public static string Dir_Config { get; private set; }
    public static string Dir_Logger { get; private set; }

    public static string Dir_Log_Crash { get; private set; }
    public static string Dir_Log_NLog { get; private set; }

    public static string File_Cache_Notepad2 { get; private set; }
    public static string File_Cache_CloudflareSpeedTest { get; private set; }

    public static string File_YimMenu_DLL { get; private set; }
    public static string File_YimMenu_DLL_X { get; private set; }
    public static string File_YimMenu_DLL_V2 { get; private set; }
    public static string File_YimMenu_DLL_V2_Online { get; private set; }
    public static string File_YimMenu_DLL_V1_Online { get; private set; }
    public static string File_YimMenu_IndexCN { get; private set; }
    public static string File_YimMenu_IndexTW { get; private set; }
    public static string File_YimMenu_ZHCN { get; private set; }
    public static string File_YimMenu_ZHTW { get; private set; }

    //////////////////////////////////////////////////////////////////

    public static string Dir_AppData { get; private set; }

    public static string Dir_AppData_YimMenu { get; private set; }
    public static string Dir_AppData_YimMenu_V2 { get; private set; }
    public static string Dir_AppData_FSL { get; private set; }
    public static string Dir_AppData_YimMenu_Scripts { get; private set; }
    public static string Dir_AppData_YimMenu_Translations { get; private set; }

    public static string File_AppData_YimMenu_Settings { get; private set; }
    public static string File_AppData_YimMenu_Logger { get; private set; }
    public static string File_AppData_YimMenu_V2_Settings { get; private set; }
    public static string File_AppData_YimMenu_V2_Logger { get; private set; }
    public static string File_AppData_YimMenu_V2_Font { get; private set; }

    //////////////////////////////////////////////////////////////////

    public static string File_Config_Vehicles { get; private set; }
    public static string File_Config_Teleports { get; private set; }
    public static string File_Config_Blips { get; private set; }
    public static string File_FSL_WINMM { get; private set; }

    //////////////////////////////////////////////////////////////////

    static FileHelper()
    {
        Dir_CommonAppData = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData);
        Dir_MyDocuments = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
        Dir_AppData = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);

        Initialization(Dir_CommonAppData);
    }

    /// <summary>
    /// 初始化配置文件夹
    /// </summary>
    /// <param name="baseDir"></param>
    private static void Initialization(string baseDir)
    {
        // 文件夹路径拼接，结尾加上 \\ ，以免出现同名文件导致不会正确打开文件夹

        Dir_Base = Path.Combine(baseDir, "GTA5OnlineTools\\");
        Dir_AppData_YimMenu = Path.Combine(Dir_AppData, "YimMenu\\");
        Dir_AppData_YimMenu_V2 = Path.Combine(Dir_AppData, "YimMenuV2\\");
        Dir_AppData_FSL = Path.Combine(Dir_AppData, "FSL\\");

        Dir_YimMenu = Path.Combine(Dir_Base, "YimMenu\\");
        Dir_FSL = Path.Combine(Dir_Base, "FSL\\");
        Dir_Cache = Path.Combine(Dir_Base, "Cache\\");
        Dir_Config = Path.Combine(Dir_Base, "Config\\");
        Dir_Logger = Path.Combine(Dir_Base, "Logger\\");

        Dir_Log_Crash = Path.Combine(Dir_Logger, "Crash\\");
        Dir_Log_NLog = Path.Combine(Dir_Logger, "NLog\\");

        Dir_AppData_YimMenu_Scripts = Path.Combine(Dir_AppData_YimMenu, "scripts\\");
        Dir_AppData_YimMenu_Translations = Path.Combine(Dir_AppData_YimMenu, "translations\\");

        ////////////////////////////////////////////////////////////

        File_AppData_YimMenu_Settings = Path.Combine(Dir_AppData_YimMenu, "settings.json");
        File_AppData_YimMenu_Logger = Path.Combine(Dir_AppData_YimMenu, "cout.log");

        File_AppData_YimMenu_V2_Settings = Path.Combine(Dir_AppData_YimMenu_V2, "settings.json");
        File_AppData_YimMenu_V2_Logger = Path.Combine(Dir_AppData_YimMenu_V2, "cout.log");
        File_AppData_YimMenu_V2_Font = Path.Combine(Dir_AppData_YimMenu_V2, "font.ttf");

        File_Cache_Notepad2 = Path.Combine(Dir_Cache, "Notepad2.exe");
        File_Cache_CloudflareSpeedTest = Path.Combine(Dir_Cache, "CloudflareSpeedTest.exe");

        File_YimMenu_DLL = Path.Combine(Dir_YimMenu, "YimMenu_uc.dll");
        File_YimMenu_DLL_X = Path.Combine(Dir_YimMenu, "YimMenu.dll");
        File_YimMenu_DLL_V2 = Path.Combine(Dir_YimMenu, "NewBase.dll");
        File_YimMenu_DLL_V2_Online = Path.Combine(Dir_YimMenu, "NewBase_net.dll");
        File_YimMenu_DLL_V1_Online = Path.Combine(Dir_YimMenu, "YimMenu_net.dll");

        File_YimMenu_IndexCN = Path.Combine(Dir_AppData_YimMenu_Translations, "index.json");
        File_YimMenu_IndexTW = Path.Combine(Dir_AppData_YimMenu_Translations, "index.json");
        File_YimMenu_ZHCN = Path.Combine(Dir_AppData_YimMenu_Translations, "zh_CN.json");
        File_YimMenu_ZHTW = Path.Combine(Dir_AppData_YimMenu_Translations, "zh_TW.json");

        File_Config_Vehicles = Path.Combine(Dir_Config, "Vehicles.json");
        File_Config_Teleports = Path.Combine(Dir_Config, "Teleports.json");
        File_Config_Blips = Path.Combine(Dir_Config, "Blips.json");
        File_FSL_WINMM = Path.Combine(Dir_FSL, "WINMM.dll");
    }

    /// <summary>
    /// 重新设置初始化文件夹（一般用于启动参数）
    /// </summary>
    /// <param name="baseDir"></param>
    public static void ResetBaseDir(string baseDir)
    {
        Initialization(baseDir);
    }

    /// <summary>
    /// 创建文件夹，如果文件夹存在则不创建
    /// </summary>
    /// <param name="dirPath"></param>
    public static void CreateDirectory(string dirPath)
    {
        if (!Directory.Exists(dirPath))
            Directory.CreateDirectory(dirPath);
    }

    /// <summary>
    /// 保存崩溃日志
    /// </summary>
    /// <param name="log">日志内容</param>
    public static void SaveCrashLog(string log)
    {
        var path = Path.Combine(Dir_Log_Crash, $"#Crash#{DateTime.Now:yyyyMMdd_HH-mm-ss_ffff}.log");
        File.WriteAllText(path, log);
    }

    /// <summary>
    /// 从资源文件中抽取资源文件
    /// </summary>
    /// <param name="resFileName">资源文件路径</param>
    /// <param name="outputFile">输出文件</param>
    public static void ExtractResFile(string resFileName, string outputFile)
    {
        BufferedStream inStream = null;
        FileStream outStream = null;

        try
        {
            var assembly = Assembly.GetExecutingAssembly();
            inStream = new BufferedStream(assembly.GetManifestResourceStream(resFileName));
            outStream = new FileStream(outputFile, FileMode.Create, FileAccess.Write);

            var buffer = new byte[1024];
            int length;

            while ((length = inStream.Read(buffer, 0, buffer.Length)) > 0)
                outStream.Write(buffer, 0, length);

            outStream.Flush();
        }
        finally
        {
            outStream?.Close();
            inStream?.Close();
        }
    }

    /// <summary>
    /// [DOG] 安装增强版 Lua 脚本包（内嵌 LuaPack.zip）到指定脚本目录。
    /// 已存在的同名文件会先备份到目标目录下的 _备份_时间戳 子目录，再做覆盖。
    /// </summary>
    /// <param name="targetDir">目标脚本目录（如 %AppData%\YimMenuV2\scripts）</param>
    /// <returns>(是否成功, 结果消息)</returns>
    public static (bool ok, string message) InstallLuaPack(string targetDir)
    {
        try
        {
            var assembly = Assembly.GetExecutingAssembly();
            using var stream = assembly.GetManifestResourceStream(Res_Lua_LuaPack);
            if (stream == null)
                return (false, "脚本包安装失败：内嵌资源缺失（LuaPack.zip）");

            CreateDirectory(targetDir);

            var rootFull = Path.GetFullPath(targetDir).TrimEnd(Path.DirectorySeparatorChar) + Path.DirectorySeparatorChar;
            var backupDir = Path.Combine(targetDir, $"_备份_{DateTime.Now:yyyyMMdd_HH-mm-ss}");

            var extracted = 0;
            var backedUp = 0;

            using (var archive = new ZipArchive(stream, ZipArchiveMode.Read))
            {
                foreach (var entry in archive.Entries)
                {
                    if (string.IsNullOrEmpty(entry.Name))
                        continue; // 目录项

                    var destPath = Path.GetFullPath(Path.Combine(targetDir, entry.FullName));
                    if (!destPath.StartsWith(rootFull, StringComparison.OrdinalIgnoreCase))
                        continue; // 路径穿越防护

                    var destDir = Path.GetDirectoryName(destPath);
                    CreateDirectory(destDir);

                    if (File.Exists(destPath))
                    {
                        var backupPath = Path.Combine(backupDir, entry.FullName);
                        CreateDirectory(Path.GetDirectoryName(backupPath));
                        File.Copy(destPath, backupPath, true);
                        backedUp++;
                    }

                    entry.ExtractToFile(destPath, true);
                    extracted++;
                }
            }

            var message = $"脚本包安装完成：{extracted} 个文件 → {targetDir}";
            if (backedUp > 0)
                message += $"\n已备份被覆盖的旧文件 {backedUp} 个到：{backupDir}";

            return (true, message);
        }
        catch (Exception ex)
        {
            return (false, $"脚本包安装失败：{ex.Message}\n如果游戏/菜单正在运行，请关闭后重试。");
        }
    }

    /// <summary>
    /// [DOG] 一键安装传承版 Lua 脚本包（内嵌 Ultimate Menu V3 + DOG 注入动效 → %AppData%\YimMenu\scripts）。
    /// 已存在的同名文件会先备份到 _备份_时间戳 子目录，再做覆盖。
    /// </summary>
    /// <returns>(是否成功, 结果消息)</returns>
    public static (bool ok, string message) InstallLegacyLuaPack()
    {
        try
        {
            var assembly = Assembly.GetExecutingAssembly();

            var items = new (string res, string fileName)[]
            {
                (Res_Lua_UltimateMenuLegacy, "Ultimate_Menu_V3_1.73_Legacy.lua"),
                (Res_Lua_DogInjectFx, "DOG_InjectFx.lua"),
            };

            var targetDir = Dir_AppData_YimMenu_Scripts;
            CreateDirectory(targetDir);

            var backupDir = Path.Combine(targetDir, $"_备份_{DateTime.Now:yyyyMMdd_HH-mm-ss}");
            var installed = new List<string>();
            var backedUp = 0;

            foreach (var (res, fileName) in items)
            {
                using var stream = assembly.GetManifestResourceStream(res);
                if (stream == null)
                    return (false, $"传承版脚本包安装失败：内嵌资源缺失（{fileName}）");

                var dest = Path.Combine(targetDir, fileName);

                if (File.Exists(dest))
                {
                    CreateDirectory(backupDir);
                    File.Copy(dest, Path.Combine(backupDir, fileName), true);
                    backedUp++;
                }

                using var fs = new FileStream(dest, FileMode.Create, FileAccess.Write);
                stream.CopyTo(fs);
                installed.Add(fileName);
            }

            var message = $"传承版脚本包安装完成：{string.Join("、", installed)} → {targetDir}\n其中 Ultimate_Menu_V3 为汉化版（适配 1.73 b3889）；DOG_InjectFx.lua 为「注入成功」启动动画（每次注入自动播放）";
            if (backedUp > 0)
                message += $"\n已备份被覆盖的旧文件 {backedUp} 个到：{backupDir}";
            message += "\n如游戏/菜单正在运行：菜单内 Scripts → Reload All；否则下次注入自动加载";

            return (true, message);
        }
        catch (Exception ex)
        {
            return (false, $"传承版脚本包安装失败：{ex.Message}\n如果游戏/菜单正在运行，请关闭后重试。");
        }
    }

    /// <summary>
    /// [DOG] 传承版「自愈补齐」：注入前自动检查，缺什么补什么（已有文件一律不动，不覆盖用户版本/配置）：
    ///  - 脚本目录没有任何 Ultimate_Menu*.lua → 部署内嵌 UM V3（玩法脚本：前置 / 分红 / 刷钱 / 等级）；
    ///  - DOG_InjectFx.lua 缺失 → 部署注入动画；
    ///  - %AppData%\YimMenu\settings.json 不存在 → 预写默认呼出键 F7（VK 118，菜单 设置 → 热键 内可改；
    ///    Mr-X 版本读取 settings.json 为「缺键合并默认」的容错逻辑，写入片段不会破坏其它设置）。
    /// </summary>
    /// <returns>(是否有新写入, 结果消息)</returns>
    public static (bool changed, string message) EnsureLegacyLuaPack()
    {
        try
        {
            var assembly = Assembly.GetExecutingAssembly();
            var targetDir = Dir_AppData_YimMenu_Scripts;
            CreateDirectory(targetDir);

            var installed = new List<string>();

            // 0) 旧英文版（含工具内置 CRLF 变体）/ 旧汉化版脚本自动升级为最新汉化版（按已知哈希精确识别，先备份再覆盖）
            var standardPath = Path.Combine(targetDir, "Ultimate_Menu_V3_1.73_Legacy.lua");
            var isEnglishLegacy = File.Exists(standardPath) &&
                (IsFileSha256(standardPath, LegacyUmEnglishSha256) || IsFileSha256(standardPath, LegacyUmEmbeddedEnSha256));
            var isOldZhLegacy = File.Exists(standardPath) &&
                (IsFileSha256(standardPath, LegacyUmZhV1Sha256) || IsFileSha256(standardPath, LegacyUmZhV2Sha256) || IsFileSha256(standardPath, LegacyUmZhV3Sha256));
            if (isEnglishLegacy || isOldZhLegacy)
            {
                var upgradeBackupDir = Path.Combine(targetDir, $"_备份_{DateTime.Now:yyyyMMdd_HH-mm-ss}");
                CreateDirectory(upgradeBackupDir);
                var backupName = isEnglishLegacy
                    ? "Ultimate_Menu_V3_1.73_Legacy_英文原版.lua"
                    : "Ultimate_Menu_V3_1.73_Legacy_旧汉化版.lua";
                File.Copy(standardPath, Path.Combine(upgradeBackupDir, backupName), true);

                if (ExtractNamedResFile(assembly, Res_Lua_UltimateMenuLegacy, standardPath))
                    installed.Add("Ultimate_Menu_V3_1.73_Legacy.lua（" + (isEnglishLegacy ? "英文版" : "旧汉化版") + "已自动升级为最新汉化版）");
            }

            // 1) 玩法脚本（Ultimate Menu V3）：已有任意 UM 版本则不覆盖
            var hasUltimateMenu = Directory.GetFiles(targetDir, "Ultimate_Menu*.lua").Length > 0;
            if (!hasUltimateMenu)
            {
                var dest = Path.Combine(targetDir, "Ultimate_Menu_V3_1.73_Legacy.lua");
                if (ExtractNamedResFile(assembly, Res_Lua_UltimateMenuLegacy, dest))
                    installed.Add("Ultimate_Menu_V3_1.73_Legacy.lua");
            }

            // 2) 注入动画
            var animDest = Path.Combine(targetDir, "DOG_InjectFx.lua");
            if (!File.Exists(animDest))
            {
                if (ExtractNamedResFile(assembly, Res_Lua_DogInjectFx, animDest))
                    installed.Add("DOG_InjectFx.lua");
            }

            // 3) 默认呼出键 F7（仅当配置文件不存在时写入）
            var settingsWritten = false;
            if (!File.Exists(File_AppData_YimMenu_Settings))
            {
                CreateDirectory(Dir_AppData_YimMenu);
                File.WriteAllText(File_AppData_YimMenu_Settings,
                    "{\r\n" +
                    "  \"settings\": {\r\n" +
                    "    \"hotkeys\": {\r\n" +
                    "      \"menu_toggle\": 118\r\n" +
                    "    }\r\n" +
                    "  }\r\n" +
                    "}");
                settingsWritten = true;
            }

            if (installed.Count == 0 && !settingsWritten)
                return (false, string.Empty);

            var parts = new List<string>();
            if (installed.Count > 0)
                parts.Add("脚本：" + string.Join("、", installed));
            if (settingsWritten)
                parts.Add("默认呼出键 F7（菜单 设置 → 热键 可改）");

            return (true, "已自动补齐传承版配置 → " + string.Join("；", parts));
        }
        catch (Exception ex)
        {
            return (false, $"传承版自动补齐失败：{ex.Message}");
        }
    }

    /// <summary>上游英文原版 UltimateMenu V3 1.73 的 SHA256（用于识别并升级为汉化版）</summary>
    private const string LegacyUmEnglishSha256 = "1A8A9EA7A7272F797A8CAEFF2BB45C25E7977BF87D368AFF4E48D101EDE3A9F7";

    /// <summary>v2.1.40 工具内置英文版（内容与上游一致，CRLF 行尾）的 SHA256（同样升级为最新汉化版）</summary>
    private const string LegacyUmEmbeddedEnSha256 = "A08DEBF5336C2468038DDF386EA4A8E4BBFE2BD884FC3ADEBF778B8E3E15AE30";

    /// <summary>v2.1.40 内置的第一版汉化 UltimateMenu V3 1.73 的 SHA256（同样升级为最新汉化版）</summary>
    private const string LegacyUmZhV1Sha256 = "5E28B9225BD3BD49A53452E536A93539EACFC427379EE237AFC148A1739C2326";

    /// <summary>v2.1.41 开发期第二版汉化（中间版）的 SHA256（同样升级为最新汉化版）</summary>
    private const string LegacyUmZhV2Sha256 = "468A79F9EB7B1FFC2C1110F64ABAE8C452809BD629F84E3CCA5835608EDB0E5F";

    /// <summary>v2.1.41 开发期第三版汉化（阶段性版本）的 SHA256（同样升级为最新汉化版）</summary>
    private const string LegacyUmZhV3Sha256 = "EB379F9A34DA9B6B048DBFD3E3EBAC7A8062BB259B842FD2D244E82173EA8289";

    /// <summary>文件 SHA256 是否等于指定值（读取失败返回 false）</summary>
    private static bool IsFileSha256(string path, string expectedHex)
    {
        try
        {
            using var sha = System.Security.Cryptography.SHA256.Create();
            using var fs = File.OpenRead(path);
            var hash = Convert.ToHexString(sha.ComputeHash(fs));
            return hash.Equals(expectedHex, StringComparison.OrdinalIgnoreCase);
        }
        catch
        {
            return false;
        }
    }

    /// <summary>
    /// [DOG] 从内嵌资源释放单个文件（目标已存在与否由调用方判断）
    /// </summary>
    private static bool ExtractNamedResFile(Assembly assembly, string resName, string outputFile)
    {
        using var stream = assembly.GetManifestResourceStream(resName);
        if (stream == null)
            return false;

        using var fs = new FileStream(outputFile, FileMode.Create, FileAccess.Write);
        stream.CopyTo(fs);
        return true;
    }

    /// <summary>
    /// 判断文件是否被占用
    /// </summary>
    /// <param name="filePath"></param>
    /// <returns></returns>
    public static bool IsOccupied(string filePath)
    {
        if (!File.Exists(filePath))
            return false;

        FileStream stream = null;

        try
        {
            stream = new FileStream(filePath, FileMode.Open, FileAccess.ReadWrite, FileShare.None);
            return false;
        }
        catch
        {
            return true;
        }
        finally
        {
            stream?.Close();
        }
    }

    /// <summary>
    /// 清空指定文件夹下的文件及文件夹
    /// </summary>
    /// <param name="srcPath">文件夹路径</param>
    public static void ClearDirectory(string srcPath)
    {
        try
        {
            var dir = new DirectoryInfo(srcPath);
            var fileinfo = dir.GetFileSystemInfos();

            foreach (var file in fileinfo)
            {
                if (file is DirectoryInfo)
                {
                    var subdir = new DirectoryInfo(file.FullName);
                    subdir.Delete(true);
                }
                else
                {
                    File.Delete(file.FullName);
                }
            }
        }
        catch { }
    }
}
