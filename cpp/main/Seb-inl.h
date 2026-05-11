// Synopsis: Library to find the smallest enclosing ball of points
//
// Authors: Martin Kutz <kutz@math.fu-berlin.de>,
//          Kaspar Fischer <kf@iaeth.ch>

#ifndef SEB_SEB_INL_H
#define SEB_SEB_INL_H

#include <algorithm>
#include <numeric>

#include "Seb.h" // Note: header included for better syntax highlighting in some IDEs.

namespace SEB_NAMESPACE {

  template<typename Float, class Pt, class PointAccessor>
  void Smallest_enclosing_ball<Float, Pt, PointAccessor>::allocate_resources()
  {
    center            = new Float[dim];
    center_to_aff     = new Float[dim];
    center_to_point   = new Float[dim];
    lambdas           = new Float[dim+1];
  }

  template<typename Float, class Pt, class PointAccessor>
  void Smallest_enclosing_ball<Float, Pt, PointAccessor>::deallocate_resources()
  {
    delete[] center;
    delete[] center_to_aff;
    delete[] center_to_point;
    delete[] lambdas;

    if (support != NULL)
      delete support;
  }

  template<typename Float, class Pt, class PointAccessor>
  void Smallest_enclosing_ball<Float, Pt, PointAccessor>::init_ball()
  // Precondition: |S| > 0
  // Sets up the search ball with an arbitrary point of S as center
  // and with with exactly one of the points farthest from center in
  // support, which is instantiated here.
  // So the current ball contains all points of S and has radius at
  // most twice as large as the minball.
  {
    SEB_ASSERT(S.size() > 0);

    // set center to the first point in S:
    for (unsigned int i = 0; i < dim; ++i)
      center[i] = S[0][i];

    // find farthest point:
    radius_square = 0;
    unsigned int farthest = 0; // Note: assignment prevents compiler warnings.
    for (unsigned int j = 1; j < S.size(); ++j) {
      // compute squared distance from center to S[j]:
      Float dist = 0;
      for (unsigned int i = 0; i < dim; ++i)
        dist += sqr(S[j][i] - center[i]);

      // enlarge radius if needed:
      if (dist >= radius_square) {
        radius_square = dist;
        farthest = j;
      }
      radius_ = sqrt(radius_square);
    }

    // initialize support to the farthest point:
    if (support != NULL)
      delete support;
    support = new Subspan<Float, Pt, PointAccessor>(dim,S,farthest);

    // statistics:
    // initialize entry-counters to zero:
    SEB_STATS(entry_count = std::vector<int>(S.size(),0));
  }

  template<typename Float, class Pt, class PointAccessor>
  bool Smallest_enclosing_ball<Float, Pt, PointAccessor>::successful_drop()
  // Precondition: center lies in aff(support).
  // If center doesn't already lie in conv(support) and is thus
  // not optimal yet, successful_drop() elects a suitable point k to
  // be removed from support -- and returns true.
  // If center lies in the convex hull, however, false is returned
  // (and support remains unaltered).
  {
    // find coefficients of the affine combination of center:
    support->find_affine_coefficients(center,lambdas);

    // find a non-positive coefficient:
    unsigned int smallest = 0; // Note: assignment prevents compiler warnings.
    Float minimum(1);
    for (unsigned int i=0; i<support->size(); ++i)
      if (lambdas[i] < minimum) {
        minimum = lambdas[i];
        smallest = i;
      }

    // drop a point with non-positive coefficient, if any:
    if (minimum <= 0) {
      SEB_LOG ("debug","  removing local point #" << smallest << std::endl);
      support->remove_point(smallest);
      return true;
    }
    return false;
  }

  template<typename Float, class Pt, class PointAccessor>
  Float Smallest_enclosing_ball<Float, Pt, PointAccessor>::find_stop_fraction(
      const Float* direction, int& stopper)
  {
    using std::inner_product;

    Float scale =  0;
    stopper     = -1;
    const Pt& support_point = S[support->any_member()];

    for (unsigned int j = 0; j < S.size(); ++j)
      if (!support->is_member(j)) {
        Float dist = 0;
        for (unsigned int i = 0; i < dim; ++i) {
          center_to_point[i] = S[j][i] - center[i];
          dist += sqr(center_to_point[i]);
        }

        for (unsigned int i = 0; i < dim; ++i)
          center_to_point[i] = S[j][i] - support_point[i];
        const Float denom =
            2 * inner_product(direction, direction+dim,
                              center_to_point, Float(0));
        if (denom == 0)
          continue;

        const Float bound = (dist - radius_square) / denom;
        if (bound > 0 && (stopper < 0 || bound < scale)) {
          scale   = bound;
          stopper = j;
        }
      }

    return scale;
  }


  template<typename Float, class Pt, class PointAccessor>
  Float Smallest_enclosing_ball<Float, Pt, PointAccessor>::find_stop_fraction(int& stopper)
  // Given the center of the current enclosing ball and the
  // walking direction center_to_aff, determine how much we can walk
  // into this direction without losing a point from S.  The (positive)
  // factor by which we can walk along center_to_aff is returned.
  // Further, stopper is set to the index of the most restricting point
  // and to -1 if no such point was found.
  {
    using std::inner_product;

    // We would like to walk the full length of center_to_aff ...
    Float scale =  1;
    stopper     = -1;

    SEB_DEBUG (Float margin = 0;)

    // ... but one of the points in S might hinder us:
    for (unsigned int j = 0; j < S.size(); ++j)
      if (!support->is_member(j)) {

        // compute vector center_to_point from center to the point S[i]:
        for (unsigned int i = 0; i < dim; ++i)
          center_to_point[i] = S[j][i] - center[i];

        const Float dir_point_prod
        = inner_product(center_to_aff,center_to_aff+dim,
                        center_to_point,Float(0));

        // we can ignore points beyond support since they stay
        // enclosed anyway:
        if (dist_to_aff_square - dir_point_prod
            // make new variable 'radius_times_dist_to_aff'? !
            < Eps * radius_ * dist_to_aff)
          continue;

        // compute the fraction we can walk along center_to_aff until
        // we hit point S[i] on the boundary:
        // (Better don't try to understand this calculus from the code,
        //  it needs some pencil-and-paper work.)
        Float bound = radius_square;
        bound -= inner_product(center_to_point,center_to_point+dim,
                               center_to_point,Float(0));
        bound /= 2 * (dist_to_aff_square - dir_point_prod);

        // watch for numerical instability - if bound=0 then we are
        // going to walk by zero units, and thus hit an infinite loop.
        //
        // If we are walking < 0, then we are going the wrong way,
        // which can happen if we were to disable the test just above
        // (the "dist_to_aff_square-dir_point_prod" test)

        // take the smallest fraction:
        if (bound > 0 && bound < scale) {
          scale   = bound;
          stopper = j;
          SEB_DEBUG (margin = dist_to_aff - dir_point_prod / dist_to_aff;)
        }
      }

    SEB_LOG ("debug","  margin = " << margin << std::endl);

    return scale;
  }


  template<typename Float, class Pt, class PointAccessor>
  void Smallest_enclosing_ball<Float, Pt, PointAccessor>::pivot()
  // The main function containing the main loop.
  // Iteratively, we compute the point in support that is closest
  // to the current center and then walk towards this target as far
  // as we can, i.e., we move until some new point touches the
  // boundary of the ball and must thus be inserted into support.
  // In each of these two alternating phases, we always have to check
  // whether some point must be dropped from support, which is the
  // case when the center lies in aff(support).
  // If such an attempt to drop fails, we are done;  because then
  // the center lies even conv(support).
  {
    while (true) {

      ++last_iteration_count;
      SEB_LOG ("debug","  iteration " << last_iteration_count << std::endl);

      SEB_LOG ("debug","  " << support->size()
               << " points on boundary" << std::endl);

      // Compute a walking direction and walking vector,
      // and check if the former is perhaps too small:
      while ((dist_to_aff
              = sqrt(dist_to_aff_square
                     = support->shortest_vector_to_span(center,
                                                        center_to_aff)))
             <= Eps * radius_)
        // We are closer than Eps * radius_square, so we try a drop:
        if (!successful_drop()) {
          // If that is not possible, the center lies in the convex hull
          // and we are done.
          SEB_TIMER_PRINT("computation");
          return;
        }

      SEB_LOG ("debug","  distance to affine hull = "
               << dist_to_aff << std::endl);

      // determine how far we can walk in direction center_to_aff
      // without losing any point ('stopper', say) in S:
      int stopper;
      Float scale = find_stop_fraction(stopper);
      SEB_LOG ("debug","  stop fraction = " << scale << std::endl);

      // Note: In theory, the following if-statement should simply read
      //
      //  if (stopper >= 0) {
      //    // ...
      //
      // However, due to rounding errors, it may happen in practice that
      // stopper is nonnegative and the support is already full (see #14);
      // in this casev we cannot add yet another point to the support.
      //
      // Therefore, the condition reads:
      if (stopper >= 0 && support->size() <= dim) {
        // stopping point exists

        // walk as far as we can
        for (unsigned int i = 0; i < dim; ++i)
          center[i] += scale * center_to_aff[i];

        // update the radius
        const Pt& stop_point = S[support->any_member()];
        radius_square = 0;
        for (unsigned int i = 0; i < dim; ++i)
          radius_square += sqr(stop_point[i] - center[i]);
        radius_ = sqrt(radius_square);
        SEB_LOG ("debug","  current radius = "
                 << std::setiosflags(std::ios::scientific)
                 << std::setprecision(17) << radius_
                 << std::endl << std::endl);

        // and add stopper to support
        support->add_point(stopper);
        SEB_STATS (++entry_count[stopper]);
        SEB_LOG ("debug","  adding global point #" << stopper << std::endl);
      }
      else {
        //  we can run unhindered into the affine hull
        SEB_LOG ("debug","  moving into affine hull" << std::endl);
        for (unsigned int i=0; i<dim; ++i)
          center[i] += center_to_aff[i];

        // update the radius:
        const Pt& stop_point = S[support->any_member()];
        radius_square = 0;
        for (unsigned int i = 0; i < dim; ++i)
          radius_square += sqr(stop_point[i] - center[i]);
        radius_ = sqrt(radius_square);
        SEB_LOG ("debug","  current radius = "
                 << std::setiosflags(std::ios::scientific)
                 << std::setprecision(17) << radius_
                 << std::endl << std::endl);

        // Theoretically, the distance to the affine hull is now zero
        // and we would thus drop a point in the next iteration.
        // For numerical stability, we don't rely on that to happen but
        // try to drop a point right now:
        if (!successful_drop()) {
          // Drop failed, so the center lies in conv(support) and is thus
          // optimal.
          SEB_TIMER_PRINT("computation");
          return;
        }
      }
    }
  }

  template<typename Float, class Pt, class PointAccessor>
  void Smallest_enclosing_ball<Float, Pt, PointAccessor>::update(
      const Float* previous_center, Float previous_squared_radius,
      unsigned int new_point_index)
  {
    SEB_TIMER_START("computation");
    last_iteration_count = 0;

    // optimistically, we set this flag now;
    // on return from this function it will be true:
    up_to_date = true;

    // Incremental construction starts from the smallest ball containing the
    // previous miniball and the new outside point.
    if (previous_center != NULL) {
      SEB_ASSERT(S.size() > 0);
      SEB_ASSERT(new_point_index < S.size());
      const Pt& new_point = S[new_point_index];
      Float dist_square = 0;
      for (unsigned int i = 0; i < dim; ++i)
        dist_square += sqr(new_point[i] - previous_center[i]);
      const Float previous_radius = sqrt(previous_squared_radius);
      const Float dist = sqrt(dist_square);
      const Float new_radius = (dist + previous_radius) / 2;
      const Float shift = (dist - previous_radius) / (2 * dist);
      for (unsigned int i = 0; i < dim; ++i)
        center[i] = previous_center[i] + shift * (new_point[i] - previous_center[i]);
      radius_square = sqr(new_radius);
      radius_ = new_radius;
      if (support != NULL)
        support->reset(new_point_index);
      else
        support = new Subspan<Float, Pt, PointAccessor>(dim, S, new_point_index);
      SEB_STATS(entry_count = std::vector<int>(S.size(),0));
    } else {
      init_ball();
    }

    // Invariant:  The ball B(center,radius_) always contains the whole
    // point set S and has the points in support on its boundary.

    pivot();
  }

  template<typename Float, class Pt, class PointAccessor>
  void Smallest_enclosing_ball<Float, Pt, PointAccessor>::append_point(
      unsigned int new_point_index)
  {
    if (!up_to_date)
      update();

    last_iteration_count = 0;

    SEB_ASSERT(new_point_index < S.size());

    if (support != NULL)
      support->resize_membership();

    Float dist = 0;
    for (unsigned int i = 0; i < dim; ++i)
      dist += sqr(S[new_point_index][i] - center[i]);
    if (dist <= radius_square)
      return;

    while (!contains(S[new_point_index])) {
      ++last_iteration_count;

      if (support->size() > dim) {
        update();
        return;
      }

      support->shortest_vector_to_span(center, center_to_aff);
      support->shortest_vector_to_span(S[new_point_index], center_to_point);
      dist_to_aff_square = 0;
      for (unsigned int i = 0; i < dim; ++i) {
        center_to_aff[i] -= center_to_point[i];
        dist_to_aff_square += sqr(center_to_aff[i]);
      }
      dist_to_aff = sqrt(dist_to_aff_square);
      if (dist_to_aff <= Eps * radius_) {
        update();
        return;
      }

      int stopper;
      Float scale = find_stop_fraction(center_to_aff, stopper);
      if (stopper < 0) {
        update();
        return;
      }

      for (unsigned int i = 0; i < dim; ++i)
        center[i] += scale * center_to_aff[i];

      const Pt& stop_point = S[support->any_member()];
      radius_square = 0;
      for (unsigned int i = 0; i < dim; ++i)
        radius_square += sqr(stop_point[i] - center[i]);
      radius_ = sqrt(radius_square);

      support->add_point(stopper);
    }

    pivot();
  }

  template<typename Float, class Pt, class PointAccessor>
  void Smallest_enclosing_ball<Float, Pt, PointAccessor>::verify()
  {
    using std::inner_product;
    using std::abs;
    using std::cout;
    using std::endl;

    Float  min_lambda      = 1;  // for center-in-convex-hull check
    Float  max_overlength  = 0;  // for all-points-in-ball check
    Float  min_underlength = 0;  // for all-boundary-points-on-boundary
    Float  ball_error;
    Float  qr_error = support->representation_error();

    // center really in convex hull?
    support->find_affine_coefficients(center,lambdas);
    for (unsigned int k = 0; k < support->size(); ++k)
      if (lambdas[k] <= min_lambda)
        min_lambda = lambdas[k];

    // all points in ball, all support points really on boundary?
    for (unsigned int k = 0; k < S.size(); ++k) {

      // compare center-to-point distance with radius
      for (unsigned int i = 0; i < dim; ++i)
        center_to_point[i] = S[k][i] - center[i];
      ball_error = sqrt(inner_product(center_to_point,center_to_point+dim,
                                      center_to_point,Float(0)))
      - radius_;

      // check for sphere violations
      if (ball_error > max_overlength) max_overlength = ball_error;

      // check for boundary violations
      if (support->is_member(k))
        if (ball_error < min_underlength) min_underlength = ball_error;
    }

    cout << "Solution errors (relative to radius, nonsquared)" << endl
    << "  final QR inconsistency     : " << qr_error << endl
    << "  minimal convex coefficient : ";
    if (min_lambda >= 0) cout << "positive";
    else cout << (-min_lambda);
    cout << endl
    << "  maximal overlength         : "
    << (max_overlength / radius_) << endl
    << "  maximal underlength        : "
    << (abs(min_underlength / radius_))
    << endl;


#ifdef SEB_STATS_MODE
    // And let's print some statistics about the rank changes
    cout << "=====================================================" << endl
    << "Statistics" << endl;

    // determine how often a single point entered support at most
    int max_enter = 0;
    for (int i = 0; i < S.size(); ++i)
      if (entry_count[i] > max_enter)
        max_enter = entry_count[i];
    ++max_enter;

    // compute a histogram from the entry data ...
    std::vector<int> histogram(max_enter+1);
    for (int j = 0; j <= max_enter; ++j)
      histogram[j] = 0;
    for (int i = 0; i < S.size(); ++i)
      histogram[entry_count[i]]++;
    // ... and print it
    for (int j = 0; j <= max_enter; j++)
      if (histogram[j])
        cout << histogram[j] << " points entered " << j << " times" << endl;
#endif // SEB_STATS MODE
  }


  template<typename Float, class Pt, class PointAccessor>
  void Smallest_enclosing_ball<Float, Pt, PointAccessor>::test_affine_stuff()
  {
    using std::cout;
    using std::endl;

    Float *direction;
    Float  error;
    Float  max_representation_error = 0;

    cout << S.size() << " points in " << dim << " dimensions" << endl;

    if (!is_empty()) {
      support = new Subspan<Float,PointAccessor,Pt>(dim,S,0);
      cout << "initializing affine subspace with point S[0]" << endl;

      direction = new Float[dim];
    }
    else return;

    for (int loop = 0; loop < 5; ++loop) {

      // Try to fill each point of S into aff
      for (int i = 0; i < S.size(); ++i) {

        cout << endl << "Trying new point #" << i << endl;

        Float dist = sqrt(support->shortest_vector_to_span(S[i],direction));
        cout << "dist(S[" << i << "],affine_hull) = "
        << dist << endl;

        if (dist > 1.0E-8) {
          cout << "inserting point S["<<i<<"] into affine hull"
          << endl;
          support->add_point(i);

          cout << "representation error: "
          << (error = support->representation_error()) << endl;
          if (error > max_representation_error)
            max_representation_error = error;
        }
      }

      //  Throw out half of the points
      while (support->size() > dim/2) {

        //  throw away the origin or point at position r/2,
        //  depending on whether size is odd or even:
        int k = support->size()/2;
        if (2 * k == support->size()) {
          //  size even
          cout << endl << "Throwing out local point #" << k << endl;
          support->remove_point(k);
        } else {
          //  size odd
          cout << endl << "Throughing out the origin" << endl;
          support->remove(support->size()-1);
        }

        cout << "representation error: "
        << (error = support->representation_error()) << endl;
        if (error > max_representation_error)
          max_representation_error = error;
      }
    }

    cout << "maximal representation error: "
    << max_representation_error << endl
    << "End of test." << endl;

    delete support;
    delete direction;
  }


  template<typename Float, class Pt, class PointAccessor>
  const Float Smallest_enclosing_ball<Float, Pt, PointAccessor>::Eps = Float(1e-14);

} // namespace SEB_NAMESPACE

#endif // SEB_SEB_INL_H
