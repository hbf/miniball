using System.Diagnostics;
using System.Text;

namespace SEB;

/// <summary>
/// A PointSet that stores <i>n</i> <i>d</i>-dimensional points in a Java array of s
/// </summary>
public class ArrayPointSet : PointSet
{

    int d, n;
    double[] c;

    /// <summary>
    /// Creates an array-based point set to store <i>n</i> <i>d</i>-dimensional points.   
    /// </summary>
    /// <param name="d">the dimensions of the ambient space</param>
    /// <param name="n">the number of points</param>
    public ArrayPointSet(int d, int n)
    {
        this.d = d;
        this.n = n;
        this.c = new double[n * d];
    }

    public int Size => n;

    public int Dimension => d;

    public double Coord(int i, int j)
    {
        Trace.Assert(0 <= i && i < n);
        Trace.Assert(0 <= j && j < d);
        return c[i * d + j];
    }

    /// <summary>
    /// Sets the <i>j</i>th Euclidean coordinate of the <i>i</i>th point to the given value.
    /// </summary>
    /// <param name="i">the number of the point, 0 ≤ i < {@code size()}</param>
    /// <param name="j">the dimension of the coordinate of interest, 0 ≤ j ≤ {@code dimension()}</param>
    /// <param name="v">the value to set as the <i>j</i>th Euclidean coordinate of the <i>i</i>th point</param>
    public void Set(int i, int j, double v)
    {
        Trace.Assert(0 <= i && i < n);
        Trace.Assert(0 <= j && j < d);
        c[i * d + j] = v;
    }

    public override string ToString()
    {
        var sb = new StringBuilder('{');
        for (int i = 0; i < n; ++i)
        {
            sb.Append('[');
            for (int j = 0; j < d; ++j)
            {
                sb.Append(Coord(i, j));
                if (j < d - 1) sb.Append(",");
            }
            sb.Append(']');
            if (i < n - 1) sb.Append(", ");
        }
        sb.Append('}');

        return sb.ToString();
    }

}
