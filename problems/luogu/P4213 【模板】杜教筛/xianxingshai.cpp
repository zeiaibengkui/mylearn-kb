#include <bits/stdc++.h>
using namespace std;

#define int long long

const int N = 1e6;
bool not_prime[N];
int phi[N], mu[N], phi_sum[N], mu_sum[N];
vector<int> prime;

void pre(int n) {
    phi[1] = 1;
    mu[0] = 1;
    mu_sum[0] = 1;
    for (int i = 2; i <= n; i++) {
        if (!not_prime[i]) {
            prime.push_back(i);
            phi[i] = i - 1;
            mu[i] = -1;
        }
        for (int j : prime) {
            if (i * j > n)
                break;
            not_prime[i * j] = true;
            if (i % j == 0) {
                phi[i * j] = phi[i] * j;
                mu[i * j] = 0;
                break;
            } else {
                phi[i * j] = phi[i] * phi[j];
                mu[i * j] = mu[i] * mu[j];
            }
        }
    }

    for (int i = 1; i <= n; i++) {
        phi_sum[i] = phi_sum[i - 1] + phi[i];
        mu_sum[i] = mu_sum[i - 1] + mu[i];
    }
}

signed main() {
    int T;
    cin >> T;
    pre(N - 1);
    while (T--) {
        int n;
        cin >> n;
        cout << phi_sum[n] << ' ' << mu_sum[n] << '\n';
    }
}
