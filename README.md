![](http://hbf.github.com/miniball/miniball.png)

# Miniball
A library to compute the [miniball](http://en.wikipedia.org/wiki/Smallest_circle_problem min-circle) (a.k.a. _min-circle_, _min-sphere_, _smallest enclosing sphere_, etc.) of a point set.

The code works for points in arbitrary dimension. It runs very fast in low dimensions and is practically efficient up to dimensions 10,000. The implementation is based on the algorithm from the paper _["Fast Smallest-Enclosing-Ball Computation in High Dimensions"](http://hbf.github.com/miniball/)_ by Kaspar Fischer, Bernd Gärtner, and Martin Kutz _(Proc. 11th European Symposium on Algorithms (ESA)_, p. 630-641, 2003).

This project is dedicated to Martin Kutz.

# Language support

Currently, only the original C++ implementation that accompanied the paper is available. If you need to have a version in Java, please vote on the [respective ticket](https://github.com/hbf/miniball/issues/1). Contributions are welcome, please open a ticket or contact us directly!

# Speed
(Graphs to come. Be patient, hm.)

# Stability

The code is well-tested and its underlying algorithm should be numerically stable. By "numerically stable" we mean that even for points in _degenerate position_ – like all on a line, all on a circle, identical points in the input, etc. – the algorithm will (i) terminate, (ii) be fast, (iii) provide an accurate result.

# Getting started

On Linux or MacOS, the following steps will get you going:

```bash
# Get the source code
git clone https://github.com/hbf/miniball.git
cd miniball/cpp/test

# Download the Boost library
wget "http://downloads.sourceforge.net/project/boost/boost/1.53.0/boost_1_53_0.tar.bz2"
tar jxf boost_1_53_0.tar.bz2

# Compile a sample
g++ -I../main -Iboost_1_53_0 example.C -o example -O3

# Run it on one million points in 3D
./example 1000000 3
```

(More documentation to come. Contact us in case you run into any problems.)

# Links
  * For small dimensions like 2D or 3D, [Bernd Gärtner's code](http://www.inf.ethz.ch/personal/gaertner/miniball.html), which is based on Welzl's algorithm, may be faster.
  * The [Computational Geometry Algorithms Library (CGAL)](http://www.cgal.org/) contains both a floating-point and an _arbitrary-precision arithmetic_ implementations of several bounding sphere algorithm. Both codes can compute the minsphere of _spheres_ (not just points as input). See the [Chapter on Bounding Volumes](http://www.cgal.org/Manual/latest/doc_html/cgal_manual/Bounding_volumes/Chapter_main.html).
