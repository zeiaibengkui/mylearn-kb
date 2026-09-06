---
title: hdu6035 Colorful Tree
category: hdu
---
> HDU [6035 Colorful Tree](http://acm.hdu.edu.cn/showproblem.php?pid=6035) · 多校（2017 多校第一场 1003）

## 题目描述

There is a tree with $n$ nodes, each of which has a type of color represented by an integer, where the color of node $i$ is $c_i$.

The path between each two different nodes is unique, of which we define the value as the number of different colors appearing in it.

Calculate the sum of values of all paths on the tree that has $\frac{n(n-1)}{2}$ paths in total.

## 输入格式

The input contains multiple test cases.

For each test case, the first line contains one positive integer $n$, indicating the number of nodes. $(2 \leq n \leq 200000)$

Next line contains $n$ integers where the $i$-th integer represents $c_i$, the color of node $i$. $(1 \leq c_i \leq n)$

Each of the next $n-1$ lines contains two positive integers $x, y$ $(1 \leq x, y \leq n, x \neq y)$, meaning an edge between node $x$ and node $y$.

It is guaranteed that these edges form a tree.

## 输出格式

For each test case, output `Case #x: y` in one line (without quotes), where $x$ indicates the case number starting from $1$ and $y$ denotes the answer of the corresponding case.

## 样例

### 样例 1

**输入**

```text
3
1 2 1
1 2
2 3
6
1 2 1 3 2 1
1 2
1 3
2 4
2 5
3 6
```

**输出**

```text
Case #1: 6
Case #2: 29
```
