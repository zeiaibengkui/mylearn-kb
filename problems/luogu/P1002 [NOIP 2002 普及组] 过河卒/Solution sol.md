---
title: Solution sol
---
Submitted 2026-09-01 — all 1 sample case(s) passed.

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m, hx, hy;
    cin >> n >> m >> hx >> hy;

    const int dx[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
    const int dy[8] = {1, 2, 2, 1, -1, -2, -2, -1};

    vector<vector<char>> blocked(n + 1, vector<char>(m + 1, 0));
    blocked[hx][hy] = 1;
    for (int k = 0; k < 8; ++k) {
        int x = hx + dx[k], y = hy + dy[k];
        if (0 <= x && x <= n && 0 <= y && y <= m) blocked[x][y] = 1;
    }

    vector<vector<long long>> dp(n + 1, vector<long long>(m + 1, 0));
    if (!blocked[0][0]) dp[0][0] = 1;

    for (int i = 0; i <= n; ++i) {
        for (int j = 0; j <= m; ++j) {
            if (i == 0 && j == 0) continue;
            if (blocked[i][j]) continue;
            if (i > 0) dp[i][j] += dp[i - 1][j];
            if (j > 0) dp[i][j] += dp[i][j - 1];
        }
    }

    cout << dp[n][m] << '\n';
    return 0;
}
```
