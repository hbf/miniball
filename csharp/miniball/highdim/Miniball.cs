using System.Diagnostics;
using System.Text;
using static System.Math;
using static SEB.Logging;

namespace SEB;

/// <summary>
/// The smallest enclosing ball (a.k.a. <i>miniball</i>) of a set of points.
/// <br/>
/// The miniball <i>MB(P)</i> of a non-empty set <i>P</i> of points in <i>d</i>-dimensional Euclidean
/// space <i>R^d</i> is defined to be the smallest ball that contains all points from <i>P</i>.
/// <br/>
/// In order to compute the miniball of a given point set, you create a PointSet instance,
/// pts, say, that provides the algorithm access to the Euclidean coordinates of the points.
/// You then pass this instance to the constructor #Miniball(PointSet). The point set may be
/// empty (i.e., pts.size() == 0}, in which case #isEmpty() will return true.
/// <br/>
/// Instances of class Miniball are immutable. That is, when you create a Miniball,
/// passing your point set to it, it computes the center and radius of the miniball and allows you to
/// access this information via #radius(), #squaredRadius(), and #center().
/// However the Miniball instance will not reflect any subsequent changes you make to the
/// underlying point set.
/// </summary>
public class Miniball
{

    static double Eps = 1e-14;

    PointSet S;
    int size;
    int dim;
    int iteration;
    double[] center, centerToAff, centerToPoint, lambdas;
    double distToAff, distToAffSquare;
    double squaredRadius, radius;
    Subspan support;
    int stopper;

    /// <summary>
    /// Computes the miniball of the given point set.
    /// 
    /// Notice that the point set {@code pts} is assumed to be immutable during the computation. That
    /// is, if you add, remove, or change points in the point set, you have to create a new instance of Miniball.
    /// </summary>
    /// <param name="pts">the point set</param>
    public Miniball(PointSet pts)
    {
        S = pts;
        size = S.Size;
        dim = S.Dimension;
        center = new double[dim];
        centerToAff = new double[dim];
        centerToPoint = new double[dim];
        lambdas = new double[dim + 1];
        support = InitBall();
        Compute();
    }

    /// <summary>
    /// Whether or not the miniball is the empty set, equivalently, whether points.size() == 0
    /// was true when this miniball instance was constructed.
    /// 
    /// Notice that the miniball of a point set <i>S</i> is empty if and only if <i>S={}</i>.
    /// </summary>
    public bool IsEmpty => size == 0;

    /// <summary>
    /// The radius of the miniball.
    /// <br/>
    /// Precondition: !isEmpty()
    /// </summary>
    /// <value>the radius of the miniball, a number ≥ 0</value>
    public double Radius => radius;

    /// <summary>
    /// The squared radius of the miniball.
    /// <br/>
    /// This is equivalent to {@code radius() * radius()}.
    /// <br/>
    /// Precondition: !isEmpty()
    /// </summary>
    /// <value>the squared radius of the miniball</value>
    public double SquaredRadius => squaredRadius;

    /// <summary>
    /// The Euclidean coordinates of the center of the miniball.
    /// <br/>
    /// Precondition: !isEmpty()
    /// </summary>
    /// <value>an array holding the coordinates of the center of the miniball</value>
    public double[] Center => center;

    /// <summary>
    /// The number of input points.
    /// </summary>
    /// <value>the number of points in the original point set, i.e., pts.size() where
    /// pts was the PointSet instance passed to the constructor of this instance</value>
    public int Size => size;

    /// <summary>
    /// TODO
    /// </summary>    
    public List<int> Support()
    {
        throw new NotImplementedException();
    }

    static double sqr(double x) => x * x;

    /// <summary>
    /// Sets up the search ball with an arbitrary point of <i>S</i> as center and with exactly one of
    /// the points farthest from center in the support. So the current ball contains all points of
    /// <i>S</i> and has radius at most twice as large as the minball.
    /// <br/>
    /// Precondition: size > 0
    /// </summary>    
    Subspan InitBall()
    {
        Trace.Assert(Size > 0);

        // Set center to the first point in S
        for (int i = 0; i < dim; ++i)
            center[i] = S.Coord(0, i);

        // Find farthest point
        squaredRadius = 0;
        var farthest = 0;
        for (int j = 1; j < S.Size; ++j)
        {
            // Compute squared distance from center to S[j]
            var dist = 0d;
            for (int i = 0; i < dim; ++i)
                dist += sqr(S.Coord(j, i) - center[i]);

            // enlarge radius if needed:
            if (dist >= squaredRadius)
            {
                squaredRadius = dist;
                farthest = j;
            }
        }
        radius = Sqrt(squaredRadius);

        // Initialize support to the farthest point:
        return new Subspan(dim, S, farthest);
    }

    void ComputeDistToAff()
    {
        distToAffSquare = support.ShortestVectorToSpan(center, centerToAff);
        distToAff = Sqrt(distToAffSquare);
    }

    void UpdateRadius()
    {
        var any = support.AnyMember();
        squaredRadius = 0;
        for (int i = 0; i < dim; ++i)
            squaredRadius += sqr(S.Coord(any, i) - center[i]);
        radius = Sqrt(squaredRadius);
        if (log) Debug("current radius = " + radius);
    }

    /// <summary>
    /// The main function containing the main loop.
    /// <br/>
    /// Iteratively, we compute the point in support that is closest to the current center and then
    /// walk towards this target as far as we can, i.e., we move until some new point touches the
    /// boundary of the ball and must thus be inserted into support. In each of these two alternating
    /// phases, we always have to check whether some point must be dropped from support, which is the
    /// case when the center lies in <i>aff(support)</i>. If such an attempt to drop fails, we are
    /// done; because then the center lies even <i>conv(support)</i>.
    /// </summary>
    void Compute()
    {
        // Invariant: The ball B(center,radius) always contains the whole
        // point set S and has the points in support on its boundary.
        while (true)
        {
            ++iteration;

            if (log)
            {
                Debug("Iteration " + iteration);
                Debug(support.Size + " points on the boundary");
            }

            // Compute a walking direction and walking vector,
            // and check if the former is perhaps too small:
            ComputeDistToAff();
            while (distToAff <= Eps * radius ||
            /*
             * Note: the following line is currently needed because of point sets like schnartz, see
             * MiniballTest.
             */
            support.Size == dim + 1)
            {
                // We are closer than Eps * radius_square, so we try a drop
                if (!SuccessfulDrop())
                {
                    // If that is not possible, the center lies in the convex hull
                    // and we are done.
                    if (log) Info("Done");
                    return;
                }
                ComputeDistToAff();
            }
            // if (log) debug("distance to affine hull = " + distToAff);

            // Determine how far we can walk in direction centerToAff
            // without losing any point ('stopper', say) in S:
            var scale = FindStopFraction();

            // Stopping point exists
            if (stopper >= 0)
            {
                // Walk as far as we can
                for (int i = 0; i < dim; ++i)
                    center[i] += scale * centerToAff[i];

                UpdateRadius();

                // and add stopper to support
                support.Add(stopper);
                // if (log) debug("adding global point #" + stopper);

                // No obstacle on our way into the affine hull
            }
            else
            {
                for (int i = 0; i < dim; ++i)
                    center[i] += centerToAff[i];

                UpdateRadius();

                // Theoretically, the distance to the affine hull is now zero
                // and we would thus drop a point in the next iteration.
                // For numerical stability, we don't rely on that to happen but
                // try to drop a point right now:
                if (!SuccessfulDrop())
                {
                    // Drop failed, so the center lies in conv(support) and is thus optimal.
                    return;
                }
            }
        }
    }

    /// <summary>
    /// If center doesn't already lie in <i>conv(support)</i> and is thus not optimal yet,
    /// #successfulDrop() elects a suitable point <i>k</i> to be removed from the support and
    /// returns true. If the center lies in the convex hull, however, false is returned (and the
    /// support remains unaltered).
    /// <br/>
    /// Precondition: center lies in <i>aff(support)</i>.
    /// </summary>
    bool SuccessfulDrop()
    {
        // Find coefficients of the affine combination of center
        support.FindAffineCoefficients(center, lambdas);

        // find a non-positive coefficient
        int smallest = 0;
        double minimum = 1;
        for (int i = 0; i < support.Size; ++i)
            if (lambdas[i] < minimum)
            {
                minimum = lambdas[i];
                smallest = i;
            }

        // Drop a point with non-positive coefficient, if any
        if (minimum <= 0)
        {
            // if (log) debug("removing local point #" + smallest);
            support.Remove(smallest);
            return true;
        }
        return false;
    }

    /// <summary>
    /// Given the center of the current enclosing ball and the walking direction centerToAff,
    /// determine how much we can walk into this direction without losing a point from <i>S</i>. The
    /// (positive) factor by which we can walk along centerToAff is returned. Further,
    /// stopper is set to the index of the most restricting point and to -1 if no such point was found.
    /// </summary>    
    double FindStopFraction()
    {
        // We would like to walk the full length of centerToAff ...
        var scale = 1d;
        stopper = -1;

        // ... but one of the points in S might hinder us
        for (int j = 0; j < size; ++j)
            if (!support.IsMember(j))
            {
                // Compute vector centerToPoint from center to the point S[j]:
                for (int i = 0; i < dim; ++i)
                    centerToPoint[i] = S.Coord(j, i) - center[i];

                var dirPointProd = 0d;
                for (int i = 0; i < dim; ++i)
                    dirPointProd += centerToAff[i] * centerToPoint[i];

                // We can ignore points beyond support since they stay enclosed anyway
                if (distToAffSquare - dirPointProd < Eps * radius * distToAff) continue;

                // Compute the fraction we can walk along centerToAff until
                // we hit point S[i] on the boundary.
                // (Better don't try to understand this calculus from the code,
                // it needs some pencil-and-paper work.)
                var bound = 0d;
                for (int i = 0; i < dim; ++i)
                    bound += centerToPoint[i] * centerToPoint[i];
                bound = (squaredRadius - bound) / 2 / (distToAffSquare - dirPointProd);

                // Take the smallest fraction
                if (bound > 0 && bound < scale)
                {
                    if (log) Debug("found stopper " + j + " bound=" + bound + " scale=" + scale);
                    scale = bound;
                    stopper = j;
                }
            }

        return scale;
    }

    /// <summary>
    /// Verifies that the computed ball is indeed the miniball.
    /// <br/>
    /// This method should be called for testing purposes only; it may not be very efficient.
    /// </summary>
    public Quality Verify()
    {
        var min_lambda = 1d; // for center-in-convex-hull check
        var max_overlength = 0d; // for all-points-in-ball check
        var min_underlength = 0d; // for all-boundary-points-on-boundary
        double ball_error;
        var qr_error = support.RepresentationError();

        // Center really in convex hull?
        support.FindAffineCoefficients(center, lambdas);
        for (int k = 0; k < support.Size; ++k)
            if (lambdas[k] <= min_lambda) min_lambda = lambdas[k];

        // All points in ball, all support points really on boundary?
        for (int k = 0; k < S.Size; ++k)
        {

            // Compare center-to-point distance with radius
            for (int i = 0; i < dim; ++i)
                centerToPoint[i] = S.Coord(k, i) - center[i];
            double sqDist = 0;
            for (int i = 0; i < dim; ++i)
                sqDist += sqr(centerToPoint[i]);
            ball_error = Sqrt(sqDist) - radius;

            // Check for sphere violations
            if (ball_error > max_overlength) max_overlength = ball_error;

            // check for boundary violations
            if (support.IsMember(k)) if (ball_error < min_underlength) min_underlength = ball_error;
        }

        return new Quality(qr_error, min_lambda, max_overlength / radius, Abs(min_underlength / radius), iteration,
            support.Size);
    }

    /// <summary>
    /// Outputs information about the miniball; this includes the quality information provided by
    /// #verify() (and as a consequence, #toString() is expensive to call).
    /// </summary>
    public override string ToString()
    {
        var sb = new StringBuilder("Miniball [");

        if (IsEmpty)
        {
            sb.Append("isEmpty=true");
        }
        else
        {
            sb.Append("center=(");
            for (int i = 0; i < dim; ++i)
            {
                sb.Append(center[i]);
                if (i < dim - 1) sb.Append(", ");
            }
            sb
                .Append("), radius=")
                .Append(radius)
                .Append(", squaredRadius=")
                .Append(squaredRadius)
                .Append(", quality=")
                .Append(Verify());
        }
        sb.Append("]");

        return sb.ToString();
    }

}