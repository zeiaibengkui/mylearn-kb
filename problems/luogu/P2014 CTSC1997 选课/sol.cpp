// P2014 [CTSC1997] 选课
// 树形背包（依赖背包）：j 门课需要选 j-1 门真正的课 + 虚拟根
// dp[u][j] = 在 u 的子树里选 j 门课、且 u 必选时的最大学分
#include <bits/stdc++.h>
using namespace std;

const int NEG = -1e9;
int n, m;
vector<int> ch[305];
int s[305];
int dp[305][305];  // dp[u][j], j = 0..sz[u]
int sz[305];

void dfs(int u) {
    // u must be taken: exactly one course consumed
    for (int j = 0; j <= m + 1; ++j) dp[u][j] = NEG;
    dp[u][1] = s[u];  // s[0] = 0 for the virtual root
    sz[u] = 1;
    for (int v : ch[u]) {
        dfs(v);
        for (int j = min(sz[u], m + 1); j >= 1; --j) {
            if (dp[u][j] == NEG) continue;
            for (int k = 1; k <= min(sz[v], m + 1 - j); ++k) {
                if (dp[v][k] == NEG) continue;
                dp[u][j + k] = max(dp[u][j + k], dp[u][j] + dp[v][k]);
            }
        }
        sz[u] = min(sz[u] + sz[v], m + 1);
    }
}

int main() {
    scanf("%d %d", &n, &m);
    for (int i = 1; i <= n; ++i) {
        int k, sc;
        scanf("%d %d", &k, &sc);
        s[i] = sc;
        ch[k].push_back(i);  // k = 0 -> virtual root
    }
    if (m > n) {
        printf("0\n");
        return 0;
    }
    dfs(0);
    printf("%d\n", max(0, dp[0][m + 1]));
    return 0;
}
