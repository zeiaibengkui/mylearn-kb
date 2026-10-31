#include <bits/stdc++.h>
using namespace std;
#define int long long
const int N = 5e5 + 5, M = 22, inf = 0x3f3f3f3f3f3f3f3fll;
#define pii pair<int, int>
#define fi first
#define se second
vector<pii> g[N];
vector<int> gg[N], imps;
bool imp[N];
int minw[N];
int f[N][M + 1], dep[N], dfn[N];
int n;

void dfs1(int u, int fa) {
    static int cnt = 0;
    dfn[u] = ++cnt;
    for (auto p : g[u]) {
        int v = p.se, w = p.fi;
        if (v == fa)
            continue;
        f[v][0] = u;
        dep[v] = dep[u] + 1;
        minw[v] = min(w, minw[u]);
        dfs1(v, u);
    }
}

void build_lca() {
    for (int j = 1; j < M; j++) {
        for (int i = 1; i <= n; i++) {
            f[i][j] = f[f[i][j - 1]][j - 1];
        }
    }
}

int lca(int u, int v) {
    if (dep[u] > dep[v])
        swap(u, v); // now dep[u]<=dep[v]
    for (int d = dep[v] - dep[u], i = 0; d; d >>= 1, i++)
        if (d & 1)
            v = f[v][i];
    if (u == v)
        return u;

    for (int j = M - 1; j >= 0; j--) {
        if (f[u][j] != f[v][j])
            u = f[u][j], v = f[v][j];
    }
    return f[u][0];
}

int dfs2(vector<int> *g, int u) {
    int sum = 0;
    for (auto v : g[u])
        sum += dfs2(g, v);

    if (imp[u])
        return imp[u] = 0, g[u].clear(), minw[u];
    g[u].clear();
    imp[u] = 0;

    return min(minw[u], sum);
}

void build_xushu() {
    vector<int> st;
#define top (st.size() - 1)
    st.push_back(1);
    sort(imps.begin(), imps.end(),
         [](int u, int v) { return dfn[u] < dfn[v]; });
    imps.erase(unique(imps.begin(), imps.end()), imps.end());

    for (auto u : imps) {
        int lc = lca(u, st[top]);
        while (st.size() >= 2 && dep[st[top - 1]] >= dep[lc]) {
            gg[st[top - 1]].push_back(st[top]);
            st.pop_back();
        }
        if (lc != st[top]) {
            gg[lc].push_back(st[top]);
            st.pop_back();
            if (lc != st[top])
                st.push_back(lc);
        }
        st.push_back(u);
    }
    for (int i = 0; i < top; i++) {
        gg[st[i]].push_back(st[i + 1]);
    }
}

signed main() {
    cin.tie(0);
    cout.tie(0);
    ios::sync_with_stdio(0);
    cin >> n;
    for (int i = 1; i < n; i++) {
        int u, v, w;
        cin >> u >> v >> w;
        g[u].push_back({w, v});
        g[v].push_back({w, u});
    }

    dep[1] = 1;
    minw[1] = inf;
    dfs1(1, 1);
    build_lca();

    int m;
    cin >> m;
    for (int i = 1; i <= m; i++) {
        int k;
        cin >> k;
        imps = vector<int>(k);
        for (int j = 0; j < k; j++) {
            cin >> imps[j];

            imp[imps[j]] = true;
        }
        build_xushu();
        cout << dfs2(gg, 1) << '\n';
    }
}
