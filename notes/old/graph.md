---
title: "图论"
---

有向图、无向图

## 建图

### 链式前向星

```cpp
struct Edge {
    int to, w, next;
} edge[MAX_EDGE];

int head[MAX_NODE], cnt = 0; // cnt用于给边编号，从0开始

void addEdge(int u, int v, int w) {
    edge[cnt].to = v;
    edge[cnt].w = w;
    edge[cnt].next = head[u]; // 新边指向原来的第一条边
    head[u] = cnt++;          // 更新头结点，指向新边
}

for (int i = head[u]; i != -1; i = edge[i].next) {
    int v = edge[i].to;
    int w = edge[i].w;
    // 处理从u到v的这条边...
}
```

head 数组通常**初始化**为 -1，边遍历的结束条件为 `i != -1`。也有实现将其初始化为 0，此时边编号从 1 开始，遍历结束条件为 `i != 0`

存储**无向图**时，需要添加两条有向边（正向和反向），因此 edge 数组的大小需要是边数的两倍。此时，可以通过 `i ^ 1` 的异或运算快速找到一条边的反向边。

### vector

```cpp
vector<int> g[N];

g[x].push_back(y);

for(auto v:g[u]) {
    cout << v;
}
```

## 最短路

### Dijkstra

### SPFA

### Floyd

求：任意两点间最短路

解：矩阵乘法中+换为min

## 树

定义: n个点，n-1条边，联通

入度、出度
