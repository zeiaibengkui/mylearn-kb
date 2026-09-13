---
title: Solution sol
---
Submitted 2026-09-13 — all 2 sample case(s) passed.

```cpp
// P1792 [国家集训队] 种树
// 贪心(反悔) + 大根堆 + 双向循环链表
// 环上选 m 个互不相邻的位置使权值和最大：先取堆顶（局部最优），
// 再把它的左右两点合并成一个权值为 A[l]+A[r]-A[i] 的新点放回堆中，
// 相当于"反悔"：若这个新点之后被选中，就等价于放弃 i、改选左右两点。
#include <bits/stdc++.h>
using namespace std;

const int N = 200005;

int val[N];          // 每个点的美观度（合并后会被覆盖为反悔权值）
int pre[N], nxt[N];  // 双向循环链表
bool vis[N];         // 是否已被合并删除

struct Node {
    int v, id;
    bool operator<(const Node &o) const { return v < o.v; }
};

priority_queue<Node> q;

int main() {
    int n, m;
    if (scanf("%d %d", &n, &m) != 2) return 0;
    for (int i = 1; i <= n; ++i) scanf("%d", &val[i]);

    // 最多能种 floor(n/2) 棵（隔一个种一个），否则无解
    if (m > n / 2) {
        printf("Error!\n");
        return 0;
    }
    if (m == 0) {
        printf("0\n");
        return 0;
    }

    for (int i = 1; i <= n; ++i) {
        pre[i] = (i == 1) ? n : i - 1;
        nxt[i] = (i == n) ? 1 : i + 1;
        q.push({val[i], i});
    }

    long long ans = 0;
    for (int t = 0; t < m; ++t) {
        while (vis[q.top().id]) q.pop();  // 丢掉已删除的惰性结点
        Node cur = q.top();
        q.pop();

        int id = cur.id, l = pre[id], r = nxt[id];
        ans += cur.v;

        // 删除左右两点，用 id 占据它们之间的位置
        vis[l] = vis[r] = true;
        val[id] = val[l] + val[r] - val[id];
        int ll = pre[l], rr = nxt[r];
        pre[id] = ll;
        nxt[ll] = id;
        nxt[id] = rr;
        pre[rr] = id;

        q.push({val[id], id});
    }

    printf("%lld\n", ans);
    return 0;
}
```
