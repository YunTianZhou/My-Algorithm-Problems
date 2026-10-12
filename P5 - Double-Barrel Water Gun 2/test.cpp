#include <bits/stdc++.h>
using namespace std;

// P4 solves Bob's best response to any submitted shooting plan.
namespace dodger {
#include "../P4 - Double-Barrel Water Gun/oracle.cpp"
}

constexpr int MAX_N = 1'000'000;
constexpr int TIME_LIMIT_MS = 1000;

int optimum(int n) {
    int answer = 0;
    for (int size = n + 1; size > 1; size /= 2) ++answer;
    return answer;
}

// Independent state DP for small plans; duplicate shots count only once.
int quadratic_minimum(const vector<int>& A, const vector<int>& B) {
    int n = int(A.size());
    vector<int> dp(n + 1, n + 1), next(n + 1, n + 1);
    dp[0] = 0;
    for (int t = 1; t <= n; ++t) {
        for (int k = 0; k <= t; ++k) {
            int best = min(k < t ? dp[k] : n + 1, k ? dp[k - 1] : n + 1);
            int position = 2 * k - t;
            next[k] = best + (position == A[t - 1] || position == B[t - 1]);
        }
        dp.swap(next);
    }
    return *min_element(dp.begin(), dp.end());
}

bool valid_answer(int n, const tuple<int, vector<int>, vector<int>>& answer, int expected) {
    const auto& [claimed, A, B] = answer;
    if (claimed != expected || A.size() != size_t(n) || B.size() != size_t(n)) return false;
    // There is no coordinate bound in the statement. Out-of-reach shots are
    // legal; evaluating Bob's response naturally ignores them, even INT_MIN.
    int actual = n <= 128 ? quadratic_minimum(A, B) : dodger::Oracle().solve(n, A, B).first;
    return actual == expected;
}

vector<int> case_sizes(int random_cases, mt19937& rng) {
    vector<int> sizes;
    for (int n = 1; n <= 128; ++n) sizes.push_back(n);
    // A full phase ends at 2^k-1; starting the next phase earns no extra hit.
    for (int power = 2; power <= MAX_N + 2; power *= 2)
        for (int delta : {-2, -1, 0, 1, 2}) {
            int n = power + delta;
            if (n >= 1 && n <= MAX_N) sizes.push_back(n);
        }
    for (int n : {333333, 500000, 777777, MAX_N - 1, MAX_N}) sizes.push_back(n);
    for (int i = 0; i < random_cases; ++i)
        sizes.push_back(uniform_int_distribution<int>(129, 10000)(rng));
    sort(sizes.begin(), sizes.end());
    sizes.erase(unique(sizes.begin(), sizes.end()), sizes.end());
    return sizes;
}

void run_tests(judge::Runner& runner, const judge::Options& options) {
    runner.set_time_limit(chrono::milliseconds(TIME_LIMIT_MS));
    mt19937 rng(options.seed);
    auto sizes = case_sizes(options.cases, rng);
    runner.section("All cases", int(sizes.size()));
    // Only N is stored. The submission constructs one plan at a time, with
    // no bulk case generation or preparation progress bar.
    for (int n : sizes) {
        if (!runner.test("N=" + to_string(n), make_tuple(n), optimum(n),
                         [n](const auto& got, int expected) {
                             return valid_answer(n, got, expected);
                         })) break;
    }
}
