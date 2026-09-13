#!/usr/bin/env python3
"""P2014 [CTSC1997] 选课 — generator for this note's data/ dir.

usage:  python3 generator.py <id>        # id in 1..8, writes that test to stdout
        python3 generator.py all        # regenerate every .in in this dir

The matching <id>.ans is the stdout of the note's reference solution
(../sol.cpp) on that input.  No sample from problem.md is copied here.

A course's prerequisite is always an *earlier* index (or 0), so the course
graph is a forest and at least one course has k_i = 0 — both guaranteed by the
statement.  Credits are 1..20 and n, m <= 300.

tests
  1  n=m=1, single root course
  2  n=m=2, root + its only child
  3  n=5, m=3, one chain from a root
  4  n=300, m=1, all roots -> pick the single best credit
  5  n=300, m=300, star -> every course can be taken
  6  n=300, m=150, random forest
  7  n=100, m=50, forest of several chains
  8  n=300, m=299, deep chain (prerequisites force almost the whole tree)
"""

import random
import sys


def forest(rng, n, roots):
    """parents[1..n] with parent < i (or 0), plus extra forced roots."""
    order = list(range(1, n + 1))
    rng.shuffle(order)
    par = [0] * (n + 1)
    for pos, node in enumerate(order):
        if pos == 0 or node in roots or rng.random() < 0.3:
            par[node] = 0
        else:
            par[node] = order[rng.randrange(pos)]
    if all(par[i] != 0 for i in range(1, n + 1)):
        par[order[0]] = 0
    return par


def emit(n, m, par, cred):
    lines = [f"{n} {m}"]
    for i in range(1, n + 1):
        lines.append(f"{par[i]} {cred[i]}")
    return "\n".join(lines) + "\n"


def gen(tid):
    if tid == 1:
        return emit(1, 1, [0, 0], [0, 7])
    if tid == 2:
        return emit(2, 2, [0, 0, 1], [0, 3, 4])
    if tid == 3:
        return emit(5, 3, [0, 0, 0, 2, 3, 4], [0, 2, 9, 1, 20, 5])
    if tid == 4:
        rng = random.Random(20242014)
        n = 300
        par = [0] * (n + 1)
        cred = [0] + [rng.randint(1, 20) for _ in range(n)]
        return emit(n, 1, par, cred)
    if tid == 5:
        rng = random.Random(20242015)
        n = 300
        par = [0] + [0] + [1] * (n - 1)
        cred = [0] + [rng.randint(1, 20) for _ in range(n)]
        return emit(n, 300, par, cred)
    if tid == 6:
        rng = random.Random(20242016)
        n = 300
        par = forest(rng, n, set())
        cred = [0] + [rng.randint(1, 20) for _ in range(n)]
        return emit(n, 150, par, cred)
    if tid == 7:
        rng = random.Random(20242017)
        n = 100
        par = [0] * (n + 1)
        i = 1
        while i <= n:                       # several chains hanging off root 0
            length = rng.randint(3, 12)
            prev = 0
            for _ in range(length):
                if i > n:
                    break
                par[i] = prev
                prev = i
                i += 1
        cred = [0] + [rng.randint(1, 20) for _ in range(n)]
        return emit(n, 50, par, cred)
    rng = random.Random(20242018)
    n = 300
    par = [0] * (n + 1)
    for i in range(2, n + 1):
        par[i] = i - 1 if rng.random() < 0.8 else 0
    cred = [0] + [rng.randint(1, 20) for _ in range(n)]
    return emit(n, 299, par, cred)


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
