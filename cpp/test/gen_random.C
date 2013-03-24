// Synopsis: Generates random points and writes them to stdout.
//
// Authors: Martin Kutz <kutz@math.fu-berlin.de>,
//          Kaspar Fischer <kf@iaeth.ch>

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <boost/random.hpp>

int main(int argn,char **argv) {
  typedef double FT;

  using std::cout;
  using std::endl;
  using std::vector;
  using std::setprecision;
  using std::setiosflags;

  if (argn < 4) {
    cout << "Usage: " << argv[0] << " n d method [arg]" << endl
	 << endl
	 << "Generates n random d-dimensional points and writes them" << endl
	 << "to standard out.  The argument method may be either box," << endl
	 << "sphere, sphere-clustered, normal or sparse:" << endl
	 << endl
	 << "  box: generates points with coordinates uniform in [-1,1)"
	 << endl << endl
	 << "  sphere: generates points uniformly distributed on the" << endl
	 << "  sphere; each point is furthermore rescaled to length" << endl
	 << "  1+w, where w is taken uniformly from [-arg,arg)." << endl
	 << endl
	 << "  sphere-clustered: generates points as in method box" << endl
	 << "  but scales them to length 1+w, where w is taken" << endl
	 << "  uniformly from [-arg,arg)." << endl
	 << endl
	 << "  normal: generates points with coordinates distributed" << endl
	 << "  normally (mean 0, standard deviation 1)." << endl
	 << endl
	 << "  sparse: generates points with expected arg-many nonzero" << endl
	 << "  coordinates, each of which is uniformly  distributed" << endl
	 << "  in [-1,1)." << endl
	 << endl;
    return 1;
  }

  // ... and parse command line arguments:
  const int n = std::atoi(argv[1]), d = std::atoi(argv[2]);
  const std::string method(argv[3]);
  double arg;
  if (argn > 4)
    arg = std::atof(argv[4]);

  // output n and d:
  std::cout << n << " " << d << std::endl;

  // initalize a unifom random number generator:
  typedef boost::lagged_fibonacci44497 Base_generator;
  typedef boost::random::uniform_real_distribution<> Uniform;
  typedef boost::random::uniform_on_sphere<> Spherical;
  typedef boost::random::normal_distribution<> Normal;
  Base_generator uniform_integer;

  if (method == "box") {
  	Uniform uniformally(-1.0, 1.0);
  	boost::variate_generator<Base_generator, Uniform> uniform(uniform_integer, uniformally);

    for (int i=0; i<n; ++i) {
      for (int j=0; j<d; ++j)
	cout << setiosflags(std::ios::scientific)
	     << setprecision(17) << uniform() << " ";
      cout << endl;
    }
  }

  else if (method == "sphere") {
    Spherical spherically(d);
  	boost::variate_generator<Base_generator, Spherical> spherical(uniform_integer, spherically);

    for (int i=0; i<n; ++i) {
      std::vector<double> pt = spherical();
      for (int j=0; j<d; ++j)
	cout << setiosflags(std::ios::scientific)
	     << setprecision(17) << pt[j] << " ";
      cout << endl;
    }
  }

  else if (method == "sphere-clustered") {
  	Uniform uniformally(-1.0, 1.0);
  	boost::variate_generator<Base_generator, Uniform> uniform(uniform_integer, uniformally);
  	Uniform uniformally_arg(-arg, arg);
  	boost::variate_generator<Base_generator, Uniform> wig(uniform_integer, uniformally_arg);
    vector<double> coords(d);

    for (int i=0; i<n; ++i) {
      // generate coordinates in [-1,1]:
      double len = 0;
      for (int j=0; j<d; ++j) {
	coords[j] = uniform();
	len += coords[j]*coords[j];
      }

      // normalize length to "almost" 1:
      len = 1/(std::sqrt(len)+wig());
      for (int j=0; j<d; ++j)
	cout << setiosflags(std::ios::scientific)
	     << setprecision(17) << (coords[j] * len) << " ";
      cout << endl;
    }
  }

  else if (method == "normal") {
    Normal normally(0,1);
  	boost::variate_generator<Base_generator, Normal> normal(uniform_integer, normally);

    for (int i=0; i<n; ++i) {
      for (int j=0; j<d; ++j)
	cout << setiosflags(std::ios::scientific)
	     << setprecision(17) << normal() << " ";
      cout << endl;
    }
  }

  else if (method == "sparse") {
    // We will make the coordinate non-zero whenever uniform() <= p.
    // This will give us on the average p*d non-zero coordinates per
    // point.  So p = arg/d:
  	Uniform uniformally(0.0, 1.0);
  	boost::variate_generator<Base_generator, Uniform> uniform(uniform_integer, uniformally);
    const double p = static_cast<double>(arg)/d;

    for (int i=0; i<n; ++i) {
      for (int j=0; j<d; ++j)
	if (uniform() <= p)
	  cout << setiosflags(std::ios::scientific)
	       << setprecision(17) << 2*uniform()-1.0 << " ";
	else
	  cout << 0 << " ";
      cout << endl;
    }
  }

  else {
    cout << "Error: distribution not specified" << endl;
    return 1;
  }
}
