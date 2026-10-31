#include <bits/stdc++.h>
using namespace std;

struct Case {
    vector<int> A, L, R, K;
    vector<char> T;
    void add(char t, int l, int r, int k = 0) {
        T.push_back(t); L.push_back(l); R.push_back(r); K.push_back(k);
    }
};

vector<int> brute(const Case& c) {
    auto a = c.A;
    vector<int> result;
    for (size_t i = 0; i < c.T.size(); ++i) {
        int l = c.L[i] - 1, r = c.R[i];
        if (c.T[i] == 'K') {
            vector<int> part(a.begin() + l, a.begin() + r);
            sort(part.begin(), part.end());
            result.push_back(part[c.K[i] - 1]);
        } else {
            int value = c.T[i] == 'L' ? a[l] : a[r - 1];
            fill(a.begin() + l, a.begin() + r, value);
        }
    }
    return result;
}

vector<int> reference_answer(const Case& c) {
    return reference::Oracle().solve(int(c.A.size()), int(c.T.size()), c.A, c.T, c.L, c.R, c.K);
}

Case generate(int n, int q, int pattern, int subtask, mt19937& rng) {
    Case c;
    c.A.resize(n);
    for (int i = 0; i < n; ++i) {
        int value = pattern == 1 ? i + 1 : pattern == 3 ? 17 :
                    pattern == 5 ? (i % 2 ? 1'000'000'000 : 1) : int(rng() % 1'000'000'000) + 1;
        c.A[i] = subtask == 3 ? (value - 1) % 10 + 1 : value;
    }
    for (int i = 0; i < q; ++i) {
        int l = int(rng() % n) + 1, r = int(rng() % n) + 1;
        if (l > r) swap(l, r);
        char t = 'K';
        if (pattern == 2) {
            if (i % 3) { t = i % 2 ? 'L' : 'R'; r = min(n, l + 1); }
        } else if (pattern == 3) {
            if (i % 2) t = i % 4 == 1 ? 'L' : 'R';
        } else if (pattern == 4) {
            if (i % 2 == 0) { t = i % 4 ? 'L' : 'R'; l = 1; r = n; }
        } else if (pattern == 5) {
            if (i % 2) t = i % 4 == 1 ? 'L' : 'R';
            l = 1 + i % max(1, n / 2); r = n - (l - 1);
        } else if (pattern == 6) {
            t = i % 2 ? 'L' : 'R'; // No outputs.
        } else if (pattern == 7) {
            int type = int(rng() % 3);
            t = type == 0 ? 'L' : type == 1 ? 'R' : 'K';
        }
        int k = i % 5 == 0 ? 1 : i % 5 == 1 ? r - l + 1 : int(rng() % (r - l + 1)) + 1;
        if (subtask == 2) k = min(k, 10);
        c.add(t, l, r, t == 'K' ? k : 0);
    }
    return c;
}

vector<Case> examples() {
    Case a{{4,1,5,2,3}, {}, {}, {}, {}};
    a.add('K',1,5,3); a.add('L',2,4); a.add('K',1,5,3);
    a.add('R',1,5); a.add('K',2,4,2); a.add('K',1,1,1);
    Case b{{2,8,2,5,8,1}, {}, {}, {}, {}};
    b.add('R',2,4); b.add('K',1,6,4); b.add('L',1,3);
    b.add('K',2,5,3); b.add('R',3,6); b.add('K',1,6,5);
    Case c{{3,1,3,2}, {}, {}, {}, {}};
    c.add('K',1,4,1); c.add('K',1,4,3); c.add('K',2,3,2); c.add('K',1,4,4);
    return {a,b,c};
}

bool judge_case(judge::Runner& runner, const Case& c) {
    auto expected = reference_answer(c);
    if (1LL * c.A.size() * c.T.size() <= 1'000'000 && expected != brute(c))
        throw logic_error("P6 oracle disagrees with brute force");
    if (all_of(c.T.begin(), c.T.end(), [](char t) { return t == 'K'; }) &&
        is_sorted(c.A.begin(), c.A.end())) {
        vector<int> known;
        for (size_t i = 0; i < c.T.size(); ++i) known.push_back(c.A[c.L[i] + c.K[i] - 2]);
        if (expected != known) throw logic_error("P6 oracle disagrees with sorted-array answers");
    }
    return runner.test("N=" + to_string(c.A.size()) + " Q=" + to_string(c.T.size()),
                       make_tuple(int(c.A.size()), int(c.T.size()), c.A, c.T, c.L, c.R, c.K), expected);
}

void run_tests(judge::Runner& runner, const judge::Options& options) {
    runner.set_time_limit(chrono::milliseconds(4000));
    mt19937 rng(options.seed);
    for (int sub = 1; sub <= 4; ++sub) {
        int large = sub == 1 ? 1 : 4;
        runner.section("Subtask " + to_string(sub), 3 + 8 + options.cases + large);
        bool ok = true;
        for (const auto& c : examples()) if (!(ok = judge_case(runner, c))) break;
        if (!ok) continue;
        for (int p = 0; p < 8; ++p)
            if (!(ok = judge_case(runner, generate(p == 3 ? 1 : 31, 80, p, sub, rng)))) break;
        if (!ok) continue;
        for (int i = 0; i < options.cases; ++i)
            if (!(ok = judge_case(runner, generate(1 + rng()%50, 1 + rng()%100, i%8, sub, rng)))) break;
        if (!ok) continue;
        // Stream full-size cases; do not retain several large inputs or show a preparation bar.
        for (int p : sub == 1 ? vector<int>{7} : vector<int>{0,1,2,5})
            if (!judge_case(runner, generate(sub == 1 ? 3000 : 200000,
                                            sub == 1 ? 3000 : 200000, p, sub, rng))) break;
    }
}
