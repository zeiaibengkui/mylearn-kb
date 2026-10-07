#include <bits/stdc++.h>
// https://www.luogu.com.cn/record/301815319
// 单组询问版本：用 dp 数组代替哈希表。多组询问时每个 n 都要清空 dp，会退化。
using namespace std;

#define int long long

const int LIM = 1700000;  // precompute prefix sums up to here (~ n^(2/3))

bool not_prime[LIM + 1];
int phi[LIM + 1], mu[LIM + 1];
int phi_sum[LIM + 1], mu_sum[LIM + 1];
vector<int> prime;

// Everything met in the recursion is floor(n / i) for the current query n,
// so a value above LIM is uniquely identified by n / x.  That turns the
// memo into a plain array instead of a hash table.
int cur;
vector<int> dp_phi, dp_mu;
vector<char> vis_phi, vis_mu;

void pre(int n) {
    phi[1] = 1;
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

int Sphi(int x) {
    if (x <= LIM)
        return phi_sum[x];
    int id = cur / x;
    if (vis_phi[id])
        return dp_phi[id];
    int ans = x * (x + 1) / 2;
    for (int l = 2, r = 0; l <= x; l++) {
        r = x / (x / l);
        ans -= Sphi(x / l) * (r - l + 1);
        l = r;
    }
    vis_phi[id] = 1;
    return dp_phi[id] = ans;
}

int Smu(int x) {
    if (x <= LIM)
        return mu_sum[x];
    int id = cur / x;
    if (vis_mu[id])
        return dp_mu[id];
    int ans = 1;
    for (int l = 2, r = 0; l <= x; l++) {
        r = x / (x / l);
        ans -= Smu(x / l) * (r - l + 1);
        l = r;
    }
    vis_mu[id] = 1;
    return dp_mu[id] = ans;
}

signed main() {
    cin.tie(0);
    cout.tie(0);
    ios::sync_with_stdio(0);
    int T;
    cin >> T;
    vector<int> q(T);
    int nmax = 0;
    for (int i = 0; i < T; i++) {
        cin >> q[i];
        nmax = max(nmax, q[i]);
    }
    pre(LIM);
    int cap = nmax / (LIM + 1) + 2;
    dp_phi.assign(cap, 0);
    dp_mu.assign(cap, 0);
    vis_phi.assign(cap, 0);
    vis_mu.assign(cap, 0);
    vector<int> ans_phi(T), ans_mu(T);
    for (int i = 0; i < T; i++) {
        int dup = -1;
        for (int k = 0; k < i; k++)
            if (q[k] == q[i]) {
                dup = k;
                break;
            }
        if (dup >= 0) {  // repeated query: reuse the answer
            ans_phi[i] = ans_phi[dup];
            ans_mu[i] = ans_mu[dup];
            continue;
        }
        cur = q[i];
        fill(vis_phi.begin(), vis_phi.end(), 0);
        fill(vis_mu.begin(), vis_mu.end(), 0);
        ans_phi[i] = Sphi(cur);
        ans_mu[i] = Smu(cur);
    }
    for (int i = 0; i < T; i++)
        cout << ans_phi[i] << ' ' << ans_mu[i] << '\n';
}
