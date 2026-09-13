#!/usr/bin/env python3
"""P2748 [USACO16OPEN] Landscaping P — generator for this note's data/ dir.

usage:  python3 generator.py <id>        # id in 1..8, writes that test to stdout
        python3 generator.py all        # regenerate every .in in this dir

The matching <id>.ans is the stdout of the note's reference solution
(../sol.cpp) on that input.  No sample from problem.md is copied here.

Constraints: n <= 1e5, 0 <= A_i, B_i <= 10, 0 <= X, Y <= 1e8, 0 <= Z <= 1000.

tests
  1  n=1, A=B               -> nothing to do
  2  n=1, borrow-and-dump is cheaper than moving nowhere
  3  n=5, small values, Z=1
  4  n=10, all A=0 B=10, X tiny -> buy everything
  5  n=100000, Z=0          -> transport is free
  6  n=100000, X=Y=1e8, Z=1000, all A=10 B=0 -> dump everything (expensive)
  7  n=100000, X=Y=0        -> free buy/dump, answer 0
  8  n=100000, random, mixed (X=Y=500, Z=7, exactly balanced totals)
"""

import random
import sys


def gen(tid):
    if tid == 1:
        return "1 100 200 1\n5 5\n"
    if tid == 2:
        return "1 3 100 1\n0 4\n"
    if tid == 3:
        return "5 100 200 1\n1 4\n2 3\n3 2\n4 0\n5 0\n"
    if tid == 4:
        return "10 2 5 1000\n" + "".join("0 10\n" for _ in range(10))
    if tid == 5:
        rng = random.Random(20242748)
        n = 100000
        rows = [(rng.randint(0, 10), rng.randint(0, 10)) for _ in range(n)]
        return f"{n} 100 100 0\n" + "".join(f"{a} {b}\n" for a, b in rows)
    if tid == 6:
        n = 100000
        return f"{n} 100000000 100000000 1000\n" + "".join("10 0\n" for _ in range(n))
    if tid == 7:
        rng = random.Random(20242749)
        n = 100000
        rows = [(rng.randint(0, 10), rng.randint(0, 10)) for _ in range(n)]
        return f"{n} 0 0 1000\n" + "".join(f"{a} {b}\n" for a, b in rows)
    rng = random.Random(20242750)
    n = 100000
    a = [rng.randint(0, 10) for _ in range(n)]
    b = a[:]                                  # balanced totals, arbitrary shape
    rng.shuffle(b)
    return f"{n} 500 500 7\n" + "".join(f"{x} {y}\n" for x, y in zip(a, b))


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
