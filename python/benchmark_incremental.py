from __future__ import annotations

import argparse
import csv
import statistics
import time
from pathlib import Path

import numpy as np

from miniball import Miniball, miniball


def unit_direction(rng: np.random.Generator, dim: int) -> np.ndarray:
    direction = rng.normal(size=dim)
    norm = np.linalg.norm(direction)
    if norm == 0:
        direction[0] = 1.0
        return direction
    return direction / norm


def generate_outside_sequence(
    rng: np.random.Generator,
    initial_points: np.ndarray,
    count: int,
    distance_factor: float,
) -> np.ndarray:
    points = np.asarray(initial_points, dtype=np.float64)
    stream = Miniball(points)
    current = stream.result()
    additions = []

    for _ in range(count):
        center = np.asarray(current["center"], dtype=np.float64)
        radius = float(current["radius"])
        distance = distance_factor * radius if radius > 0 else distance_factor
        point = center + distance * unit_direction(rng, points.shape[1])
        additions.append(point)

        points = np.vstack([points, point])
        current = stream.add(point)

    return np.asarray(additions, dtype=np.float64)


def time_full_recomputation(
    all_points: np.ndarray, initial_count: int
) -> tuple[float, list[np.ndarray], dict[str, float]]:
    centers = []
    iterations = []
    start = time.perf_counter()
    for size in range(initial_count + 1, len(all_points) + 1):
        current = miniball(all_points[:size])
        centers.append(np.asarray(current["center"]))
        iterations.append(int(current["iterations"]))
    elapsed = time.perf_counter() - start
    return elapsed, centers, iteration_summary(iterations, "full")


def time_incremental(
    all_points: np.ndarray, initial_count: int, centers: list[np.ndarray]
) -> tuple[float, dict[str, float]]:
    stream = Miniball(all_points[:initial_count])
    iterations = []

    start = time.perf_counter()
    for index in range(initial_count, len(all_points)):
        current = stream.add(all_points[index])
        iterations.append(int(current["iterations"]))
        np.testing.assert_allclose(
            current["center"],
            centers[index - initial_count],
            rtol=1e-10,
            atol=1e-10,
        )
    elapsed = time.perf_counter() - start
    return elapsed, iteration_summary(iterations, "update")


def iteration_summary(iterations: list[int], prefix: str) -> dict[str, float]:
    return {
        f"{prefix}_max_iterations": max(iterations) if iterations else 0,
        f"{prefix}_median_iterations": (
            statistics.median(iterations) if iterations else 0
        ),
        f"{prefix}_average_iterations": (
            statistics.fmean(iterations) if iterations else 0.0
        ),
    }


def run_case(
    rng: np.random.Generator,
    dim: int,
    additions_count: int,
    initial_count: int,
    distance_factor: float,
) -> dict[str, float]:
    initial_points = rng.normal(size=(initial_count, dim))
    additions = generate_outside_sequence(
        rng, initial_points, additions_count, distance_factor
    )
    all_points = np.vstack([initial_points, additions])

    full_seconds, centers, full_iteration_stats = time_full_recomputation(
        all_points, initial_count
    )
    print('Finished full recomputation for dimension {}, point insertions {}'.format(dim, additions_count))
    print('Full recomputation took {:.4f} seconds'.format(full_seconds))
    print(
        "Full recomputation iterations: max {}, median {:.1f}, average {:.2f}".format(
            full_iteration_stats["full_max_iterations"],
            full_iteration_stats["full_median_iterations"],
            full_iteration_stats["full_average_iterations"],
        )
    )
    update_seconds, iteration_stats = time_incremental(
        all_points, initial_count, centers
    )
    print('Finished updates for dimension {}, point insertions {}'.format(dim, additions_count))
    print('Updates took {:.4f} seconds'.format(update_seconds))
    print(
        "Iterations per update: max {}, median {:.1f}, average {:.2f}".format(
            iteration_stats["update_max_iterations"],
            iteration_stats["update_median_iterations"],
            iteration_stats["update_average_iterations"],
        )
    )
    return {
        "dimension": dim,
        "additions": additions_count,
        "full_seconds": full_seconds,
        "update_seconds": update_seconds,
        "speedup": full_seconds / update_seconds
        if update_seconds > 0
        else float("inf"),
        **full_iteration_stats,
        **iteration_stats,
    }


def write_csv(path: Path, rows: list[dict[str, float]]) -> None:
    with path.open("w", newline="") as file:
        writer = csv.DictWriter(file, fieldnames=list(rows[0].keys()))
        writer.writeheader()
        writer.writerows(rows)


def print_table(title: str, rows: list[dict[str, float]], x_key: str) -> None:
    print(f"\n{title}")
    print("-" * len(title))
    print(
        f"{x_key:>10}  {'full (s)':>12}  {'updates (s)':>17}  "
        f"{'speedup':>10}  {'full max':>8}  {'full med':>8}  "
        f"{'full avg':>8}  {'upd max':>8}  {'upd med':>8}  {'upd avg':>8}"
    )
    for row in rows:
        print(
            f"{int(row[x_key]):>10}  "
            f"{row['full_seconds']:>12.4f}  "
            f"{row['update_seconds']:>17.4f}  "
            f"{row['speedup']:>10.2f}  "
            f"{row['full_max_iterations']:>8.0f}  "
            f"{row['full_median_iterations']:>8.1f}  "
            f"{row['full_average_iterations']:>8.2f}  "
            f"{row['update_max_iterations']:>8.0f}  "
            f"{row['update_median_iterations']:>8.1f}  "
            f"{row['update_average_iterations']:>8.2f}"
        )


def plot_results(path: Path, title: str, x_label: str, x_values, rows) -> None:
    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    full = [row["full_seconds"] for row in rows]
    updates = [row["update_seconds"] for row in rows]

    fig, ax = plt.subplots(figsize=(8, 5))
    ax.plot(x_values, full, marker="o", label="Full Recomputation")
    ax.plot(x_values, updates, marker="o", label="Warm Started Balls")
    ax.set_title(title)
    ax.set_xlabel(x_label)
    ax.set_ylabel("Runtime (seconds)")
    ax.grid(True, which="both", alpha=0.3)
    ax.legend()
    fig.tight_layout()
    fig.savefig(path, dpi=200)
    plt.close(fig)


def plot_iteration_results(
    path: Path, title: str, x_label: str, x_values, rows
) -> None:
    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    full_average = [row["full_average_iterations"] for row in rows]
    updates_average = [row["update_average_iterations"] for row in rows]

    fig, ax = plt.subplots(figsize=(8, 5))
    ax.plot(x_values, full_average, marker="o", label="Full Recomputation")
    ax.plot(x_values, updates_average, marker="o", label="Warm Started Balls")
    ax.set_title(title)
    ax.set_xlabel(x_label)
    ax.set_ylabel("Avg. # of Iterations")
    ax.grid(True, which="both", alpha=0.3)
    ax.legend()
    fig.tight_layout()
    fig.savefig(path, dpi=200)
    plt.close(fig)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output-dir", default="benchmark_results")
    parser.add_argument("--seed", type=int, default=0)
    parser.add_argument("--initial-count", type=int, default=100)
    parser.add_argument("--distance-factor", type=float, default=1.05)
    args = parser.parse_args()

    output_dir = Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)
    rng = np.random.default_rng(args.seed)

    dimension_rows = [
        run_case(rng, dim, 500, args.initial_count, args.distance_factor)
        for dim in [5, 10, 50, 100, 500, 1000]
    ]
    write_csv(output_dir / "vary_dimension.csv", dimension_rows)
    print_table("Vary dimension, 500 point insertions", dimension_rows, "dimension")
    plot_results(
        output_dir / "vary_dimension.png",
        "Runtime vs dimension, 500 point insertions",
        "# of Dimensions",
        [row["dimension"] for row in dimension_rows],
        dimension_rows,
    )
    plot_iteration_results(
        output_dir / "vary_dimension_iterations.png",
        "Iterations vs dimension, 500 point insertions",
        "# of Dimensions",
        [row["dimension"] for row in dimension_rows],
        dimension_rows,
    )

    addition_rows = [
        run_case(rng, 500, additions, args.initial_count, args.distance_factor)
        for additions in [5, 10, 50, 100, 500, 1000]
    ]
    write_csv(output_dir / "vary_additions.csv", addition_rows)
    print_table("Vary point insertions, 500 dimensions", addition_rows, "additions")
    plot_results(
        output_dir / "vary_additions.png",
        "Runtime vs point insertions, 500 dimensions",
        "# of Points Added",
        [row["additions"] for row in addition_rows],
        addition_rows,
    )
    plot_iteration_results(
        output_dir / "vary_additions_iterations.png",
        "Iterations vs point insertions, 500 dimensions",
        "# of Points Added",
        [row["additions"] for row in addition_rows],
        addition_rows,
    )

    print(f"\nWrote results to {output_dir.resolve()}")


if __name__ == "__main__":
    main()
