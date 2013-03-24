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

def avg(n: Int, dim: Int) = {
  println(s"Measuring (d,n)=($dim,$n) ...")
  (1 to 10).map(_ => time(n, dim)).sum/10.0
}

val plots = Map(
    3 -> List(10000, 50000, 100000, 250000, 500000, 750000, 1000000, 2000000),
    5 -> List(10000, 50000, 100000, 250000, 500000, 750000, 1000000, 2000000),
    10 -> List(10000, 50000, 100000, 250000, 500000, 750000, 1000000)
  )

val measures = (
    for ((d,ns) <- plots)
      yield (d, ns.map(n => (n, avg(n,d))))
  ).toList

val headers = measures.map { case (d, times) => s"'-' using 1:2 axes x1y1 with linespoints title 'd=$d'"}.mkString(", ")
val data = measures.map { case (d, times) => times.map { case (n,t) => s" $n $t\n"}.mkString + " e\n" }.mkString

val plot = """set terminal png size 500,380
set output 'times.png'
set xtics ("10k" 10000, "100k" 100000, "250k" 250000, "500k" 500000, "1m" 1000000, "2m" 2000000, "5m" 5000000)
set xlabel "number of points"
set ylabel "time (s)"
plot """ + headers + "\n" + data
("gnuplot" #< new ByteArrayInputStream(plot.getBytes("UTF-8"))).!!