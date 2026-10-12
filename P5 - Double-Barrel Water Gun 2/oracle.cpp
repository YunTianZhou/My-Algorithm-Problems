#include <bits/stdc++.h>
using namespace std;

class Oracle {
public:
    tuple<int, vector<int>, vector<int>> solve(int n) {
        vector<int> A(n), B(n);

        int ans = 1, L = 1;
        for (int i = 1; i <= n; i++) {
            if (i == 2 * L) {
                ans++;
                L *= 2;
            }

            int a = 3 * i - 4 * L;
            A[i - 1] = a;
            B[i - 1] = a + 2;
        }

        if (n + 1 < 2 * L)
            ans--;
        return {ans, A, B};
    }
};
