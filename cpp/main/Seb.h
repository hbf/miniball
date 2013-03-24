// Synopsis: Library to find the smallest enclosing ball of points
//
// Authors: Martin Kutz <kutz@math.fu-berlin.de>,
//          Kaspar Fischer <kf@iaeth.ch>

#ifndef SEB_SEB_H
#define SEB_SEB_H

#include <vector>
#include <Seb_configure.h>
#include <Subspan.h>

namespace SEB_NAMESPACE {

  template<typename Float>
  class Smallest_enclosing_ball
  // An instance of class Smallest_enclosing_ball<Float> represents
  // the smallest enclosing ball of a set S of points.  Initially, the
  // set S is empty; you can add points by calling insert().
  {
  private: // internal representation of points:
    typedef Point<Float> Pt;

  public: // iterator-type to iterate over the center coordinates of
	  // the miniball (cf. center_begin() below):
    typedef Float *Coordinate_iterator;

  public: // construction and destruction:

    Smallest_enclosing_ball(int d)
    // Constructs an instance representing the miniball of the empty
    // set S={}.  The dimension of the ambient space is fixed to d for
    // lifetime of the instance.
      : dim(d), up_to_date(true), support(NULL)
    {
      allocate_resources();
    }

    ~Smallest_enclosing_ball()
    {
      deallocate_resources();
    }

  public: // modification:

    template<typename InputIterator>
    void insert(InputIterator first)
    // Inserts the point p into the instance's set S.  The point p is
    // specified by its d coordinates from the range [first,first+d).
    {
      S.push_back(Pt(dim,first));    //  TODO!  open design decision
      up_to_date = false;
    }

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
    bool successful_drop();

    void update();

  private: // we forbid copying (since we have dynamic storage):
    Smallest_enclosing_ball(const Smallest_enclosing_ball&);
    Smallest_enclosing_ball& operator=(const Smallest_enclosing_ball&);

  private: // member fields:
    int dim;                          // dimension of the amient space
    std::vector<Pt> S;                // set S of inserted points
    bool up_to_date;                  // whether the miniball has
				      // already been computed
    Float *center;                    // center of the miniball
    Float radius, radius_square;      // squared radius of the miniball
    Subspan<Float> *support;          // the points that lie on the current
                                      // boundary and "support" the ball;
                                      // the essential structure for update()

  private: // member fields for temporary use:
    Float *center_to_aff;
    Float *center_to_point;
    Float *lambdas;
    Float  dist_to_aff, dist_to_aff_square;

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

#include <Seb.C>

#endif // SEB_SEB_H
