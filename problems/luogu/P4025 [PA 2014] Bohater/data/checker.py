#!/usr/bin/env python3
"""P4025 [PA 2014] Bohater — special judge (SPJ) for this note's test data.

usage:  python3 checker.py <test.in> <contestant.out> [<test.ans>]
        exit code 0 = accepted, 1 = wrong answer (reason on stdout)

The problem is SPJ: the second line may be *any* order of the monsters, so a
plain diff against the reference output (.ans) would reject correct answers.
Accepted output:
  * `NIE` — only if the reference also says NIE (existence of an order is a
    property of the input, not of the output);
  * `TAK` followed by a permutation of 1..n such that the HP never drops to 0
    or below: for every monster in that order, hp - d_i > 0, then hp += a_i - d_i.
"""

import sys


def fail(reason):
    print(f"WRONG ANSWER: {reason}")
    sys.exit(1)


def main():
    if len(sys.argv) < 3:
        print(__doc__.strip())
        sys.exit(2)
    in_path, out_path = sys.argv[1], sys.argv[2]
    ans_path = sys.argv[3] if len(sys.argv) > 3 else None

    with open(in_path) as fh:
        n, z = map(int, fh.readline().split())
        monsters = [tuple(map(int, fh.readline().split())) for _ in range(n)]

    with open(out_path) as fh:
        tokens = fh.read().split()

    expected_verdict = None
    if ans_path:
        with open(ans_path) as fh:
            expected_verdict = fh.readline().strip()

    if not tokens:
        fail("empty output")
    verdict = tokens[0]
    if verdict not in ("TAK", "NIE"):
        fail(f"first token is {verdict!r}, expected TAK or NIE")
    if expected_verdict and verdict != expected_verdict:
        fail(f"verdict {verdict!r} but the reference says {expected_verdict!r}")

    if verdict == "NIE":
        if len(tokens) > 1:
            fail("extra output after NIE")
        print("OK (NIE)")
        return

    order = tokens[1:]
    if len(order) != n:
        fail(f"expected {n} monster ids, got {len(order)}")
    try:
        perm = [int(x) for x in order]
    except ValueError:
        fail("monster ids must be integers")
    if sorted(perm) != list(range(1, n + 1)):
        fail("not a permutation of 1..n")

    hp = z
    for i in perm:
        d, a = monsters[i - 1]
        if hp - d <= 0:
            fail(f"HP would be {hp - d} after fighting monster {i} (d={d})")
        hp += a - d
    print(f"OK (TAK, final HP {hp})")


if __name__ == "__main__":
    main()
