#include <bits/stdc++.h>
using namespace std;

// Output a single non-negative integer represents the minimum time (in milliseconds) it takes to transport all residents.
class Solution {
public:
    long long solve(int n, const vector<int>& A, const vector<int>& B) {
		return 0;
    }
};

int main() {
    int n; cin >> n;

    vector<int> A(n), B(n);
    for (int i = 0; i < n; i++) {
        int a, b;
        cin >> a >> b;
        A[i] = a;
        B[i] = b;
    }

    cout << Solution().solve(n, A, B);
}
