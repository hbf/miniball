package com.dreizak.miniball.highdim;

import static com.dreizak.miniball.model.PointSetUtils.randomPointSet;

import java.util.Random;

import org.junit.Test;

import com.dreizak.miniball.model.PointSet;

public class MiniballTest
{
  @Test
  public void randomTest()
  {
    Miniball mb = randomTest(3, 10, new Random(31415));
  }

  Miniball randomTest(int d, int n, Random r)
  {
    PointSet pts = randomPointSet(d, n, r);
    Miniball mb = new Miniball(pts);

    System.out.println(pts);
    System.out.println(mb);
    return mb;
  }
}
