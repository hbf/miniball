// Synopsis: Example program illustrating how to use the Seb library
//
// Authors: Martin Kutz <kutz@math.fu-berlin.de>,
//          Kaspar Fischer <kf@iaeth.ch>

#include <iostream>
#include <cstdio>

#include <Seb.h>
#include <Seb_Debug.C> // ... only because we use Seb::Timer below

int main(int argn,char **argv) {
  typedef double FT;
  typedef Seb::Point<FT> Point;
  typedef Seb::Smallest_enclosing_ball<FT> Miniball;

  using std::cout;
  using std::endl;
  using std::vector;

  cout << "====================================================" << endl
       << "Seb example" << endl;
  // check for right number of arguments ...
  if (argn < 3) {
    cout << "Usage: " << argv[0] << " number-of-points dimension" << endl
	 << "====================================================" << endl;
    return 1;
  }
  cout << "====================================================" << endl;
  // ... and parse command line arguments:
  const int n = std::atoi(argv[1]), d = std::atoi(argv[2]);

  // construct n random points in dimension d:
  vector<Point> S;
  vector<double> coords(d);
  srand(clock());
  for (int i=0; i<n; ++i) {

    // generate coorindates in [-1,1]:
    double len = 0;
    for (int j=0; j<d; ++j) {
      coords[j] = static_cast<FT>(2.0*rand()/RAND_MAX - 1.0);
      len += coords[j]*coords[j];
    }

    // normalize length to "almost" 1:
    const double Wiggle = 1e-2;
    len = 1/(std::sqrt(len)+Wiggle*rand()/RAND_MAX);
    for (int j=0; j<d; ++j)
      coords[j] *= len;

    S.push_back(Point(d,coords.begin()));
  }
  cout << "Starting computation..." << endl
       << "====================================================" << endl;
  Seb::Timer::instance().start("all");

  // compute the miniball:
  Miniball mb(d);
  for (int i=0; i<S.size(); ++i)
    mb.insert(S[i].begin());

  // output:
  FT rad = mb.squared_radius();
  cout << "Running time: " << Seb::Timer::instance().lapse("all") << "s" << endl
       << "Radius = " << std::sqrt(rad) << endl
       << "=====================================================" << endl;

  mb.verify();
  cout << "=====================================================" << endl;
}



