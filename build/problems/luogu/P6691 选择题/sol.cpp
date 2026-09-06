#include <bits/stdc++.h>
using namespace std;

const int MOD = 998244353;

struct DSU {
    vector<int> parent, sz, parity; // parity[x] = color[x] XOR color[parent[x]]
    DSU(int n) : parent(n+1), sz(n+1,1), parity(n+1,0) {
        for (int i=1;i<=n;i++) parent[i]=i;
    }
    int find(int x){
        if (parent[x]==x) return x;
        int r = find(parent[x]);
        parity[x] ^= parity[parent[x]];
        parent[x] = r;
        return r;
    }
    // require color[x] XOR color[y] == p
    bool unite(int x, int y, int p){
        int rx = find(x), ry = find(y);
        if (rx==ry){
            return (parity[x]^parity[y]) == p;
        }
        int val = p ^ parity[x] ^ parity[y]; // color[rx] XOR color[ry]
        if (sz[rx] < sz[ry]){
            parent[rx] = ry;
            parity[rx] = val;
            sz[ry] += sz[rx];
        } else {
            parent[ry] = rx;
            parity[ry] = val;
            sz[rx] += sz[ry];
        }
        return true;
    }
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    if(!(cin>>n)) return 0;
    DSU dsu(n);
    bool ok = true;
    for(int i=1;i<=n;i++){
        int a, opt; cin>>a>>opt;
        int p = 1 - opt; // color[i] XOR color[a] = 1-opt
        if(!dsu.unite(i, a, p)) ok=false;
    }
    if(!ok){ cout<<"No answer\n"; return 0; }

    vector<int> cnt0(n+1,0);
    for(int i=1;i<=n;i++){
        int r = dsu.find(i);
        if(dsu.parity[i]==0) cnt0[r]++;
    }

    long long ansCnt = 1;
    long long maxOnes = 0, minOnes = 0;
    for(int i=1;i<=n;i++){
        if(dsu.parent[i]==i){ // root
            int s = dsu.sz[i];
            int c0 = cnt0[i];
            int c1 = s - c0;
            maxOnes += max(c0, c1);
            minOnes += min(c0, c1);
            ansCnt = (ansCnt * 2) % MOD;
        }
    }
    cout<<ansCnt<<"\n"<<maxOnes<<"\n"<<minOnes<<"\n";
    return 0;
}
