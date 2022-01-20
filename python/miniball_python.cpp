/*
  Synopsis: A binder for enabling this package using numpy arrays.

  Author: Filip Cornell <fcornell@kth.se, c.filip.cornell@gmail.com>

*/
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <Python.h>
#include <numpy/arrayobject.h>
#include <iostream>
#include <vector>
#include <tuple>
#include "../cpp/main/Seb.h"

typedef Seb::Point<double> Point;
typedef Seb::Smallest_enclosing_ball<double> Miniball;

static std::tuple<double*, double> miniball(const double *data, int rows, int cols) {
    std::vector<Point> points;
    int d = cols;
    std::vector<double> coords(d);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            coords[j] = data[j + d * i];
        }
        points.push_back(Point(d, coords.begin()));
    }

    Miniball mb(d, points);
    double radius = mb.radius();

    double *center = new double[d];
    Miniball::Coordinate_iterator center_it = mb.center_begin();
    for (int j = 0; j < d; ++j) {
        center[j] = center_it[j];
    }

    return std::tuple<double*, double>(center, radius);
}

static PyObject *miniball_python(PyObject *self, PyObject *args) {
    // We expect a single 2-d array argument of points
    PyArrayObject *arr;
    if (!PyArg_ParseTuple(args, "O!", &PyArray_Type, &arr)) {
        return NULL;
    }

    int ndim = PyArray_NDIM(arr);
    if (ndim != 2) {
        return NULL;
    }

    // Extract required data
    double *data = (double *)PyArray_DATA(arr);
    npy_intp *dims = PyArray_DIMS(arr);
    int rows = (int)dims[0];
    int cols = (int)dims[1];

    // Compute the miniball
    double radius;
    double *center;
    std::tie(center, radius) = miniball(data, rows, cols);

    // Convert everything to Python objects and return as the tuple (center,
    // radius).
    npy_intp center_dims[] = {cols};
    PyObject *center_array =
        PyArray_SimpleNewFromData(1, center_dims, NPY_DOUBLE, center);

    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, center_array);
    PyTuple_SetItem(ret, 1, PyFloat_FromDouble(radius));
    return ret;
}

static PyMethodDef MiniballMethods[] = {
    {"miniball", miniball_python, METH_VARARGS,
     "Compute the smallest enclosing ball for a set of points."},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static struct PyModuleDef miniballmodule = {
    PyModuleDef_HEAD_INIT, "miniball",
    "Compute the smallest enclosing ball for a set of points.", -1,
    MiniballMethods};

PyMODINIT_FUNC PyInit_miniball(void) {
    import_array();
    return PyModule_Create(&miniballmodule);
}
