// Synopsis: Example program illustrating how to use the Seb library
//
// Authors: Martin Kutz <kutz@math.fu-berlin.de>,
//          Kaspar Fischer <kf@iaeth.ch>

#include <iostream>
#include <cstdio>

#include "Seb.h"
#include "Seb_debug.C" // ... only needed because we use Seb::Timer below

class PointAcc_Array
{
   double const* ptr;
   size_t num_points;
   size_t dims;

public:
   PointAcc_Array( double const* ptr, size_t num_points, size_t dims ) :
      ptr(ptr), num_points(num_points), dims(dims)
   {
   }

   double const* operator[]( size_t ptr_idx ) const
   {
      return ptr + (ptr_idx * dims);
   }

   size_t size() const { return num_points; }
};



int main(int argn,char **argv) {
  using std::cout;
  using std::endl;
  using std::vector;

{
   cout << "Start test (this would go in an infinite loop)" << endl
       << "====================================================" << endl;

  Seb::Timer::instance().start("infinite_loop_test_1");

  // 3 dimensions, x,y,z,x,y,z,etc
  double bad_coords[] = {
      19926.975, 49716.629, 9507.179,
      19926.697, 49694.336, 9495.121,
      19919.318, 49702.293, 9477.504,
      19938.859, 49714.676, 9488.590,
      19943.236, 49713.438, 9495.020,
      19933.016, 49705.695, 9480.108,
      19924.43, 49699.211, 9476.300,
      19924.43, 49699.211, 9476.300,
      19941.326, 49710.121, 9500.256
  };
  int num_pts = 9;
  int dims = 3;

  // Compute the miniball by inserting each value
  typedef Seb::Smallest_enclosing_ball< double, double const*, PointAcc_Array > Miniball;
  PointAcc_Array acc(bad_coords, num_pts, dims);
  Miniball mb(dims, acc);

  // Output
  double rad = mb.radius();
  double rad_squared = mb.squared_radius();
  cout << "Running time: " << Seb::Timer::instance().lapse("all") << "s" << endl
       << "Radius = " << rad << " (squared: " << rad_squared << ")" << endl
       << "Center:" << endl;
  Miniball::Coordinate_iterator center_it = mb.center_begin();
  for (int j=0; j<dims; ++j)
    cout << "  " << center_it[j] << endl;
  cout << "=====================================================" << endl;

  mb.verify();
  cout << "=====================================================" << endl;
}

}



