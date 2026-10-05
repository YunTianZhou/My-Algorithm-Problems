#include <bits/stdc++.h>
using namespace std;

class Oracle {
public:
    pair<int, string> solve(int n, const vector<int>& A, const vector<int>& B) {
        // k = number of right moves. Position at turn t is 2*k - t
        // diff is the discrete derivative of the backward cost function
        vector<int> diff(n + 2), offset(n + 1);
        vector<int> negative, next, right;
        right.reserve(2 * n);

        auto shoot = [&](int position, int t) {
            if (position < -t || position > t || (position + t) % 2) return;
            int k = (position + t) / 2;
            ++diff[k];
            if (--diff[k + 1] < 0) {
                auto it = lower_bound(negative.begin(), negative.end(), k + 1);
                if (it == negative.end() || *it != k + 1)
                    negative.insert(it, k + 1);
            }
        };

        for (int t = n; t >= 1; --t) {
            shoot(A[t - 1], t);
            if (B[t - 1] != A[t - 1]) shoot(B[t - 1], t);
            offset[t] = int(right.size());
            next.clear();
            // Ascending order makes in-place shifts simultaneous: a move
            // affects only an index smaller than every unprocessed source
            for (int p : negative) {
                if (diff[p] >= 0) continue;
                right.push_back(p); // Right is better when k + 1 == p
                diff[p - 1] += diff[p];
                diff[p] = 0;
                if (diff[p - 1] < 0) next.push_back(p - 1);
            }
            negative.swap(next);
        }
        offset[0] = int(right.size());

        string path(n, 'L');
        int k = 0;
        for (int t = 1; t <= n; ++t) {
            for (int i = offset[t]; i < offset[t - 1]; ++i) {
                if (right[i] == k + 1) {
                    path[t - 1] = 'R';
                    ++k;
                    break;
                }
            }
        }
        return {diff[0], move(path)};
    }
};
