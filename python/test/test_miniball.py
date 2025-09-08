import pytest
from miniball import miniball
import numpy as np


def test_package():
    from miniball import miniball

    assert miniball.__doc__ is not None and (
        "Compute the smallest enclosing ball for a set of points." in miniball.__doc__
    )


def test_empty_vector():
    with pytest.raises(TypeError, match="Input array must be 2-dimensional"):
        miniball(None)


def test_identical_points():
    point = [3.0, 1.0, 0.0]
    test_vector = np.array([point, point], dtype=np.double)
    res = miniball(test_vector)
    np.testing.assert_array_equal(res["center"], point)
    assert res["radius"] == 0
    assert res["radius_squared"] == 0


def test_two_points():
    test_vector = np.array([[3.0, 1.0], [3.0, 1.0], [1.0, 0.0]], dtype=np.double)
    res = miniball(test_vector)
    np.testing.assert_allclose(res["center"], [2.0, 0.5])
    assert res["radius_squared"] == 1.25


def test_three_points():
    test_vector = [[0, 1], [1, 0], [1, 1]]
    res = miniball(test_vector)
    np.testing.assert_allclose(res["center"], [0.5, 0.5])
    assert res["radius_squared"] == 0.5
