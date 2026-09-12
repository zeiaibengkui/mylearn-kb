---
title: Solution sol
---
Submitted 2026-09-08 — all 1 sample case(s) passed.

```cpp
#include <bits/stdc++.h>
using namespace std;
typedef long long LL;

// USACO Landscaping (Gold/Platinum) — min cost to turn A into B.
// Operations: buy a unit (X), remove a unit (Y), or move a unit between
// beds i,j at cost Z*|i-j|.
//
// Idea: "unroll" each bed into |A_i - B_i| unit points on the number line
// (a surplus point for A_i>B_i, a deficit point for A_i<B_i), then match
// source->target points.  Edit-distance style: matching a source at s with a
// target at t costs min(Z*|s-t|, X+Y) (a distance that isn't worth moving is a
// buy+remove), unmatched sources cost Y, unmatched targets cost X.  In an
// optimal matching, edges do not cross; grouping by "level" makes each level an
// alternating source/target sequence, and each level is solved independently.

const int MAX_TOT = 100000 * 10;   // N_max * K_max
static vector<int> rows[2 * MAX_TOT + 5];  // rows[lvl] : positions, alternating source/target
static int rtype[2 * MAX_TOT + 5];         // +1 source, -1 target (for the unmatched odd element)
static LL res1[2 * MAX_TOT + 5], res2[2 * MAX_TOT + 5];

int N;
LL X, Y, Z;

// dp over an alternating sequence v[0..M-1] (+/-), all matched.
// results[j] (j odd) = min cost to fully match v[0..j].
// Pairing is non-crossing; for a matched pair we either use a direct
// consecutive edge ("short") or a "long" edge capped at X+Y that swallows a
// balanced inner region.
static void dp(const vector<int>& v, LL* results) {
    const LL M = (LL)v.size();
    LL i = -2, prefcost_i = 0, prefcost_j = 0, prevLong = LLONG_MAX / 4;
    for (LL j = 1; j < M; j += 2) {
        if (j > 1) {
            LL cost = Z * abs(v[j - 2] - v[j - 1]);  // edge (j-2, j-1)
            prefcost_j += cost;
            prevLong += cost;
        }
        // find the cheapest "long edge" (i, j): its inner region is matched
        // consecutively, the prefix before i is optimally matched.
        while (i + 2 < j && X + Y <= Z * abs(v[j] - v[i + 2])) {
            i += 2;
            if (i > 0) prefcost_i += Z * abs(v[i] - v[i - 1]);
            prevLong = min(prevLong,
                X + Y + (prefcost_j - prefcost_i) + (i > 0 ? results[i - 1] : 0));
        }
        // short edge (j-1, j) plus optimally matched prefix
        results[j] = min(prevLong,
            Z * abs(v[j] - v[j - 1]) + (j > 1 ? results[j - 2] : 0));
    }
}

// Solve one level.  ecost = cost of the single unmatched element (Y if it's a
// surplus source, X if it's a deficit target).
static LL solveLevel(vector<int>& v, LL ecost) {
    const LL M = (LL)v.size();
    if (M == 0) return 0;
    if (M == 1) return ecost;

    dp(v, res1);
    reverse(v.begin(), v.end());
    dp(v, res2);
    reverse(res2, res2 + M);
    reverse(v.begin(), v.end());   // restore for any caller

    if (M % 2 == 0) return res1[M - 1];            // all matched
    // odd: one element left out; split into balanced prefix + that element +
    // balanced suffix.
    LL best = ecost + min(res1[M - 2], res2[1]);
    for (LL i = 2; i <= M - 3; i += 2)
        best = min(best, res1[i - 1] + ecost + res2[i + 1]);
    return best;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    if (!(cin >> N >> X >> Y >> Z)) return 0;

    memset(rtype, 0, sizeof(rtype));
    for (int lvl = 0; lvl < 2 * MAX_TOT + 5; lvl++) rows[lvl].clear();

    int lastDir = 0, level = MAX_TOT;
    for (int i = 1; i <= N; i++) {
        int a, b;
        cin >> a >> b;
        int dir = (max(a, b) == a) ? +1 : -1;   // +1 surplus, -1 deficit
        int m = max(a, b) - min(a, b);
        while (m-- > 0) {
            if (lastDir == dir) level += dir;    // same sign climbs a level
            if (rtype[level] == 0) rtype[level] = dir;
            rows[level].push_back(i);
            lastDir = dir;
        }
    }

    LL total = 0;
    for (int lvl = 0; lvl < 2 * MAX_TOT + 5; lvl++)
        total += solveLevel(rows[lvl], rtype[lvl] > 0 ? Y : X);

    cout << total << '\n';
    return 0;
}
```
