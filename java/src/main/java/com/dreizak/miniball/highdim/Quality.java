package com.dreizak.miniball.highdim;

/**
 * Information about the quality of the computed ball.
 */
public final class Quality
{
  private final double qrInconsistency;
  private final double minConvexCoefficient;
  private final double maxOverlength;
  private final double maxUnderlength;
  private final int iterations;
  private final int supportSize;

  Quality(double qrInconsistency, double minConvexCoefficient, double maxOverlength, double maxUnderlength,
      int iterations, int supportSize)
  {
    super();
    this.qrInconsistency = qrInconsistency;
    this.minConvexCoefficient = minConvexCoefficient;
    this.maxOverlength = maxOverlength;
    this.maxUnderlength = maxUnderlength;
    this.iterations = iterations;
    this.supportSize = supportSize;
  }

  /**
   * A measure for the quality of the internally used support points.
   * <p>
   * The returned number should in theory be zero (but may be non-zero due to rounding errors).
   */
  public final double getQrInconsistency()
  {
    return qrInconsistency;
  }

  /**
   * A measure for the minimality of the computed ball.
   * 
   * The returned number should in theory be non-zero and positive. Due to rounding errors, it may
   * be negative.
   */
  public final double getMinConvexCoefficient()
  {
    return minConvexCoefficient;
  }

  /**
   * The maximal over-length of a point from the input set, relative to the computed miniball's
   * radius.
   * <p>
   * For each point <i>p</i> from the input point set, it is computed how far it is <i>outside</i>
   * the miniball ("over-length"). The returned number is the maximal such over-length, divided by
   * the radius of the computed miniball.
   * <p>
   * Notice that {@code getMaxOverlength() == 0} if and only if all points are contained in the
   * miniball.
   * 
   * @return the maximal over-length, a number ≥ 0
   */
  public final double getMaxOverlength()
  {
    return maxOverlength;
  }

  /**
   * The maximal under-length of a point from the input set, relative to the computed miniball's
   * radius.
   * <p>
   * For each point <i>p</i> from the input point set, it is computed how far one has to walk from
   * this point towards the boundary of the miniball ("under-length"). The returned number is the
   * maximal such under-length, divided by the radius of the computed miniball.
   * <p>
   * Notice that in theory {@code getMaxUnderlength()} should be zero, otherwise the computed
   * miniball is enclosing but not minimal.
   * 
   * @return the maximal under-length, a number ≥ 0
   */
  public final double getMaxUnderlength()
  {
    return maxUnderlength;
  }

  /**
   * The number of iterations that the algorithm needed to compute the miniball.
   * 
   * @return number of iterations
   */
  public final int getIterations()
  {
    return iterations;
  }

  /**
   * The size of the support.
   * <p>
   * Refer to the documentation of {@link Miniball#support()} for more information on the
   * <i>support</i>.
   * 
   * @return size of the support
   */
  public final int getSupportSize()
  {
    return supportSize;
  }

  @Override
  public String toString()
  {
    return "Quality [qrInconsistency="
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
}