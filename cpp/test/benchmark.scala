#!/bin/sh
exec scala "$0" "$@"
!#

import scala.sys.process._
import java.io.ByteArrayInputStream

def time(n: Int, dim: Int) = {
  val Time = """(?s).*Running time: ([0-9.]+)s.*""".r
  val cmd = s"./example $n $dim"
  val Time(time) = cmd.!!
  time.toDouble
}

def avg(n: Int, dim: Int) = (1 to 10).map(_ => time(n, dim)).sum/10.0

val plots = Map(
    3 -> List(10000, 50000, 100000 ),//, 250000, 500000, 750000, 1000000, 2000000, 5000000),
    5 -> List(10000, 50000, 100000 ),//, 250000, 500000, 750000, 1000000, 2000000),
    10 -> List(10000, 50000 )//, 100000, 250000, 500000, 750000, 1000000)
  )

val times = for ((d,ns) <- plots)
  yield (d, ns.map(n => (n, avg(n,d))))
println(times)

//println(avg(1000000, 3))
val plot = """set terminal png size 400,250
set output 'times.png'
set xlabel "number of points"
set ylabel "time (s)"
plot """
"""
'-' using 1:2 axes x1y1 with lines title "asd2", '-' using 1:2 axes x1y1 with lines title "asda"
        1 10
        2 20
        3 32
        4 40
        5 50
        e
        1 30
        2 20
        3 12
        4 80
        5 10
        e
"""
//("gnuplot" #< new ByteArrayInputStream(plot.getBytes("UTF-8"))).!!