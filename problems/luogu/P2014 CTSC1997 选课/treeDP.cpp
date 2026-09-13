#include <bits/stdc++.h>
using namespace std;
#define pii pair<int, int>
#define fi first
#define se second
const int N = 305;
vector<int> g[N]; // parent is first learned
int dp[N][N]; // dp_ij is the max score of the first j courses in subtree of u
int newdp[N], score[N], sz[N];

int n, m;
void dfs(int u) {
    dp[u][1] = score[u];
    sz[u] = 1;
    for (int v : g[u]) {
        dfs(v);
        vector<int> ndp(sz[u] + sz[v] + 1);
        for (int j = 1; j <= sz[u]; j++) {
            for (int k = 0; k <= sz[v]; k++) {
                ndp[j + k] = max(ndp[j + k], dp[u][j] + dp[v][k]);
            }
        }
        sz[u] += sz[v];
        copy(ndp.begin(), ndp.end(), dp[u]);
    }
}

int main() {
#ifndef ONLINE_JUDGE
    freopen("in", "r", stdin);
#endif
    cin >> n >> m;
    for (int i = 1; i <= n; i++) {
        int k, s;
        cin >> k >> s;
        g[k].push_back(i);
        score[i] = s;
    }
    dfs(0);
    cout << dp[0][m + 1] << '\n';
}
