#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    long long a[16][16];
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            cin >> a[i][j];

    const int FULL = 1 << n;

    // score[mask] = sum of a[i][j] for i < j, both in mask (the score of the
    // mask when it is taken as a single group).
    // Initialise the pair deltas, then a superset i.e. SOS zeta transform so
    // score[mask] accumulates every pair fully contained in mask.
    vector<long long> score(FULL, 0);
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j)
            score[(1 << i) | (1 << j)] += a[i][j];

    for (int b = 0; b < n; ++b)
        for (int mask = 0; mask < FULL; ++mask)
            if (mask & (1 << b))
                score[mask] += score[mask ^ (1 << b)];

    // dp[mask] = best total score from partitioning the rabbits in mask.
    const long long NEG = LLONG_MIN / 4;
    vector<long long> dp(FULL, NEG);
    dp[0] = 0;
    for (int mask = 1; mask < FULL; ++mask) {
        int lb = mask & -mask; // lowest set bit -> representative of its group
        for (int sub = mask; sub; sub = (sub - 1) & mask) {
            if (sub & lb) {
                dp[mask] = max(dp[mask], dp[mask ^ sub] + score[sub]);
            }
        }
    }

    cout << dp[FULL - 1] << '\n';
    return 0;
}
