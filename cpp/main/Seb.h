// Synopsis: Library to find the smallest enclosing ball of points
//
// Authors: Martin Kutz <kutz@math.fu-berlin.de>,
//          Kaspar Fischer <kf@iaeth.ch>

#ifndef SEB_SEB_H
#define SEB_SEB_H

#include <vector>
#include "Seb_configure.h"
#include "Seb_point.h"
#include "Seb_simd.h"
#include "Subspan.h"

namespace SEB_NAMESPACE {

  // template arguments:
  // Float must be a floating point data type for which * / - + are defined
  // Pt[i] must return the i-th coordinate as a Float
  // PointAccessor[j] must return the j-th point in the data set as Pt and
  // size_t size() returns the size of the data set

  template<typename Float, class Pt = Point<Float>, class PointAccessor = std::vector<Pt> >
  class Smallest_enclosing_ball
  // An instance of class Smallest_enclosing_ball<Float> represents
  // the smallest enclosing ball of a set S of points.  Initially, the
  // set S is empty; you can add points by calling insert().
  {
  public: // iterator-type to iterate over the center coordinates of
	  // the miniball (cf. center_begin() below):
    typedef Float *Coordinate_iterator;

  public: // construction and destruction:

    Smallest_enclosing_ball(unsigned int d, const PointAccessor &P,
                            bool use_simd_if_available = true)
    // Constructs an instance representing the miniball of points from
    // set S.  The dimension of the ambient space is fixed to d for
    // lifetime of the instance.
    : dim(d), S(P), up_to_date(true), support(NULL),
      use_simd(use_simd_if_available && detail::simd_available_for<Float>()),
      last_iteration_count(0)
    {
      allocate_resources();
      SEB_ASSERT(!is_empty());
      update();
    }

    Smallest_enclosing_ball(unsigned int d, const PointAccessor &P,
                            const Float* previous_center,
                            Float previous_squared_radius,
                            unsigned int new_point_index,
                            bool use_simd_if_available = true)
    // Constructs an instance representing the miniball of points from
    // set S, using the miniball of S without new_point_index as a warm
    // start. The new point is assumed to lie outside that previous ball.
    : dim(d), S(P), up_to_date(true), support(NULL),
      use_simd(use_simd_if_available && detail::simd_available_for<Float>()),
      last_iteration_count(0)
    {
      allocate_resources();
      SEB_ASSERT(!is_empty());
      update(previous_center, previous_squared_radius, new_point_index);
      // Warm starting is an optimization; keep the constructor exact if that
      // start does not preserve the enclosing-ball invariant.
      for (unsigned int j = 0; j < S.size(); ++j)
        if (!contains(S[j])) {
          update();
          break;
        }
    }

    ~Smallest_enclosing_ball()
    {
      deallocate_resources();
    }

  public: // modification:

    void invalidate()
    // Notifies the instance that the underlying point set S (passed to the constructor
    // of this instance as parameter P) has changed. This will cause the miniball to
    // be recomputed lazily (i.e., when you call for example radius(), the recalculation
    // will be triggered).
    {
      up_to_date = false;
    }

    void append_point(unsigned int new_point_index);

  public: // access:

    bool is_empty()
    // Returns whether the miniball is empty, i.e., if no point has
    // been inserted so far.
    {
      return S.size() == 0;
    }

    Float squared_radius()
    // Returns the squared radius of the miniball.
    // Precondition: !is_empty()
    {
      if (!up_to_date)
        update();

      SEB_ASSERT(!is_empty());
      return radius_square;
    }

    Float radius()
    // Returns the radius of the miniball.
    // This is equivalent to std:sqrt(squared_radius())
    // Precondition: !is_empty()
    {
      if (!up_to_date)
        update();

      SEB_ASSERT(!is_empty());
      return radius_;
    }

    Coordinate_iterator center_begin()
    // Returns an iterator to the first Cartesian coordinate of the
    // center of the miniball.
    // Precondition: !is_empty()
    {
      if (!up_to_date)
        update();

      SEB_ASSERT(!is_empty());
      return center;
    }

    Coordinate_iterator center_end()
    // Returns the past-the-end iterator past the last Cartesian
    // coordinate of the center of the miniball.
    // Precondition: !is_empty
    {
      if (!up_to_date)
        update();

      SEB_ASSERT(!is_empty());
      return center+dim;
    }

    bool contains(const Pt& point)
    // Returns whether point is contained in the miniball.
    // Precondition: !is_empty()
    {
      if (!up_to_date)
        update();

      SEB_ASSERT(!is_empty());
      Float dist = 0;
      dist = detail::squared_distance<Float>(point, center, dim, use_simd);
      return dist <= radius_square;
    }

    static bool simd_available()
    {
      return detail::simd_available();
    }

    unsigned int iterations()
    // Returns the number of iterations used by the most recent update.
    {
      return last_iteration_count;
    }

  public: // testing:

    void verify();
    //  Verifies whether center lies really in affine hull,
    //  determines the consistency of the QR decomposition,
    //  and check whether all points of Q lie on the ball
    //  and all others within;
    //  prints the respective errors.

    void test_affine_stuff();
    // Runs some testing routines on the affine hull layer,
    // using the points in S.
    // Creates a new Q, so better use before or after actual computations.
    // Prints the accumulated error.


  private: // internal routines concerning storage:
    void allocate_resources();
    void deallocate_resources();

  private: // internal helper routines for the actual algorithm:
    void init_ball();
    Float find_stop_fraction(int& hinderer);
    Float find_stop_fraction(const Float* direction, int& hinderer);
    bool successful_drop();
    void pivot();

    void update(const Float* previous_center = NULL,
                Float previous_squared_radius = 0,
                unsigned int new_point_index = 0);

  private: // we forbid copying (since we have dynamic storage):
    Smallest_enclosing_ball(const Smallest_enclosing_ball&);
    Smallest_enclosing_ball& operator=(const Smallest_enclosing_ball&);

  private: // member fields:
    unsigned int dim;                 // dimension of the amient space
    const PointAccessor &S;           // set S of inserted points
    bool up_to_date;                  // whether the miniball has
                                      // already been computed
    Float *center;                    // center of the miniball
    Float radius_, radius_square;     // squared radius of the miniball
    Subspan<Float, Pt, PointAccessor> *support;          // the points that lie on the current
    // boundary and "support" the ball;
    // the essential structure for update()
    bool use_simd;

  private: // member fields for temporary use:
    Float *center_to_aff;
    Float *center_to_point;
    Float *lambdas;
    Float  dist_to_aff, dist_to_aff_square;
    unsigned int last_iteration_count;

#ifdef SEB_STATS_MODE
  private: // memeber fields for statistics
    std::vector<int> entry_count;     // counts how often a point enters
    // the support; only available in
    // stats mode
#endif // SEB_STATS_MODE

  private: // constants:
    static const Float Eps;
  };

} // namespace SEB_NAMESPACE

#include "Seb-inl.h"

#endif // SEB_SEB_H
