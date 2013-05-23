package com.dreizak.miniball.model;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.UnsupportedEncodingException;
import java.util.Scanner;

public class PointSetUtils
{
  /**
   * A random number generator for use in {@link PointSetUtils#randomPointSet(int, int, Random)}.
   */
  public static interface Random
  {
    double nextDouble();
  }

  /**
   * Generates a random point set given a random number generator.
   * 
   * @param d
   *          dimension of the points in the point set
   * @param n
   *          number of points
   * @param r
   *          the random number generator
   * @return a point set with {@code n} {@code d}-dimensional points
   */
  public static ArrayPointSet randomPointSet(int d, int n, Random r)
  {
    ArrayPointSet pts = new ArrayPointSet(d, n);

    for (int i = 0; i < n; ++i)
      for (int j = 0; j < d; ++j)
        pts.set(i, j, r.nextDouble());

    return pts;
  }

  /**
   * Reads a point set from an input stream.
   * <p>
   * The stream is assumed to be encoded in UTF-8 and should contain integers and double values, all
   * separated by space or return. The first two numbers must be integers, specifying the number of
   * points in the point set and the dimension. The following numbers are all doubles and specify
   * the points by their Euclidean coordinates.
   * <p>
   * For example, the three two-dimensional points {@code (0,0)}, {@code (2,3)}, {@code (4,5)} could
   * be stored as follows:
   * 
   * <pre>
   *  3 2
   *  0 0
   *  2 3
   *  4 5
   * </pre>
   * 
   * @param s
   *          the input stream to read from
   * @return
   */
  public static final ArrayPointSet pointsFromStream(InputStream s)
  {
    try
    {
      Reader r = new InputStreamReader(new BufferedInputStream(s), "UTF-8");
      Scanner in = new Scanner(r);
      try
      {
        final int n = in.nextInt();
        final int d = in.nextInt();
        final ArrayPointSet pts = new ArrayPointSet(d, n);
        for (int i = 0; i < n; ++i)
          for (int j = 0; j < d; ++j)
            pts.set(i, j, in.nextDouble());
        return pts;
      }
      finally
      {
        try
        {
          r.close();
        }
        catch (IOException e)
        {
          throw new RuntimeException("Could not read points.", e);
        }
      }
    }
    catch (UnsupportedEncodingException e)
    {
      throw new RuntimeException("Unkown encoding.", e);
    }
  }
}
