#include <bits/stdc++.h>
using namespace std;
namespace reference {
#include "../P5 - Double-Barrel Water Gun 2/oracle.cpp"
}
using Solution = reference::Oracle;
#include "../judge/runner.hpp"
namespace problem {
#include "../P5 - Double-Barrel Water Gun 2/test.cpp"
}

void require(bool ok, const string& message) {
    if (!ok) throw runtime_error(message);
}

tuple<int, vector<int>, vector<int>> squeeze(int n) {
    vector<int> A(n), B(n);
    int answer = 0, L = 1;
    for (int t = 1; t <= n; ++t) {
        if (t == 2 * L) L *= 2;
        A[t - 1] = t - 2 * L;
        B[t - 1] = -A[t - 1];
        answer += t == 2 * L - 1;
    }
    return {answer, move(A), move(B)};
}

int enumerated = 0;
void exhaustive(vector<int>& A, vector<int>& B, int index, int& maximum) {
    int n = int(A.size());
    if (index == n) {
        int hits = problem::quadratic_minimum(A, B);
        require(hits <= problem::optimum(n), "upper bound violated");
        if (n <= 4)
            require(problem::dodger::Oracle().solve(n, A, B).first == hits, "P4 checker mismatch");
        maximum = max(maximum, hits);
        ++enumerated;
        return;
    }
    int t = index + 1;
    vector<int> positions;
    for (int k = 0; k <= t; ++k) positions.push_back(2 * k - t);
    positions.push_back(INT_MAX); // Represents every irrelevant shot.
    for (size_t a = 0; a < positions.size(); ++a)
        for (size_t b = a; b < positions.size(); ++b) {
            A[index] = positions[a]; B[index] = positions[b];
            exhaustive(A, B, index + 1, maximum);
        }
}

int main() {
    try {
        for (int n = 1; n <= 5; ++n) {
            vector<int> A(n), B(n);
            int maximum = -1;
            exhaustive(A, B, 0, maximum);
            require(maximum == problem::optimum(n), "maximin optimum mismatch");
        }
        mt19937 rng(2026);
        for (int trial = 0; trial < 1000; ++trial) {
            int n = 1 + rng() % 200;
            vector<int> A(n), B(n);
            for (int t = 1; t <= n; ++t) {
                A[t - 1] = 2 * int(rng() % (t + 1)) - t;
                B[t - 1] = trial % 3 ? 2 * int(rng() % (t + 1)) - t : A[t - 1];
            }
            int best = problem::quadratic_minimum(A, B);
            require(best <= problem::optimum(n), "random upper bound mismatch");
            require(problem::dodger::Oracle().solve(n, A, B).first == best, "random fast checker mismatch");
        }
        for (int n = 1; n <= 256; ++n) {
            for (auto plan : {squeeze(n), reference::Oracle().solve(n)}) {
                auto& [count, A, B] = plan;
                require(count == problem::optimum(n), "construction count mismatch");
                require(problem::quadratic_minimum(A, B) == count, "construction DP mismatch");
                require(problem::valid_answer(n, plan, count), "rejected construction");
                for (int i = 0; i < n; ++i) { A[i] = -A[i]; B[i] = -B[i]; }
                swap(A, B);
                require(problem::valid_answer(n, plan, count), "rejected reflected/swapped construction");
            }
        }
        auto sizes = problem::case_sizes(3, rng);
        for (int n : sizes) require(n >= 1 && n <= problem::MAX_N, "invalid generated N");
        require(binary_search(sizes.begin(), sizes.end(), problem::MAX_N), "missing max N");
        for (int p = 2; p <= problem::MAX_N; p *= 2)
            for (int n : {p - 1, p, p + 1})
                require(binary_search(sizes.begin(), sizes.end(), n), "missing phase boundary");
        require(!problem::valid_answer(1, {1, {}, {1}}, 1), "accepted short A");
        require(!problem::valid_answer(1, {1, {-1}, {}}, 1), "accepted short B");
        require(!problem::valid_answer(1, {1, {-1, -1}, {1}}, 1), "accepted extra entries");
        require(!problem::valid_answer(1, {0, {-1}, {1}}, 1), "accepted wrong claimed optimum");
        require(!problem::valid_answer(1, {1, {1}, {1}}, 1), "accepted duplicate-only plan");
        require(!problem::valid_answer(1, {1, {0}, {2}}, 1), "accepted wrong-parity plan");
        require(!problem::valid_answer(1, {1, {INT_MIN}, {INT_MAX}}, 1), "accepted unreachable-only plan");
        require(problem::valid_answer(2, {1, {-1, INT_MIN}, {1, INT_MAX}}, 1), "rejected legal irrelevant shots");
        for (int n : {129, 1023, 524287, 524288, problem::MAX_N}) {
            auto plan = squeeze(n);
            require(problem::valid_answer(n, plan, problem::optimum(n)), "large squeeze rejected");
            plan = reference::Oracle().solve(n);
            require(problem::valid_answer(n, plan, problem::optimum(n)), "large oracle rejected");
        }
        auto preview = judge::preview(make_tuple(7, vector<int>(10000, 1), vector<int>(10000, -1)));
        require(preview.size() < 250 && preview.find("10000") != string::npos, "unbounded tuple diagnostics");
        cout << "Validated " << enumerated << " exhaustive shooting plans, both constructions, checker, and phase boundaries.\n";
        return 0;
    } catch (const exception& e) {
        cerr << e.what() << '\n';
        return 1;
    }
}
