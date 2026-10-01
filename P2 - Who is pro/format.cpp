#include <bits/stdc++.h>
using namespace std;

// If impossible, return an empty string
// If possible, return a string of n characters, where each character is "P" (pro) or "N" (noob)
// The ith character represents the ith person's role
class Solution {
public:
    string solve(int n, int m, const vector<int>& A, const vector<int>& B, 
                               const vector<string>& X, const vector<string>& Y) {
        return "";
    }
};

int main() {
    int n, m; cin >> n >> m;

    vector<int> A(m), B(m);
    vector<string> X(m), Y(m);
    for (int i = 0; i < m; i++) {
        int a, b;
        string x, y;
        cin >> a >> x >> b >> y;
        A[i] = a;
        B[i] = b;
        X[i] = x;
        Y[i] = y;
    }

    cout << Solution().solve(n, m, A, B, X, Y);
}
