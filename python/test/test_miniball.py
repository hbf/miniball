from miniball import miniball
import numpy as np


def test_empty_vector():
    try:
        miniball(None)
    except TypeError:
        assert True


def test_identical_points():
    test_vector = np.array([[3.0, 1.0, 0.0], [3.0, 1.0, 0.0]], dtype=np.double)
    res = miniball(test_vector)
    assert (res["center"] == np.array([[3.0, 1.0, 0.0]])).all()
    assert res["radius"] == 0


def test_two_points():
    test_vector = np.array([[3.0, 1.0], [3.0, 1.0], [1.0, 0.0]], dtype=np.double)
    res = miniball(test_vector)
    assert (res["center"] == np.array([2.0, 0.5])).all()
    assert res["radius_squared"] == 1.25
