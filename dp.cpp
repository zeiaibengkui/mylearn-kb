// https://codeforces.com/problemset/submission/1875/393855941
#include <bits/stdc++.h>
using namespace std;

void solve() {
    int n, mex = 0;
    cin >> n;
    map<int, int> cnt;
    for (int i = 1; i <= n; i++) {
        int x;
        cin >> x;
        cnt[x]++;
    }
    vector<int> dp(5000 + 5, 0x3f3f3f3f);
    while (cnt[mex])
        mex++;
    dp[mex] = 0;
    for (int i = mex; i > 0; i--) {
        for (int j = 0; j < i; j++) {
            dp[j] = min(dp[j], dp[i] + i * (cnt[j] - 1) + j);
        }
    }
    cout << dp[0] << '\n';
}

int main() {
    int T;
    cin >> T;
    while (T--)
        solve();
}
