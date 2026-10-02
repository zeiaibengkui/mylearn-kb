#include <bits/stdc++.h>
using namespace std;

using cd = complex<double>;
#define int long long
const int N = 2 ^ 21 + 1; // must be power of 2
cd tmp[N];

void fft(cd *f, int len, bool rev) {
    if (len == 1)
        return;
    const int m = len >> 1;
    for (int i = 0; i < len; i++)
        tmp[i] = f[i];
    for (int i = 0; i < m; i++) {
        f[i] = tmp[i * 2];
        f[i + m] = tmp[i * 2 + 1];
    }
    fft(f, m, rev);
    fft(f + m, len / 2, rev);

    // f(k/len)=h(2*k/len) + w_len^k* g(2*k/len)
    // h(-x)=h(x)
    // tmp[k]=f[k]+cur*f[k+len/2]
    cd step = cd(cos(2 * M_PI / len), sin(2 * M_PI / len) * pow(-1, rev)),
       cur = cd(1, 0);
    for (int k = 0; k < m; k++, cur *= step) {
        tmp[k] = f[k] + cur * f[k + m];
        tmp[k + m] = f[k] - cur * f[k + m];
    }
    for (int i = 0; i < len; i++)
        f[i] = tmp[i];
}

vector<cd> convolution(vector<cd> a, vector<cd> b) {
    int m = a.size() + b.size() - 1;
    m = pow(2, ceil(log2(m)));

    a.resize(m), b.resize(m);
    fft(a.data(), m, 0);
    fft(b.data(), m, 0);
    for (int i = 0; i < m; i++) {
        a[i] *= b[i];
    }
    fft(a.data(), m, 1);
    for (int i = 0; i < m; i++)
        a[i] /= m;
    return a;
}

signed main() {
    cin.tie(0);
    cout.tie(0);
    ios::sync_with_stdio(0);
#ifndef ONLINE_JUDGE
    freopen("in", "r", stdin);
#endif
    int n, m;
    cin >> n >> m;
    vector<cd> a(n + 1), b(m + 1);
    for (int i = 0; i < n + 1; i++)
        cin >> a[i];
    for (int i = 0; i < m + 1; i++)
        cin >> b[i];
    vector<cd> c = convolution(a, b);
    for (int i = 0; i < m + n + 1; i++)
        cout << (int)round(c[i].real()) << ' ';
}
