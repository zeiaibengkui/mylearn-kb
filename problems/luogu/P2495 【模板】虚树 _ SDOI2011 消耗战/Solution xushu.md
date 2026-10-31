---
title: Solution xushu
---
Submitted 2026-10-11 — all 1 sample case(s) passed.

```cpp
// P2495 【模板】虚树 / [SDOI2011] 消耗战
// 把询问点和它们的 LCA 建成虚树，在虚树上做树形 DP。
// 根固定为 1，记 mn[v] = 1 -> v 路径上的最小边权（等于“一刀砍断 v 的整棵子树”的最小代价）。
//   v 是关键点：必须砍断 1 -> v 上的一条边，答案 mn[v]；
//   v 不是关键点：dp[v] = min(mn[v], Σ dp[child])。
// 一次询问 O(k log k + k log n)。
#include <bits/stdc++.h>
using namespace std;

static const int MAXN = 250005;
static const int LOG = 19;  // 2^19 > 2.5e5
static const long long INF = (long long)4e18;

namespace io {
const int SZ = 1 << 20;
char buf[SZ];
int len = 0, pos = 0;
inline int gc() {
    if (pos == len) {
        len = (int)fread(buf, 1, SZ, stdin);
        pos = 0;
        if (len <= 0) return -1;
    }
    return buf[pos++];
}
inline int readInt() {
    int c = gc(), sgn = 1;
    while (c != -1 && (c < '0' || c > '9')) {
        if (c == '-') sgn = -1;
        c = gc();
    }
    int x = 0;
    while (c >= '0' && c <= '9') {
        x = x * 10 + (c - '0');
        c = gc();
    }
    return x * sgn;
}
}  // namespace io

int n, m;
int head[MAXN], nxt[2 * MAXN], to_[2 * MAXN], wt[2 * MAXN], ecnt;

inline void addEdge(int u, int v, int w) {
    to_[ecnt] = v;
    wt[ecnt] = w;
    nxt[ecnt] = head[u];
    head[u] = ecnt++;
}

int dfn[MAXN], dep[MAXN], par[MAXN];
long long mn_[MAXN];  // 1 -> v 路径上的最小边权
int up[LOG][MAXN];
int timer;

// 递归 DFS：求 dfn / 深度 / 父亲 / 路径最小边权（洛谷的栈够深，链也不会爆栈）
void dfs(int u) {
    dfn[u] = ++timer;
    for (int e = head[u]; e != -1; e = nxt[e]) {
        int v = to_[e];
        if (v == par[u]) continue;
        par[v] = u;
        dep[v] = dep[u] + 1;
        mn_[v] = min(mn_[u], (long long)wt[e]);
        dfs(v);
    }
}

int lca(int u, int v) {
    if (dep[u] < dep[v]) swap(u, v);
    int d = dep[u] - dep[v];
    for (int i = 0; i < LOG; i++)
        if (d >> i & 1) u = up[i][u];
    if (u == v) return u;
    for (int i = LOG - 1; i >= 0; i--)
        if (up[i][u] != up[i][v]) {
            u = up[i][u];
            v = up[i][v];
        }
    return up[0][u];
}

// 虚树 DP 用到的数组（key_ 标记关键点，sum_ 汇总儿子的 dp，pv_ 记虚树父亲）
int key_[MAXN], pv_[MAXN];
long long sum_[MAXN];

int main() {
    n = io::readInt();
    memset(head, -1, sizeof(int) * (n + 1));
    for (int i = 1; i < n; i++) {
        int u = io::readInt(), v = io::readInt(), w = io::readInt();
        addEdge(u, v, w);
        addEdge(v, u, w);
    }

    dep[1] = 0;
    par[1] = 0;
    mn_[1] = INF;  // 1 号点不能被砍断
    dfs(1);

    for (int v = 1; v <= n; v++) up[0][v] = par[v];
    for (int j = 1; j < LOG; j++)
        for (int v = 1; v <= n; v++) up[j][v] = up[j - 1][up[j - 1][v]];

    m = io::readInt();
    vector<int> hs, stk, nodes;
    vector<pair<int, int>> edges;  // 虚树边 (父亲, 儿子)
    string out;
    out.reserve((size_t)m * 8);
    char tmp[32];

    while (m--) {
        int k = io::readInt();
        hs.resize(k);
        for (int i = 0; i < k; i++) hs[i] = io::readInt();
        sort(hs.begin(), hs.end(), [](int a, int b) { return dfn[a] < dfn[b]; });
        hs.erase(unique(hs.begin(), hs.end()), hs.end());  // 防御重复的关键点
        int kk = (int)hs.size();

        // 单调栈建虚树
        stk.clear();
        edges.clear();
        stk.push_back(hs[0]);
        for (int i = 1; i < kk; i++) {
            int u = hs[i];
            int l = lca(u, stk.back());
            while (stk.size() >= 2 && dep[stk[stk.size() - 2]] >= dep[l]) {
                edges.push_back(make_pair(stk[stk.size() - 2], stk.back()));
                stk.pop_back();
            }
            if (stk.back() != l) {
                edges.push_back(make_pair(l, stk.back()));
                stk.pop_back();
                stk.push_back(l);
            }
            stk.push_back(u);
        }
        while (stk.size() >= 2) {
            edges.push_back(make_pair(stk[stk.size() - 2], stk.back()));
            stk.pop_back();
        }
        int top = stk[0];

        // 虚树点集：根 top + 每条边的儿子；按 dfn 递增排序后，倒序即为儿子先于父亲的顺序
        nodes.clear();
        nodes.push_back(top);
        for (size_t i = 0; i < edges.size(); i++) {
            nodes.push_back(edges[i].second);
            pv_[edges[i].second] = edges[i].first;
        }
        sort(nodes.begin(), nodes.end(), [](int a, int b) { return dfn[a] < dfn[b]; });
        nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());
        for (size_t i = 0; i < nodes.size(); i++) {
            sum_[nodes[i]] = 0;
            key_[nodes[i]] = 0;
        }
        for (int i = 0; i < kk; i++) key_[hs[i]] = 1;

        long long ans = 0;
        for (int i = (int)nodes.size() - 1; i >= 0; i--) {
            int v = nodes[i];
            long long val = key_[v] ? mn_[v] : min(mn_[v], sum_[v]);
            if (v == top)
                ans = val;
            else
                sum_[pv_[v]] += val;
        }

        int len = 0;
        if (ans == 0) {
            tmp[len++] = '0';
        } else {
            while (ans > 0) {
                tmp[len++] = char('0' + ans % 10);
                ans /= 10;
            }
        }
        while (len) out.push_back(tmp[--len]);
        out.push_back('\n');
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
```
