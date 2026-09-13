#!/usr/bin/env python3
"""P1792 [国家集训队] 种树 — generator for this note's data/ dir.

usage:  python3 generator.py <id>        # id in 1..8, writes that test to stdout
        python3 generator.py all        # regenerate every .in in this dir

The matching <id>.ans is the stdout of the note's reference solution
(../sol.cpp) on that input — including the `Error!` cases, which is why some
tests deliberately use m > n/2.  No sample from problem.md is copied here.

tests
  1  n=m=1                      -> m > n/2, no valid plan
  2  n=2, m=1                   -> the smallest non-trivial ring
  3  n=5, m=2, mixed signs
  4  n=6, m=3, all positive
  5  n=7, m=4                   -> Error! (needs floor(7/2) = 3 at most)
  6  n=10, m=5, all negative    -> forced to take the 5 least bad positions
  7  n=200000, m=100000         -> upper bound, random values
  8  n=199999, m=99999, random values, one huge value surrounded by huge ones
"""

import random
import sys


def gen(tid):
    if tid == 1:
        return "1 1\n-1000\n"
    if tid == 2:
        return "2 1\n7 -3\n"
    if tid == 3:
        return "5 2\n-4 9 -4 9 -1000\n"
    if tid == 4:
        return "6 3\n1 2 3 4 5 6\n"
    if tid == 5:
        return "7 4\n1 2 3 4 5 6 7\n"
    if tid == 6:
        return "10 5\n" + " ".join(["-1000"] * 9 + ["-999"]) + "\n"
    if tid == 7:
        rng = random.Random(20241792)
        n, m = 200000, 100000
        vals = [rng.randint(-1000, 1000) for _ in range(n)]
        return f"{n} {m}\n" + " ".join(map(str, vals)) + "\n"
    if tid == 8:
        rng = random.Random(20241793)
        n, m = 199999, 99999
        vals = [rng.randint(-1000, 1000) for _ in range(n)]
        # the three largest values sit next to each other, so the greedy has to
        # regret: the two outer ones beat the middle one
        p = rng.randrange(n - 2)
        vals[p], vals[p + 1], vals[p + 2] = 1000, 999, 1000
        return f"{n} {m}\n" + " ".join(map(str, vals)) + "\n"
    rng = random.Random(20240000 + tid)
    n = rng.randint(3, 20)
    m = rng.randint(1, n // 2)
    vals = [rng.randint(-1000, 1000) for _ in range(n)]
    return f"{n} {m}\n" + " ".join(map(str, vals)) + "\n"


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
