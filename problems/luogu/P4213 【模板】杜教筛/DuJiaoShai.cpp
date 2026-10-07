#include <bits/stdc++.h>
// https://www.luogu.com.cn/record/301815319
using namespace std;

#define int long long

const int N = 1667000;
bool not_prime[N];
int phi[N], mu[N], phi_sum[N], mu_sum[N];
unordered_map<int, int> memo_phi, memo_mu;
vector<int> prime;

void pre(int n) {
    phi[1] = 1;
    mu[0] = 1;
    mu[1] = 1;
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

int Smu(int n) {
    if (n < N)
        return mu_sum[n];
    if (memo_mu[n])
        return memo_mu[n];
    int ans = 1;
    for (int l = 2, r = 0; l <= n; l++) {
        r = n / (n / l);
        ans -= Smu(n / l) * (r - l + 1);
        l = r;
    }
    memo_mu[n] = ans;
    return ans;
}
int Sphi(int n) {
    if (n < N)
        return phi_sum[n];
    if (memo_phi[n])
        return memo_phi[n];
    int ans = n * (n + 1) / 2;
    for (int l = 2, r = 0; l <= n; l++) {
        r = n / (n / l);
        ans -= Sphi(n / l) * (r - l + 1);
        l = r;
    }
    memo_phi[n] = ans;
    return ans;
}

signed main() {
    cin.tie(0);
    cout.tie(0);
    ios::sync_with_stdio(0);
    int T;
    cin >> T;
    pre(N - 1);
    while (T--) {
        int n;
        cin >> n;
        cout << Sphi(n) << ' ' << Smu(n) << '\n';
    }
}
