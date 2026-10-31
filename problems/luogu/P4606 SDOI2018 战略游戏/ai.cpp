// P4606 [SDOI2018] 战略游戏
// 圆方树 + 虚树：答案 = 虚树各条边并起来经过的圆点数 - |S|
#include <bits/stdc++.h>
using namespace std;

const int MAXN = 100005;
const int MAXM = 200005;
const int MAXV = 200005;  // 圆方树最多 2n 个点
const int LOG = 18;       // 2^18 > 2*10^5

int T, n, m, q;
vector<vector<int>> g;     // 原图（重边已删）
vector<vector<int>> tree;  // 圆方树
pair<int, int> es[MAXM];   // 读入的边，用来去重

int dfn[MAXN], low[MAXN], timer;
int bcc;        // 方点个数，方点编号为 n+1, n+2, ...
vector<int> st; // Tarjan 的顶点栈

int dfn_tree[MAXV], dep[MAXV], fa[MAXV][LOG], rcnt[MAXV];
// dfn_tree: 圆方树的 dfs 序；dep: 深度；fa: 倍增父亲
// rcnt: 根到该点的路径上有几个圆点（原图点）
int dfs_timer;

bool cmpDfn(int a, int b) { return dfn_tree[a] < dfn_tree[b]; }

void init() {
    g.assign(n + 1, {});
    tree.assign(2 * n + 5, {});
    timer = bcc = dfs_timer = 0;
    st.clear();
    fill(dfn, dfn + n + 1, 0);
    fill(low, low + n + 1, 0);
}

// 求点双连通分量，同时建圆方树（手写栈，防止深递归爆栈）
void build_tree() {
    vector<int> su, sp, si; // 当前点、它的父亲、以及它已经遍历到第几条边
    for (int root = 1; root <= n; root++) {
        if (dfn[root]) continue;
        st.clear();
        dfn[root] = low[root] = ++timer;
        st.push_back(root);
        su.push_back(root);
        sp.push_back(0);
        si.push_back(0);
        while (!su.empty()) {
            int u = su.back();
            if (si.back() == (int)g[u].size()) { // u 的边遍历完了，回溯
                su.pop_back();
                si.pop_back();
                int p = sp.back();
                sp.pop_back();
                if (p != 0) {
                    low[p] = min(low[p], low[u]);
                    if (low[u] >= dfn[p]) { // 弹出一个点双
                        int fang = n + (++bcc);
                        while (true) {
                            int x = st.back();
                            st.pop_back();
                            tree[fang].push_back(x);
                            tree[x].push_back(fang);
                            if (x == u) break;
                        }
                        tree[fang].push_back(p); // p 也属于这个点双
                        tree[p].push_back(fang);
                    }
                }
                continue;
            }
            int v = g[u][si.back()++];
            if (v == sp.back()) continue; // 重边已删，父亲只会出现一次
            if (!dfn[v]) {
                dfn[v] = low[v] = ++timer;
                st.push_back(v);
                su.push_back(v);
                sp.push_back(u);
                si.push_back(0);
            } else {
                low[u] = min(low[u], dfn[v]); // 反祖边
            }
        }
    }
}

// 圆方树上预处理 dfs 序、深度、倍增父亲、圆点前缀数（同样手写栈）
void build_lca() {
    auto visit = [&](int v, int p) {
        dfn_tree[v] = ++dfs_timer;
        dep[v] = dep[p] + 1;
        rcnt[v] = rcnt[p] + (v <= n);
        fa[v][0] = p;
        for (int i = 1; i < LOG; i++)
            fa[v][i] = fa[fa[v][i - 1]][i - 1];
    };
    vector<int> su, si;
    visit(1, 0);
    su.push_back(1);
    si.push_back(0);
    while (!su.empty()) {
        int u = su.back();
        if (si.back() == (int)tree[u].size()) {
            su.pop_back();
            si.pop_back();
            continue;
        }
        int v = tree[u][si.back()++];
        if (v == fa[u][0]) continue;
        visit(v, u);
        su.push_back(v);
        si.push_back(0);
    }
}

int lca(int u, int v) {
    if (dep[u] < dep[v]) swap(u, v);
    int d = dep[u] - dep[v];
    for (int i = 0; i < LOG; i++)
        if (d >> i & 1) u = fa[u][i];
    if (u == v) return u;
    for (int i = LOG - 1; i >= 0; i--)
        if (fa[u][i] != fa[v][i]) {
            u = fa[u][i];
            v = fa[v][i];
        }
    return fa[u][0];
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> T;
    while (T--) {
        cin >> n >> m;
        init();
        for (int i = 0; i < m; i++) {
            int u, v;
            cin >> u >> v;
            if (u > v) swap(u, v);
            es[i] = make_pair(u, v);
        }
        // 去掉重边（重边不影响圆方树的结构）
        sort(es, es + m);
        int cnt = unique(es, es + m) - es;
        for (int i = 0; i < cnt; i++) {
            int u = es[i].first, v = es[i].second;
            g[u].push_back(v);
            g[v].push_back(u);
        }

        build_tree();
        build_lca();

        cin >> q;
        while (q--) {
            int k;
            cin >> k;
            vector<int> s(k), vs; // s: 询问点集；vs: 虚树上的所有点
            for (int i = 0; i < k; i++) cin >> s[i];
            vs = s;
            // 按 dfs 序排序后，把相邻两个点的 lca 也加入
            sort(vs.begin(), vs.end(), cmpDfn);
            for (int i = 0; i + 1 < k; i++)
                vs.push_back(lca(vs[i], vs[i + 1]));
            sort(vs.begin(), vs.end(), cmpDfn);
            vs.erase(unique(vs.begin(), vs.end()), vs.end());

            // 单调栈建虚树：每弹出一条虚树边 (p,v)，就加上 p~v 路径上的圆点数（不含 p）
            int total = 0;
            vector<int> sta;
            for (int i = 0; i < (int)vs.size(); i++) {
                int u = vs[i];
                if (!sta.empty()) {
                    int l = lca(u, sta.back());
                    while (sta.size() > 1 && dep[sta[sta.size() - 2]] >= dep[l]) {
                        int v = sta.back();
                        sta.pop_back();
                        total += rcnt[v] - rcnt[sta.back()];
                    }
                    if (sta.back() != l) {
                        int v = sta.back();
                        sta.pop_back();
                        total += rcnt[v] - rcnt[l];
                        sta.push_back(l);
                    }
                }
                sta.push_back(u);
            }
            while (sta.size() > 1) {
                int v = sta.back();
                sta.pop_back();
                total += rcnt[v] - rcnt[sta.back()];
            }
            total += (sta[0] <= n); // 虚树根也是一个经过的点

            cout << total - k << '\n';
        }
    }
    return 0;
}
