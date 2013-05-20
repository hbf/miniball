package com.dreizak.miniball.model;

/**
 * A {@link PointSet} that stores <i>n</i> <i>d</i>-dimensional points in a Java array of
 * {@code double}s.
 */
public final class ArrayPointSet implements PointSet
{
  private int d, n;
  private double[] c;

  /**
   * Creates an array-based point set to store <i>n</i> <i>d</i>-dimensional points.
   * 
   * @param d
   *          the dimensions of the ambient space
   * @param n
   *          the number of points
   */
  public ArrayPointSet(int d, int n)
  {
    this.d = d;
    this.n = n;
    this.c = new double[n * d];
  }

  @Override
  public int size()
  {
    return n;
  }

  @Override
  public int dimension()
  {
    return d;
  }

  @Override
  public double coord(int i, int j)
  {
    assert 0 <= i && i < n;
    assert 0 <= j && j < d;
    return c[i * d + j];
  }

  /**
   * Sets the <i>j</i>th Euclidean coordinate of the <i>i</i>th point to the given value.
   * 
   * @param i
   *          the number of the point, 0 ≤ i < {@code size()}
   * @param j
   *          the dimension of the coordinate of interest, 0 ≤ j ≤ {@code dimension()}
   * @param v
   *          the value to set as the <i>j</i>th Euclidean coordinate of the <i>i</i>th point
   */
  public void set(int i, int j, double v)
  {
    assert 0 <= i && i < n;
    assert 0 <= j && j < d;
    c[i * d + j] = v;
  }

  public String toString()
  {
    StringBuffer s = new StringBuffer("{");
    for (int i = 0; i < n; ++i)
    {
      s.append('[');
      for (int j = 0; j < d; ++j)
      {
        s.append(coord(i, j));
        if (j < d - 1) s.append(",");
      }
      s.append(']');
      if (i < n - 1) s.append(", ");
    }
    s.append('}');
    return s.toString();
  }
}
