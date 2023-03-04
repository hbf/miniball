using System.Collections;
using System.Diagnostics;
using static System.Math;
using static SEB.Logging;

namespace SEB;

/// <summary>
/// Affine hull of a non-empty set of affinely independent points.
/// <br/>
/// An instance of this class represents the affine hull of a non-empty set <i>M</i> of affinely
/// independent points. The set <i>M</i> is not represented explicity; instead, when an instance of
/// this class is constructed, you pass a list <i>S</i> of points to it (which the instance will
/// never change and which is assumed to stay fixed for the lifetime of this instance): The set
/// <i>M</i> is then a subset of <i>S</i>, and its members are identified by their (zero-based)
/// indices in <i>S</i>.
/// </summary>
public class Subspan
{

    PointSet S;

    /// <summary>
    /// S[i] in M iff memberhsip[i]
    /// </summary>
    BitArray membership;

    /// <summary>
    /// Ambient dimension (not to be confused with the rank r from below)
    /// </summary>
    int dim;

    /// <summary>
    /// members[i] contains the index into S of the i-th point of M.
    /// The point members[r] is called the "origin".
    /// </summary>
    int[] members;

    /// <summary>
    /// (dim x dim)-matrices Q (orthogonal) and R (upper triangular);
    /// notice that Q[j][i] is the element in row i and column j
    /// </summary>
    double[][] Q, R;

    /// <summary>
    /// dim-vectors needed for rank-1 update
    /// </summary>
    double[] u, w;

    /// <summary>
    /// Rank of R (i.e. #points - 1)
    /// </summary>
    int r;

    /// <summary>
    /// Used in givens() below, see documentation of the latter.
    /// </summary>
    double c, s;

    /// <summary>
    /// Constructs an instance representing the affine hull <i>aff(M)</i> of <i>M={p}</i>, where
    /// <i>p</i> is the point <i>S[k]</i> from <i>S</i>.
    /// <br/>
    /// Notice that <i>S</i> must not change during the lifetime of this instance.
    /// </summary>
    /// <param name="dim">the ambient space dimension</param>
    /// <param name="S">the point set</param>
    /// <param name="k">index of the point <i>p</i> in the point set <i>S</i></param>
    public Subspan(int dim, PointSet S, int k)
    {
        this.S = S;
        this.dim = dim;
        membership = new BitArray(S.Size);
        members = new int[dim + 1];
        r = 0;

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
        membership.Set(k, true);

        if (log) Info("rank: " + r);
    }

    public int Dimension => dim;

    /// <summary>
    /// The size of the instance's set <i>M</i>, a number between 0 and dim+1.
    /// <br/>
    /// Complexity: O(1).
    /// </summary>
    public int Size => r + 1;

    /// <summary>
    /// Whether <i>S[i]</i> is a member of <i>M</i>.
    /// <br/>
    /// Complexity: O(1)
    /// </summary>
    /// <param name="i">the "global" index into <i>S</i></param>
    /// <returns>true iff <i>S[i]</i> is a member of <i>M</i></returns>
    public bool IsMember(int i)
    {
        Trace.Assert(0 <= i && i < S.Size);
        return membership.Get(i);
    }

    /// <summary>
    /// The global index (into <i>S</i>) of an arbitrary element of <i>M</i>.
    /// <br/>
    /// Precondition: size()>0
    /// <br/>
    /// Postcondition: isMember(anyMember())
    /// </summary>
    /// <returns></returns>
    public int AnyMember()
    {
        Trace.Assert(Size > 0);
        return members[r];
    }

    /// <summary>
    /// The index (into <i>S</i>) of the <i>i</i>th point in <i>M</i>. The points in <i>M</i> are
    /// internally ordered (in an arbitrary way) and this order only changes when add() or remove() is called.
    /// <br/>
    /// Complexity: O(1)
    /// </summary>
    /// <param name="i">the "local" index, 0 ≤ i &lt; size()</param>
    /// <returns><i>j</i> such that <i>S[j]</i> equals the <i>i</i>th point of M</returns>
    public int GlobalIndex(int i)
    {
        Trace.Assert(0 <= i && i < Size);
        return members[i];
    }

    /// <summary>
    /// Short-hand for code readability to access element <i>(i,j)</i> of a matrix that is stored in a
    /// one-dimensional array.
    /// </summary>
    /// <param name="i">zero-based row number</param>
    /// <param name="j">zero-based column number</param>
    /// <returns>the index into the one-dimensional array to get the element at position <i>(i,j)</i> in
    /// the matrix</returns>
    int Ind(int i, int j) => i * dim + j;

    /// <summary>
    /// The point members[r] is called the <i>origin</i>.        
    /// </summary>
    /// <value>
    /// index into <i>S</i> of the origin.
    /// </value>
    int Origin => members[r];

    /// <summary>
    /// Determine the Givens coefficients <i>(c,s)</i> satisfying
    /// <pre>
    /// c * a + s * b = +/- (a^2 + b^2) c * b - s * a = 0
    /// </pre>
    /// We don't care about the signs here, for efficiency, so make sure not to rely on them anywhere.
    /// <br/>
    /// <i>Source:</i> "Matrix Computations" (2nd edition) by Gene H. B. Golub & Charles F. B. Van Loan
    /// (Johns Hopkins University Press, 1989), p. 216.
    /// <br/>
    /// Note that the code of this class sometimes does not call this method but only mentions it in a
    /// comment. The reason for this is performance; Java does not allow an efficient way of returning
    /// a pair of doubles, so we sometimes manually "inline" givens() for the sake of
    /// performance.
    /// </summary>
    /// <param name="a"></param>
    /// <param name="b"></param>
    void Givens(double a, double b)
    {
        if (b == 0.0)
        {
            c = 1.0;
            s = 0.0;
        }
        else if (Abs(b) > Abs(a))
        {
            var t = a / b;
            s = 1 / Sqrt(1 + t * t);
            c = s * t;
        }
        else
        {
            var t = b / a;
            c = 1 / Sqrt(1 + t * t);
            s = c * t;
        }
    }

    /// <summary>
    /// Appends the new column <i>u</i> (which is a member field of this instance) to the right of <i>A
    /// = QR</i>, updating<i> Q</i> and<i> R</i>. It assumes <i>r</i> to still be the old value, i.e.,
    /// the index of the column used now for insertion; <i>r</i> is not altered by this routine and
    /// should be changed by the caller afterwards.
    /// <br/>
    /// Precondition: r&lt;dim
    /// </summary>
    void AppendColumn()
    {
        Trace.Assert(r < dim);

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
            Givens(R[r][j - 1], R[r][j]); // PERF: inline

            // Rotate one R-entry (the other one is an implicit zero)
            R[r][j - 1] = c * R[r][j - 1] + s * R[r][j];

            // Rotate two Q-columns
            for (int i = 0; i < dim; ++i)
            {
                var a = Q[j - 1][i];
                var b = Q[j][i];
                Q[j - 1][i] = c * a + s * b;
                Q[j][i] = c * b - s * a;
            }
        }
    }

    /// <summary>
    /// Adds the point <i>S[index]</i> to the instance's set <i>M</i>.
    /// <br/>
    /// Precondition: !isMember(index)
    /// <br/>
    /// Complexity: O(dim^2).
    /// </summary>
    /// <param name="index">index into <i>S</i> of the point to add</param>
    public void Add(int index)
    {
        Trace.Assert(!IsMember(index));

        // Compute S[i] - origin into u
        var o = Origin;
        for (int i = 0; i < dim; ++i)
            u[i] = S.Coord(index, i) - S.Coord(o, i);

        // Appends new column u to R and updates QR-decomposition (note: routine works with old r)
        AppendColumn();

        // move origin index and insert new index:
        membership.Set(index, true);
        members[r + 1] = members[r];
        members[r] = index;
        ++r;

        Info("rank: " + r);
    }

    /// <summary>
    /// Computes the vector <i>w</i> directed from point <i>p</i> to <i>v</i>, where <i>v</i> is the
    /// point in <i>aff(M)</i> that lies nearest to <i>p</i>.
    /// <br/>
    /// Precondition: size()>0
    /// <br/>
    /// Complexity: O(dim^2)
    /// </summary>
    /// <param name="p">Euclidean coordinates of point <i>p</i></param>
    /// <param name="w">the squared length of <i>w</i></param>
    /// <returns></returns>
    public double ShortestVectorToSpan(double[] p, double[] w)
    {
        // Compute vector from p to origin, i.e., w = origin - p
        var o = Origin;
        for (int i = 0; i < dim; ++i)
            w[i] = S.Coord(o, i) - p[i];

        // Remove projections of w onto the affine hull
        for (int j = 0; j < r; ++j)
        {
            var scale = 0d;
            for (int i = 0; i < dim; ++i)
                scale += w[i] * Q[j][i];
            for (int i = 0; i < dim; ++i)
                w[i] -= scale * Q[j][i];
        }

        var sl = 0d;
        for (int i = 0; i < dim; ++i)
            sl += w[i] * w[i];

        return sl;
    }

    /// <summary>
    /// Use this for testing only; the method allocates additional storage and copies point
    /// coordinates.
    /// </summary>    
    public double RepresentationError()
    {
        var lambdas = new double[Size];
        var pt = new double[dim];
        double max = 0, error;

        // Cycle through all points in hull
        for (int j = 0; j < Size; ++j)
        {
            // Get point
            for (int i = 0; i < dim; ++i)
                pt[i] = S.Coord(GlobalIndex(j), i);

            // Compute the affine representation:
            FindAffineCoefficients(pt, lambdas);

            // compare coefficient of point j to 1.0
            error = Abs(lambdas[j] - 1.0);
            if (error > max) max = error;

            // compare the other coefficients against 0.0
            for (int i = 0; i < j; ++i)
            {
                error = Abs(lambdas[i] - 0.0);
                if (error > max) max = error;
            }
            for (int i = j + 1; i < Size; ++i)
            {
                error = Abs(lambdas[i] - 0.0);
                if (error > max) max = error;
            }
        }

        return max;
    }

    /// <summary>
    /// Calculates the size()}many coefficients in the representation of <i>p</i> as an affine
    /// combination of the points <i>M</i>.
    /// <br/>
    /// The <i>i</i>th computed coefficient lambdas[i] corresponds to the <i>i</i>th point in
    /// <i>M</i>, or, in other words, to the point in <i>S</i> with index globalIndex(i).
    /// <br/>
    /// Complexity: O(dim^2)
    /// <br/>
    /// Preconditions: c lies in the affine hull aff(M) and size() > 0.
    /// </summary>
    /// <param name="p"></param>
    /// <param name="lambdas"></param>
    internal void FindAffineCoefficients(double[] p, double[] lambdas)
    {
        // Compute relative position of p, i.e., u = p - origin
        var o = Origin;
        for (int i = 0; i < dim; ++i)
            u[i] = p[i] - S.Coord(o, i);

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
            var lj = w[j] / R[j][j];
            lambdas[j] = lj;
            origin_lambda -= lj;
        }
        // The r-th coefficient corresponds to the origin (see remove()):
        lambdas[r] = origin_lambda;
    }

    /// <summary>
    /// Given <i>R</i> in lower Hessenberg form with subdiagonal entries 0 to {@code pos-1} already all
    /// zero, clears the remaining subdiagonal entries via Givens rotations.
    /// </summary>
    /// <param name="pos"></param>
    void Hessenberg_clear(int pos)
    {
        // Clear new subdiagonal entries
        for (; pos < r; ++pos)
        {
            // Note: pos is the column index of the entry to be cleared

            // Compute Givens coefficients c,s
            Givens(R[pos][pos], R[pos][pos + 1]); // PERF: inline

            // Rotate partial R-rows (of the first pair, only one entry is
            // needed, the other one is an implicit zero)
            R[pos][pos] = c * R[pos][pos] + s * R[pos][pos + 1];
            // Then begin at position pos+1
            for (int j = pos + 1; j < r; ++j)
            {
                var a = R[j][pos];
                var b = R[j][pos + 1];
                R[j][pos] = c * a + s * b;
                R[j][pos + 1] = c * b - s * a;
            }

            // Rotate Q-columns
            for (int i = 0; i < dim; ++i)
            {
                var a = Q[pos][i];
                var b = Q[pos + 1][i];
                Q[pos][i] = c * a + s * b;
                Q[pos + 1][i] = c * b - s * a;
            }
        }
    }

    /// <summary>
    /// Update current QR-decomposition <i>A = QR</i> to
    /// <pre>
    /// A + u * [1,...,1] = Q' R'.
    /// </pre>
    /// </summary>
    void Special_rank_1_update()
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
            Givens(w[k - 1], w[k]);

            // rotate w-entry
            w[k - 1] = c * w[k - 1] + s * w[k];

            // Rotate two R-rows;
            // the first column has to be treated separately
            // in order to account for the implicit zero in R[k-1][k]
            R[k - 1][k] = -s * R[k - 1][k - 1];
            R[k - 1][k - 1] *= c;
            for (int j = k; j < r; ++j)
            {
                var a = R[j][k - 1];
                var b = R[j][k];
                R[j][k - 1] = c * a + s * b;
                R[j][k] = c * b - s * a;
            }

            // Rotate two Q-columns
            for (int i = 0; i < dim; ++i)
            {
                var a = Q[k - 1][i];
                var b = Q[k][i];
                Q[k - 1][i] = c * a + s * b;
                Q[k][i] = c * b - s * a;
            }
        }

        // Add w * (1,...,1)^T to new R, which means simply to add u[0] to each column
        // since the other entries of u have just been eliminated
        for (int j = 0; j < r; ++j)
            R[j][0] += w[0];

        // Clear subdiagonal entries
        Hessenberg_clear(0);
    }

    public void Remove(int index)
    {
        Trace.Assert(IsMember(GlobalIndex(index)) && Size > 1);

        membership.Set(GlobalIndex(index), false);

        if (index == r)
        {
            // Origin must be deleted.
            var o = Origin;

            // We choose the right-most member of Q, i.e., column r-1 of R,
            // as the new origin. So all relative vectors (i.e., the
            // columns of "A = QR") have to be updated by u:= old origin -
            // S[global_index(r-1)]:
            var gi = GlobalIndex(r - 1);
            for (int i = 0; i < dim; ++i)
                u[i] = S.Coord(o, i) - S.Coord(gi, i);

            --r;

            if (log) Info("rank: " + r);

            Special_rank_1_update();

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
            Hessenberg_clear(index);
        }
    }
}