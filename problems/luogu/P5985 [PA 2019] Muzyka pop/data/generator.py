#!/usr/bin/env python3
"""P5985 [PA 2019] Muzyka pop — generator for this note's data/ dir.

usage:  python3 generator.py <id>        # id in 1..8, writes that test to stdout
        python3 generator.py all        # regenerate every .in in this dir

The matching <id>.ans is the stdout of the note's reference solution
(../p5985-muzyka-pop.cpp) on that input.  No sample from problem.md is copied
here.

Constraints: 1 <= n <= 200, n-1 <= m <= 1e18, |a_i| <= 1e14.  With m >= n-1
the b_i can always be chosen (0..n-1 works), so there is no `no answer` case.

tests
  1  n=1, m=1, positive a                  -> take b=1 to gain popcount
  2  n=1, m=1e18, very negative a          -> forced to b=0
  3  n=3, m=4, mixed signs
  4  n=200, m=1e18, random a in +-1e14
  5  n=200, m=199 (minimal range: b_i is forced to 0..198)
  6  n=2, m=1e18, one huge positive and one huge negative
  7  n=200, m=1e18, all a_i = 1e14 (answer is near the 64-bit edge)
  8  n=50, m=1e18, alternating huge signs
"""

import random
import sys


def emit(n, m, a):
    return f"{n} {m}\n" + " ".join(map(str, a)) + "\n"


def gen(tid):
    if tid == 1:
        return emit(1, 1, [5])
    if tid == 2:
        return emit(1, 10**18, [-(10**14)])
    if tid == 3:
        return emit(3, 4, [3, -2, 5])
    if tid == 4:
        rng = random.Random(20245985)
        n = 200
        a = [rng.randint(-10**14, 10**14) for _ in range(n)]
        return emit(n, 10**18, a)
    if tid == 5:
        rng = random.Random(20245986)
        n = 200
        a = [rng.randint(-1000, 100) for _ in range(n)]   # mostly negative
        return emit(n, n - 1, a)
    if tid == 6:
        return emit(2, 10**18, [10**14, -(10**14)])
    if tid == 7:
        n = 200
        return emit(n, 10**18, [10**14] * n)
    n = 50
    a = [(10**14 if i % 2 == 0 else -(10**14)) for i in range(n)]
    return emit(n, 10**18, a)


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
