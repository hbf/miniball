/*
  Synopsis: A binder for enabling this package using numpy arrays.

  Original author: Filip Cornell <fcornell@kth.se, c.filip.cornell@gmail.com>
  Modified by: Adam Heins <mail@adamheins.com>
  Modified by: Jenna Bradley <jenbrad@umich.edu>

*/
#include "../cpp/main/Seb.h"
#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>

namespace nb = nanobind;

using Point = Seb::Point<double>;
using Miniball = Seb::Smallest_enclosing_ball<double>;

/**
 * @brief Computes the smallest enclosing ball for a set of points.
 *
 * This function takes a 2D NumPy array of points and returns a dictionary
 * containing the center, radius, and squared radius of the enclosing ball.
 *
 * @param points_arr A 2D, C-contiguous NumPy array of dtype float64,
 * where each row represents a point.
 * @return A dictionary with keys "center" (np.ndarray), "radius" (float),
 * and "radius_squared" (float).
 */
nb::dict compute_miniball(
    nb::ndarray<double, nb::shape<-1, -1>, nb::c_contig> points_arr) {
  size_t n_points = points_arr.shape(0);
  size_t dim = points_arr.shape(1);

  const double *data = points_arr.data();

  // Create a vector of Point objects
  std::vector<Point> points;
  points.reserve(n_points);
  for (size_t i = 0; i < n_points; ++i) {
    // Pass a pointer to the beginning of the i-th row.
    points.emplace_back(dim, data + i * dim);
  }

  // Compute the smallest enclosing ball.
  Miniball mb(dim, points);

  nb::dict result;
  result["center"] =
      nb::ndarray<double, nb::numpy>(mb.center_begin(), {dim}).cast();
  result["radius"] = mb.radius();
  result["radius_squared"] = mb.squared_radius();

  return result;
}

// Define the Python module using the NB_MODULE macro.
// This replaces all the PyMethodDef, PyModuleDef, and PyInit boilerplate.
NB_MODULE(_miniball, m) {
  m.def("_compute_miniball", &compute_miniball, nb::arg("points"),
        "Compute the smallest enclosing ball for a set of points.");
}
