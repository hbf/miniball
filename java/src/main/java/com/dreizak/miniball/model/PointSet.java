package com.dreizak.miniball.model;

import java.util.List;

import com.dreizak.miniball.highdim.Miniball;

/**
 * Abstraction to access the points and their Euclidean coordinates of a set of <i>n</i> points.
 * <p>
 * Classes like {@link Miniball} do not take their input point set as a Java {@link List} are
 * similar data structure, as this forces the user to provide the points in a certain format.
 * Instead, the algorithms require an {@link PointSet} that allows the necessary characteristics of
 * the points to be queried.
 * <p>
 * Notice that most algorithms that use {@link PointSet}s will assume that the underlying point set
 * is <i>immutable</i>.
 * <p>
 * For optimal performance, you may want to copy your points to a Java array and use a
 * {@link ArrayPointSet}.
 * 
 * @see ArrayPointSet
 */
public interface PointSet
{
  /**
   * Number of points.
   * 
   * @return the number of points in the point set.
   */
  int size();

  /**
   * The dimension of the ambient space of the points.
   * <p>
   * Each point has {@code dimension()} many Euclidean coordinates.
   * 
   * @return the dimension of the ambient space
   */
  int dimension();

  /**
   * The <i>j</i>th Euclidean coordinate of the <i>i</i>th point.
   * 
   * @param i
   *          the number of the point, 0 ≤ i < {@code size()}
   * @param j
   *          the dimension of the coordinate of interest, 0 ≤ j ≤ {@code dimension()}
   * @return the <i>j</i>th Euclidean coordinate of the <i>i</i>th point
   */
  double coord(int i, int j);
}
