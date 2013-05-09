package com.dreizak.miniball.highdim;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertFalse;
import static junit.framework.Assert.assertTrue;

import org.junit.Test;

import com.dreizak.miniball.model.ArrayPointSet;

public class SubspanTest
{
  final static double Tolerance = 1.0e-15; // TODO

  @Test
  public void subspan2x2WithLast()
  {
    // S = [ (1, 2), (5, 2)] in the plane
    ArrayPointSet S = new ArrayPointSet(2, 2);
    S.set(0, 0, 1);
    S.set(0, 1, 2);
    S.set(1, 0, 5);
    S.set(1, 1, 2);

    // Sub-span containing point 1 (i.e., the last one)
    Subspan span = new Subspan(2, S, 1);
    assertFalse(span.isMember(0));
    assertTrue(span.isMember(1));
    assertEquals(1, span.globalIndex(0));
    assertEquals(1, span.size());
    assertEquals(0.0, span.representationError());

    // Compute shortest vector to affine hull from a test point
    {
      double[] pt = {
          0, 0
      }, expected = {
          5, 2
      };
      shortestVectorToHull(span, pt, expected);
    }

    // Add point 0
    span.add(0);
    assertTrue(span.isMember(0));
    assertTrue(span.isMember(1));
    assertEquals(0, span.globalIndex(0));
    assertEquals(1, span.globalIndex(1));
    assertEquals(2, span.size());
    assertTrue(span.representationError() <= Tolerance);

    // Compute shortest vector to affine hull from a few test points
    {
      double[] pt = {
          0, 0
      }, expected = {
          0, 2
      };
      shortestVectorToHull(span, pt, expected);
    }
    {
      double[] pt = {
          4, 1
      }, expected = {
          0, 1
      };
      shortestVectorToHull(span, pt, expected);
    }
    {
      double[] pt = {
          4, 2
      }, expected = {
          0, 0
      };
      shortestVectorToHull(span, pt, expected);
    }
  }

  static void shortestVectorToHull(Subspan span, double[] pt, double[] expected)
  {
    double[] sv = new double[span.dimension()];
    span.shortestVectorToSpan(pt, sv);
    for (int i = 0; i < span.dimension(); ++i)
      assertTrue(Math.abs(expected[i] - sv[i]) <= Tolerance);
  }
}
