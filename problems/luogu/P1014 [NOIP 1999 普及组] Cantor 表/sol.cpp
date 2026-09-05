#include <iostream>
using namespace std;

int main() {
    long long n;
    if (!(cin >> n)) return 0;

    // Terms are grouped by anti-diagonal k = numerator + denominator;
    // diagonal k holds (k-1) terms, and the cumulative count through diagonal k
    // is k*(k-1)/2.
    long long k = 1;
    while (k * (k - 1) / 2 < n) ++k;

    long long before = (k - 1) * (k - 2) / 2; // terms before this diagonal
    long long pos = n - before;               // 1-based position on this diagonal

    long long num, den;
    if (k % 2 == 0) {
        // even diagonal: numerators go k-1, k-2, ... down to 1
        num = k - pos;
        den = pos;
    } else {
        // odd diagonal: numerators go 1, 2, ... up to k-1
        num = pos;
        den = k - pos;
    }

    cout << num << "/" << den << "\n";
    return 0;
}
