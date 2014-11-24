// Synopsis: Reads points from a file and computes their miniball.
//
// Authors: Martin Kutz <kutz@math.fu-berlin.de>,
//          Kaspar Fischer <kf@iaeth.ch>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>

#include "Seb.h"
#include "Seb_debug.C"

int main(int argn,char **argv) {
  typedef double FT;
  typedef Seb::Point<FT> Point;
  typedef Seb::Smallest_enclosing_ball<FT> Miniball;

  using std::cout;
  using std::endl;
  using std::vector;

  cout << "=====================================================" << endl
       << "Reads points from a file and computes their miniball." << endl
       << "The file is assumed to start with two integers, the" << endl
       << "number of points and the dimension.  Then come the" << endl
       << "n points, each in a line of d floating-point numbers." << endl;

  // check for right number of arguments:
  if (argn < 2) {
    cout << "Usage: " << argv[0] << " file-name" << endl
	 << "=====================================================" << endl;
    return 1;
  }
  cout << "=====================================================" << endl;

  // open input file:
  std::ifstream f(argv[1]);
  if (!f) {
    cout << "Could not open file '" << argv[1] << "'." << endl;
    return 1;
  }
  int n, d;
  f >> n;
  f >> d;

  // construct n random points in dimension d:
  vector<Point> S;
  vector<double> coords(d);
  for (int i=0; i<n; ++i) {

    // read coorindates:
    for (int j=0; j<d; ++j)
      f >> coords[j];

    // construct and add point:
    S.push_back(Point(d,coords.begin()));
  }

  // compute the miniball:
  cout << "Starting computation..." << endl;
  SEB_NAMESPACE::Timer::instance().start("all");
  Miniball mb(d, S);

  // output:
  FT rad = mb.squared_radius();
  cout << "Squared radius = " << std::setiosflags(std::ios::scientific)
       << std::setprecision(17) << rad << endl;
  cout << "Center = ";
  for (int i=0; i<d; ++i)
    cout << std::setiosflags(std::ios::scientific) << std::setprecision(17)
	 << *(mb.center_begin() + i) << " ";
  cout << endl << "=====================================================" << endl;

  mb.verify();
  cout << "=====================================================" << endl;
  cout << "Took " << (SEB_NAMESPACE::Timer::instance().lapse("all")) << "ms." << endl;
}
