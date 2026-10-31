#include <bits/stdc++.h>

using namespace std;

const int N = 600; // 叶子 ≤256，内部 ≤255
struct node {
    int l, r, freq;
} tree[N];
int tot;
char charOfLeaf[N]; // 叶子编号 -> 字符

namespace {

int buildTree(const uint32_t freq[256]) {
    tot = 0;
    priority_queue<pair<uint32_t, int>, vector<pair<uint32_t, int>>,
                   greater<pair<uint32_t, int>>>
        pq;
    for (int i = 0; i < 256; ++i) {
        if (freq[i]) {
            tree[tot] = {-1, -1, (int)freq[i]};
            charOfLeaf[tot] = (char)i;
            pq.push({freq[i], tot});
            ++tot;
        }
    }
    if (pq.empty())
        return -1;
    if (pq.size() == 1)
        return pq.top().second;
    while (pq.size() > 1) {
        auto [f1, a] = pq.top();
        pq.pop();
        auto [f2, b] = pq.top();
        pq.pop();
        tree[tot] = {a, b, (int)(f1 + f2)};
        pq.push({f1 + f2, tot});
        ++tot;
    }
    return pq.top().second;
}

void genCodes(int u, string &prefix, string codes[256]) {
    if (u == -1)
        return;
    if (tree[u].l == -1) {
        codes[(unsigned char)charOfLeaf[u]] = prefix.empty() ? "0" : prefix;
        return;
    }
    prefix += '0';
    genCodes(tree[u].l, prefix, codes);
    prefix.pop_back();
    prefix += '1';
    genCodes(tree[u].r, prefix, codes);
    prefix.pop_back();
}

} // namespace

string compress(string s) {
    uint32_t len = (uint32_t)s.size();
    uint32_t freq[256] = {0};
    for (char c : s)
        ++freq[(unsigned char)c];

    string out;
    // vocab
    out.append(reinterpret_cast<const char *>(&len), 4);
    out.append(reinterpret_cast<const char *>(freq), 256 * 4);
    if (len == 0)
        return out;

    int root = buildTree(freq);
    string codes[256], prefix;
    genCodes(root, prefix, codes);

    uint8_t buf = 0;
    int nbits = 0;
    for (char c : s) {
        for (char b : codes[(unsigned char)c]) {
            buf = (uint8_t)((buf << 1) + (b - '0'));
            if (++nbits == 8) {
                out += (char)buf;
                buf = 0;
                nbits = 0;
            }
        }
    }
    if (nbits)
        out += (char)(buf << (8 - nbits));
    return out;
}

string decompress(string s) {
    if (s.size() < 4 + 1024)
        return "";

    uint32_t len;
    memcpy(&len, s.data(), 4);
    uint32_t freq[256];
    memcpy(freq, s.data() + 4, 1024);
    if (len == 0)
        return "";

    int root = buildTree(freq);

    string ans;
    ans.reserve(len);
    if (tree[root].l == -1) { // 单字符
        ans.assign(len, charOfLeaf[root]);
        return ans;
    }

    size_t pos = 4 + 1024;
    int cur = root;
    uint8_t buf = 0;
    int nbits = 0;
    while (ans.size() < len) {
        if (nbits == 0) {
            if (pos >= s.size())
                break;
            buf = (uint8_t)s[pos++];
            nbits = 8;
        }
        int bit = (buf >> 7) & 1;
        buf = (uint8_t)(buf << 1);
        --nbits;
        cur = bit ? tree[cur].r : tree[cur].l;
        if (tree[cur].l == -1) {
            ans += charOfLeaf[cur];
            cur = root;
        }
    }
    return ans;
}

// int main() {
//     string op, s;
//     cin >> op >> s;
//     if (op == "COMPRESS")
//         cout << compress(s);
//     else
//         cout << decompress(s);
// }

int main() {
    string s = "abcabcabc";
    for (int i = 1; i <= 18; i++)
        s += s;
    string a = compress(compress(s));
    string b = decompress(decompress(a));
    cout << (s == b) << " " << (a.size() / (float)b.size()) << '\n';
}
