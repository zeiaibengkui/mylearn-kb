
#include <bits/stdc++.h>
using namespace std;
#define int long long
const int N = 300005, M = 19, inf = 0x3f3f3f3f3f3f3f3fll;
#define pii pair<int, int>
#define fi first
#define se second
vector<pii> g[N];
vector<int> gg[N], imps; // gg 只存单向边：父 -> 子
bool imp[N];
int minw[N];
int f[N][M + 1], dep[N], dfn[N], par_[N], pv[N];
long long sumv[N];
int n, timer_;

void dfs1(int root) {
    vector<int> stk;
    stk.push_back(root);
    par_[root] = 0;
    while (!stk.empty()) {
        int u = stk.back();
        stk.pop_back();
        dfn[u] = ++timer_;
        for (auto p : g[u]) {
            int v = p.se, w = p.fi;
            if (v == par_[u])
                continue;
            par_[v] = u;
            f[v][0] = u;
            dep[v] = dep[u] + 1;
            minw[v] = min(minw[u], w);
            stk.push_back(v);
        }
    }
}

void build_lca() {
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j < M; j++) {
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

long long dfs2() {
    static vector<int> stk, order;
    stk.clear();
    order.clear();
    stk.push_back(1);
    while (!stk.empty()) {
        int u = stk.back();
        stk.pop_back();
        order.push_back(u);
        for (auto v : gg[u]) {
            pv[v] = u;
            stk.push_back(v);
        }
    }
    long long ans = 0;
    for (int i = (int)order.size() - 1; i >= 0; i--) {
        int u = order[i];
        long long val = imp[u] ? minw[u] : min(minw[u], sumv[u]);
        gg[u].clear();
        imp[u] = 0;
        sumv[u] = 0;
        if (u == 1)
            ans = val;
        else
            sumv[pv[u]] += val;
    }
    return ans;
}

void build_xushu() {
    sort(imps.begin(), imps.end(),
         [](int u, int v) { return dfn[u] < dfn[v]; });
    imps.erase(unique(imps.begin(), imps.end()), imps.end()); // 防御重复关键点

    vector<int> st;
    st.push_back(1);
    for (auto u : imps) {
        int lc = lca(u, st.back());
        while (st.size() >= 2 && dep[st[st.size() - 2]] >= dep[lc]) {
            gg[st[st.size() - 2]].push_back(st.back());
            st.pop_back();
        }
        if (st.back() != lc) {
            gg[lc].push_back(st.back());
            st.pop_back();
            st.push_back(lc);
        }
        st.push_back(u);
    }
    while (st.size() >= 2) { // 把栈里剩下的右链连起来
        gg[st[st.size() - 2]].push_back(st.back());
        st.pop_back();
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
    dfs1(1);
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
        cout << dfs2() << '\n';
    }
}
