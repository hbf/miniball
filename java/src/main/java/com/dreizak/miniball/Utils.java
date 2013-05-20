package com.dreizak.miniball;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.UnsupportedEncodingException;
import java.util.Scanner;

import com.dreizak.miniball.model.ArrayPointSet;

/**
 * Utility methods for point sets and miniballs.
 */
public class Utils
{
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
