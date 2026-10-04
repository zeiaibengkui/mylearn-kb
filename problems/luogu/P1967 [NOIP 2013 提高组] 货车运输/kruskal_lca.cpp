// https://www.luogu.com.cn/record/300837785
#include <bits/stdc++.h>
using namespace std;

const int N = 1e4 + 5, M = 5e4 + 5, log2N = 15, inf = 1e5 + 5;
using pii = pair<int, int>;
#define fi first
#define se second
vector<pii> g[N];
int n, m, q;
int fa[N];                            // 并查集
int F[N][log2N], W[N][log2N], dep[N]; // 倍增LCA

int find(int x) { return fa[x] == x ? x : fa[x] = find(fa[x]); }

void dfs_lca(int u) {
    static bool vis[N];
    vis[u] = true;
    for (auto v : g[u]) {
        if (vis[v.se])
            continue;
        dep[v.se] = dep[u] + 1, W[v.se][0] = v.fi;
        F[v.se][0] = u;
        dfs_lca(v.se);
    }
}

void build_lca() {
    for (int i = 1; i <= n; i++) {
        dep[i] = 1;
        F[i][0] = i;
        W[i][0] = inf;
    }
    for (int i = 1; i <= n; i++)
        dfs_lca(i);
    for (int j = 1; j < log2N; j++) {
        for (int i = 1; i <= n; i++) {
            F[i][j] = F[F[i][j - 1]][j - 1];
            W[i][j] = min(W[i][j - 1], W[F[i][j - 1]][j - 1]);
        }
    }
}

int lca(int x, int y) {
    if (find(x) != find(y))
        return -1;
    int ans = inf;
    if (dep[x] > dep[y])
        swap(x, y); // dep[x] <= dep[y] now
    for (int i = log2N - 1; i >= 0; i--) {
        if (dep[F[y][i]] >= dep[x]) {
            ans = min(ans, W[y][i]);
            y = F[y][i];
        }
    }
    if (x == y)
        return ans;
    for (int i = log2N - 1; i >= 0; i--) {
        if (F[x][i] != F[y][i]) {
            ans = min({ans, W[x][i], W[y][i]});
            x = F[x][i];
            y = F[y][i];
        }
    }
    return min({ans, W[x][0], W[y][0]});
}

struct edge {
    int u, v, w;
    bool operator>(const edge b) const { return w > b.w; }
};
vector<edge> e;
void kruskal() {
    for (int i = 1; i <= n; i++)
        fa[i] = i;
    sort(e.begin(), e.end(), greater<edge>());
    for (edge ed : e) {
        if (find(ed.u) != find(ed.v)) {
            fa[find(ed.u)] = find(ed.v);
            g[ed.u].push_back({ed.w, ed.v});
            g[ed.v].push_back({ed.w, ed.u});
        }
    }
}

int main() {
    cin.tie(0);
    cout.tie(0);
    ios::sync_with_stdio(0);
    cin >> n >> m;
    for (int i = 1; i <= m; i++) {
        int x, y, z;
        cin >> x >> y >> z;
        e.push_back({x, y, z});
    }
    kruskal();
    build_lca();

    cin >> q;
    for (int i = 1; i <= q; i++) {
        int x, y;
        cin >> x >> y;
        cout << lca(x, y) << '\n';
    }
}
