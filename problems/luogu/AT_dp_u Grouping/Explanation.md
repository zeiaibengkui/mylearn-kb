# AT_dp_u Grouping

## 思路 / Idea

把 $N$ 只兔子分成若干组，收益是每组内部两两相性 $a_{i,j}$（$i<j$）之和的总和。$N\le 16$，可以枚举子集，这是典型的**子集 DP（SOS DP / bitmask DP）**。

### 1. 预计算每个子集作为单独一组的得分

记 `score[mask]` 为兔子集合 `mask` 内部所有 $a_{i,j}$（$i<j$）之和，也就是把这 `mask` 只看成**一组**时能拿到的分数。

先给每一对 `(i,j)` 的位掩码 `(1<<i)|(1<<j)` 累加 `a[i][j]`，再做**超集 SOS（zeta 变换）**：

```
for b in 0..n-1:
  for mask in 0..(1<<n)-1:
    if (mask & (1<<b)):
      score[mask] += score[mask ^ (1<<b)]
```

变换后 `score[mask]` 就等于所有能被 `mask` 覆盖的 pair 分数之和，即 mask 作为一组的内部分数。这一步是标准的 SOS DP，把枚举所有 pair 的 $O(n^2 2^n)$ 收敛为一次 $\Theta(n\,2^n)$ 的变换。

### 2. 子集划分 DP

令 `dp[mask]` 为把 `mask` 划分成若干组能获得的最大总分。

为了不重复计数，固定 `mask` 的**最低位** `lb` 作为它所在组的代表。枚举所有包含 `lb` 的子集 `sub`：

```
dp[mask] = max( dp[mask ^ sub] + score[sub] )
```

- `score[sub]`：这一组 `sub` 的得分；
- `dp[mask ^ sub]`：剩下兔子 `mask \ sub` 的最优划分。

因为每组都唯一对应到「包含最低位的子集」，每个划分恰好被统计一次，无需去重。

### 3. 复杂度

- `score`：初始 pair 累加 $O(n^2)$ + SOS 变换 $O(n\,2^n)$。
- `dp`：对每个 mask 枚举其子集，总复杂度 $O(3^n)$（$\sum_{\text{mask}} 2^{\text{popcount(mask)}} = 3^n$）。

$n=16$ 时 $3^{16}\approx 4.3\times10^7$，可轻松通过。答案可达 $\sum_{i<j}|a_{i,j}|\approx 1.2\times10^{11}$，故用 `long long`。

## 关键点 / Key observations

- **最低位代表** 用 `lb` 计数，保证分区不重不漏，避免 $O(2^{\text{popcount}})$ 的全枚举以去重。
- **SOS 超集变换** 一次得到所有子集内部分数，避免重复枚举 pair。
- 可能存在负相性，因此不能贪心，必须枚举划分；但也不能直接把人合并成一组（样例 2 拆开更好）。

## 这个题教会我 / What it teaches

- 子集状压 DP + 用最低位固定代表来去重。
- SOS DP（超集 / 子集 zeta）把 $O(n^2 2^n)$ 的「每对覆盖所有超集」化简为 $\Theta(n2^n)$。
- 划分式 DP 的经典结构：`dp[mask] = max(dp[mask ^ sub] + value[sub])`。
