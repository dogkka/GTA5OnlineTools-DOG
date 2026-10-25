using Forms = System.Windows.Forms;

namespace WinFormLib;

public static class SendKeys
{
    public static void Flush()
    {
        Forms.SendKeys.Flush();
    }

    public static void SendWait(string msg)
    {
        Forms.SendKeys.SendWait(msg);
    }
}
