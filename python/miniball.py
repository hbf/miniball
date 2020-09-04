from ctypes import cdll, POINTER, c_int, c_double, byref
import numpy as np
import ctypes
import pandas as pd
from numpy.ctypeslib import ndpointer

lib = cdll.LoadLibrary("./miniball_python.so")


def miniball(val):
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
    )  # ndpointer(ctypes.c_double, flags="C_CONTIGUOUS")
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
