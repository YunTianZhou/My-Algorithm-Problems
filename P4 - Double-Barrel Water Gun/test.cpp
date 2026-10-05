#include <bits/stdc++.h>
using namespace std;

constexpr int TIME_LIMIT_MS = 1000;
constexpr int MAX_N = 2'000'000;
constexpr int LIMIT = 999'999'999;

struct Case {
    vector<int> A, B;
    string label;
};

// Independent O(N^2) forward DP, used for all small expected answers.
int quadratic_minimum(const Case& c) {
    int n = int(c.A.size());
    vector<int> dp(n + 1, n + 1), next(n + 1);
    dp[0] = 0;
    for (int t = 1; t <= n; ++t) {
        for (int k = 0; k <= t; ++k) {
            int best = min(k < t ? dp[k] : n + 1, k ? dp[k - 1] : n + 1);
            int x = 2 * k - t;
            next[k] = best + (x == c.A[t - 1] || x == c.B[t - 1]);
        }
        dp.swap(next);
    }
    return *min_element(dp.begin(), dp.end());
}

bool valid_answer(const vector<int>& A, const vector<int>& B,
                  const pair<int, string>& answer, int optimum) {
    if (answer.first != optimum || answer.second.size() != A.size()) return false;
    int x = 0, hits = 0;
    for (size_t i = 0; i < A.size(); ++i) {
        if (answer.second[i] == 'L') --x;
        else if (answer.second[i] == 'R') ++x;
        else return false;
        hits += x == A[i] || x == B[i]; // Coincident barrels count once.
    }
    return hits == optimum;
}

const vector<string> PATTERNS = {
    "unreachable", "wrong-parity", "duplicates", "adjacent",
    "reachable-random", "central-random", "extreme-paths", "stationary-cage",
    "guided-trap", "backward-plateau", "backward-pairs", "nested-sweeps",
    "late-trap", "alternating-blocks"
};

Case make_case(int n, int pattern, int subtask, mt19937& rng, bool mirror = false) {
    Case c{vector<int>(n), vector<int>(n), PATTERNS.at(pattern)};
    auto random = [&](int lo, int hi) { return uniform_int_distribution<int>(lo, hi)(rng); };
    int guide = 0;
    for (int t = 1; t <= n; ++t) {
        int a = 0, b = 0;
        switch (pattern) {
        case 0: a = -LIMIT; b = LIMIT; break;
        case 1: a = -t + 1; b = t - 1; break;
        case 2: a = 2 * random(0, t) - t; b = a; break;
        case 3: a = 2 * random(0, t) - t; b = a + 1; break;
        case 4: a = 2 * random(0, t) - t; b = 2 * random(0, t) - t; break;
        case 5: a = 2 * random(-3, 3) + t % 2; b = 2 * random(-3, 3) + t % 2; break;
        case 6: a = -t; b = t; break;
        case 7: a = -(t % 2 ? 1 : 2); b = -a; break;
        case 8:
            a = guide - 1; b = guide + 1;
            guide += random(0, 1) ? 1 : -1;
            break;
        case 9: {
            // In reverse time, extend a cost plateau one cell per turn.
            // Its negative boundary survives many simultaneous shifts.
            int right = n / 2 + 1, left = n / 2 - (n - t);
            a = left >= 0 && left <= t ? 2 * left - t : LIMIT;
            b = right <= t ? 2 * right - t : -LIMIT;
            break;
        }
        case 10: {
            int k = random(0, t - 1);
            a = 2 * k - t; b = a + 2;
            break;
        }
        case 11: {
            int end = 2;
            while (end <= t) end *= 2;
            a = -(end - t); b = -a;
            break;
        }
        case 12:
            a = t <= n / 2 ? LIMIT : -(n - t + (n % 2));
            b = -a;
            break;
        case 13: {
            // Alternate a moving wall, duplicate bullets, and irrelevant shots.
            int phase = (t / 31) % 4;
            a = phase == 0 ? -t : phase == 1 ? t % 2 : phase == 2 ? LIMIT : guide - 1;
            b = phase == 0 ? t : phase == 1 ? a : phase == 2 ? -LIMIT : guide + 1;
            guide += t % 3 ? 1 : -1;
            break;
        }
        }
        if (mirror) { a = -a; b = -b; }
        if (subtask == 1) b = a;
        if (subtask == 2) { a = min(a, LIMIT - 1); b = a + 1; }
        // Barrel order must have no effect (except ordered subtask 2).
        if (subtask != 2 && t % 3 == 0) swap(a, b);
        c.A[t - 1] = a; c.B[t - 1] = b;
    }
    if (mirror) c.label += " reflected";
    return c;
}

struct Plan { int n, pattern; bool mirror = false; };

vector<Plan> plans(int subtask, int random_cases, mt19937& rng) {
    vector<Plan> result;
    for (int pattern = 0; pattern < int(PATTERNS.size()); ++pattern) {
        result.push_back({1, pattern});
        result.push_back({37, pattern});
        result.push_back({38, pattern, true});
    }
    for (int i = 0; i < random_cases; ++i)
        result.push_back({uniform_int_distribution<int>(2, 100)(rng),
                          4 + i % 10, bool(i % 2)});
    // Immediately before/after each completed inward sweep: the optimum
    // increases at N = 1, 3, 7, 15, ... rather than staying near zero.
    for (int n : {2, 3, 4, 7, 8, 15, 16, 31, 32, 63, 64, 127, 128, 1023, 1024})
        result.push_back({n, 11});
    if (subtask == 3) {
        for (int pattern : {4, 5, 7, 9, 10, 11, 13}) result.push_back({2000, pattern});
    } else if (subtask == 4) {
        for (int pattern : {5, 9, 10, 11}) result.push_back({100'000, pattern});
    } else if (subtask == 5) {
        for (int pattern : {4, 5, 9, 10, 11, 13}) result.push_back({MAX_N, pattern});
    } else {
        result.push_back({MAX_N, 8});
    }
    return result;
}

bool judge_case(judge::Runner& runner, Case c, int subtask) {
    int n = int(c.A.size());
    auto expected = reference::Oracle().solve(n, c.A, c.B);
    int optimum = subtask <= 2 ? 0 : n <= 2000 ? quadratic_minimum(c) : expected.first;
    if (!valid_answer(c.A, c.B, expected, optimum))
        throw logic_error("P4 oracle disagrees with independent DP / produces an invalid path: " + c.label);
    // Keep the checker inputs independent of any submission mutations.
    return runner.test(c.label + " N=" + to_string(n), make_tuple(n, c.A, c.B), optimum,
        [&](const pair<int, string>& got, int best) {
            return valid_answer(c.A, c.B, got, best);
        });
}

void run_tests(judge::Runner& runner, const judge::Options& options) {
    runner.set_time_limit(chrono::milliseconds(TIME_LIMIT_MS));
    mt19937 rng(options.seed);
    for (int sub = 1; sub <= 5; ++sub) {
        auto cases = plans(sub, options.cases, rng);
        vector<Case> examples;
        if (sub >= 3) examples = {
            {{-1, -2, -1}, {2, 2, 1}, "sample 1 / future trap"},
            {{1, -2, -1, 0, 3}, {1, -2, -1, 0, 3}, "sample 2 / unique zero-hit path"},
            {{-1, -2, -1}, {1, 2, 1}, "sample 3 / forced repeated hit"},
            {{-1}, {1}, "both first moves hit"},
            {{1}, {1}, "duplicate hit counts once"}
        };
        runner.section("Subtask " + to_string(sub), int(cases.size() + examples.size()));
        bool ok = true;
        for (auto& c : examples) if (!(ok = judge_case(runner, move(c), sub))) break;
        if (!ok) continue;
        // Stream every case; in particular never retain several 2M-element
        // cases. No preparation progress bar for this streaming generator.
        for (auto plan : cases)
            if (!judge_case(runner, make_case(plan.n, plan.pattern, sub, rng, plan.mirror), sub)) break;
    }
}
