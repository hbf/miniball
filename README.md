![](http://hbf.github.com/miniball/miniball.png)

# Miniball
A C++ library to compute the [miniball](http://en.wikipedia.org/wiki/Bounding_sphere) (a.k.a. _min-circle_, _min-sphere_, _smallest enclosing sphere_, etc.) of a point set.

The code works for points in arbitrary dimension. It runs very fast in low dimensions and is practically efficient up to dimensions 10,000. The implementation is based on the algorithm from the paper _["Fast Smallest-Enclosing-Ball Computation in High Dimensions"](http://hbf.github.com/miniball/seb.pdf)_ by Kaspar Fischer, Bernd Gärtner, and Martin Kutz _([Proc. 11th European Symposium on Algorithms (ESA)](http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.90.5783)_, p. 630-641, 2003).

This project is dedicated to Martin Kutz.

# Language support

The project provides:

 * A Java implementation; and
 * A C++ implementation.
 
Contributions are welcome, please open a ticket or contact us directly!

**Note**: A Java implementation is underway. We are awaiting approval by Sonatype to publish it in the Sonatype OSS Maven Repository and will release afterwards, this won't take long.

# Speed
On a 2.66 GHz Intel Core i7 MacBook Pro, the code performs as follows:

![](http://hbf.github.com/miniball/times.png)

The chart shows the time in seconds (y-axis) needed for the computation of the miniball of *n* random points (x-axis) in dimensions *d* (3, 5, or 10 in this chart).

# Stability

The code is well-tested and its underlying algorithm should be numerically stable. By "numerically stable" we mean that even for points in _degenerate position_ – like all on a line, all on a circle, identical points in the input, etc. – the algorithm will (i) terminate, (ii) be fast, (iii) provide an accurate result.

# Getting started (Java)

You can either download the latest JAR file from TODO or use a build system like Maven or Graddle, or SBT (for Scala users).

Maven dependency:

```xml
TODO
```

SBT dependency:

    TODO

Documentation:

 * [Project information](http://hbf.github.com/miniball/)
 * [JavaDoc](http://hbf.github.io/miniball/apidocs/com/dreizak/miniball/highdim/Miniball.html)
 * [`MiniballTest.java`](https://github.com/hbf/miniball/blob/wip-java/java/src/test/java/com/dreizak/miniball/highdim/MiniballTest.java): A few tests take will help you understand how to use the library. Take a look at method `TODO` for a simple example.

# Getting started (C++)

On Linux or MacOS, the following steps will get you going:

```bash
# Get the source code
git clone https://github.com/hbf/miniball.git
cd miniball/cpp/test

# Download the Boost library
wget "http://downloads.sourceforge.net/project/boost/boost/1.53.0/boost_1_53_0.tar.bz2"
tar jxf boost_1_53_0.tar.bz2

# Compile an example, which generates random points and computes their miniball
g++ -I../main -Iboost_1_53_0 example.C -o example -O3

# Run it on one million points in 3D
./example 1000000 3
```

(More documentation to come. Contact us in case you run into any problems.)

# License
The code is available under the [Apache 2 License](http://www.apache.org/licenses/LICENSE-2.0.html), which is explained [here](http://www.tldrlegal.com/license/apache-license-2.0-(apache-2.0).

If you use the code in your project/product, please drop us a note – we are always interested in learning about new applications!

# Authors & acknowledgements

Authors:

 * Martin Kutz, FU Berlin
 * [Kaspar Fischer](http://github.com/hbf), ETH Zurich
 * [Bernd Gärtner](http://www.inf.ethz.ch/personal/gaertner/), ETH Zurich

Many thanks go to the following people who have – sometimes substantially – contributed to the code:


 * Thomas Otto (University of Hamburg) for [submitting several compiler fixes](https://github.com/hbf/miniball/issues/3) (g++ 4.7 and 4.5 on SUSE Linux 12.2 and 11.3) and for [introducing generic point and point coordinate accessors](https://github.com/hbf/miniball/pull/5) in the code.

# Links
  * For small dimensions like 2D or 3D, [Bernd Gärtner's code](http://www.inf.ethz.ch/personal/gaertner/miniball.html), which is based on Welzl's algorithm, may be faster.
  * The [Computational Geometry Algorithms Library (CGAL)](http://www.cgal.org/) contains both floating-point and _arbitrary-precision arithmetic_ implementations of several bounding sphere algorithms. Among then, there is an algorithm to compute the minsphere of _spheres_ (not just points as input). See the [Chapter on Bounding Volumes](http://www.cgal.org/Manual/latest/doc_html/cgal_manual/Bounding_volumes/Chapter_main.html).
