#include <bits/stdc++.h>
using namespace std;
#define pii pair<int, int>
#define fi first
#define se second
#define ll long long
const int N = 200005;
int l[N], r[N], score[N];
bool vis[N];

int main() {
    ll n, m, ans = 0;
    cin >> n >> m;
    if (n < m * 2) {
        cout << "Error!\n";
        return 0;
    }
    priority_queue<pii> q;
    for (int i = 1; i <= n; i++) {
        cin >> score[i];
        l[i] = i == 1 ? n : i - 1;
        r[i] = i == n ? 1 : i + 1;
        q.push({score[i], i});
    }
    for (int j = 1; j <= m; j++) {
        pii t = q.top();
        q.pop();
        const int i = t.se, s = t.fi;
        if (vis[i]) {
            j--;
            continue;
        };
        int L = l[i], R = r[i];
        int LL = l[L], RR = r[R];
        vis[L] = vis[R] = true;
        score[i] = -s + score[L] + score[R];
        q.push({score[i], i});
        ans += s;
        // delete
        r[LL] = i, l[RR] = i;
        l[i] = LL, r[i] = RR;
    }
    cout << ans << '\n';

    return 0;
}
