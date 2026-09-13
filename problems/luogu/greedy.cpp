// P4025 luogu
#include <bits/stdc++.h>
using namespace std;
#define int long long
struct aa {
    int cost, recover, diff, idx;
    friend bool operator>(aa a, aa b) {
        if (a.diff >= 0 && b.diff >= 0)
            return a.cost < b.cost;
        else if (a.diff < 0 && b.diff < 0)
            return a.recover > b.recover;
        else
            return a.diff > b.diff;
    }
};

signed main() {
    int n, z;
    cin >> n >> z;
    vector<aa> a(n);

    for (int i = 1; i <= n; i++) {
        int cost, recover;
        cin >> cost >> recover;
        a[i - 1] = {cost, recover, recover - cost, i};
    }
    sort(a.begin(), a.end(), greater<aa>());
    for (aa &u : a) {
        if (z - u.cost <= 0) {
            cout << "NIE" << '\n';
            exit(0);
        }
        z += u.diff;
    }
    cout << "TAK\n";
    for (aa &u : a)
        cout << u.idx << ' ';
    cout << '\n';
}
