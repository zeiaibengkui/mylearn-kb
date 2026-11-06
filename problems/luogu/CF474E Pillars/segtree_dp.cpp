#include <bits/stdc++.h>
using namespace std;

#define int long long
const int N = 1e5 + 5;
int h[N], d, n, dp[N], pre[N];
#define rep(i, a, b) for (int i = (a); i <= (b); i++)

struct node {
    int val = 0, idx = 0;
    node merge(node b) const {
        if (b.val > val) {
            return b;
        } else
            return *this;
    }
    node &merge_(node b) {
        if (b.val > val) {
            this->val = b.val, this->idx = b.idx;
        }
        return *this;
    }
};

struct Segtree {
    node tree[N * 4];
    void update(int u, int l, int r, const int p, const node &v) {
        if (l == r) {
            tree[u].merge_(v);
            return;
        }
        int mid = (l + r) >> 1;
        if (p <= mid)
            update(u << 1, l, mid, p, v);
        else
            update(u << 1 | 1, mid + 1, r, p, v);
        tree[u].merge_(tree[u << 1]).merge_(tree[u << 1 | 1]);
    }
    node query(int u, int l, int r, const int ql, const int qr) {
        if (ql > qr)
            return node{0, 0};
        if (ql <= l && r <= qr)
            return tree[u];
        int mid = (l + r) >> 1;
        node ans = {0, 0};
        if (ql <= mid)
            ans.merge_(query(u << 1, l, mid, ql, qr));
        if (qr > mid)
            ans.merge_(query(u << 1 | 1, mid + 1, r, ql, qr));
        return ans;
    }
};

signed main() {
    freopen("in", "r", stdin);
    cin >> n >> d;
    vector<int> tmp(n);
    Segtree T;
    rep(i, 1, n) cin >> h[i], tmp[i - 1] = h[i];
    sort(tmp.begin(), tmp.end());
    tmp.erase(unique(tmp.begin(), tmp.end()), tmp.end());
    const int m = tmp.size();
    int mx = 0, mxPos = 0;
    rep(i, 1, n) {
        // i-j>=d, j<=i-d
        int l = upper_bound(tmp.begin(), tmp.end(), h[i] - d) - tmp.begin();
        // j-i>=d, j>=i+d
        int r = lower_bound(tmp.begin(), tmp.end(), h[i] + d) - tmp.begin() + 1;
        node now = T.query(1, 1, m, 1, l).merge(T.query(1, 1, m, r, m));
        int p = lower_bound(tmp.begin(), tmp.end(), h[i]) - tmp.begin() +
                1; // idx for h[i] in h, 1-based
        dp[i] = now.val + 1;
        pre[i] = now.idx;
        T.update(1, 1, m, p, node{dp[i], i});
        if (dp[i] > mx) {
            mx = dp[i], mxPos = i;
        }
    }
    cout << dp[mxPos] << '\n';
    tmp.clear();
    for (int p = mxPos; p; p = pre[p])
        tmp.push_back(p);
    reverse(tmp.begin(), tmp.end());
    for (int p : tmp)
        cout << p << ' ';
    cout << endl;
    return 0;
}
