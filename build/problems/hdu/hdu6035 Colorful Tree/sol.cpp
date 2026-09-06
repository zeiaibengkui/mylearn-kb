#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

const int N = 200200;

int n, c[N];
int vis[N];                 // whether color i appears
vector<int> vec[N];
ll sum[N];                  // merged node count of each color
int sons[N];                // subtree size
ll s;                       // sum of C(ct,2) over the "cut" blocks

struct Frame {
    int u, fa, idx;
    ll pre;
};

// Iterative DFS: mimics recursion (post-order children, interleaved merge
// bookkeeping) but avoids the stack overflow of a 200k-node path.
void solve(int root) {
    vector<Frame> st;
    st.reserve(n);
    auto enter = [&](int u, int fa) {
        sons[u] = 1;
        sum[c[u]]++;
        st.push_back({u, fa, 0, sum[c[u]]});
    };
    enter(root, 0);
    while (!st.empty()) {
        Frame &f = st.back();
        if (f.idx < (int)vec[f.u].size()) {
            int v = vec[f.u][f.idx++];
            if (v == f.fa) continue;
            enter(v, f.u);
        } else {
            int u = f.u, fa = f.fa;
            st.pop_back();
            if (fa != 0) {
                sons[fa] += sons[u];
                ll ct = sons[u] - (sum[c[fa]] - st.back().pre);
                s += ct * (ct - 1) / 2;
                sum[c[fa]] += ct;
                st.back().pre = sum[c[fa]];
            }
        }
    }
}

int main() {
    int cas = 0;
    while (scanf("%d", &n) == 1) {
        s = 0;
        memset(sum, 0, sizeof(sum));
        memset(vis, 0, sizeof(vis));
        int num = 0;
        for (int i = 1; i <= n; i++) {
            scanf("%d", &c[i]);
            if (!vis[c[i]]) { num++; vis[c[i]] = 1; }
            vec[i].clear();
        }
        for (int i = 1; i < n; i++) {
            int u, v;
            scanf("%d %d", &u, &v);
            vec[u].push_back(v);
            vec[v].push_back(u);
        }
        solve(1);
        ll total = (ll)num * n * (n - 1) / 2;
        ll ans = total - s;
        for (int i = 1; i <= n; i++) if (vis[i]) {
            ll ct = n - sum[i];
            ans -= ct * (ct - 1) / 2;
        }
        printf("Case #%d: %lld\n", ++cas, ans);
    }
    return 0;
}
