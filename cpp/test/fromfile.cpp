// Synopsis: Reads points from a file and computes their miniball.
//
// Authors: Martin Kutz <kutz@math.fu-berlin.de>,
//          Kaspar Fischer <kf@iaeth.ch>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <vector>

#include "Seb.h"
#include "Seb_debug.C"

static const double check_threshold = 1e-6;

int main(int argn,char **argv) {
  typedef double FT;
  typedef Seb::Point<FT> Point;
  typedef Seb::Smallest_enclosing_ball<FT> Miniball;

  using std::cout;
  using std::cerr;
  using std::endl;
  using std::vector;

  cout << "=====================================================" << endl
       << "Reads points from a file and computes their miniball." << endl
       << "The file is assumed to start with two integers, the" << endl
       << "number of points and the dimension." << endl
       << "Then comes a line with RADIUS and then d numbers for the expected center." << endl
       << "Then come n points, each in a line of d floating-point numbers." << endl;

  // check for right number of arguments:
  if (argn < 2) {
    cerr << "Usage: " << argv[0] << " file-name" << endl
	 << "=====================================================" << endl;
    return 1;
  }
  cout << "=====================================================" << endl;

  // open input file:
  std::ifstream f(argv[1]);
  if (!f) {
    cerr << "Could not open file '" << argv[1] << "'." << endl;
    return 1;
  }
  unsigned int n, d;
  f >> n;
  f >> d;

  double expected_radius;
  vector<double> expected_center(d);
  f >> expected_radius;
  cout << "Expecting radius: " << expected_radius << endl;

  cout << "Expecting center: ";
  for (unsigned int j=0; j<d; ++j)
  {
     f >> expected_center[j];
     cout << expected_center[j] << ",";
  }
  cout << endl;

  // construct n random points in dimension d:
  vector<Point> S;
  vector<double> coords(d);
  for (unsigned int i=0; i<n; ++i) {

    // read coorindates:
    for (unsigned int j=0; j<d; ++j)
      f >> coords[j];

    // construct and add point:
    S.push_back(Point(d,coords.begin()));
  }

  // compute the miniball:
  cout << "Starting computation..." << endl;
  SEB_NAMESPACE::Timer::instance().start("all");
  Miniball mb(d, S);

  // output:
  FT sq_rad = mb.squared_radius();
  FT rad = sqrt(sq_rad);
  cout << "Squared radius = " << std::setiosflags(std::ios::scientific)
       << std::setprecision(17) << sq_rad << endl;
  cout << "Radius = " << std::setiosflags(std::ios::scientific)
       << std::setprecision(17) << rad << endl;
  cout << "Center = ";
  for (unsigned int i=0; i<d; ++i)
    cout << std::setiosflags(std::ios::scientific) << std::setprecision(17)
	 << *(mb.center_begin() + i) << " ";
  cout << endl << "=====================================================" << endl;

  mb.verify();
  cout << "=====================================================" << endl;
  cout << "Took " << (SEB_NAMESPACE::Timer::instance().lapse("all")) << "ms." << endl;

  bool radius_failed = (fabs(rad - expected_radius) > check_threshold);
  if (radius_failed)
     cerr << "Radius not within threshold (" << fabs(rad - expected_radius) << " > " << check_threshold << ")" << endl;

  bool center_failed = false;
  for (unsigned int j=0; j<d; ++j)
     center_failed = center_failed || (fabs(expected_center[j] - *(mb.center_begin()+j)) > check_threshold);
  if (center_failed)
  {
     cerr << "center not within threshold" << endl;
     for (unsigned int j=0; j<d; ++j)
        cerr << "Dim " << j << " : " << fabs(expected_center[j] - *(mb.center_begin()+j)) << " > " << check_threshold << ")" << endl;
  }

  return (radius_failed || center_failed ? 1 : 0);
}
