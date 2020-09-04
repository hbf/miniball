"""
    Synopsis: A binder for enabling this package using numpy arrays.

    Author: Filip Cornell <fcornell@kth.se, c.filip.cornell@gmail.com>

"""
from ctypes import cdll, POINTER, c_int, c_double, byref
import numpy as np
import ctypes
import pandas as pd
from numpy.ctypeslib import ndpointer

lib = cdll.LoadLibrary("./miniball_python.so")


def miniball(val):
    """
        Computes the miniball.

        input: val, a 2D numpy-array with points as rows, features as columns.
        output: a dict containing:
            - center: a 1D numpy-vector with the center of the miniball.
            - radius: The radius.
            - radius_squared. The radius squared. 
    """
    if isinstance(val, pd.DataFrame):
        val = val.values

    assert isinstance(val, np.ndarray)
    a = c_double(0)
    b = c_double(0)
    lib.miniball.argtypes = [
        ndpointer(ctypes.c_double, flags="C_CONTIGUOUS"),
        c_int,
        c_int,
        POINTER(c_double),
        POINTER(ctypes.c_double),
    ]
    rows = int(val.shape[0])
    cols = int(val.shape[1])
    lib.miniball.restype = POINTER(
        ctypes.c_double * val.shape[1]
    )
    center = lib.miniball(val, rows, cols, byref(a), byref(b))
    return {
        "center": np.array([i for i in center.contents]),
        "radius": a.value,
        "radius_squared": b.value,
    }


if __name__ == "__main__":
    print(
        miniball(np.array([[3.0, 1.0], [3.0, 1.0], [1.0, 0.0]], dtype=np.double))[
            "center"
        ]
    )
