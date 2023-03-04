namespace SEB;

/// <summary>
/// Information about the quality of the computed ball.
/// </summary>
public class Quality
{

    /// <summary>
    /// A measure for the quality of the internally used support points.
    /// <br/>
    /// The returned number should in theory be zero (but may be non-zero due to rounding errors).
    /// </summary>
    public double qrInconsistency { get; private set; }

    /// <summary>
    /// A measure for the minimality of the computed ball.
    /// The returned number should in theory be non-zero and positive. Due to rounding errors, it may
    /// be negative.
    /// </summary>    
    public double minConvexCoefficient { get; private set; }

    /// <summary>
    /// The maximal over-length of a point from the input set, relative to the computed miniball's radius.
    /// <br/>
    /// For each point <i>p</i> from the input point set, it is computed how far it is <i>outside</i>
    /// the miniball ("over-length"). The returned number is the maximal such over-length, divided by
    /// the radius of the computed miniball.
    /// <br/>
    /// Notice that getMaxOverlength() == 0 if and only if all points are contained in the miniball.
    /// </summary>
    /// <value>the maximal over-length, a number ≥ 0</value>
    public double maxOverlength { get; private set; }

    /// <summary>
    /// The maximal under-length of a point from the input set, relative to the computed miniball's radius.
    /// <br/>
    /// For each point <i>p</i> from the input point set, it is computed how far one has to walk from
    /// this point towards the boundary of the miniball ("under-length"). The returned number is the
    /// maximal such under-length, divided by the radius of the computed miniball.
    /// <br/>
    /// Notice that in theory getMaxUnderlength() should be zero, otherwise the computed
    /// miniball is enclosing but not minimal.
    /// </summary>
    /// <value>the maximal under-length, a number ≥ 0</value>
    public double maxUnderlength { get; private set; }

    /// <summary>
    /// The number of iterations that the algorithm needed to compute the miniball.
    /// </summary>
    /// <value>number of iterations</value>
    public int iterations { get; private set; }

    /// <summary>
    /// The size of the support.
    /// <br/>
    /// Refer to the documentation of {@link Miniball#support()} for more information on the
    /// <i>support</i>.
    /// </summary>
    /// <value>size of the support</value>
    public int supportSize { get; private set; }

    public Quality(double qrInconsistency, double minConvexCoefficient, double maxOverlength, double maxUnderlength,
      int iterations, int supportSize)
    {
        this.qrInconsistency = qrInconsistency;
        this.minConvexCoefficient = minConvexCoefficient;
        this.maxOverlength = maxOverlength;
        this.maxUnderlength = maxUnderlength;
        this.iterations = iterations;
        this.supportSize = supportSize;
    }

    public override string ToString() => "Quality [qrInconsistency="
        + qrInconsistency
        + ", minConvexCoefficient="
        + minConvexCoefficient
        + ", maxOverlength="
        + maxOverlength
        + ", maxUnderlength="
        + maxUnderlength
        + ", iterations="
        + iterations
        + ", supportSize="
        + supportSize
        + "]";

}