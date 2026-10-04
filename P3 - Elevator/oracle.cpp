#include <bits/stdc++.h>
using namespace std;

class Oracle {
    using int64 = long long;
    using Request = pair<int64, int64>;
    static constexpr int64 INF = 1LL << 62;

    int64 solveDirection(const vector<Request>& requests) {
        int64 left = INF, right = -INF;
        vector<int64> floors;
        vector<Request> downward; // (destination, origin)

        for (auto [a, b] : requests) {
            left = min({left, a, b});
            right = max({right, a, b});
            floors.push_back(a);
            floors.push_back(b);
            if (a > b) downward.push_back({b, a});
        }

        const int64 start = clamp(0LL, left, right);
        floors.push_back(start);
        sort(floors.begin(), floors.end());
        floors.erase(unique(floors.begin(), floors.end()), floors.end());
        sort(downward.begin(), downward.end());

        const int size = int(floors.size());
        auto indexOf = [&](int64 floor) {
            return int(lower_bound(floors.begin(), floors.end(), floor)
                       - floors.begin());
        };

        vector<int> peakDiff(size + 1), finishDiff(size + 1);
        for (auto [b, a] : downward) {
            int lo = indexOf(b), hi = indexOf(a);
            ++peakDiff[lo];       // b <= p < a
            --peakDiff[hi];
            ++finishDiff[lo + 1]; // b < q <= a
            --finishDiff[hi + 1];
        }

        vector<bool> blockedPeak(size), blockedFinish(size);
        vector<int64> covered(size, 0);
        int activePeak = 0, activeFinish = 0;
        for (int i = 0; i < size; ++i) {
            activePeak += peakDiff[i];
            activeFinish += finishDiff[i];
            blockedPeak[i] = activePeak > 0;
            blockedFinish[i] = activeFinish > 0;

            if (i > 0) {
                covered[i] = covered[i - 1];
                if (blockedPeak[i - 1])
                    covered[i] += floors[i] - floors[i - 1];
            }
        }

        // Case B uses the smallest valid initial peak.
        int peakIndex = indexOf(start);
        while (blockedPeak[peakIndex]) ++peakIndex;
        const int64 fixedPeak = floors[peakIndex];
        const int64 base = 2 * (right - left) - start;

        int64 best = INF;
        if (start == left)
            best = (right - left) + 2 * covered.back();

        int next = 0;
        int64 requiredPeak = start;
        for (int i = 0; i < size; ++i) {
            int64 finish = floors[i];

            // Case A: requests ending below finish must be served early.
            while (next < int(downward.size()) &&
                   downward[next].first < finish) {
                requiredPeak = max(requiredPeak, downward[next].second);
                ++next;
            }
            int64 peak = max(requiredPeak, finish);
            best = min(best, base + 2 * peak - finish);

            // Case B: only the middle interval needs extra backtracking.
            if (fixedPeak < finish && !blockedFinish[i]) {
                int64 middleUnion = covered[i] - covered[peakIndex];
                best = min(best, base + 2 * fixedPeak - finish
                                      + 2 * middleUnion);
            }
        }

        return abs(start) + best;
    }

public:
    long long solve(int n, const vector<int>& A, const vector<int>& B) {
        vector<Request> requests;
        for (int i = 0; i < n; ++i)
            requests.push_back({A[i], B[i]});

        int64 answer = solveDirection(requests);
        for (auto& [a, b] : requests) {
            a = -a;
            b = -b;
        }
        return min(answer, solveDirection(requests));
    }
};
