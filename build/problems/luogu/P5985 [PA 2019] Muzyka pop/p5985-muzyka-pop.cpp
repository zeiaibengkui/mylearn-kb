// P5985 [PA 2019] Muzyka pop
//
// Choose 0 <= b_1 < ... < b_n <= m maximizing sum a_i * popcount(b_i).
//
// Idea: the values are the leaves of the complete binary tree over bits
// [B0..0], ordered left (small) to right (large). The chosen b_i are n
// leaves assigned to rows 1..n in that order. At the node for bit B, every
// row in its LEFT half has bit B = 0 and every row in its RIGHT half has
// bit B = 1; since rows are increasing, each contiguous segment of rows
// shows the bit-B pattern 0*1* — a prefix of the segment goes left, a
// suffix goes right (a single split point k; rows to the right gain one
// popcount, i.e. +a_i).
//
// dp(B, l, r, tight): max value for rows l..r assigned to the subtree at
// bit B (values within [0, 2^(B+1))), "tight" meaning the subtree range is
// clamped by m's prefix already matched (every value must stay <= m).
//   B < 0        -> 0 if the segment is a single row (one exact value),
//                   else -inf (two rows would collide on the same value).
//   tight, m bit B = 0 -> rows cannot have bit B = 1: all go left, tight.
//   otherwise    -> for each split k (l..r+1):
//                   dp(B-1, l, k-1, 0) + dp(B-1, k, r, tight?1:0)
//                   + (a_k + ... + a_r)
//
// Complexity: O(B0 * n^3) transitions (~8e7 at the limits).

#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

static const int64 NEG = -(int64)4e18;  // infeasible subtree
static const int64 UNSET = LLONG_MIN;   // memo: not computed yet

int64 solve(int n, int64 m, const vector<int64>& a) {
    const int B0 = 60;  // m <= 1e18 < 2^60
    const int W = n + 2;

    vector<int64> pref(n + 1);
    for (int i = 0; i < n; ++i) pref[i + 1] = pref[i] + a[i];
    auto rangeSum = [&](int l, int r) -> int64 {  // inclusive; empty -> 0
        return pref[r + 1] - pref[l];
    };

    vector<array<int64, 2>> memo((B0 + 1) * W * W);
    for (auto& c : memo) c = {UNSET, UNSET};
    auto key = [&](int B, int l, int r) { return (B * W + l) * W + r; };

    auto dp = [&](auto&& self, int B, int l, int r, int tight) -> int64 {
        if (l > r) return 0;
        if (B < 0) return l == r ? 0 : NEG;
        int64& res = memo[key(B, l, r)][tight];
        if (res != UNSET) return res;
        const int mbit = (int)((m >> B) & 1);
        int64 best = NEG;
        if (tight && !mbit) {
            // m's bit is 0 here: every value in range must have bit B = 0.
            best = self(self, B - 1, l, r, 1);
        } else {
            // bit B = 0 (left child: now always strictly below m's prefix)
            // or bit B = 1 (right child: only possible when tight's m bit is 1)
            for (int k = l; k <= r + 1; ++k) {
                int64 L = self(self, B - 1, l, k - 1, 0);
                int64 R = self(self, B - 1, k, r, tight ? 1 : 0);
                if (L <= NEG / 2 || R <= NEG / 2) continue;
                best = max(best, L + R + rangeSum(k, r));
            }
        }
        return res = best;
    };

    return dp(dp, B0, 0, n - 1, 1);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    int64 m;
    if (!(cin >> n >> m)) return 0;
    vector<int64> a(n);
    for (auto& x : a) cin >> x;
    cout << solve(n, m, a) << "\n";
}
