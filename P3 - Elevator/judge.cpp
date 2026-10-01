// judge_elevator.cpp
// Judge + random test generator for P0003 "Elevator"
// Subtasks:
// 1. A_i = B_i
// 2. 0 <= A_i, B_i <= 10^5
// 3. 0 <= A_i, B_i <= 10^9
// 4. N <= 10
// 5. No additional constraints (-10^9 < A_i, B_i < 10^9)

#include <bits/stdc++.h>
using namespace std;

// ---------- Oracle / Reference Solution ----------
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
            left = min({ left, a, b });
            right = max({ right, a, b });
            floors.push_back(a);
            floors.push_back(b);
            if (a > b) downward.push_back({ b, a });
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
            requests.push_back({ A[i], B[i] });

        int64 answer = solveDirection(requests);
        for (auto& [a, b] : requests) {
            a = -a;
            b = -b;
        }
        return min(answer, solveDirection(requests));
    }
};


long long getMinTime(int n, const vector<int>& A, const vector<int>& B) {
    return Oracle().solve(n, A, B);
}

// ---------- Random helpers ----------
static mt19937_64 rng((unsigned)chrono::steady_clock::now().time_since_epoch().count());
static inline long long randint(long long L, long long R) {
    if (L > R) return L;
    uniform_int_distribution<long long> d(L, R);
    return d(rng);
}

// ---------- Generation honoring subtasks ----------
struct CaseData {
    int N;
    vector<int> A, B;
    long long expected;
};

CaseData build_case(int N, const vector<int>& A, const vector<int>& B) {
    CaseData cd;
    cd.N = N;
    cd.A = A;
    cd.B = B;
    cd.expected = getMinTime(N, A, B);
    return cd;
}

vector<CaseData> gen_cases_for_subtask_ordered(int subtask, int K = 100) {
    vector<CaseData> tests; tests.reserve(K);
    const int MAX_N = 100000;
    const long long LIMIT = 1000000000LL - 1;

    for (int idx = 0; idx < K; ++idx) {
        // band small->big
        double frac = double(idx) / double(max(1, K - 1));
        int band = 0;
        if (frac < 0.50) band = 0;
        else if (frac < 0.85) band = 1;
        else if (frac < 0.98) band = 2;
        else band = 3;

        int N_min, N_max;
        if (subtask == 4) {
            N_min = 1; N_max = 10;
        }
        else {
            if (band == 0) { N_min = 1; N_max = 500; }
            else if (band == 1) { N_min = 501; N_max = 50000; }
            else if (band == 2) { N_min = 50001; N_max = 90000; }
            else { N_min = 90001; N_max = MAX_N; }
        }

        int N = randint(N_min, min(N_max, MAX_N));

        vector<int> A(N), B(N);
        for (int i = 0; i < N; ++i) {
            long long a, b;
            if (subtask == 1) {
                a = randint(-LIMIT, LIMIT);
                b = a;
            }
            else if (subtask == 2) {
                a = randint(0, 100000);
                b = randint(0, 100000);
                while (a == b) b = randint(0, 100000);
            }
            else if (subtask == 3) {
                a = randint(0, LIMIT);
                b = randint(0, LIMIT);
                while (a == b) b = randint(0, LIMIT);
            }
            else {
                // Subtasks 4 and 5 use the full coordinate range.
                a = randint(-LIMIT, LIMIT);
                b = randint(-LIMIT, LIMIT);
                while (a == b) b = randint(-LIMIT, LIMIT);
            }
            A[i] = a;
            B[i] = b;
        }

        tests.push_back(build_case(N, A, B));
    }

    // ensure increasing order by size
    sort(tests.begin(), tests.end(), [](const CaseData& a, const CaseData& b) {
        return a.N < b.N;
        });

    return tests;
}

// ---------- User Solution ----------
class Solution {
public:
    long long solve(int n, const vector<int>& A, const vector<int>& B) {
        return 0;
    }
};

// ---------- Main: generate tests, run solver, report ----------
int main() {
    const int CASES_PER_SUBTASK = 100;

    Solution solver;

    const string RED = "\033[1;31m";
    const string GREEN = "\033[1;32m";
    const string CYAN = "\033[1;36m";
    const string RESET = "\033[0m";

    using clock = chrono::high_resolution_clock;
    auto total_start = clock::now();

    int global_pass = 0, global_fail = 0;
    int case_id = 0;

    cerr << "Generating & judging P0003 Elevator (ordered small -> big per subtask)\n";

    for (int sub = 1; sub <= 5; ++sub) {
        cout << "\n"
            << CYAN
            << "==================== SUBTASK " << sub << " ====================\n"
            << RESET;

        auto tests = gen_cases_for_subtask_ordered(sub, CASES_PER_SUBTASK);

        int pass = 0, fail = 0;
        auto sub_start = clock::now();

        for (size_t i = 0; i < tests.size(); ++i) {
            const CaseData& c = tests[i];
            ++case_id;

            auto start = clock::now();
            long long got = solver.solve(c.N, c.A, c.B);
            auto end = clock::now();
            long long dur_us =
                chrono::duration_cast<chrono::microseconds>(end - start).count();

            bool ok = (got == c.expected);

            if (ok) {
                cout << GREEN << "AC" << RESET;
                ++pass;
                ++global_pass;
            }
            else {
                cout << RED << "WA" << RESET;
                ++fail;
                ++global_fail;
            }

            cout << "  Case " << case_id
                << " | N=" << c.N
                << " | expected=" << c.expected
                << " got=" << got
                << " | time=" << fixed << setprecision(3)
                << (dur_us / 1000.0) << " ms\n";

            if (!ok) {
                int preview = min((int)c.A.size(), 20);
                cout << "  Requests preview (First " << preview << " elements):\n";

                ostringstream is;
                for (int j = 0; j < preview; ++j) {
                    is << "    " << c.A[j] << " -> " << c.B[j] << "\n";
                }
                cout << is.str();

                if (preview < (int)c.A.size())
                    cout << "    ...\n";
            }
        }

        auto sub_end = clock::now();
        long long sub_us =
            chrono::duration_cast<chrono::microseconds>(sub_end - sub_start).count();

        cout << CYAN
            << "-------------------- SUBTASK " << sub << " SUMMARY --------------------\n"
            << RESET;
        cout << "Passed: " << pass << " / " << tests.size()
            << ", Failed: " << fail
            << ", Time: " << fixed << setprecision(3)
            << (sub_us / 1000.0) << " ms\n";
    }

    auto total_end = clock::now();
    long long total_us =
        chrono::duration_cast<chrono::microseconds>(total_end - total_start).count();

    cout << "\n"
        << CYAN
        << "==================== FINAL SUMMARY ====================\n"
        << RESET;
    cout << "Total passed: " << global_pass
        << ", Total failed: " << global_fail
        << ", Total cases: " << (global_pass + global_fail) << "\n";
    cout << "Total time: " << fixed << setprecision(3)
        << (total_us / 1000.0) << " ms\n";

    if (global_fail > 0) {
        cout << RED
            << "At least one WA -> Solution is incorrect on these tests."
            << RESET << "\n";
    }
    else {
        cout << GREEN
            << "All tests passed -> Solution looks correct on generated tests."
            << RESET << "\n";
    }

    return 0;
}
