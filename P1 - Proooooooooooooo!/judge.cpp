#include <bits/stdc++.h>
using namespace std;

struct Case {
    int n;
    int k;
    vector<string> S;
    string ans;
};

int getMaxScore(int n, const vector<string>& S) {
    int i = n - 1;
    int score = 0;

    if (S[i] == "Tim") {
        score--;
        while (i > 0 && S[i] == "Tim") i--;
        i--;
        while (i > 0 && S[i] == "Tim") i--;
    }

    for (; i > 0; --i)
        if (S[i] == "Scifish" && S[i - 1] == "Tim")
            score++;

    return score;
}

Case genCaseWithN(int n, mt19937& rng) {
    vector<string> S(n);
    uniform_int_distribution<int> coin(0, 1);
    for (int i = 0; i < n; ++i)
        S[i] = coin(rng) ? "Scifish" : "Tim";

    if (find(S.begin(), S.end(), "Scifish") == S.end()) {
        uniform_int_distribution<int> pos(0, n - 1);
        S[pos(rng)] = "Scifish";
    }
    if (find(S.begin(), S.end(), "Tim") == S.end()) {
        uniform_int_distribution<int> pos(0, n - 1);
        S[pos(rng)] = "Tim";
    }

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

    int mx = getMaxScore(n, S);

    uniform_int_distribution<int> delta(-2, 2);
    int k = mx + delta(rng);
    k = max(0, min(k, n));

    return Case{ n, k, S, (mx >= k ? "Yes" : "No") };
}

class Solution;  // Replace with the actual solution class

int main() {
    mt19937 rng((unsigned)chrono::steady_clock::now().time_since_epoch().count());

    const int NUM_CASES = 100;
    const int MAX_N = pow(67, 4);
    vector<Case> tests;
    tests.reserve(NUM_CASES);

    for (int t = 0; t < NUM_CASES; ++t) {
        int n;
        if (t < 40) {
            uniform_int_distribution<int> d(2, 20);
            n = d(rng);
        }
        else if (t < 80) {
            uniform_int_distribution<int> d(21, 200);
            n = d(rng);
        }
        else if (t < 95) {
            uniform_int_distribution<int> d(201, 2000);
            n = d(rng);
        }
        else {
            uniform_int_distribution<int> d(2001, MAX_N);
            n = d(rng);
        }
        tests.push_back(genCaseWithN(n, rng));
    }

    sort(tests.begin(), tests.end(), [](const Case& a, const Case& b) {
        if (a.n != b.n) return a.n < b.n;
        return a.k < b.k;
    });

    Solution solver;

    const string RED = "\033[1;31m";
    const string GREEN = "\033[1;32m";
    const string RESET = "\033[0m";

    int pass = 0, fail = 0;

    using clock = std::chrono::high_resolution_clock;
    auto total_start = clock::now();

    for (size_t idx = 0; idx < tests.size(); ++idx) {
        const Case& c = tests[idx];

        auto case_start = clock::now();
        string got = solver.solve(c.n, c.k, c.S);
        auto case_end = clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::microseconds>(case_end - case_start).count();

        bool ok = (got == c.ans);
        if (ok) {
            cout << GREEN << "AC" << RESET;
            ++pass;
        }
        else {
            cout << RED << "WA" << RESET;
            ++fail;
        }

        double ms = dur / 1000.0;
        cout << "  Case " << (idx + 1) << ": n=" << c.n << " k=" << c.k;
        cout << " expected=" << c.ans << " got=" << got;
        cout << "  time=" << fixed << setprecision(3) << ms << " ms\n";

        if (!ok) {
            int preview = min(c.n, 80);
            cout << "  S[0.." << preview - 1 << "]: ";
            for (int i = 0; i < preview; ++i) {
                if (i) cout << ",";
                cout << (c.S[i] == "Scifish" ? 'S' : 'T');
            }
            if (preview < c.n) cout << ",...";
            cout << "\n";
        }
    }

    auto total_end = clock::now();
    auto total_us = std::chrono::duration_cast<std::chrono::microseconds>(total_end - total_start).count();

    cout << "\nSummary: " << pass << " passed, " << fail << " failed out of " << tests.size() << " tests.\n";
    cout << "Total duration: " << fixed << setprecision(3) << (total_us / 1000.0) << " ms\n";

    if (fail > 0) {
        cout << RED << "At least one WA -> your Solution is incorrect on these tests." << RESET << "\n";
    }
    else {
        cout << GREEN << "All tests passed -> Solution looks correct on generated tests." << RESET << "\n";
    }
}
