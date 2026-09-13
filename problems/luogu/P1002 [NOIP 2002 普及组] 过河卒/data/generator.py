#!/usr/bin/env python3
"""P1002 [NOIP 2002 普及组] 过河卒 — generator for this note's data/ dir.

usage:  python3 generator.py <id>        # id in 1..8, writes that test to stdout
        python3 generator.py all        # regenerate every .in in this dir

The matching <id>.ans is the stdout of the note's reference solution
(../sol.cpp) on that input.  No sample from problem.md is copied here — every
test is generated, and the horse is allowed to sit outside the board (the
statement only bounds its coordinates by 20).

tests
  1  n=m=1, horse far outside the board
  2  a 1 x 20 strip, horse on the top edge
  3  a 20 x 1 strip, horse on the left edge
  4  full 20 x 20 board, horse in the far corner
  5  full 20 x 20 board, horse in the middle
  6  full 20 x 20 board, horse next to the start (blocking part of the first row/column)
  7  small 3 x 4 board
  8  random board and horse (seeded), start guaranteed uncontrolled
"""

import random
import sys

DX = (-2, -1, 1, 2, 2, 1, -1, -2)
DY = (1, 2, 2, 1, -1, -2, -2, -1)


def controls(hx, hy):
    pts = {(hx, hy)}
    for k in range(8):
        pts.add((hx + DX[k], hy + DY[k]))
    return pts


def gen(tid):
    fixed = {
        1: (1, 1, 20, 20),
        2: (1, 20, 0, 10),
        3: (20, 1, 10, 0),
        4: (20, 20, 20, 20),
        5: (20, 20, 10, 10),
        6: (20, 20, 0, 1),
        7: (3, 4, 1, 1),
    }
    if tid in fixed:
        n, m, hx, hy = fixed[tid]
    else:
        rng = random.Random(20240000 + tid)
        while True:
            n, m = rng.randint(1, 20), rng.randint(1, 20)
            hx, hy = rng.randint(0, 20), rng.randint(0, 20)
            if (0, 0) not in controls(hx, hy):
                break
    return f"{n} {m} {hx} {hy}\n"


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
