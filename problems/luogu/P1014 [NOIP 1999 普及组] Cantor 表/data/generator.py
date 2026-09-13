#!/usr/bin/env python3
"""P1014 [NOIP 1999 普及组] Cantor 表 — generator for this note's data/ dir.

usage:  python3 generator.py <id>        # id in 1..8, writes that test to stdout
        python3 generator.py all        # regenerate every .in in this dir

The matching <id>.ans is the stdout of the note's reference solution
(../sol.cpp) on that input.  No sample from problem.md is copied here.

The table is numbered along anti-diagonals: diagonal k holds k-1 terms and the
count through diagonal k is k*(k-1)/2, so the tests include the first terms,
diagonal boundaries and the upper end of N <= 1e7.

tests
  1  N = 1 (first term)
  2  N = 2
  3  N = 3
  4  N = 10 (past the sample size, mixed direction)
  5  N = last term of diagonal 1414
  6  N = first term of diagonal 1415
  7  N = 9999999
  8  N = 10000000 (upper bound)
"""

import random
import sys

DIAG = 1414  # 1415*1414/2 = 1000405, small enough to keep boundaries visible


def gen(tid):
    if tid == 1:
        return "1\n"
    if tid == 2:
        return "2\n"
    if tid == 3:
        return "3\n"
    if tid == 4:
        return "10\n"
    if tid == 5:
        return f"{DIAG * (DIAG - 1) // 2}\n"          # last term of that diagonal
    if tid == 6:
        return f"{DIAG * (DIAG - 1) // 2 + 1}\n"      # first term of the next one
    if tid == 7:
        return "9999999\n"
    if tid == 8:
        return "10000000\n"
    rng = random.Random(20240000 + tid)
    return f"{rng.randint(1, 10_000_000)}\n"


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
