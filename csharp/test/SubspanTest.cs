using SEB;
using static System.Math;

namespace test;

public class SubspanTest
{
    static double Tolerance = 1.0e-15; // TODO

    [Fact]
    public void subspan2x2WithLast()
    {
        // S = [ (1, 2), (5, 2)] in the plane
        var S = new ArrayPointSet(2, 2);
        S.Set(0, 0, 1);
        S.Set(0, 1, 2);
        S.Set(1, 0, 5);
        S.Set(1, 1, 2);

        // Sub-span containing point 1 (i.e., the last one)
        var span = new Subspan(2, S, 1);
        Assert.False(span.IsMember(0));
        Assert.True(span.IsMember(1));
        Assert.Equal(1, span.GlobalIndex(0));
        Assert.Equal(1, span.Size);
        Assert.Equal(0.0, span.RepresentationError());

        // Compute shortest vector to affine hull from a test point
        {
            double[] pt = {
                0, 0
            }, expected = {
                5, 2
            };
            ShortestVectorToHull(span, pt, expected);
        }

        // Add point 0
        span.Add(0);
        Assert.True(span.IsMember(0));
        Assert.True(span.IsMember(1));
        Assert.Equal(0, span.GlobalIndex(0));
        Assert.Equal(1, span.GlobalIndex(1));
        Assert.Equal(2, span.Size);
        Assert.True(span.RepresentationError() <= Tolerance);

        // Compute shortest vector to affine hull from a few test points
        {
            double[] pt = {
            0, 0
            }, expected = {
                0, 2
            };
            ShortestVectorToHull(span, pt, expected);
        }
        {
            double[] pt = {
                4, 1
            }, expected = {
                0, 1
            };
            ShortestVectorToHull(span, pt, expected);
        }
        {
            double[] pt = {
                4, 2
            }, expected = {
                0, 0
            };
            ShortestVectorToHull(span, pt, expected);
        }
    }

    static void ShortestVectorToHull(Subspan span, double[] pt, double[] expected)
    {
        var sv = new double[span.Dimension];
        span.ShortestVectorToSpan(pt, sv);
        for (int i = 0; i < span.Dimension; ++i)
            Assert.True(Abs(expected[i] - sv[i]) <= Tolerance);
    }
}