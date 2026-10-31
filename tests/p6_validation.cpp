#include <bits/stdc++.h>
using namespace std;
#include P6_EDITORIAL
namespace reference {
#include "../P6 - Swipe/oracle.cpp"
}
using Solution = reference::Oracle;
#include "../judge/runner.hpp"
namespace problem {
#include "../P6 - Swipe/test.cpp"
}

void require(bool ok, const string& message) {
    if (!ok) throw runtime_error(message);
}
int checked = 0;
void check(const problem::Case& c) {
    int n = int(c.A.size()), q = int(c.T.size());
    auto expected = problem::brute(c);
    auto run = [&](auto solver) { return solver.solve(n,q,c.A,c.T,c.L,c.R,c.K); };
    require(run(reference::Oracle()) == expected, "oracle mismatch");
    require(run(editorial1::Solution()) == expected, "subtask 1 mismatch");
    if (*max_element(c.A.begin(), c.A.end()) <= 10)
        require(run(editorial3::Solution()) == expected, "subtask 3 mismatch");
    if (*max_element(c.K.begin(), c.K.end()) <= 10)
        require(run(editorial2::Solution()) == expected, "subtask 2 mismatch");
    require(run(editorial4::Solution()) == expected, "general editorial mismatch");
    ++checked;
}

void validate_constraints(const problem::Case& c, int sub) {
    int n = int(c.A.size()), q = int(c.T.size());
    require(n >= 1 && n <= 200000 && q >= 1 && q <= 200000, "N/Q bounds");
    for (int v : c.A) require(v >= 1 && v <= (sub == 3 ? 10 : 1000000000), "A bounds");
    require(sub != 1 || (n <= 3000 && q <= 3000), "subtask 1 bounds");
    for (int i = 0; i < q; ++i) {
        require(c.L[i] >= 1 && c.L[i] <= c.R[i] && c.R[i] <= n, "range bounds");
        if (c.T[i] == 'K') {
            require(c.K[i] >= 1 && c.K[i] <= c.R[i] - c.L[i] + 1, "rank bounds");
            require(sub != 2 || c.K[i] <= 10, "subtask 2 bounds");
        } else require((c.T[i] == 'L' || c.T[i] == 'R') && c.K[i] == 0, "swipe format");
    }
}

int main() {
    try {
        // Query boundaries cut weighted runs, then later swipes cross those cuts.
        problem::Case cuts{{8,1,9,2,7,3,6,4}, {}, {}, {}, {}};
        cuts.add('L',1,4,0);
        cuts.add('K',3,7,4);
        cuts.add('K',2,5,3);
        cuts.add('R',2,7,0);
        cuts.add('K',4,8,4);
        cuts.add('K',1,5,5);
        cuts.add('K',5,6,1);
        check(cuts);
        for (int n = 1; n <= 3; ++n) {
            vector<tuple<char,int,int,int>> ops;
            for (int l = 1; l <= n; ++l) for (int r = l; r <= n; ++r) {
                ops.push_back({'L',l,r,0}); ops.push_back({'R',l,r,0});
                for (int k = 1; k <= r-l+1; ++k) ops.push_back({'K',l,r,k});
            }
            int arrays = 1;
            for (int i = 0; i < n; ++i) arrays *= 3;
            for (int mask = 0; mask < arrays; ++mask) {
                vector<int> a(n);
                int code = mask;
                for (int& v : a) { v = 1 + code % 3; code /= 3; }
                for (auto first : ops) for (auto second : ops) {
                    problem::Case c{a, {}, {}, {}, {}};
                    for (auto op : {first, second}) {
                        auto [t,l,r,k] = op; c.add(t,l,r,k);
                    }
                    check(c);
                }
            }
        }
        mt19937 rng(2468);
        for (int i = 0; i < 2000; ++i) {
            int sub = 1 + i % 4;
            auto c = problem::generate(1 + rng()%60, 1 + rng()%120, i%8, sub, rng);
            validate_constraints(c, sub); check(c);
        }
        for (int sub = 1; sub <= 4; ++sub)
            for (int pattern = 0; pattern < 8; ++pattern) {
                auto c = problem::generate(100, 200, pattern, sub, rng);
                validate_constraints(c, sub); check(c);
            }
        vector<vector<int>> sample_answers{{3,1,3,3}, {5,5,2}, {1,3,3,3}};
        auto samples = problem::examples();
        for (size_t i = 0; i < samples.size(); ++i) {
            require(problem::brute(samples[i]) == sample_answers[i], "statement example");
            check(samples[i]);
        }
        // Reuse one oracle instance to catch state leaking between calls.
        reference::Oracle reused;
        for (const auto& c : samples)
            require(reused.solve(c.A.size(),c.T.size(),c.A,c.T,c.L,c.R,c.K) == problem::brute(c), "state leak");
        cout << "Validated " << checked << " exhaustive/random cases and all four editorial implementations.\n";
        return 0;
    } catch (const exception& e) { cerr << e.what() << '\n'; return 1; }
}
