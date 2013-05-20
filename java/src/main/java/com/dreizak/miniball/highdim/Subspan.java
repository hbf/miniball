package com.dreizak.miniball.highdim;

import static com.dreizak.miniball.highdim.Logging.info;
import static com.dreizak.miniball.highdim.Logging.log;
import static java.lang.Math.abs;
import static java.lang.Math.sqrt;

import java.util.BitSet;

import com.dreizak.miniball.model.PointSet;

/**
 * Affine hull of a non-empty set of affinely independent points.
 * <p>
 * An instance of this class represents the affine hull of a non-empty set <i>M</i> of affinely
 * independent points. The set <i>M</i> is not represented explicity; instead, when an instance of
 * this class is constructed, you pass a list <i>S</i> of points to it (which the instance will
 * never change and which is assumed to stay fixed for the lifetime of this instance): The set
 * <i>M</i> is then a subset of <i>S</i>, and its members are identified by their (zero-based)
 * indices in <i>S</i>.
 */
final class Subspan
{
  private final PointSet S;

  // S[i] in M iff memberhsip[i]
  private final BitSet membership;

  // Ambient dimension (not to be confused with the rank r from below)
  private final int dim;

  // members[i] contains the index into S of the i-th point of M.
  // The point members[r] is called the "origin".
  private final int members[];

  // (dim x dim)-matrices Q (orthogonal) and R (upper triangular);
  // notice that Q[j][i] is the element in row i and column j
  private final double[][] Q, R;

  // dim-vectors needed for rank-1 update
  private final double[] u, w;

  // Rank of R (i.e. #points - 1)
  private int r;

  // Used in givens() below, see documentation of the latter.
  private double c, s;

  /**
   * Constructs an instance representing the affine hull <i>aff(M)</i> of <i>M={p}</i>, where
   * <i>p</i> is the point <i>S[k]</i> from <i>S</i>.
   * <p>
   * Notice that <i>S</i> must not change during the lifetime of this instance.
   * 
   * @param dim
   *          the ambient space dimension
   * @param S
   *          the point set
   * @param k
   *          index of the point <i>p</i> in the point set <i>S</i>
   */
  Subspan(int dim, PointSet S, int k)
  {
    this.S = S;
    this.dim = dim;
    this.membership = new BitSet(S.size());
    this.members = new int[dim + 1];
    this.r = 0;

    // Allocate storage for Q, R, u, and w
    Q = new double[dim][];
    R = new double[dim][];
    for (int i = 0; i < dim; ++i)
    {
      Q[i] = new double[dim];
      R[i] = new double[dim];
    }
    u = new double[dim];
    w = new double[dim];

    // Initialize Q to the identity matrix:
    for (int i = 0; i < dim; ++i)
      for (int j = 0; j < dim; ++j)
        Q[j][i] = (i == j) ? 1.0 : 0.0;

    members[r] = k;
    membership.set(k);

    if (log) info("rank: " + r);
  }

  public int dimension()
  {
    return dim;
  }

  /**
   * The size of the instance's set <i>M</i>, a number between 0 and {@code dim+1}.
   * <p>
   * Complexity: O(1).
   * 
   * @return <i>|M|</i>
   */
  public int size()
  {
    return r + 1;
  }

  /**
   * Whether <i>S[i]</i> is a member of <i>M</i>.
   * <p>
   * Complexity: O(1)
   * 
   * @param i
   *          the "global" index into <i>S</i>
   * @return true iff <i>S[i]</i> is a member of <i>M</i>
   */
  public boolean isMember(int i)
  {
    assert 0 <= i && i < S.size();
    return membership.get(i);
  }

  /**
   * The global index (into <i>S</i>) of an arbitrary element of <i>M</i>.
   * <p>
   * Precondition: {@code size()>0}
   * <p>
   * Postcondition: {@code isMember(anyMember())}
   */
  public int anyMember()
  {
    assert size() > 0;
    return members[r];
  }

  /**
   * The index (into <i>S</i>) of the <i>i</i>th point in <i>M</i>. The points in <i>M</i> are
   * internally ordered (in an arbitrary way) and this order only changes when {@link add()} or
   * {@link remove()} is called.
   * <p>
   * Complexity: O(1)
   * 
   * @param i
   *          the "local" index, 0 ≤ i < {@code size()}
   * @return <i>j</i> such that <i>S[j]</i> equals the <i>i</i>th point of M
   */
  public int globalIndex(int i)
  {
    assert 0 <= i && i < size();
    return members[i];
  }

  /**
   * Short-hand for code readability to access element <i>(i,j)</i> of a matrix that is stored in a
   * one-dimensional array.
   * 
   * @param i
   *          zero-based row number
   * @param j
   *          zero-based column number
   * @return the index into the one-dimensional array to get the element at position <i>(i,j)</i> in
   *         the matrix
   */
  private final int ind(int i, int j)
  {
    return i * dim + j;
  }

  /**
   * The point {@code members[r]} is called the <i>origin</i>.
   * 
   * @return index into <i>S</i> of the origin.
   */
  private final int origin()
  {
    return members[r];
  }

  /**
   * Determine the Givens coefficients <i>(c,s)</i> satisfying
   * 
   * <pre>
   * c * a + s * b = +/- (a^2 + b^2) c * b - s * a = 0
   * </pre>
   * 
   * We don't care about the signs here, for efficiency, so make sure not to rely on them anywhere.
   * <p>
   * <i>Source:</i> "Matrix Computations" (2nd edition) by Gene H. B. Golub & Charles F. B. Van Loan
   * (Johns Hopkins University Press, 1989), p. 216.
   * <p>
   * Note that the code of this class sometimes does not call this method but only mentions it in a
   * comment. The reason for this is performance; Java does not allow an efficient way of returning
   * a pair of doubles, so we sometimes manually "inline" {@code givens()} for the sake of
   * performance.
   */
  private final void givens(final double a, final double b)
  {
    if (b == 0.0)
    {
      c = 1.0;
      s = 0.0;
    }
    else if (abs(b) > abs(a))
    {
      final double t = a / b;
      s = 1 / sqrt(1 + t * t);
      c = s * t;
    }
    else
    {
      final double t = b / a;
      c = 1 / sqrt(1 + t * t);
      s = c * t;
    }
  }

  /**
   * Appends the new column <i>u</i> (which is a member field of this instance) to the right of <i>A
   * = QR</i>, updating <i>Q</i> and <i>R</i>. It assumes <i>r</i> to still be the old value, i.e.,
   * the index of the column used now for insertion; <i>r</i> is not altered by this routine and
   * should be changed by the caller afterwards.
   * <p>
   * Precondition: {@code r<dim}
   */
  private void appendColumn()
  {
    assert r < dim;

    // Compute new column R[r] = Q^T * u
    for (int i = 0; i < dim; ++i)
    {
      R[r][i] = 0;
      for (int k = 0; k < dim; ++k)
        R[r][i] += Q[i][k] * u[k];
    }

    // Zero all entries R[r][dim-1] down to R[r][r+1]
    for (int j = dim - 1; j > r; --j)
    {
      // Note: j is the index of the entry to be cleared with the help of entry j-1.

      // Compute Givens coefficients c,s
      givens(R[r][j - 1], R[r][j]); // PERF: inline

      // Rotate one R-entry (the other one is an implicit zero)
      R[r][j - 1] = c * R[r][j - 1] + s * R[r][j];

      // Rotate two Q-columns
      for (int i = 0; i < dim; ++i)
      {
        final double a = Q[j - 1][i];
        final double b = Q[j][i];
        Q[j - 1][i] = c * a + s * b;
        Q[j][i] = c * b - s * a;
      }
    }
  }

  /**
   * Adds the point <i>S[index]</i> to the instance's set <i>M</i>.
   * <p>
   * Precondition: {@code !isMember(index)}
   * <p>
   * Complexity: O(dim^2).
   * 
   * @param index
   *          index into <i>S</i> of the point to add
   */
  public void add(int index)
  {
    assert !isMember(index);

    // Compute S[i] - origin into u
    final int o = origin();
    for (int i = 0; i < dim; ++i)
      u[i] = S.coord(index, i) - S.coord(o, i);

    // Appends new column u to R and updates QR-decomposition (note: routine works with old r)
    appendColumn();

    // move origin index and insert new index:
    membership.set(index);
    members[r + 1] = members[r];
    members[r] = index;
    ++r;

    info("rank: " + r);
  }

  /**
   * Computes the vector <i>w</i> directed from point <i>p</i> to <i>v</i>, where <i>v</i> is the
   * point in <i>aff(M)</i> that lies nearest to <i>p</i>.
   * <p>
   * Precondition: {@code size()}>0
   * <p>
   * Complexity: O(dim^2)
   * 
   * @param p
   *          Euclidean coordinates of point <i>p</i>
   * @param w
   *          the squared length of <i>w</i>
   * @return
   */
  public double shortestVectorToSpan(double[] p, double[] w)
  {
    // Compute vector from p to origin, i.e., w = origin - p
    final int o = origin();
    for (int i = 0; i < dim; ++i)
      w[i] = S.coord(o, i) - p[i];

    // Remove projections of w onto the affine hull
    for (int j = 0; j < r; ++j)
    {
      double scale = 0;
      for (int i = 0; i < dim; ++i)
        scale += w[i] * Q[j][i];
      for (int i = 0; i < dim; ++i)
        w[i] -= scale * Q[j][i];
    }

    double sl = 0;
    for (int i = 0; i < dim; ++i)
      sl += w[i] * w[i];
    return sl;
  }

  /**
   * Use this for testing only; the method allocates additional storage and copies point
   * coordinates.
   */
  public double representationError()
  {
    final double[] lambdas = new double[size()];
    final double[] pt = new double[dim];
    double max = 0, error;

    // Cycle through all points in hull
    for (int j = 0; j < size(); ++j)
    {
      // Get point
      for (int i = 0; i < dim; ++i)
        pt[i] = S.coord(globalIndex(j), i);

      // Compute the affine representation:
      findAffineCoefficients(pt, lambdas);

      // compare coefficient of point j to 1.0
      error = abs(lambdas[j] - 1.0);
      if (error > max) max = error;

      // compare the other coefficients against 0.0
      for (int i = 0; i < j; ++i)
      {
        error = abs(lambdas[i] - 0.0);
        if (error > max) max = error;
      }
      for (int i = j + 1; i < size(); ++i)
      {
        error = abs(lambdas[i] - 0.0);
        if (error > max) max = error;
      }
    }

    return max;
  }

  /**
   * Calculates the {@code size()}-many coefficients in the representation of <i>p</i> as an affine
   * combination of the points <i>M</i>.
   * <p>
   * The <i>i</i>th computed coefficient {@code lambdas[i]} corresponds to the <i>i</i>th point in
   * <i>M</i>, or, in other words, to the point in <i>S</i> with index {@code globalIndex(i)}.
   * <p>
   * Complexity: O(dim^2)
   * <p>
   * Preconditions: c lies in the affine hull aff(M) and size() > 0.
   */
  void findAffineCoefficients(double[] p, double[] lambdas)
  {
    // Compute relative position of p, i.e., u = p - origin
    final int o = origin();
    for (int i = 0; i < dim; ++i)
      u[i] = p[i] - S.coord(o, i);

    // Calculate Q^T u into w
    for (int i = 0; i < dim; ++i)
    {
      w[i] = 0;
      for (int k = 0; k < dim; ++k)
        w[i] += Q[i][k] * u[k];
    }

    // We compute the coefficients by backsubstitution. Notice that
    //
    // c = \sum_{i\in M} \lambda_i (S[i] - origin)
    // = \sum_{i\in M} \lambda_i S[i] + (1-s) origin
    //
    // where s = \sum_{i\in M} \lambda_i.-- We compute the coefficient
    // (1-s) of the origin in the variable origin_lambda:
    double origin_lambda = 1;
    for (int j = r - 1; j >= 0; --j)
    {
      for (int k = j + 1; k < r; ++k)
        w[j] -= lambdas[k] * R[k][j];
      final double lj = w[j] / R[j][j];
      lambdas[j] = lj;
      origin_lambda -= lj;
    }
    // The r-th coefficient corresponds to the origin (see remove()):
    lambdas[r] = origin_lambda;
  }

  /**
   * Given <i>R</i> in lower Hessenberg form with subdiagonal entries 0 to {@code pos-1} already all
   * zero, clears the remaining subdiagonal entries via Givens rotations.
   */
  private void hessenberg_clear(int pos)
  {
    // Clear new subdiagonal entries
    for (; pos < r; ++pos)
    {
      // Note: pos is the column index of the entry to be cleared

      // Compute Givens coefficients c,s
      givens(R[pos][pos], R[pos][pos + 1]); // PERF: inline

      // Rotate partial R-rows (of the first pair, only one entry is
      // needed, the other one is an implicit zero)
      R[pos][pos] = c * R[pos][pos] + s * R[pos][pos + 1];
      // Then begin at position pos+1
      for (int j = pos + 1; j < r; ++j)
      {
        final double a = R[j][pos];
        final double b = R[j][pos + 1];
        R[j][pos] = c * a + s * b;
        R[j][pos + 1] = c * b - s * a;
      }

      // Rotate Q-columns
      for (int i = 0; i < dim; ++i)
      {
        final double a = Q[pos][i];
        final double b = Q[pos + 1][i];
        Q[pos][i] = c * a + s * b;
        Q[pos + 1][i] = c * b - s * a;
      }
    }
  }

  /**
   * Update current QR-decomposition <i>A = QR</i> to
   * 
   * <pre>
   *   A + u * [1,...,1] = Q' R'.
   * </pre>
   */
  private void special_rank_1_update()
  {
    // Compute w = Q^T * u
    for (int i = 0; i < dim; ++i)
    {
      w[i] = 0;
      for (int k = 0; k < dim; ++k)
        w[i] += Q[i][k] * u[k];
    }

    // Rotate w down to a multiple of the first unit vector;
    // the operations have to be recorded in R and Q
    for (int k = dim - 1; k > 0; --k)
    {
      // Note: k is the index of the entry to be cleared with the help of entry k-1.

      // Compute Givens coefficients c,s
      givens(w[k - 1], w[k]);

      // rotate w-entry
      w[k - 1] = c * w[k - 1] + s * w[k];

      // Rotate two R-rows;
      // the first column has to be treated separately
      // in order to account for the implicit zero in R[k-1][k]
      R[k - 1][k] = -s * R[k - 1][k - 1];
      R[k - 1][k - 1] *= c;
      for (int j = k; j < r; ++j)
      {
        final double a = R[j][k - 1];
        final double b = R[j][k];
        R[j][k - 1] = c * a + s * b;
        R[j][k] = c * b - s * a;
      }

      // Rotate two Q-columns
      for (int i = 0; i < dim; ++i)
      {
        final double a = Q[k - 1][i];
        final double b = Q[k][i];
        Q[k - 1][i] = c * a + s * b;
        Q[k][i] = c * b - s * a;
      }
    }

    // Add w * (1,...,1)^T to new R, which means simply to add u[0] to each column
    // since the other entries of u have just been eliminated
    for (int j = 0; j < r; ++j)
      R[j][0] += w[0];

    // Clear subdiagonal entries
    hessenberg_clear(0);
  }

  public void remove(int index)
  {
    assert isMember(globalIndex(index)) && size() > 1;

    membership.clear(globalIndex(index));

    if (index == r)
    {
      // Origin must be deleted.
      final int o = origin();

      // We choose the right-most member of Q, i.e., column r-1 of R,
      // as the new origin. So all relative vectors (i.e., the
      // columns of "A = QR") have to be updated by u:= old origin -
      // S[global_index(r-1)]:
      final int gi = globalIndex(r - 1);
      for (int i = 0; i < dim; ++i)
        u[i] = S.coord(o, i) - S.coord(gi, i);

      --r;

      if (log) info("rank: " + r);

      special_rank_1_update();

    }
    else
    {
      // General case: delete column from R

      // Shift higher columns of R one step to the left
      double[] dummy = R[index];
      for (int j = index + 1; j < r; ++j)
      {
        R[j - 1] = R[j];
        members[j - 1] = members[j];
      }
      members[r - 1] = members[r]; // Shift down origin
      R[--r] = dummy; // Relink trash column

      // Zero out subdiagonal entries in R
      hessenberg_clear(index);
    }
  }
}