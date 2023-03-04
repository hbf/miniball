using System.Diagnostics;
using SEB;

using static SEB.PointSetUtils;

namespace example;
class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("====================================================");
        Console.WriteLine("Seb example");

        // Check for right number of arguments ...
        if (args.Length < 2)
        {
            Console.WriteLine($"Usage {AppDomain.CurrentDomain.FriendlyName} number-of-points dimension [boundary]");
            Console.WriteLine("If 'boundary' is given, all points will be on the boundary of a sphere.");
            Console.WriteLine("====================================================");
            Environment.Exit(1);
        }
        Console.WriteLine("====================================================");

        // ... and parse command line arguments
        var n = int.Parse(args[0]);
        var d = int.Parse(args[1]);
        var on_boundary = args.Length > 2 && args[2] == "boundary";

        // Construct n random points in dimension d
        var rnd = new Random();
        var S = RandomPointSet(d, n, rnd, on_boundary);

        var sw = new Stopwatch();

        Console.WriteLine("Starting computation...");
        Console.WriteLine("====================================================");

        sw.Start();

        var mb = new Miniball(S);

        var rad = mb.Radius;
        var rad_squared = mb.SquaredRadius;
        var center = mb.Center;

        sw.Stop();

        // Output
        Console.WriteLine($"Running time: {sw.Elapsed.TotalSeconds}s");
        Console.WriteLine($"Radius = {rad} (squared: {rad_squared})");
        Console.WriteLine("Center:");
        for (int j = 0; j < center.Length; ++j)
            Console.WriteLine($"  {center[j]}");

        Console.WriteLine("====================================================");
        Console.Write(mb.Verify().ConsoleFmt());
        Console.WriteLine("====================================================");
    }
}
