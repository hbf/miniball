namespace SEB;

/// <summary>
/// Abstraction to access the points and their Euclidean coordinates of a set of <i>n</i> points.
/// Classes like {@link Miniball} do not take their input point set as a Java {@link List} are
/// similar data structure, as this forces the user to provide the points in a certain format.
/// Instead, the algorithms require an {@link PointSet} that allows the necessary characteristics of
/// the points to be queried.
/// <p>
/// Notice that most algorithms that use {@link PointSet}s will assume that the underlying point set
/// is <i>immutable</i>.
/// </p>
/// For optimal performance, you may want to copy your points to a Java array and use a ArrayPointSet.
/// </summary>
public interface PointSet
{
    /// <summary>
    /// Number of points.
    /// </summary>
    /// <value>the number of points in the point set.</value>
    int Size { get; }

    /// <summary>
    /// The dimension of the ambient space of the points.
    /// <br/>
    /// Each point has dimension() many Euclidean coordinates.
    /// </summary>
    /// <value>the dimension of the ambient space</value>
    int Dimension { get; }

    /// <summary>
    /// The <i>j</i>th Euclidean coordinate of the <i>i</i>th point.
    /// </summary>    
    /// <param name="i">the number of the point, 0 ≤ i &lt; size()</param>    
    /// <param name="j">the dimension of the coordinate of interest, 0 ≤ j ≤ dimension()</param>
    /// <returns>the <i>j</i>th Euclidean coordinate of the <i>i</i>th point</returns>
    double Coord(int i, int j);
}
