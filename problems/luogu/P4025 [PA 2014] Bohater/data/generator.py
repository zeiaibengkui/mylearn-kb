#!/usr/bin/env python3
"""P4025 [PA 2014] Bohater — generator for this note's data/ dir.

usage:  python3 generator.py <id>        # id in 1..8, writes that test to stdout
        python3 generator.py all        # regenerate every .in in this dir

The matching <id>.ans is the stdout of the note's reference solution
(../sol.cpp) on that input.  No sample from problem.md is copied here.

!!! This problem is special-judged (SPJ): the second output line is *any*
valid order, so a wrong-order-but-valid answer must not be judged with a plain
diff against the .ans.  data/checker.py is the judge to use with this data
(verdict + permutation + HP simulation); the .ans files are the reference
output kept for reproduction/logging.

Constraints: 1 <= n, z <= 1e5, 0 <= d_i, a_i <= 1e5, HP must stay > 0 after
every fight.

tests
  1  n=1, d=0 (free)                      -> TAK
  2  n=1, d=z                              -> NIE (HP would hit 0)
  3  n=3, the sample-shaped case, mixed gains/losses
  4  n=100000, all gains, generous HP
  5  n=100000, z=1, d=1e5 everywhere       -> NIE
  6  n=100000, random, constructed to be feasible (HP grows fast enough)
  7  n=100000, HP can never exceed 100000 (every earlier monster is a wash),
     then a monster costing 100000 -> NIE exactly on the "HP must stay > 0"
     boundary (HP - d = 0 is already death)
  8  n=100000, all d=a=0                   -> TAK in any order
"""

import random
import sys


def feasible(n, z, monsters):
    """same greedy as the reference — used to keep the TAK cases interesting"""
    gain = sorted((m for m in monsters if m[1] >= m[0]), key=lambda m: m[0])
    loss = sorted((m for m in monsters if m[1] < m[0]), key=lambda m: -m[1])
    hp = z
    for d, a in gain + loss:
        if hp - d <= 0:
            return False
        hp += a - d
    return True


def emit(n, z, monsters):
    return f"{n} {z}\n" + "".join(f"{d} {a}\n" for d, a in monsters)


def gen(tid):
    if tid == 1:
        return emit(1, 1, [(0, 0)])
    if tid == 2:
        return emit(1, 1, [(1, 5)])
    if tid == 3:
        return emit(3, 5, [(3, 1), (4, 8), (8, 3)])
    if tid == 4:
        rng = random.Random(20244025)
        n = 100000
        return emit(n, 100000, [(rng.randint(0, 1000), rng.randint(1000, 100000)) for _ in range(n)])
    if tid == 5:
        n = 100000
        return emit(n, 1, [(100000, 100000)] * n)
    if tid == 6:
        rng = random.Random(20244026)
        n = 100000
        while True:
            ms = [(rng.randint(0, 500), rng.randint(0, 500)) for _ in range(n)]
            if feasible(n, 100000, ms):
                return emit(n, 100000, ms)
    if tid == 7:
        rng = random.Random(20244027)
        n = 100000
        ms = []
        for _ in range(n - 1):
            d = rng.randint(0, 1000)
            ms.append((d, d))                # a == d: HP never grows
        ms.append((100000, 100000))          # HP would drop to exactly 0
        return emit(n, 100000, ms)
    return emit(100000, 1, [(0, 0)] * 100000)


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
