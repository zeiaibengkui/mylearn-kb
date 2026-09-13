#!/usr/bin/env python3
"""AT_dp_v Subtree — generator for this note's data/ dir.

usage:  python3 generator.py <id>        # id in 1..8, writes that test to stdout
        python3 generator.py all        # regenerate every .in in this dir

The matching <id>.ans is the stdout of the note's reference solution
(../sol_AT_dp_v.cpp) on that input.  No sample from problem.md is copied here.

Constraints: 1 <= N <= 1e5, 2 <= M <= 1e9, the N-1 edges form a tree on
1..N (1-based, so a test with N=1 has no edge line at all).

tests
  1  N=1, M=100                    -> single vertex
  2  N=2 path, M=100
  3  N=5 small tree, M=100
  4  N=100000 path, M=1e9
  5  N=100000 star, M=1e9
  6  N=100000 random tree, M=2      -> answers collapse to 0/1
  7  N=100000 random tree, M=999999937
  8  N=100000 caterpillar (deep spine + leaves), M=1e9
"""

import random
import sys


def emit(n, m, edges):
    out = [f"{n} {m}"]
    out += [f"{u} {v}" for u, v in edges]
    return "\n".join(out) + "\n"


def gen(tid):
    if tid == 1:
        return emit(1, 100, [])
    if tid == 2:
        return emit(2, 100, [(1, 2)])
    if tid == 3:
        return emit(5, 100, [(1, 2), (1, 3), (3, 4), (3, 5)])
    if tid == 4:
        n = 100000
        return emit(n, 10**9, [(i, i + 1) for i in range(1, n)])
    if tid == 5:
        n = 100000
        return emit(n, 10**9, [(1, i) for i in range(2, n + 1)])
    if tid == 6:
        rng = random.Random(20240000)
        n = 100000
        return emit(n, 2, [(rng.randint(1, i - 1), i) for i in range(2, n + 1)])
    if tid == 7:
        rng = random.Random(20240001)
        n = 100000
        return emit(n, 999999937, [(rng.randint(1, i - 1), i) for i in range(2, n + 1)])
    rng = random.Random(20240002)
    n = 100000
    spine = n // 2
    edges = [(i, i + 1) for i in range(1, spine)]
    for v in range(spine + 1, n + 1):
        edges.append((rng.randint(1, spine), v))
    return emit(n, 10**9, edges)


def main():
    arg = sys.argv[1] if len(sys.argv) > 1 else "1"
    if arg == "all":
        for tid in range(1, 9):
            with open(f"{tid}.in", "w") as fh:
                fh.write(gen(tid))
        return
    sys.stdout.write(gen(int(arg)))


if __name__ == "__main__":
    main()
