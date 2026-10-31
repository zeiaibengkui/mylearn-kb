#include <bits/stdc++.h>
using namespace std;
using ll = long long;
int c, t;
void init(int cc, int tt) {
    c = cc;
    t = tt;
};

ll cost(ll x, ll z) {
    ll res = 0;
    while (z >= 2 * x) {
        res += (z & 1) + 1; // 奇数先减1（代价1），再除以2（代价1）
        z >>= 1;
    }
    res += z - x; // 此时只能一路减到 x
    return res;
}
#define bitlen(x) 64 - __builtin_clzll(x)
ll calc(ll x, ll z) {
    int bz = bitlen(z), bt = bitlen(x);
    int k = bz - bt;
    if (k < 0)
        k = 0;
    if ((z >> k) >= x * 2)
        k++;
    ll r = z & ((1LL << k) - 1);
    ll c = (ll)k + __builtin_popcountll(r) + (z >> k) - x;
    return c;
}

ll binary(ll x, ll y) {
    if (x > y)
        swap(x, y);
    ll mi = LLONG_MAX - 1;
    for (ll z = y; z <= y * 2; z++) {
        mi = min(mi, calc(x, z) + z - y);
    }
    return mi;
}

int main() { cout << binary(7, 33); }
