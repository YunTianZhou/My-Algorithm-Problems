#include <bits/stdc++.h>
using namespace std;
using reference::Clause;

// ---------- Random helpers ----------
static mt19937 rng;
static inline int randint(int L, int R) {
    if (L > R) return L;
    uniform_int_distribution<int> d(L, R);
    return d(rng);
}
static inline bool coin() { return randint(0, 1) == 1; }

// produce random clause A,B in [2..N-1], A!=B when possible
Clause random_clause(int N) {
    Clause c;
    c.A = randint(2, N - 1);
    c.B = randint(2, N - 1);
    if (N > 3) while (c.B == c.A) c.B = randint(2, N - 1);
    c.A_pro = coin(); c.B_pro = coin();
    return c;
}

// ---------- Generation honoring subtasks ----------

// Generate M clauses for subtask 2: exactly one clause per A=2..N-1 (so M = N-2)
vector<Clause> gen_subtask2_clauses(int N) {
    vector<Clause> clauses;
    clauses.reserve(max(0, N - 2));
    for (int A = 2; A <= N - 1; ++A) {
        // choose B != A
        int B = randint(2, N - 1);
        if (N > 3) while (B == A) B = randint(2, N - 1);
        clauses.push_back({ A, coin(), B, coin() });
    }
    return clauses;
}

// Generate M clauses for subtask 3: symmetric pairs (A pro->B noob and A noob->B pro OR pro->pro and noob->noob)
vector<Clause> gen_subtask3_clauses(int N, int M) {
    vector<Clause> clauses; clauses.reserve(M);
    if (M % 2 == 1) --M; // keep even
    while ((int)clauses.size() < M) {
        int A = randint(2, N - 1), B = randint(2, N - 1);
        while (B == A) B = randint(2, N - 1);
        if (coin()) {
            clauses.push_back({ A,true,B,false });
            clauses.push_back({ A,false,B,true });
        }
        else {
            clauses.push_back({ A,true,B,true });
            clauses.push_back({ A,false,B,false });
        }
    }
    if ((int)clauses.size() > M) clauses.resize(M);
    return clauses;
}

// General generation
vector<Clause> gen_general_clauses(int N, int M) {
    vector<Clause> clauses; clauses.reserve(M);
    for (int i = 0; i < M; ++i) clauses.push_back(random_clause(N));
    return clauses;
}

// Main per-subtask generator (always normal generation, ordered small->big)
struct CaseData {
    int N, M;
    vector<int> A, B;
    vector<string> X, Y;
    string expected;        // "" for impossible, else N chars 'P'/'N'
};

bool is_valid_answer(const CaseData& c, const string& ans) {
    if (ans.size() != c.expected.size()) return false;
    if (c.expected.empty()) return true;
    if (ans[0] != 'N' || ans.back() != 'P') return false;

    for (char c : ans)
        if (c != 'P' && c != 'N')
            return false;
    
    for (int i = 0; i < c.M; i++)
        if ((c.X[i] == "pro" && ans[c.A[i] - 1] == 'P') ||
            (c.X[i] == "noob" && ans[c.A[i] - 1] == 'N'))
            if ((c.Y[i] == "pro" && ans[c.B[i] - 1] == 'N') ||
                (c.Y[i] == "noob" && ans[c.B[i] - 1] == 'P'))
                return false;

    return true;
}

CaseData build_case_from_clauses(int N, const vector<Clause>& cls) {
    CaseData cd; cd.N = N; cd.M = (int)cls.size();
    cd.A.reserve(cd.M);
    cd.B.reserve(cd.M);
    cd.X.reserve(cd.M);
    cd.Y.reserve(cd.M);
    for (auto& c : cls) {
        cd.A.push_back(c.A);
        cd.B.push_back(c.B);
        cd.X.push_back(c.A_pro ? "pro" : "noob");
        cd.Y.push_back(c.B_pro ? "pro" : "noob");
    }
    cd.expected = reference::Oracle().solve(cd.N, cd.M, cd.A, cd.B, cd.X, cd.Y);
    return cd;
}

vector<CaseData> gen_cases_for_subtask_ordered(int subtask, int K, judge::Runner& runner) {
    vector<CaseData> tests; tests.reserve(K);
    const int MAX_N = 300763;
    const int MAX_M = 676767;

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
        if (subtask == 1) { N_min = 4; N_max = 20; }
        else if (subtask == 4) {
            if (band == 0) { N_min = 4; N_max = 50; }
            else if (band == 1) { N_min = 51; N_max = 500; }
            else if (band == 2) { N_min = 501; N_max = 1500; }
            else { N_min = 1501; N_max = 3000; }
        }
        else {
            if (band == 0) { N_min = 4; N_max = 60; }
            else if (band == 1) { N_min = 61; N_max = 800; }
            else if (band == 2) { N_min = 801; N_max = 8000; }
            else { N_min = 8001; N_max = MAX_N; }
        }

        int N = randint(N_min, min(N_max, MAX_N));
        if (N < 3) N = 3;

        int M;
        vector<Clause> clauses;
        if (subtask == 2) {
            // enforce exactly one A==j for each j in 2..N-1 => M = N-2
            M = max(0, N - 2);
            clauses = gen_subtask2_clauses(N);
        }
        else {
            if (subtask == 1) M = randint(1, min(50, max(1, N * 4)));
            else if (subtask == 4) M = randint(1, min(3000, max(1, N * 3)));
            else M = randint(1, min(MAX_M, max(1, N * 3)));

            if (subtask == 3) clauses = gen_subtask3_clauses(N, M);
            else clauses = gen_general_clauses(N, M);
        }

        tests.push_back(build_case_from_clauses(N, clauses));
        runner.preparing(idx + 1, K);
    }

    // ensure increasing order by size
    sort(tests.begin(), tests.end(), [](const CaseData& a, const CaseData& b) {
        if (a.N != b.N) return a.N < b.N;
        if (a.M != b.M) return a.M < b.M;
        return a.expected < b.expected;
        });

    return tests;
}

void run_tests(judge::Runner& runner, const judge::Options& options) {
    runner.set_time_limit(chrono::milliseconds(2000)); // See problem.md.
    rng.seed(options.seed);
    for (int sub = 1; sub <= 5; ++sub) {
        runner.section("Subtask " + to_string(sub), options.cases);
        for (const auto& c : gen_cases_for_subtask_ordered(sub, options.cases, runner)) {
            if (!runner.test("N=" + to_string(c.N) + " M=" + to_string(c.M),
                        make_tuple(c.N, c.M, c.A, c.B, c.X, c.Y), c.expected,
                        [&](string got, const string&) {
                            while (!got.empty() && isspace(static_cast<unsigned char>(got.back())))
                                got.pop_back();
                            return is_valid_answer(c, got);
                        })) break;
        }
    }
}
