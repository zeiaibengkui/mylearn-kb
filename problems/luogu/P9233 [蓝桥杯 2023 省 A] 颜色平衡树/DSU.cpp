// https://www.luogu.com.cn/record/300809179
#include <bits/stdc++.h>
using namespace std;

const int N = 2e5 + 5;
vector<int> g[N];
int n, col[N], cnt[N], ccnt[N];
int sz[N], son[N], sum;

void dfs1(int u, int fa) {
    int mxsz = 0;
    sz[u] = 1;
    for (auto v : g[u]) {
        if (v == fa)
            continue;
        dfs1(v, u);
        sz[u] += sz[v];
        if (sz[v] > mxsz) {
            son[u] = v;
            mxsz = sz[v];
        }
    }
}

int getans(int u) { return ccnt[cnt[col[u]]] * cnt[col[u]] == sz[u]; }

void add(int c, int delta) {
    ccnt[cnt[c]]--;
    cnt[c] += delta;
    ccnt[cnt[c]]++;
}

void add_subtree(int u, int fa, int delta) {
    add(col[u], delta);
    for (auto v : g[u]) {
        if (v == fa)
            continue;
        add_subtree(v, u, delta);
    }
}

void dfs2(int u, int fa, bool keep) {
    for (auto v : g[u]) {
        if (v == fa || v == son[u])
            continue;
        dfs2(v, u, 0);
    }
    if (son[u])
        dfs2(son[u], u, 1);
    for (auto v : g[u]) {
        if (v == fa || v == son[u])
            continue;
        add_subtree(v, u, 1);
    }
    add(col[u], 1);
    sum += getans(u);

    if (!keep) {
        add_subtree(u, fa, -1);
    }
}

int main() {
#ifndef ONLINE_JUDGE
    freopen("in", "r", stdin);
#endif
    cin.tie(0);
    cout.tie(0);
    ios::sync_with_stdio(0);
    cin >> n;
    for (int i = 1; i <= n; i++) {
        int fa;
        cin >> col[i] >> fa;
        if (fa == 0)
            continue;
        g[fa].push_back(i);
        g[i].push_back(fa);
    }
    dfs1(1, 1);
    dfs2(1, 1, 1);
    cout << sum << endl;
}
