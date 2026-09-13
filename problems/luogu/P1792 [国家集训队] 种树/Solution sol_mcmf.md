---
title: Solution sol_mcmf
---
Submitted 2026-09-13 — all 2 sample case(s) passed.

```cpp
// P1792 [国家集训队] 种树 —— 费用流（最小费用最大流）版本
//
// 建模：
//   位置 i 种树 = 一条"匹配边"。把环上相邻的两个位置看作一条边，
//   于是"选 m 个互不相邻的位置"= 在路径/环图上选 m 条两两不共端点的边，
//   也就是大小为 m 的（最大权）匹配。匹配用费用流求：
//     S -> L_v     容量 1，费用 0      （顶点 v 最多被用一次）
//     L_v -> R_u   容量 1，费用 -A_i    （图上的边 {v,u}，即种位置 i）
//     R_u -> T     容量 1，费用 0
//   顶点按下标的奇偶二染色（路径是二分图），所以只连 偶->奇 方向。
//   推 m 单位流量、费用取负即为答案。
//
// 环的处理：枚举 1 号位置种不种，各退化成一个路径上的匹配问题：
//   ① 1 号不种：位置 2..n 上选 m 个；
//   ② 1 号种：  必须先选 A_1，位置 3..n-1 上再选 m-1 个（2 和 n 被禁）。
//   取两种情形的较大值。
//
// 复杂度 O(m · E log V)，E = O(n)：正确但只有中小数据跑得动。
// 上限数据 (n=2e5, m=1e5) 请用 sol.cpp —— 堆+双向链表那版正是这份费用流
// 的"模拟费用流"写法（每次增广恰好 +1 棵树，退流边就是反悔项）。
#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
const ll INF = (ll)4e18;
const ll NEG = -(ll)1e18;

struct MCMF {
    struct Edge {
        int to, rev, cap;
        ll cost;
    };
    vector<vector<Edge>> g;

    explicit MCMF(int n) : g(n) {}

    void add(int u, int v, int cap, ll cost) {
        g[u].push_back({v, (int)g[v].size(), cap, cost});
        g[v].push_back({u, (int)g[u].size() - 1, 0, -cost});
    }

    /** min cost to push exactly `need` units; INF if impossible */
    ll flow(int s, int t, int need) {
        if (need == 0) return 0;
        const int n = (int)g.size();
        vector<ll> h(n, INF), dist(n);
        vector<int> prevv(n), preve(n);

        // initial potentials: SPFA (the graph has negative cost arcs)
        deque<int> dq;
        vector<bool> inq(n, false);
        h[s] = 0;
        dq.push_back(s);
        inq[s] = true;
        while (!dq.empty()) {
            int u = dq.front();
            dq.pop_front();
            inq[u] = false;
            for (const Edge &e : g[u]) {
                if (e.cap > 0 && h[u] + e.cost < h[e.to]) {
                    h[e.to] = h[u] + e.cost;
                    if (!inq[e.to]) {
                        inq[e.to] = true;
                        dq.push_back(e.to);
                    }
                }
            }
        }

        ll total = 0;
        while (need > 0) {
            // Dijkstra on the reduced costs
            fill(dist.begin(), dist.end(), INF);
            dist[s] = 0;
            priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> pq;
            pq.push({0, s});
            while (!pq.empty()) {
                auto [d, u] = pq.top();
                pq.pop();
                if (d > dist[u]) continue;
                for (int i = 0; i < (int)g[u].size(); ++i) {
                    const Edge &e = g[u][i];
                    if (e.cap <= 0) continue;
                    ll nd = d + e.cost + h[u] - h[e.to];
                    if (nd < dist[e.to]) {
                        dist[e.to] = nd;
                        prevv[e.to] = u;
                        preve[e.to] = i;
                        pq.push({nd, e.to});
                    }
                }
            }
            if (dist[t] == INF) return INF;
            for (int v = 0; v < n; ++v)
                if (dist[v] < INF) h[v] += dist[v];

            int push = need;
            for (int v = t; v != s; v = prevv[v])
                push = min(push, g[prevv[v]][preve[v]].cap);
            for (int v = t; v != s; v = prevv[v]) {
                Edge &e = g[prevv[v]][preve[v]];
                e.cap -= push;
                g[v][e.rev].cap += push;
            }
            total += (ll)push * h[t];   // h[t] = true shortest distance s->t
            need -= push;
        }
        return total;
    }
};

/** best total of exactly `need` pairwise non-adjacent values from a LINE of
 *  values; NEG when there are not enough positions */
ll pathBest(const vector<ll> &w, int need) {
    const int k = (int)w.size();
    if (need == 0) return 0;
    if (need > (k + 1) / 2) return NEG;   // ceiling(k/2) is the maximum

    // gap vertices 0..k, item j = edge (j, j+1); colour vertices by parity
    const int S = 0, T = 1;
    const int L = 2;                 // L_v = L + v
    const int R = L + (k + 1);       // R_v = R + v
    MCMF f(R + (k + 1));
    for (int v = 0; v <= k; ++v) {
        if (v % 2 == 0) f.add(S, L + v, 1, 0);
        else            f.add(R + v, T, 1, 0);
    }
    for (int j = 0; j < k; ++j) {     // item j joins vertices j and j+1
        if (j % 2 == 0) f.add(L + j, R + (j + 1), 1, -w[j]);
        else            f.add(L + (j + 1), R + j, 1, -w[j]);
    }

    ll cost = f.flow(S, T, need);
    return cost >= INF ? NEG : -cost;
}

int main() {
    int n, m;
    if (scanf("%d %d", &n, &m) != 2) return 0;
    vector<ll> a(n + 1);
    for (int i = 1; i <= n; ++i) scanf("%lld", &a[i]);

    if (m > n / 2) {              // at most floor(n/2) trees fit on the ring
        printf("Error!\n");
        return 0;
    }
    if (m == 0) {
        printf("0\n");
        return 0;
    }

    ll best = NEG;
    {   // case 1: position 1 is not planted -> positions 2..n form a line
        vector<ll> line;
        for (int i = 2; i <= n; ++i) line.push_back(a[i]);
        ll v = pathBest(line, m);
        if (v > NEG / 2) best = max(best, v);
    }
    {   // case 2: position 1 is planted -> pick m-1 from 3..n-1
        vector<ll> line;
        for (int i = 3; i <= n - 1; ++i) line.push_back(a[i]);
        ll v = pathBest(line, m - 1);
        if (v > NEG / 2) best = max(best, a[1] + v);
    }

    printf("%lld\n", best);
    return 0;
}
```
