// https://codeforces.com/problemset/submission/600/393124829
#include <bits/stdc++.h>
using namespace std;

#define int long long
const int N = 1e5 + 5;
using color = int;
int n, sum, mx;
color col[N];
int cnt[N], ans[N];
int son[N], sz[N]; // heavy son
vector<int> g[N];

void dfs1(int u, int fa) {
    // compute heavy son
    sz[u] = 1;
    int mxsz = 0;
    for (auto v : g[u]) {
        if (v == fa)
            continue;
        dfs1(v, u);
        sz[u] += sz[v];
        if (sz[v] > mxsz) {
            mxsz = sz[v];
            son[u] = v;
        }
    }
}

void add(color c, int delta) {
    cnt[c] += delta;
    if (delta < 0)
        return;
    if (cnt[c] > mx) {
        mx = cnt[c];
        sum = c;
    } else if (cnt[c] == mx) {
        sum += c;
    }
}

void add_subtree(int u, int fa, int delta) {
    // delta= 1 or -1
    for (auto v : g[u]) {
        if (v != fa)
            add_subtree(v, u, delta);
    }
    add(col[u], delta);
}

void dfs2(int u, int fa, bool keep) {
    for (auto v : g[u]) {
        if (v != fa && v != son[u]) {
            // light son
            dfs2(v, u, false);
        }
    }
    if (son[u])
        dfs2(son[u], u, true);
    for (auto v : g[u]) {
        if (v != fa && v != son[u])
            add_subtree(v, u, 1);
    }
    add(col[u], 1);
    ans[u] = sum;
    if (!keep) {
        add_subtree(u, fa, -1);
        sum = mx = 0;
    }
}

signed main() {
    cin.tie(0);
    cout.tie(0);
    cin >> n;
    for (int i = 1; i <= n; i++)
        cin >> col[i];
    for (int i = 1; i < n; i++) {
        int x, y;
        cin >> x >> y;
        g[x].push_back(y);
        g[y].push_back(x);
    }

    dfs1(1, 0);
    dfs2(1, 0, 1);

    for (int i = 1; i <= n; i++)
        cout << ans[i] << ' ';
    cout << endl;
}
