/*
  Synopsis: A binder for enabling this package using numpy arrays.

  Original author: Filip Cornell <fcornell@kth.se, c.filip.cornell@gmail.com>
  Modified by: Adam Heins <mail@adamheins.com>

*/
#define PY_SSIZE_T_CLEAN
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <Python.h>
#include <numpy/arrayobject.h>
#include <vector>
#include "../cpp/main/Seb.h"

typedef Seb::Point<double> Point;
typedef Seb::Smallest_enclosing_ball<double> Miniball;

static double *miniball(const double *data, int rows, int cols, double *radius,
                        double *radius_squared) {
    int d = cols;
    std::vector<Point> points;
    std::vector<double> coords(d);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            coords[j] = data[j + d * i];
        }
        points.push_back(Point(d, coords.begin()));
    }

    Miniball mb(d, points);
    *radius = mb.radius();
    *radius_squared = mb.squared_radius();

    double *center = new double[d];
    Miniball::Coordinate_iterator center_it = mb.center_begin();
    for (int j = 0; j < d; ++j) {
        center[j] = center_it[j];
    }

    return center;
}

static PyObject *miniball_python(PyObject *self, PyObject *args) {
    // We expect a single 2-d array argument of points
    PyArrayObject *arr;
    if (!PyArg_ParseTuple(args, "O!", &PyArray_Type, &arr)) {
        return NULL;
    }

    int ndim = PyArray_NDIM(arr);
    if (ndim != 2) {
        PyErr_SetString(PyExc_RuntimeError, "Expected ndim=2");
        return NULL;
    }

    int type = PyArray_TYPE(arr);
    if (type != NPY_DOUBLE) {
        PyErr_SetString(PyExc_RuntimeError, "Expected dtype=float64");
        return NULL;
    }

    arr = PyArray_GETCONTIGUOUS(arr);

    // Extract required data
    double *data = (double *)PyArray_DATA(arr);
    npy_intp *dims = PyArray_DIMS(arr);
    int rows = (int)dims[0];
    int cols = (int)dims[1];

    // Compute the miniball
    double radius, radius_squared;
    double *center = miniball(data, rows, cols, &radius, &radius_squared);

    // Convert everything to Python objects and return as the tuple (center,
    // radius).
    npy_intp center_dims[] = {cols};
    PyObject *center_array =
        PyArray_SimpleNewFromData(1, center_dims, NPY_DOUBLE, center);

    PyObject *res = PyDict_New();
    PyDict_SetItemString(res, "center", center_array);
    PyDict_SetItemString(res, "radius", PyFloat_FromDouble(radius));
    PyDict_SetItemString(res, "radius_squared",
                         PyFloat_FromDouble(radius_squared));
    return res;
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
