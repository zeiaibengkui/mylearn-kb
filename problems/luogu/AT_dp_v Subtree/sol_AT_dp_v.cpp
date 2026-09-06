#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    long long M;
    cin >> N >> M;

    vector<vector<int>> adj(N + 1);
    for (int i = 0; i < N - 1; i++) {
        int x, y;
        cin >> x >> y;
        adj[x].push_back(y);
        adj[y].push_back(x);
    }

    // Iterative DFS from root 1 to get parent[] and a top-down order.
    vector<int> parent(N + 1, 0), order;
    order.reserve(N);
    parent[1] = -1;
    stack<int> st;
    st.push(1);
    while (!st.empty()) {
        int u = st.top();
        st.pop();
        order.push_back(u);
        for (int v : adj[u]) {
            if (v == parent[u]) continue;
            parent[v] = u;
            st.push(v);
        }
    }

    // down[u] = ways to color u's subtree so u is black & blacks are connected.
    // For a leaf down[u] = 1; for internal: down[u] = prod (down[c] + 1).
    vector<long long> down(N + 1, 1), up(N + 1, 0), ans(N + 1);
    for (int idx = N - 1; idx >= 0; idx--) {
        int u = order[idx];
        long long val = 1;
        for (int v : adj[u]) {
            if (parent[v] == u) {
                val = val * ((down[v] + 1) % M) % M;
            }
        }
        down[u] = val % M;
    }

    // Top-down rerooting.
    up[1] = 0;
    for (int idx = 0; idx < N; idx++) {
        int u = order[idx];
        ans[u] = down[u] * ((up[u] + 1) % M) % M;

        vector<int> childs;
        vector<long long> factors;
        for (int v : adj[u]) {
            if (parent[v] == u) {
                childs.push_back(v);
                factors.push_back((down[v] + 1) % M);
            }
        }
        if (u != 1) factors.push_back((up[u] + 1) % M);

        int k = (int)factors.size();
        vector<long long> pref(k + 1, 1), suff(k + 1, 1);
        for (int i = 0; i < k; i++) pref[i + 1] = pref[i] * factors[i] % M;
        for (int i = k - 1; i >= 0; i--) suff[i] = suff[i + 1] * factors[i] % M;

        for (int i = 0; i < (int)childs.size(); i++) {
            up[childs[i]] = pref[i] * suff[i + 1] % M;
        }
    }

    for (int i = 1; i <= N; i++) cout << ans[i] % M << "\n";
    return 0;
}
