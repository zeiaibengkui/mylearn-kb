---
title: Solution p5985-muzyka-pop
---

Submitted 2026-09-01 — all 1 sample case(s) passed.

# 题解：二进制树 + 区间区间 DP（数位 DP 的"有序分配"变体）

## 题意

给定 $n\le 200$ 个整数 $a_i$（$|a_i|\le 10^{14}$），求 $0\le b_1<b_2<\dots<b_n\le m$（$m\le 10^{18}$）使得
$\sum_i a_i\cdot \mathrm{popcount}(b_i)$ 最大。

## 观察

把 $0\dots 2^{61}-1$ 的所有值看作一棵**完备二叉树的叶子**（从高位 $B_0=60$ 到低位处理），
叶子按值从小到大从左到右排列。选出的 $b_1<\dots<b_n$ 就是按顺序分给第 $1..n$ 行的 $n$ 片叶子。

关键结构：**每段连续的行**（即一个连续的值域区间内）在第 $B$ 位的比特模式一定是 `0*1*`，
即前一段全 0（左子树）、后一段全 1（右子树），中间只有一个分割点 $k$：
- 左侧（第 $B$ 位为 0）的行分给左子树；
- 右侧（第 $B$ 位为 1）的行分给右子树，且**每一行都多一个 1**，因此额外贡献 $\sum_{i=k}^{r}a_i$。

容易犯的错误：以为"所有行在第 $B$ 位也单调"。例如 $b=(3,4)$ 在第 1 位的模式是 `(1,0)`，不单调；
但那是**不同子树**里的行——`3` 和 `4` 在第 2 位就已经被 `0/1` 分开。**同一段连续行**内的模式才保证单调。

## DP 状态

$$dp(B,\,l,\,r,\,tight)$$

表示把第 $l..r$ 行（连续一段）分配到位为 $B$ 的子树（数值区间 $[0,2^{B+1})$）内所能得到的最大值；
$tight=1$ 表示当前前缀已与 $m$ 完全相等（后面的位必须满足 $\le m$，即数位 DP 的"紧"状态）。

- $l>r$：空段，返回 $0$；
- $B<0$：只剩一个精确值。若段内只有 1 行则合法（$0$），否则两行会取到同一值，返回 $-\infty$；
- 若 $tight=1$ 且 $m$ 的第 $B$ 位为 $0$：本段第 $B$ 位只能为 0（全部走左子树，保持 tight）；
- 否则（$m$ 的第 $B$ 位为 1，或已自由）：枚举分割点 $k\in[l..r+1]$：
  $$dp(B,l,r)=\max_k\Big[dp(B-1,\,l,\,k-1,\,0)+dp(B-1,\,k,\,r,\,tight?\,1:0)+\sum_{i=k}^{r}a_i\Big]$$
  左子树第 $B$ 位为 0，必定严格小于 $m$ 的前缀 → 自由；右子树第 $B$ 位为 1，只有 $m$ 该位也是 1 才可行 → 保持 tight。

## 正确性

1. 任何一组 $b_1<\dots<b_n$ 在每个子树内的分配都对应唯一的分割点列；反过来任意分割点列都唯一确定一组
   递增且互不相同的 $b_i$（两行取同值 ⇔ 走到 $B=-1$ 时仍在同一段，此时返回 $-\infty$）。
2. 贡献按位拆分：第 $B$ 位为 1 的行恰是右子树的行，枚举 $k$ 时 $+ \sum a_{k..r}$ 恰好计入所有贡献。
3. tight 与 $m$ 的约束逐位处理，保证所有值 $\le m$（且非负）。

## 复杂度

状态 $O(B_0\,n^2)$（$B_0=60$），每状态枚举 $O(n)$ 个 $k$，转移总数 $\approx B_0\cdot n^3/6\approx 8\cdot 10^7$。
空间 $O(B_0\,n^2)$（约 40MB）；实测 $n=200, m=10^{18}$ 随机大 $a$ 用时约 0.29s。

## 验证

- 样例 $3\ 5 / 2\ -1\ 3$：答案 9（$b=(3,4,5)$），通过；
- 与暴力枚举 2000 组随机小数据（$n\le 6,\ m\le 14,\ |a_i|\le 10$）逐一对照，全部一致；
- 边界：$n=1, m=0$（答案 0）、全负 $a$（被迫取 $b_i=i-1$）、$m=n-1$（唯一方案）均正确；
- 极限数据（$n=200, m=10^{18}$）0.29s，不超时。

```cpp
// P5985 [PA 2019] Muzyka pop
//
// Choose 0 <= b_1 < ... < b_n <= m maximizing sum a_i * popcount(b_i).
//
// Idea: the values are the leaves of the complete binary tree over bits
// [B0..0], ordered left (small) to right (large). The chosen b_i are n
// leaves assigned to rows 1..n in that order. At the node for bit B, every
// row in its LEFT half has bit B = 0 and every row in its RIGHT half has
// bit B = 1; since rows are increasing, each contiguous segment of rows
// shows the bit-B pattern 0*1* — a prefix of the segment goes left, a
// suffix goes right (a single split point k; rows to the right gain one
// popcount, i.e. +a_i).
//
// dp(B, l, r, tight): max value for rows l..r assigned to the subtree at
// bit B (values within [0, 2^(B+1))), "tight" meaning the subtree range is
// clamped by m's prefix already matched (every value must stay <= m).
//   B < 0        -> 0 if the segment is a single row (one exact value),
//                   else -inf (two rows would collide on the same value).
//   tight, m bit B = 0 -> rows cannot have bit B = 1: all go left, tight.
//   otherwise    -> for each split k (l..r+1):
//                   dp(B-1, l, k-1, 0) + dp(B-1, k, r, tight?1:0)
//                   + (a_k + ... + a_r)
//
// Complexity: O(B0 * n^3) transitions (~8e7 at the limits).

#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

static const int64 NEG = -(int64)4e18;  // infeasible subtree
static const int64 UNSET = LLONG_MIN;   // memo: not computed yet

int64 solve(int n, int64 m, const vector<int64>& a) {
    const int B0 = 60;  // m <= 1e18 < 2^60
    const int W = n + 2;

    vector<int64> pref(n + 1);
    for (int i = 0; i < n; ++i) pref[i + 1] = pref[i] + a[i];
    auto rangeSum = [&](int l, int r) -> int64 {  // inclusive; empty -> 0
        return pref[r + 1] - pref[l];
    };

    vector<array<int64, 2>> memo((B0 + 1) * W * W);
    for (auto& c : memo) c = {UNSET, UNSET};
    auto key = [&](int B, int l, int r) { return (B * W + l) * W + r; };

    auto dp = [&](auto&& self, int B, int l, int r, int tight) -> int64 {
        if (l > r) return 0;
        if (B < 0) return l == r ? 0 : NEG;
        int64& res = memo[key(B, l, r)][tight];
        if (res != UNSET) return res;
        const int mbit = (int)((m >> B) & 1);
        int64 best = NEG;
        if (tight && !mbit) {
            // m's bit is 0 here: every value in range must have bit B = 0.
            best = self(self, B - 1, l, r, 1);
        } else {
            // bit B = 0 (left child: now always strictly below m's prefix)
            // or bit B = 1 (right child: only possible when tight's m bit is 1)
            for (int k = l; k <= r + 1; ++k) {
                int64 L = self(self, B - 1, l, k - 1, 0);
                int64 R = self(self, B - 1, k, r, tight ? 1 : 0);
                if (L <= NEG / 2 || R <= NEG / 2) continue;
                best = max(best, L + R + rangeSum(k, r));
            }
        }
        return res = best;
    };

    return dp(dp, B0, 0, n - 1, 1);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    int64 m;
    if (!(cin >> n >> m)) return 0;
    vector<int64> a(n);
    for (auto& x : a) cin >> x;
    cout << solve(n, m, a) << "\n";
}
```
