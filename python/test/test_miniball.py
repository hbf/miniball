import pytest
from miniball import Miniball, incremental_miniball, miniball, simd_available
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


def test_incremental_point_inside_current_ball():
    test_vector = np.array([[0, 0], [2, 0]], dtype=np.double)
    current = miniball(test_vector)

    res = incremental_miniball(test_vector, current, [1, 0])

    assert res is current


def test_incremental_point_outside_matches_full_recomputation():
    test_vector = np.array([[0, 0], [2, 0], [1, 1]], dtype=np.double)
    point = np.array([1, 4], dtype=np.double)
    current = miniball(test_vector)

    incremental = incremental_miniball(test_vector, current, point)
    full = miniball(np.vstack([test_vector, point]))

    np.testing.assert_allclose(incremental["center"], full["center"])
    assert incremental["radius"] == pytest.approx(full["radius"])
    assert incremental["radius_squared"] == pytest.approx(full["radius_squared"])


def test_incremental_random_high_dimensional_sequence_matches_full_recomputation():
    rng = np.random.default_rng(0)
    points = rng.normal(size=(2000, 100))
    current = miniball(points)

    for point in rng.normal(size=(100, 100)):
        previous = points
        points = np.vstack([points, point])
        current = incremental_miniball(previous, current, point)
        full = miniball(points)

        np.testing.assert_allclose(current["center"], full["center"], rtol=1e-10)
        assert current["radius"] == pytest.approx(full["radius"], rel=1e-10)
        assert current["radius_squared"] == pytest.approx(
            full["radius_squared"], rel=1e-10
        )


def test_stateful_incremental_result_matches_full_recomputation():
    points = np.array([[0, 0], [2, 0], [1, 1]], dtype=np.double)
    stream = Miniball(points)

    np.testing.assert_allclose(stream.result()["center"], miniball(points)["center"])

    for point in np.array([[1, 4], [-1, 2], [3, 3]], dtype=np.double):
        points = np.vstack([points, point])
        current = stream.add(point)
        full = miniball(points)

        np.testing.assert_allclose(current["center"], full["center"])
        assert current["radius"] == pytest.approx(full["radius"])
        assert current["radius_squared"] == pytest.approx(full["radius_squared"])


def test_stateful_incremental_add_points_matches_repeated_add():
    initial = np.array([[0, 0], [2, 0], [1, 1]], dtype=np.double)
    additions = np.array([[1, 4], [-1, 2], [3, 3]], dtype=np.double)
    repeated = Miniball(initial)
    batched = Miniball(initial)

    for point in additions:
        repeated_result = repeated.add(point)
    batched_result = batched.add_points(additions)

    np.testing.assert_allclose(batched_result["center"], repeated_result["center"])
    assert batched_result["radius"] == pytest.approx(repeated_result["radius"])
    assert batched_result["radius_squared"] == pytest.approx(
        repeated_result["radius_squared"]
    )


def test_stateful_incremental_dimension_mismatch():
    stream = Miniball(np.array([[0, 0], [2, 0]], dtype=np.double))

    with pytest.raises(Exception, match="dimension"):
        stream.add([1, 2, 3])


def test_stateful_incremental_rejects_empty_initial_points():
    with pytest.raises(Exception, match="at least one point"):
        Miniball(np.empty((0, 2), dtype=np.double))


def test_simd_availability_flag_is_boolean():
    assert isinstance(simd_available(), bool)


def test_simd_and_scalar_find_same_ball_for_random_high_dimensional_points():
    rng = np.random.default_rng(42)
    points = rng.normal(size=(300, 64))

    scalar = miniball(points, use_simd_if_available=False)
    simd = miniball(points, use_simd_if_available=True)

    np.testing.assert_allclose(simd["center"], scalar["center"], rtol=1e-10)
    assert simd["radius"] == pytest.approx(scalar["radius"], rel=1e-10)
    assert simd["radius_squared"] == pytest.approx(
        scalar["radius_squared"], rel=1e-10
    )


def test_simd_and_scalar_incremental_find_same_ball():
    rng = np.random.default_rng(43)
    points = rng.normal(size=(200, 32))
    point = rng.normal(size=32) * 4
    scalar_current = miniball(points, use_simd_if_available=False)
    simd_current = miniball(points, use_simd_if_available=True)

    scalar = incremental_miniball(
        points, scalar_current, point, use_simd_if_available=False
    )
    simd = incremental_miniball(points, simd_current, point, use_simd_if_available=True)

    np.testing.assert_allclose(simd["center"], scalar["center"], rtol=1e-10)
    assert simd["radius"] == pytest.approx(scalar["radius"], rel=1e-10)
    assert simd["radius_squared"] == pytest.approx(
        scalar["radius_squared"], rel=1e-10
    )


def test_simd_and_scalar_stateful_incremental_find_same_ball():
    rng = np.random.default_rng(44)
    initial = rng.normal(size=(100, 24))
    additions = rng.normal(size=(20, 24))
    scalar = Miniball(initial, use_simd_if_available=False)
    simd = Miniball(initial, use_simd_if_available=True)

    for point in additions:
        scalar_result = scalar.add(point)
        simd_result = simd.add(point)

    np.testing.assert_allclose(simd_result["center"], scalar_result["center"], rtol=1e-10)
    assert simd_result["radius"] == pytest.approx(scalar_result["radius"], rel=1e-10)
    assert simd_result["radius_squared"] == pytest.approx(
        scalar_result["radius_squared"], rel=1e-10
    )
