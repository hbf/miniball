// Synopsis: Converts a point file to a MPS file (which can be passed
// to CPLEX) The points file is assumed to start with two integers,
// the number of points and the dimension.  Then come the n points,
// each in a line of d floating-point numbers.
//
// Authors: Martin Kutz <kutz@math.fu-berlin.de>,
//          Kaspar Fischer <kf@iaeth.ch>
//
// Revision: $Revision: 1094 $ ($Date: 2004-09-02 12:42:47 +0200 (Thu, 02 Sep 2004) $)

#include <iostream>
#include <fstream>
#include <iomanip>
#include <Seb_point.h>

int main(int argn,char **argv) {
  typedef double FT;
  typedef Seb::Point<FT> Point;

  using std::cout;
  using std::endl;
  using std::vector;

  // check for right number of arguments:
  cout << "====================================================" << endl;
  if (argn < 2) {
    cout << "Usage: " << argv[0] << " point-file outfile" << endl
	 << "====================================================" << endl;
    return 1;
  }

  // open input file:
  std::ifstream f(argv[1]);
  if (!f) {
    cout << "Could not open file '" << argv[1] << "'." << endl;
    return 1;
  }
  int n, d;
  f >> n;
  f >> d;

  // open output file:
  std::ofstream o(argv[2]);
  if (!o) {
    cout << "Could not open output file '" << argv[2] << "'." << endl;
    return 1;
  }

  // read in the points:
  vector<Point> S;
  vector<double> coords(d);
  for (int i=0; i<n; ++i) {
    for (int j=0; j<d; ++j)
      f >> coords[j];
    S.push_back(Point(d,coords.begin()));
  }

  // We output the miniball program as in (MB'), eq. (18) in "An
  // efficient, exact, and generic quadratic programming solver for
  // geometric optimization" by B. Gaertner and S. Schoenherr: Notice
  // that the constraints x[i]>=0 need not be written down; they are
  // implicit.
  //
  // Set C to be the matrix holding the points S[i] in its columns.
  // We produce the following program:
  //
  //  (obj)  maximize  -y^T y + sum_{i=0}^{n-1} S[i]^TS[i] x[i]
  //  (s)    s. t.     x[0] + ... + x[n-1] = 1
  //  (r[i])          -y[i] + sum_{j=0}^{n-1}C_{ij} x[j] = 0
  //
  // Here, the rows r[0] to r[d-1] encode the constraint y=Cx from (18).

  o << "NAME miniball.mps" << endl
    << "OBJSENSE" << endl
    << " max" << endl;

  // We have d+2 rows: obj, s, r[0] to r[d-1].  The first is the
  // objective row (N), the remaining ones are equality rows (E):
  o << "ROWS" << endl
    << " N obj" << endl
    << " E s" << endl;
  for (int i=0; i<d; ++i)
    o  << " E r" << i << endl;

  // We need to specify the non-zero entries in the coefficient matrix
  // of the above program.
  o << "COLUMNS" << endl;
  for (int i=0; i<n; ++i) {
    using std::setiosflags;
    using std::ios;

    // compute S[i]^T S[i]:
    double ss = 0.0;
    for (int j=0; j<d; ++j)
      ss += S[i][j] * S[i][j];

    // The coefficient of x[i] in (obj) is ss, and the coefficient of
    // x[i] in (s) is 1:
    o << " x" << i << " obj " << setiosflags(ios::scientific)
      << std::setprecision(17) << ss << " s 1 " << endl;

    // The coefficient of x[i] in (r[j]) is S[i][j]:
    for (int j=0; j<d; ++j)
      o << " x" << i << " r" << j << " " << setiosflags(ios::scientific)
	<< std::setprecision(17) << S[i][j] << endl;
  }

  for (int i=0; i<d; ++i)
    // The coefficient of y[i] in r[i] is simply -1:
    o << " y" << i << " " << "r" << i << " -1" << endl;

  // After having specified the coefficient matrix, we give the
  // right-hands sides:
  o << "RHS" << endl;
  o << " rhs s 1" << endl;
  for (int j=0; j<d; ++j)
    o << " rhs r" << j << " 0" << endl;

  // We specify the y[i]'s to be free.  (Since we don't say anything
  // about the x[i]'s, these are assumed to be >=0.)
  o << "BOUNDS" << endl;
  for (int j=0; j<d; ++j)
    o << " FR BOUND y" << j << endl;

  // Finnaly, heres the definition of the quadratic term "-y^T y"
  // (which is minus the identity matrix): CPLEX wants this to be
  // scaled by 1/2, so we premultiply by 2:
  o << "QMATRIX" << endl;
  for (int j=0; j<d; ++j)
      o << " y" << j << " y" << j << " -2" << endl;
  
  o << "ENDATA" << endl;

  cout << "Conversion finished." << endl
       << "====================================================" << endl;
}
