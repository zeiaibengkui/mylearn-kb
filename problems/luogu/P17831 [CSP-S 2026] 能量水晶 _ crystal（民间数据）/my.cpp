#include <bits/stdc++.h>
using namespace std;
#define rep(i, a, b) for (int i = (a); i <= b; i++)
#define int long long
signed main() {
    int n, m;
    cin >> n >> m;
    vector<int> a(n + 1), sum(n + 1);
    rep(i, 1, n) cin >> a[i], sum[i] = sum[i - 1] + a[i];
    int l = 1, mx = 0;
    for (int r = 1; r <= n; r++) {
        while (l < r) {
            // const int mid=(l+r)>>1;
            const int len = r - l + 1;
            const int target = len * a[r];
            const int have = sum[r] - sum[l - 1];
            const int need = -have + target;

            if (need > m)
                l++;
            else
                mx = max(mx, len);
        }
    }
    cout << mx << '\n';
}
