#include <bits/stdc++.h>
using namespace std;

struct Case {
    int n;
    int k;
    vector<string> S;
    string ans;
};

Case genCaseWithN(int n, mt19937& rng) {
    vector<string> S(n);
    uniform_int_distribution<int> coin(0, 1);
    for (int i = 0; i < n; ++i)
        S[i] = coin(rng) ? "Scifish" : "Tim";

    uniform_int_distribution<int> runChance(0, 4);
    if (runChance(rng) == 0 && n >= 3) {
        int runs = max(1, n / 6);
        uniform_int_distribution<int> start(0, n - 2);
        for (int r = 0; r < runs; ++r) {
            int s = start(rng);
            S[s] = S[s + 1];
            if (s + 2 < n && coin(rng))
                S[s + 2] = S[s];
        }
    }

    if (find(S.begin(), S.end(), "Scifish") == S.end()) {
        uniform_int_distribution<int> pos(0, n - 1);
        S[pos(rng)] = "Scifish";
    }
    if (find(S.begin(), S.end(), "Tim") == S.end()) {
        uniform_int_distribution<int> pos(0, n - 1);
        S[pos(rng)] = "Tim";
    }

    int mx = reference::getMaxScore(n, S);

    uniform_int_distribution<int> delta(-2, 2);
    int k = mx + delta(rng);
    k = max(0, min(k, n));

    return Case{ n, k, S, reference::Oracle().solve(n, k, S) };
}

void run_tests(judge::Runner& runner, const judge::Options& options) {
    runner.set_time_limit(chrono::milliseconds(2000)); // See problem.md.
    runner.section("All cases", options.cases);
    mt19937 rng(options.seed);
    for (int t = 0; t < options.cases; ++t) {
        double fraction = double(t) / options.cases;
        int lo = 2, hi = 20;
        if (fraction >= .95) { lo = 2001; hi = 67 * 67 * 67 * 67; }
        else if (fraction >= .80) { lo = 201; hi = 2000; }
        else if (fraction >= .40) { lo = 21; hi = 200; }
        runner.preparing(t, options.cases);
        auto c = genCaseWithN(uniform_int_distribution<int>(lo, hi)(rng), rng);
        runner.preparing(t + 1, options.cases);
        if (!runner.test("n=" + to_string(c.n) + " k=" + to_string(c.k),
                    make_tuple(c.n, c.k, move(c.S)), c.ans)) break;
    }
}
