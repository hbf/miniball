/*
  Synopsis: A binder for enabling this package using numpy arrays.

  Original author: Filip Cornell <fcornell@kth.se, c.filip.cornell@gmail.com>
  Modified by: Adam Heins <mail@adamheins.com>
  Modified by: Jenna Bradley <jenbrad@umich.edu>

*/
#include "../cpp/main/Seb.h"
#include <memory>
#include <stdexcept>
#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>

namespace nb = nanobind;

using Point = Seb::Point<double>;
using NativeMiniball = Seb::Smallest_enclosing_ball<double>;

nb::dict miniball_result(NativeMiniball &mb, size_t dim) {
  nb::dict result;
  result["center"] =
      nb::ndarray<double, nb::numpy>(mb.center_begin(), {dim}).cast();
  result["radius"] = mb.radius();
  result["radius_squared"] = mb.squared_radius();
  result["iterations"] = mb.iterations();

  return result;
}

class Miniball {
public:
  Miniball(
      nb::ndarray<double, nb::shape<-1, -1>, nb::c_contig> points_arr,
      bool use_simd_if_available = true)
      : use_simd_if_available_(use_simd_if_available) {
    size_t n_points = points_arr.shape(0);
    dim_ = points_arr.shape(1);
    if (n_points == 0) {
      throw std::invalid_argument("Input array must contain at least one point.");
    }

    const double *data = points_arr.data();
    points_.reset(new std::vector<Point>());
    points_->reserve(n_points);
    for (size_t i = 0; i < n_points; ++i) {
      points_->emplace_back(dim_, data + i * dim_);
    }
    miniball_.reset(new NativeMiniball(static_cast<unsigned int>(dim_), *points_,
                                       use_simd_if_available_));
  }

  nb::dict add(nb::ndarray<double, nb::shape<-1>, nb::c_contig> point_arr) {
    if (point_arr.shape(0) != dim_) {
      throw std::invalid_argument("Point must match the point-set dimension.");
    }
    add_point(point_arr.data());
    return result();
  }

  nb::dict add_points(
      nb::ndarray<double, nb::shape<-1, -1>, nb::c_contig> points_arr) {
    if (points_arr.shape(1) != dim_) {
      throw std::invalid_argument("Points must match the point-set dimension.");
    }
    const double *data = points_arr.data();
    for (size_t i = 0; i < points_arr.shape(0); ++i) {
      add_point(data + i * dim_);
    }
    return result();
  }

  nb::dict result() { return miniball_result(*miniball_, dim_); }

private:
  void add_point(const double *point) {
    points_->emplace_back(dim_, point);
    miniball_->append_point(static_cast<unsigned int>(points_->size() - 1));
  }

  size_t dim_;
  bool use_simd_if_available_;
  std::unique_ptr<std::vector<Point>> points_;
  std::unique_ptr<NativeMiniball> miniball_;
};

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
    nb::ndarray<double, nb::shape<-1, -1>, nb::c_contig> points_arr,
    bool use_simd_if_available = true) {
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
  NativeMiniball mb(dim, points, use_simd_if_available);

  return miniball_result(mb, dim);
}

/**
 * @brief Computes the smallest enclosing ball after appending one point.
 *
 * The caller supplies the existing point set, its current miniball, and a new
 * point outside that ball. The search starts from the smallest ball that
 * contains the old ball and has the new point on its boundary.
 */
nb::dict compute_miniball_incremental(
    nb::ndarray<double, nb::shape<-1, -1>, nb::c_contig> points_arr,
    nb::ndarray<double, nb::shape<-1>, nb::c_contig> center_arr,
    double radius_squared,
    nb::ndarray<double, nb::shape<-1>, nb::c_contig> point_arr,
    bool use_simd_if_available = true) {
  size_t n_points = points_arr.shape(0);
  size_t dim = points_arr.shape(1);

  if (center_arr.shape(0) != dim || point_arr.shape(0) != dim) {
    throw std::invalid_argument(
        "Center and point must match the point-set dimension.");
  }

  const double *data = points_arr.data();
  const double *old_center = center_arr.data();
  const double *new_point = point_arr.data();

  std::vector<Point> points;
  points.reserve(n_points + 1);
  for (size_t i = 0; i < n_points; ++i) {
    points.emplace_back(dim, data + i * dim);
  }
  points.emplace_back(dim, new_point);

  NativeMiniball mb(dim, points, old_center, radius_squared,
                    static_cast<unsigned int>(n_points),
                    use_simd_if_available);

  return miniball_result(mb, dim);
}

// Define the Python module using the NB_MODULE macro.
// This replaces all the PyMethodDef, PyModuleDef, and PyInit boilerplate.
NB_MODULE(_miniball, m) {
  m.def("_compute_miniball", &compute_miniball, nb::arg("points"),
        nb::arg("use_simd_if_available") = true,
        "Compute the smallest enclosing ball for a set of points.");
  m.def("_compute_miniball_incremental", &compute_miniball_incremental,
        nb::arg("points"), nb::arg("center"), nb::arg("radius_squared"),
        nb::arg("point"), nb::arg("use_simd_if_available") = true,
        "Compute the smallest enclosing ball after appending one outside point.");
  m.def("_simd_available", &NativeMiniball::simd_available,
        "Return whether this build can use SIMD kernels on this CPU.");
  nb::class_<Miniball>(m, "_Miniball")
      .def(nb::init<nb::ndarray<double, nb::shape<-1, -1>, nb::c_contig>,
                    bool>(),
           nb::arg("points"), nb::arg("use_simd_if_available") = true)
      .def("add", &Miniball::add)
      .def("add_points", &Miniball::add_points)
      .def("result", &Miniball::result);
}
