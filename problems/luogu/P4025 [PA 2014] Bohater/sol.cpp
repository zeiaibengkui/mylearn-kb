// P4025 [PA 2014] Bohater
// 贪心：先打「有赚」的怪 (a >= d)，按消耗 d 升序；再打「亏」的怪，按回复 a 降序。
// 血量全程必须 > 0（即打完每只怪后 hp - d >= 1）。
#include <bits/stdc++.h>
using namespace std;

struct Mon {
    int d, a, id;
};

int main() {
    int n;
    long long z;
    if (scanf("%d %lld", &n, &z) != 2) return 0;
    vector<int> d(n + 1), a(n + 1);
    vector<Mon> gain, loss;
    for (int i = 1; i <= n; ++i) {
        scanf("%d %d", &d[i], &a[i]);
        if (a[i] >= d[i])
            gain.push_back({d[i], a[i], i});
        else
            loss.push_back({d[i], a[i], i});
    }
    sort(gain.begin(), gain.end(), [](const Mon &x, const Mon &y) {
        if (x.d != y.d) return x.d < y.d;
        return x.id < y.id;
    });
    sort(loss.begin(), loss.end(), [](const Mon &x, const Mon &y) {
        if (x.a != y.a) return x.a > y.a;
        return x.id < y.id;
    });

    vector<int> order;
    order.reserve(n);
    for (const Mon &mon : gain) order.push_back(mon.id);
    for (const Mon &mon : loss) order.push_back(mon.id);

    long long hp = z;
    for (int id : order) {
        if (hp - d[id] <= 0) {
            printf("NIE\n");
            return 0;
        }
        hp += a[id] - d[id];
    }
    printf("TAK\n");
    for (size_t i = 0; i < order.size(); ++i)
        printf("%d%c", order[i], i + 1 == order.size() ? '\n' : ' ');
    return 0;
}
