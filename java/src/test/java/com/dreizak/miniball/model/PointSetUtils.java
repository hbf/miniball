package com.dreizak.miniball.model;

import java.util.Random;

import com.dreizak.miniball.model.ArrayPointSet;

public class PointSetUtils
{
  public static ArrayPointSet randomPointSet(int d, int n, Random r)
  {
    ArrayPointSet pts = new ArrayPointSet(d, n);

    for (int i = 0; i < n; ++i)
      for (int j = 0; j < d; ++j)
        pts.set(i, j, r.nextDouble());

    return pts;
  }
}
