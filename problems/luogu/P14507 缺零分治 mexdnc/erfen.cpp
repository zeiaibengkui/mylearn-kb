#include <bits/stdc++.h>
using namespace std;
#define rep(i, a, b) for (int i = (a); i <= (b); i++)
#define int long long
const int inf = 1e13;

void solve() {
    int n, q, flag = false;
    cin >> n >> q;
    int stop = n;
    vector<int> a(n + 1), b(n + 1), sz(n + 1), num(n + 1), sum(n + 1),
        sumnum(n + 1);
    // sz is mex
    rep(i, 1, n) {
        cin >> a[i] >> b[i]; // mex_i = a[i]+1=i until stop+1
        if (i == 1)
            continue;
        if (a[i] - a[i - 1] > 1 && !flag)
            stop = i - 1, flag = true;
        b[i] = min(b[i], b[i - 1]);
    }
    for (int i = stop, top = 1; i; i--, top++) {
        num[top] = b[i] - sumnum[top - 1];
        sz[top] = num[top] * i;
        sum[top] = sum[top - 1] + sz[top];
        sumnum[top] = sumnum[top - 1] + num[top];
    }
    rep(qq, 1, q) {
        int m;
        cin >> m;
        if (a[1] != 0) {
            if (m == 0)
                cout << 1 << '\n';
            else
                cout << -1 << '\n';
        } else if (m > sum[stop] || m == 0) {
            cout << -1 << '\n';
        } else if (m < stop)
            cout << 2 << '\n';
        else {
            int i = upper_bound(sum.begin() + 1, sum.begin() + stop + 1, m) -
                    sum.begin() - 1;

            if (m == sum[i]) {
                cout << sumnum[i] << '\n';
            } else {
                int mex = stop - i;
                int ans = sumnum[i] + (m - sum[i] + mex - 1) / (mex); // ceil
                cout << ans << '\n';
            }
        }
    }
}

signed main() {
    cin.tie(0), cout.tie(0);
    ios::sync_with_stdio(0);
    int T;
    cin >> T;
    while (T--)
        solve();
}
