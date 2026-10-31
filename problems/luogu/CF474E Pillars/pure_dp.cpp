#include <bits/stdc++.h>
using namespace std;
#define rep(i, a, b) for (int i = a; i <= (b); i++)
#define mxto(a, b) a = max(a, b)

int main() {
    // freopen("in", "r", stdin);
    cin.tie(0), cout.tie(0), ios::sync_with_stdio(0);
    int n, d, mx = 0, mxpos = 0;
    cin >> n >> d;
    vector<int> a(n + 1), dp(n + 1, 1), pre(n + 1);
    rep(i, 1, n) cin >> a[i];
    rep(i, 1, n) rep(j, 1, i - 1) {
        if (a[i] - a[j] >= d || a[j] - a[i] >= d)
            if (dp[i] < dp[j] + 1) {
                dp[i] = dp[j] + 1;
                pre[i] = j;
                if (dp[i] > mx)
                    mx = dp[i], mxpos = i;
            }
    }
    cout << mx << '\n';
    vector<int> pos;
    for (int p = mxpos; p; p = pre[p])
        pos.push_back(p);
    reverse(pos.begin(), pos.end());
    for (int p : pos)
        cout << p << ' ';
    cout << '\n';

    return 0;
}
