package com.dreizak.miniball.highdim;

import static com.dreizak.miniball.highdim.Logging.debug;
import static com.dreizak.miniball.highdim.Logging.info;
import static com.dreizak.miniball.highdim.Logging.log;
import static java.lang.Math.sqrt;

import java.util.List;

import com.dreizak.miniball.model.ArrayPointSet;
import com.dreizak.miniball.model.PointSet;

/**
 * The smallest enclosing ball (a.k.a. <i>miniball</i>) of a set of points.
 * <p>
 * The miniball <i>MB(P)</i> of a non-empty set <i>P</i> of points in <i>d</i>-dimensional Euclidean
 * space <i>R^d</i> is defined to be the smallest ball that contains all points from <i>P</i>.
 * <p>
 * In order to compute the miniball of a given point set, you create a {@link PointSet} instance,
 * {@code pts}, say, that provides the algorithm access to the Euclidean coordinates of the points.
 * You then pass this instance to the constructor {@link #Miniball(PointSet)}. The point set may be
 * empty (i.e., {@code pts.size() == 0}), in which case {@link #isEmpty()} will return true.
 * <p>
 * Instances of class {@link Miniball} are immutable. That is, when you create a {@link Miniball},
 * passing your point set to it, it computes the center and radius of the miniball and allows you to
 * access this information via {@link #radius()}, {@link #squaredRadius()}, and {@link #center()}.
 * However the {@link Miniball} instance will not reflect any subsequent changes you make to the
 * underlying point set.
 * 
 * @see ArrayPointSet
 */
public class Miniball
{
  private final static double Eps = 1e-14;

  private final PointSet S;
  private final int size;
  private final int dim;
  private int iteration;
  private final double[] center, centerToAff, centerToPoint, lambdas;
  private double distToAff, distToAffSquare;
  private double squaredRadius, radius;
  private final Subspan support;
  private int stopper;

  /**
   * Computes the miniball of the given point set.
   * 
   * Notice that the point set {@code pts} is assumed to be immutable during the computation. That
   * is, if you add, remove, or change points in the point set, you have to create a new instance of
   * {@link Miniball}.
   * 
   * @param pts
   *          the point set
   */
  public Miniball(PointSet pts)
  {
    S = pts;
    size = S.size();
    dim = S.dimension();
    center = new double[dim];
    centerToAff = new double[dim];
    centerToPoint = new double[dim];
    lambdas = new double[dim + 1];
    support = initBall();
    compute();
  }

  /**
   * Whether or not the miniball is the empty set, equivalently, whether {@code points.size() == 0}
   * was true when this miniball instance was constructed.
   * 
   * Notice that the miniball of a point set <i>S</i> is empty if and only if <i>S={}</i>.
   * 
   * @return true iff
   */
  public boolean isEmpty()
  {
    return size == 0;
  }

  /**
   * The radius of the miniball.
   * <p>
   * Precondition: {@code !isEmpty()}
   * 
   * @return the radius of the miniball, a number ≥ 0
   */
  public double radius()
  {
    return radius;
  }

  /**
   * The squared radius of the miniball.
   * <p>
   * This is equivalent to {@code radius() * radius()}.
   * <p>
   * Precondition: {@code !isEmpty()}
   * 
   * @return the squared radius of the miniball
   */
  public double squaredRadius()
  {
    return squaredRadius;
  }

  /**
   * The Euclidean coordinates of the center of the miniball.
   * <p>
   * Precondition: {@code !isEmpty()}
   * 
   * @return an array holding the coordinates of the center of the miniball
   */
  public double[] center()
  {
    return center;
  }

  /**
   * The number of input points.
   * 
   * @return the number of points in the original point set, i.e., {@code pts.size()} where
   *         {@code pts} was the {@link PointSet} instance passed to the constructor of this
   *         instance
   */
  public int size()
  {
    return size;
  }

  /**
   * TODO
   * 
   * @return
   */
  public List<Integer> support()
  {
    throw new RuntimeException("Not implemented yet.");
  }

  private static double sqr(double x)
  {
    return x * x;
  }

  /**
   * Sets up the search ball with an arbitrary point of <i>S</i> as center and with exactly one of
   * the points farthest from center in the support. So the current ball contains all points of
   * <i>S</i> and has radius at most twice as large as the minball.
   * <p>
   * Precondition: {@code size > 0}
   */
  private Subspan initBall()
  {
    assert size > 0;

    // Set center to the first point in S
    for (int i = 0; i < dim; ++i)
      center[i] = S.coord(0, i);

    // Find farthest point
    squaredRadius = 0;
    int farthest = 0;
    for (int j = 1; j < S.size(); ++j)
    {
      // Compute squared distance from center to S[j]
      double dist = 0;
      for (int i = 0; i < dim; ++i)
        dist += sqr(S.coord(j, i) - center[i]);

      // enlarge radius if needed:
      if (dist >= squaredRadius)
      {
        squaredRadius = dist;
        farthest = j;
      }
    }
    radius = Math.sqrt(squaredRadius);

    // Initialize support to the farthest point:
    return new Subspan(dim, S, farthest);
  }

  private void computeDistToAff()
  {
    distToAffSquare = support.shortestVectorToSpan(center, centerToAff);
    distToAff = Math.sqrt(distToAffSquare);
  }

  private void updateRadius()
  {
    final int any = support.anyMember();
    squaredRadius = 0;
    for (int i = 0; i < dim; ++i)
      squaredRadius += sqr(S.coord(any, i) - center[i]);
    radius = sqrt(squaredRadius);
    if (log) debug("current radius = " + radius);
  }

  /**
   * The main function containing the main loop.
   * <p>
   * Iteratively, we compute the point in support that is closest to the current center and then
   * walk towards this target as far as we can, i.e., we move until some new point touches the
   * boundary of the ball and must thus be inserted into support. In each of these two alternating
   * phases, we always have to check whether some point must be dropped from support, which is the
   * case when the center lies in <i>aff(support)</i>. If such an attempt to drop fails, we are
   * done; because then the center lies even <i>conv(support)</i>.
   */
  private void compute()
  {
    // Invariant: The ball B(center,radius) always contains the whole
    // point set S and has the points in support on its boundary.
    while (true)
    {
      ++iteration;

      if (log)
      {
        debug("Iteration " + iteration);
        debug(support.size() + " points on the boundary");
      }

      // Compute a walking direction and walking vector,
      // and check if the former is perhaps too small:
      computeDistToAff();
      while (distToAff <= Eps * radius ||
      /*
       * Note: the following line is currently needed because of point sets like schnartz, see
       * MiniballTest.
       */
      support.size() == dim + 1)
      {
        // We are closer than Eps * radius_square, so we try a drop
        if (!successfulDrop())
        {
          // If that is not possible, the center lies in the convex hull
          // and we are done.
          if (log) info("Done");
          return;
        }
        computeDistToAff();
      }
      // if (log) debug("distance to affine hull = " + distToAff);

      // Determine how far we can walk in direction centerToAff
      // without losing any point ('stopper', say) in S:
      final double scale = findStopFraction();

      // Stopping point exists
      if (stopper >= 0)
      {
        // Walk as far as we can
        for (int i = 0; i < dim; ++i)
          center[i] += scale * centerToAff[i];

        updateRadius();

        // and add stopper to support
        support.add(stopper);
        // if (log) debug("adding global point #" + stopper);

        // No obstacle on our way into the affine hull
      }
      else
      {
        for (int i = 0; i < dim; ++i)
          center[i] += centerToAff[i];

        updateRadius();

        // Theoretically, the distance to the affine hull is now zero
        // and we would thus drop a point in the next iteration.
        // For numerical stability, we don't rely on that to happen but
        // try to drop a point right now:
        if (!successfulDrop())
        {
          // Drop failed, so the center lies in conv(support) and is thus optimal.
          return;
        }
      }
    }
  }

  /**
   * If center doesn't already lie in <i>conv(support)</i> and is thus not optimal yet,
   * {@link #successfulDrop()} elects a suitable point <i>k</i> to be removed from the support and
   * returns true. If the center lies in the convex hull, however, false is returned (and the
   * support remains unaltered).
   * <p>
   * Precondition: center lies in <i>aff(support)</i>.
   */
  boolean successfulDrop()
  {
    // Find coefficients of the affine combination of center
    support.findAffineCoefficients(center, lambdas);

    // find a non-positive coefficient
    int smallest = 0;
    double minimum = 1;
    for (int i = 0; i < support.size(); ++i)
      if (lambdas[i] < minimum)
      {
        minimum = lambdas[i];
        smallest = i;
      }

    // Drop a point with non-positive coefficient, if any
    if (minimum <= 0)
    {
      // if (log) debug("removing local point #" + smallest);
      support.remove(smallest);
      return true;
    }
    return false;
  }

  /**
   * Given the center of the current enclosing ball and the walking direction {@code centerToAff},
   * determine how much we can walk into this direction without losing a point from <i>S</i>. The
   * (positive) factor by which we can walk along {@code centerToAff} is returned. Further,
   * {@code stopper} is set to the index of the most restricting point and to -1 if no such point
   * was found.
   */
  private double findStopFraction()
  {
    // We would like to walk the full length of centerToAff ...
    double scale = 1;
    stopper = -1;

    // ... but one of the points in S might hinder us
    for (int j = 0; j < size; ++j)
      if (!support.isMember(j))
      {
        // Compute vector centerToPoint from center to the point S[j]:
        for (int i = 0; i < dim; ++i)
          centerToPoint[i] = S.coord(j, i) - center[i];

        double dirPointProd = 0;
        for (int i = 0; i < dim; ++i)
          dirPointProd += centerToAff[i] * centerToPoint[i];

        // We can ignore points beyond support since they stay enclosed anyway
        if (distToAffSquare - dirPointProd < Eps * radius * distToAff) continue;

        // Compute the fraction we can walk along centerToAff until
        // we hit point S[i] on the boundary.
        // (Better don't try to understand this calculus from the code,
        // it needs some pencil-and-paper work.)
        double bound = 0;
        for (int i = 0; i < dim; ++i)
          bound += centerToPoint[i] * centerToPoint[i];
        bound = (squaredRadius - bound) / 2 / (distToAffSquare - dirPointProd);

        // Take the smallest fraction
        if (bound < scale)
        {
          if (log) debug("found stopper " + j + " bound=" + bound + " scale=" + scale);
          scale = bound;
          stopper = j;
        }
      }

    return scale;
  }

  /**
   * Verifies that the computed ball is indeed the miniball.
   * <p>
   * This method should be called for testing purposes only; it may not be very efficient.
   */
  public Quality verify()
  {
    double min_lambda = 1; // for center-in-convex-hull check
    double max_overlength = 0; // for all-points-in-ball check
    double min_underlength = 0; // for all-boundary-points-on-boundary
    double ball_error;
    double qr_error = support.representationError();

    // Center really in convex hull?
    support.findAffineCoefficients(center, lambdas);
    for (int k = 0; k < support.size(); ++k)
      if (lambdas[k] <= min_lambda) min_lambda = lambdas[k];

    // All points in ball, all support points really on boundary?
    for (int k = 0; k < S.size(); ++k)
    {

      // Compare center-to-point distance with radius
      for (int i = 0; i < dim; ++i)
        centerToPoint[i] = S.coord(k, i) - center[i];
      double sqDist = 0;
      for (int i = 0; i < dim; ++i)
        sqDist += sqr(centerToPoint[i]);
      ball_error = Math.sqrt(sqDist) - radius;

      // Check for sphere violations
      if (ball_error > max_overlength) max_overlength = ball_error;

      // check for boundary violations
      if (support.isMember(k)) if (ball_error < min_underlength) min_underlength = ball_error;
    }

    return new Quality(qr_error, min_lambda, max_overlength / radius, Math.abs(min_underlength / radius), iteration,
        support.size());
  }

  /**
   * Outputs information about the miniball; this includes the quality information provided by
   * {@link #verify()} (and as a consequence, {@link #toString()} is expensive to call).
   */
  @Override
  public String toString()
  {
    StringBuilder s = new StringBuilder("Miniball [");
    if (isEmpty())
    {
      s.append("isEmpty=true");
    }
    else
    {
      s.append("center=(");
      for (int i = 0; i < dim; ++i)
      {
        s.append(center[i]);
        if (i < dim - 1) s.append(", ");
      }
      s.append("), radius=")
          .append(radius)
          .append(", squaredRadius=")
          .append(squaredRadius)
          .append(", quality=")
          .append(verify());
    }
    return s.append("]").toString();
  }
}
