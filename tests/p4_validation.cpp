// g++ -std=c++17 -O2 tests/p4_validation.cpp -o /tmp/p4-validation
// /tmp/p4-validation [--benchmark]
#include <bits/stdc++.h>
using namespace std;
namespace reference {
#include "../P4 - Double-Barrel Water Gun/oracle.cpp"
}
using Solution = reference::Oracle;
#include "../judge/runner.hpp"
namespace problem {
#include "../P4 - Double-Barrel Water Gun/test.cpp"
}

void require(bool ok, const string& message) {
    if (!ok) throw runtime_error(message);
}

int checked = 0;
void check(const problem::Case& c) {
    auto answer = reference::Oracle().solve(int(c.A.size()), c.A, c.B);
    int best = problem::quadratic_minimum(c);
    require(problem::valid_answer(c.A, c.B, answer, best), "oracle mismatch: " + c.label);
    ++checked;
}

void exhaustive(problem::Case& c, int i) {
    if (i == int(c.A.size())) { check(c); return; }
    int t = i + 1;
    vector<int> positions;
    for (int k = 0; k <= t; ++k) positions.push_back(2 * k - t);
    positions.push_back(problem::LIMIT); // All unreachable/parity-mismatched shots are equivalent.
    for (size_t a = 0; a < positions.size(); ++a)
        for (size_t b = a; b < positions.size(); ++b) {
            c.A[i] = positions[a]; c.B[i] = positions[b];
            exhaustive(c, i + 1);
        }
}

// Correct O(N log N) comparison: use a Fenwick tree for point differences.
// This is a calibration baseline, not a claim about every possible log-N solver.
class Fenwick {
    vector<int> bit;
public:
    explicit Fenwick(int n) : bit(n + 1) {}
    void add(int p, int x) { for (++p; p < int(bit.size()); p += p & -p) bit[p] += x; }
    int prefix(int p) const { int s = 0; for (++p; p > 0; p -= p & -p) s += bit[p]; return s; }
    int get(int p) const { return prefix(p) - prefix(p - 1); }
};
pair<int, string> fenwick_solve(const problem::Case& c) {
    int n = int(c.A.size());
    Fenwick diff(n + 2);
    vector<int> offset(n + 1), negative, next, right;
    right.reserve(2 * n);
    auto shoot = [&](int x, int t) {
        if (x < -t || x > t || (x + t) % 2) return;
        int k = (x + t) / 2;
        diff.add(k, 1); diff.add(k + 1, -1);
        if (diff.get(k + 1) < 0) {
            auto it = lower_bound(negative.begin(), negative.end(), k + 1);
            if (it == negative.end() || *it != k + 1) negative.insert(it, k + 1);
        }
    };
    for (int t = n; t >= 1; --t) {
        shoot(c.A[t - 1], t);
        if (c.B[t - 1] != c.A[t - 1]) shoot(c.B[t - 1], t);
        offset[t] = int(right.size()); next.clear();
        for (int p : negative) {
            int value = diff.get(p);
            if (value >= 0) continue;
            right.push_back(p);
            diff.add(p - 1, value); diff.add(p, -value);
            if (diff.get(p - 1) < 0) next.push_back(p - 1);
        }
        negative.swap(next);
    }
    offset[0] = int(right.size());
    int k = 0;
    string path(n, 'L');
    for (int t = 1; t <= n; ++t)
        for (int i = offset[t]; i < offset[t - 1]; ++i)
            if (right[i] == k + 1) { ++k; path[t - 1] = 'R'; break; }
    return {diff.get(0), move(path)};
}

int main(int argc, char** argv) {
    try {
        mt19937 rng(42);
        if (argc > 1 && string(argv[1]) == "--benchmark") {
            for (int pattern : {4, 5, 9, 10, 11, 13}) {
                auto c = problem::make_case(problem::MAX_N, pattern, 5, rng);
                auto start = chrono::steady_clock::now();
                auto linear = reference::Oracle().solve(int(c.A.size()), c.A, c.B);
                auto middle = chrono::steady_clock::now();
                auto logarithmic = fenwick_solve(c);
                auto end = chrono::steady_clock::now();
                require(problem::valid_answer(c.A, c.B, logarithmic, linear.first), "benchmark mismatch");
                cout << c.label << ": linear="
                     << chrono::duration<double, milli>(middle - start).count()
                     << " ms, Fenwick=" << chrono::duration<double, milli>(end - middle).count()
                     << " ms, hits=" << linear.first << '\n';
            }
            return 0;
        }
        for (int n = 1; n <= 5; ++n) {
            problem::Case c{vector<int>(n), vector<int>(n), "exhaustive n=" + to_string(n)};
            exhaustive(c, 0);
        }
        for (int sub = 1; sub <= 5; ++sub) {
            for (int pattern = 0; pattern < int(problem::PATTERNS.size()); ++pattern) {
                for (int n : {1, 2, 3, 10, 31, 32, 37, 38, 101}) {
                    auto c = problem::make_case(n, pattern, sub, rng);
                    check(c);
                    int best = problem::quadratic_minimum(c);
                    require(sub > 2 || best == 0, "zero-hit subtask failed");
                    require(problem::valid_answer(c.A, c.B, fenwick_solve(c), best), "Fenwick mismatch");
                    for (int i = 0; i < n; ++i) {
                        require(abs(c.A[i]) < 1'000'000'000 && abs(c.B[i]) < 1'000'000'000, "coordinate bounds");
                        require(sub != 1 || c.A[i] == c.B[i], "subtask 1 constraint");
                        require(sub != 2 || c.A[i] + 1 == c.B[i], "subtask 2 constraint");
                        c.A[i] = -c.A[i]; c.B[i] = -c.B[i];
                    }
                    auto reflected = reference::Oracle().solve(n, c.A, c.B);
                    require(problem::valid_answer(c.A, c.B, reflected, best), "reflection mismatch");
                    swap(c.A, c.B);
                    check(c);
                }
            }
            for (auto plan : problem::plans(sub, 2, rng)) {
                require(plan.n >= 1 && plan.n <= problem::MAX_N, "N out of range");
                require(sub != 3 || plan.n <= 2000, "subtask 3 constraint");
                require(sub != 4 || plan.n <= 100000, "subtask 4 constraint");
            }
        }
        for (int i = 0; i < 3000; ++i)
            check(problem::make_case(1 + rng() % 100, rng() % problem::PATTERNS.size(), 5, rng));
        int greedy_failures = 0;
        for (int pattern = 0; pattern < int(problem::PATTERNS.size()); ++pattern) {
            auto c = problem::make_case(100, pattern, 5, rng);
            int x = 0, hits = 0;
            for (int t = 0; t < 100; ++t) {
                bool left_hit = x - 1 == c.A[t] || x - 1 == c.B[t];
                bool right_hit = x + 1 == c.A[t] || x + 1 == c.B[t];
                x += left_hit && !right_hit ? 1 : -1;
                hits += x == c.A[t] || x == c.B[t];
            }
            greedy_failures += hits != problem::quadratic_minimum(c);
        }
        require(greedy_failures > 0, "patterns failed to reject local greedy");
        for (int n : {1, 2, 3, 4, 7, 8, 31, 32, 1023, 1024, 2000}) {
            auto c = problem::make_case(n, 11, 5, rng);
            int expected = 0;
            for (int size = n + 1; size > 1; size /= 2) ++expected;
            require(problem::quadratic_minimum(c) == expected, "nested sweep boundary");
        }
        require(!problem::valid_answer({-1}, {1}, {1, ""}, 1), "accepted short path");
        require(!problem::valid_answer({-1}, {1}, {1, "X"}, 1), "accepted invalid move");
        require(!problem::valid_answer({1}, {1}, {0, "R"}, 0), "accepted lying hit count");
        require(problem::valid_answer({1}, {1}, {1, "R"}, 1), "double-counted barrels");
        require(!problem::valid_answer({1}, {1}, {1, "R"}, 0), "accepted suboptimal path");
        cout << "Validated " << checked << " cases against independent quadratic DP; constraints, reflection, checker and Fenwick checks passed.\n";
        return 0;
    } catch (const exception& e) {
        cerr << e.what() << '\n'; return 1;
    }
}
