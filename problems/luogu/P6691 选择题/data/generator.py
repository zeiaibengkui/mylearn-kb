#!/usr/bin/env python3
"""P6691 选择题 — generator for this note's data/ dir.

usage:  python3 generator.py <id>        # id in 1..8, writes that test to stdout
        python3 generator.py all        # regenerate every .in in this dir

The matching <id>.ans is the stdout of the note's reference solution
(../sol.cpp) on that input.  No sample from problem.md is copied here.

Constraints: n <= 1e6, 1 <= a_i <= n, a_i != i, opt_i in {0,1}.  Each option
gives one XOR constraint colour[i] XOR colour[a_i] = 1 - opt_i, so a test is
`No answer` exactly when those constraints are inconsistent.

tests
  1  n=2, mutually referring pair             -> consistent, one component
  2  n=4, a 4-cycle that has no answer
  3  n=8, four independent pairs (2^4 answers)
  4  n=1000, random a_i != i, random opt
  5  n=1000000, random a_i != i, random opt (upper bound)
  6  n=1000000, one n-cycle with all opt=1    -> consistent, count = 2
  7  n=300000, perfect matching (i<->j), same opt both sides -> 2^(n/2) answers
  8  n=300000, random functional graph with every cycle made consistent
"""

import random
import sys


def emit(rows):
    return f"{len(rows)}\n" + "".join(f"{a} {o}\n" for a, o in rows)


def gen(tid):
    if tid == 1:
        return emit([(2, 1), (1, 1)])
    if tid == 2:
        # 1->2->3->4->1 with all opt=1: the XOR around the even cycle must be 0
        return emit([(3, 1), (4, 1), (1, 1), (2, 1)])
    if tid == 3:
        return emit([(2, 1), (1, 1), (4, 0), (3, 0), (6, 0), (5, 0), (8, 1), (7, 1)])
    if tid == 4:
        rng = random.Random(20246691)
        n = 1000
        rows = []
        for i in range(1, n + 1):
            while True:
                j = rng.randint(1, n)
                if j != i:
                    break
            rows.append((j, rng.randint(0, 1)))
        return emit(rows)
    if tid == 5:
        rng = random.Random(20246692)
        n = 1000000
        rows = []
        for i in range(1, n + 1):
            j = rng.randint(1, n - 1)
            if j >= i:
                j += 1
            rows.append((j, rng.randint(0, 1)))
        return emit(rows)
    if tid == 6:
        n = 1000000
        return emit([(i % n + 1, 1) for i in range(1, n + 1)])
    if tid == 7:
        rng = random.Random(20246694)
        n = 300000
        rows = [(0, 0)] * n
        for i in range(1, n + 1, 2):
            o = rng.randint(0, 1)
            rows[i - 1] = (i + 1, o)
            rows[i] = (i, o)
        return emit(rows)
    rng = random.Random(20246695)
    n = 300000
    a = [0] * (n + 1)
    for i in range(1, n + 1):
        j = rng.randint(1, n - 1)
        if j >= i:
            j += 1
        a[i] = j
    # draw a random colouring and derive the options from it: consistent by
    # construction, but with a non-trivial mix of 0/1 options
    colour = [rng.randint(0, 1) for _ in range(n + 1)]
    return emit([(a[i], 1 - (colour[i] ^ colour[a[i]])) for i in range(1, n + 1)])


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
