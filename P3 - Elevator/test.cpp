// Random test generator for P0003 "Elevator".
// Subtasks:
// 1. A_i = B_i
// 2. 0 <= A_i, B_i <= 10^5
// 3. 0 <= A_i, B_i <= 10^9
// 4. N <= 10
// 5. No additional constraints (-10^9 < A_i, B_i < 10^9)

#include <bits/stdc++.h>
using namespace std;

// ---------- Random helpers ----------
static mt19937_64 rng;
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
    cd.expected = reference::Oracle().solve(N, A, B);
    return cd;
}

vector<CaseData> gen_cases_for_subtask_ordered(int subtask, int K, judge::Runner& runner) {
    vector<CaseData> tests; tests.reserve(K);
    const int MAX_N = 100000;
    const long long LIMIT = 1000000000LL - 1;

    runner.preparing(0, K);
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
        runner.preparing(idx + 1, K);
    }

    // ensure increasing order by size
    sort(tests.begin(), tests.end(), [](const CaseData& a, const CaseData& b) {
        return a.N < b.N;
        });

    return tests;
}

void run_tests(judge::Runner& runner, const judge::Options& options) {
    runner.set_time_limit(chrono::milliseconds(2000)); // See problem.md.
    rng.seed(options.seed);
    for (int sub = 1; sub <= 5; ++sub) {
        runner.section("Subtask " + to_string(sub), options.cases);
        for (const auto& c : gen_cases_for_subtask_ordered(sub, options.cases, runner)) {
            if (!runner.test("N=" + to_string(c.N), make_tuple(c.N, c.A, c.B), c.expected)) break;
        }
    }
}
