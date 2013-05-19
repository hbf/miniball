package com.dreizak.miniball.highdim;

/**
 * A very simple logging class used for debugging.
 */
class Logging
{
  final static boolean log = false;

  public final static void warn(String msg)
  {
    if (log) System.err.println("[warn] " + msg);
  }

  public final static void info(String msg)
  {
    if (log) System.err.println("[info] " + msg);
  }

  public final static void debug(String msg)
  {
    if (log) System.err.println("[debug] " + msg);
  }
}
