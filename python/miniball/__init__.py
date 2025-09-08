from ._miniball import _compute_miniball

import numpy as np

__all__ = ["miniball"]


def miniball(points: np.typing.ArrayLike):
    """Compute the smallest enclosing ball for a set of points."""

    points = np.ascontiguousarray(points, dtype=np.float64)

    if len(points.shape) != 2:
        msg = f"Input array must be 2-dimensional! Got shape `{points.shape}`."
        raise TypeError(msg)
    return _compute_miniball(points)
