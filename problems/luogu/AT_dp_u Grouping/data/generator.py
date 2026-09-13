#!/usr/bin/env python3
"""AT_dp_u Grouping — generator for this note's data/ dir.

usage:  python3 generator.py <id>        # id in 1..8, writes that test to stdout
        python3 generator.py all        # regenerate every .in in this dir

The matching <id>.ans is the stdout of the note's reference solution
(../sol.cpp) on that input.  No sample from problem.md is copied here — the
notes of this problem currently carry a mangled sample-1 block, which is one
more reason the data here is generated independently.

Constraints: N <= 16, a_ii = 0, a_ij = a_ji, |a_ij| <= 1e9.

tests
  1  N=1
  2  N=2, positive pair
  3  N=3, one positive and one negative pair
  4  N=8, random small values
  5  N=16, all 1e9                     -> one big group, ~1.2e11
  6  N=16, all -1e9                    -> singletons, answer 0
  7  N=16, random values in +-1e9
  8  N=16, two positive cliques joined by negative edges
"""

import random
import sys


def emit(a):
    n = len(a)
    return f"{n}\n" + "".join(" ".join(map(str, row)) + "\n" for row in a)


def gen(tid):
    if tid == 1:
        return emit([[0]])
    if tid == 2:
        return emit([[0, 7], [7, 0]])
    if tid == 3:
        return emit([[0, 5, -3], [5, 0, -4], [-3, -4, 0]])
    if tid == 5:
        n = 16
        return emit([[0 if i == j else 10**9 for j in range(n)] for i in range(n)])
    if tid == 6:
        n = 16
        return emit([[0 if i == j else -(10**9) for j in range(n)] for i in range(n)])
    if tid == 8:
        rng = random.Random(20240002)
        n = 16
        a = [[0] * n for _ in range(n)]
        for i in range(n):
            for j in range(i + 1, n):
                same = (i < 8) == (j < 8)
                v = rng.randint(1, 10**9) if same else -rng.randint(1, 10**9)
                a[i][j] = a[j][i] = v
        return emit(a)
    if tid == 4:
        rng = random.Random(20240000)
        n, lo, hi = 8, -10, 10
    else:
        rng = random.Random(20240001)
        n, lo, hi = 16, -(10**9), 10**9
    a = [[0] * n for _ in range(n)]
    for i in range(n):
        for j in range(i + 1, n):
            v = rng.randint(lo, hi)
            a[i][j] = a[j][i] = v
    return emit(a)


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
