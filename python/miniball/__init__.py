from ._miniball import (
    _Miniball,
    _compute_miniball,
    _compute_miniball_incremental,
)

import numpy as np

__all__ = ["miniball", "incremental_miniball", "Miniball"]


def miniball(points: np.typing.ArrayLike):
    """Compute the smallest enclosing ball for a set of points."""

    points = np.ascontiguousarray(points, dtype=np.float64)

    if len(points.shape) != 2:
        msg = f"Input array must be 2-dimensional! Got shape `{points.shape}`."
        raise TypeError(msg)
    return _compute_miniball(points)


def incremental_miniball(
    points: np.typing.ArrayLike,
    current: dict,
    point: np.typing.ArrayLike,
):
    """Compute the miniball after appending one point to an existing point set."""

    points = np.ascontiguousarray(points, dtype=np.float64)
    if len(points.shape) != 2:
        msg = f"Input array must be 2-dimensional! Got shape `{points.shape}`."
        raise TypeError(msg)

    point = np.ascontiguousarray(point, dtype=np.float64)
    if len(point.shape) != 1:
        msg = f"New point must be 1-dimensional! Got shape `{point.shape}`."
        raise TypeError(msg)

    center = np.ascontiguousarray(current["center"], dtype=np.float64)
    radius_squared = float(current["radius_squared"])
    delta = point - center
    distance_squared = float(np.dot(delta, delta))

    # If the new point is already enclosed, the previous miniball is unchanged.
    if distance_squared <= radius_squared:
        return current

    # Otherwise the native helper starts from a ball with point on the boundary.
    return _compute_miniball_incremental(points, center, radius_squared, point)


class Miniball:
    """Maintain a miniball as points are appended."""

    def __init__(self, points: np.typing.ArrayLike):
        points = np.ascontiguousarray(points, dtype=np.float64)
        if len(points.shape) != 2:
            msg = f"Input array must be 2-dimensional! Got shape `{points.shape}`."
            raise TypeError(msg)
        self._miniball = _Miniball(points)

    def add(self, point: np.typing.ArrayLike):
        point = np.ascontiguousarray(point, dtype=np.float64)
        if len(point.shape) != 1:
            msg = f"New point must be 1-dimensional! Got shape `{point.shape}`."
            raise TypeError(msg)
        return self._miniball.add(point)

    def add_points(self, points: np.typing.ArrayLike):
        points = np.ascontiguousarray(points, dtype=np.float64)
        if len(points.shape) != 2:
            msg = f"Input array must be 2-dimensional! Got shape `{points.shape}`."
            raise TypeError(msg)
        return self._miniball.add_points(points)

    def result(self):
        return self._miniball.result()
