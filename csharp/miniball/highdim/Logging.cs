namespace SEB;

/// <summary>
/// A very simple logging class used for debugging.
/// </summary>
static class Logging
{

    public static bool log = false;

    public static void Warn(string msg)
    {
        if (log) System.Console.WriteLine($"[warn] {msg}");
    }

    public static void Info(string msg)
    {
        if (log) System.Console.WriteLine($"[info] {msg}");
    }

    public static void Debug(string msg)
    {
        if (log) System.Console.WriteLine($"[debug] {msg}");
    }

}