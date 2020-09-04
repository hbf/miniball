/*
Copyright 2020 Filip Cornell
Synopsis: A binder for enabling this package using numpy arrays.

Author: Filip Cornell <fcornell@kth.se>

*/
#include <Python/Python.h>
#include <vector>
#include <iostream>
#include "../cpp/main/Seb.h"

typedef double FT;
typedef Seb::Point<FT> Point;
typedef std::vector<Point> PointVector;
typedef Seb::Smallest_enclosing_ball<FT> Miniball;
using std::vector;

extern "C" {

    double* miniball(const double * val, int rows, int cols, double* a, double* b) {
      //const double * val = (double *) indatav;
      PointVector S;
      int d = cols;
      vector<double> coords(d);
      std::cout << "d is " << d << std::endl;
      for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < d; ++j) {
          coords[j] = val[j + d * i];
        }
        S.push_back(Point(d, coords.begin()));
      }

      Miniball mb(d, S);
      *a = mb.radius();
      *b = mb.squared_radius();
      std::cout << *a << std::endl;
      std::cout << *b << std::endl;

      double* center = new double[d];

      Miniball::Coordinate_iterator center_it = mb.center_begin();
      for (int j=0; j<d; ++j)
        center[j] = center_it[j];

      return center;
    }

}
