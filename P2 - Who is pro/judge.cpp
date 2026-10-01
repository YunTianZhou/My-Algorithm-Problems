// judge_who_is_pro.cpp
// Simplified judge + random test generator for P0002 "Who is pro"
// Always uses the normal random generator (no acceptance loops).
// Enforces Subtask 2: every j in [2..N-1] appears exactly once as A (so M = N-2).
// Interface: expects Solution::solve(int N,int M,const vector<string>& clauses)
// Return: "" (empty) if impossible; otherwise N chars 'P'/'N' for persons 1..N.

#include <bits/stdc++.h>
using namespace std;

// ---------- Clause / helpers ----------
struct Clause {
    int A; bool A_pro;
    int B; bool B_pro;
};

static inline int lit_id(int person, bool is_pro) {
    return 2 * (person - 1) + (is_pro ? 0 : 1);
}

// ---------- Iterative Kosaraju 2-SAT (used by generator & sample solver) ----------
bool check_2sat_with_assign(int N, const vector<Clause>& clauses, vector<int>* out_assign = nullptr) {
    int nodes = 2 * N;
    vector<vector<int>> g(nodes), gr(nodes);
    auto add_edge = [&](int u, int v) {
        g[u].push_back(v);
        gr[v].push_back(u);
        };
    auto add_imp = [&](int L1, int L2) {
        add_edge(L1, L2);
        add_edge(L2 ^ 1, L1 ^ 1);
        };
    for (auto& c : clauses) {
        int L1 = lit_id(c.A, c.A_pro), L2 = lit_id(c.B, c.B_pro);
        add_imp(L1, L2);
    }
    // force Scifish (1) = noob, Tim (N) = pro
    add_edge(lit_id(1, true), lit_id(1, false));
    add_edge(lit_id(N, false), lit_id(N, true));

    // pass1 iterative
    vector<char> vis(nodes, 0);
    vector<int> order; order.reserve(nodes);
    for (int s = 0; s < nodes; ++s) {
        if (vis[s]) continue;
        vector<pair<int, int>> st; st.emplace_back(s, 0);
        vis[s] = 1;
        while (!st.empty()) {
            int u = st.back().first;
            int& it = st.back().second;
            if (it < (int)g[u].size()) {
                int v = g[u][it++];
                if (!vis[v]) { vis[v] = 1; st.emplace_back(v, 0); }
            }
            else {
                order.push_back(u);
                st.pop_back();
            }
        }
    }

    // pass2 iterative
    vector<int> comp(nodes, -1);
    int cid = 0;
    for (int i = (int)order.size() - 1; i >= 0; --i) {
        int v = order[i];
        if (comp[v] != -1) continue;
        vector<int> st; st.push_back(v); comp[v] = cid;
        while (!st.empty()) {
            int u = st.back(); st.pop_back();
            for (int w : gr[u]) if (comp[w] == -1) { comp[w] = cid; st.push_back(w); }
        }
        ++cid;
    }

    for (int p = 1; p <= N; ++p) {
        int t = lit_id(p, true), f = lit_id(p, false);
        if (comp[t] == comp[f]) return false;
    }

    if (out_assign) {
        out_assign->assign(N + 1, 0);
        for (int p = 1; p <= N; ++p) {
            int t = lit_id(p, true), f = lit_id(p, false);
            (*out_assign)[p] = (comp[t] > comp[f]) ? 1 : 0;
        }
    }
    return true;
}

// ---------- Random helpers ----------
static mt19937 rng((unsigned)chrono::steady_clock::now().time_since_epoch().count());
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
    vector<int> assign;
    bool sat = check_2sat_with_assign(N, cls, &assign);
    if (!sat) cd.expected.clear();
    else {
        string s; s.reserve(N);
        for (int p = 1; p <= N; ++p) s.push_back(assign[p] ? 'P' : 'N');
        cd.expected = move(s);
    }
    return cd;
}

vector<CaseData> gen_cases_for_subtask_ordered(int subtask, int K = 100) {
    vector<CaseData> tests; tests.reserve(K);
    const int MAX_N = 300763;
    const int MAX_M = 676767;

    for (int idx = 0; idx < K; ++idx) {
        // band small->big
        double frac = double(idx) / double(max(1, K - 1));
        int band = 0;
        if (frac < 0.50) band = 0;
        else if (frac < 0.85) band = 1;
        else if (frac < 0.98) band = 2;
        else band = 3;

        int N_min, N_max;
        if (subtask == 1) { N_min = 3; N_max = 20; }
        else if (subtask == 4) {
            if (band == 0) { N_min = 3; N_max = 50; }
            else if (band == 1) { N_min = 51; N_max = 500; }
            else if (band == 2) { N_min = 501; N_max = 1500; }
            else { N_min = 1501; N_max = 3000; }
        }
        else {
            if (band == 0) { N_min = 3; N_max = 60; }
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
    }

    // ensure increasing order by size
    sort(tests.begin(), tests.end(), [](const CaseData& a, const CaseData& b) {
        if (a.N != b.N) return a.N < b.N;
        if (a.M != b.M) return a.M < b.M;
        return a.expected < b.expected;
        });

    return tests;
}

// ---------- Sample Solution (2-SAT) - returns "" or N chars 'P'/'N' ----------
class Solution {
public:
    string solve(int n, int m, const vector<int>& A, const vector<int>& B,
        const vector<string>& X, const vector<string>& Y) {
        vector<Clause> clauses; clauses.reserve(m);
        for (int i = 0; i < m; i++) {
            clauses.push_back({ A[i], X[i] == "pro", B[i], Y[i] == "pro" });
        }
        vector<int> assign;
        bool sat = check_2sat_with_assign(n, clauses, &assign);
        if (!sat) return string("");
        string out; out.reserve(n);
        for (int p = 1; p <= n; ++p) out.push_back(assign[p] ? 'P' : 'N');
        return out;
    }
};

// ---------- Main: generate tests, run solver, report ----------
int main() {
    const int CASES_PER_SUBTASK = 100;

    Solution solver;

    const string RED = "\033[1;31m";
    const string GREEN = "\033[1;32m";
    const string CYAN = "\033[1;36m";
    const string RESET = "\033[0m";

    using clock = chrono::high_resolution_clock;
    auto total_start = clock::now();

    int global_pass = 0, global_fail = 0;
    int case_id = 0;

    cerr << "Generating & judging tests (ordered small -> big per subtask)\n";

    for (int sub = 1; sub <= 5; ++sub) {
        cout << "\n"
            << CYAN
            << "==================== SUBTASK " << sub << " ====================\n"
            << RESET;

        auto tests = gen_cases_for_subtask_ordered(sub, CASES_PER_SUBTASK);

        int pass = 0, fail = 0;
        auto sub_start = clock::now();

        for (size_t i = 0; i < tests.size(); ++i) {
            const CaseData& c = tests[i];
            ++case_id;

            auto start = clock::now();
            string got = solver.solve(c.N, c.M, c.A, c.B, c.X, c.Y);
            auto end = clock::now();
            long long dur_us =
                chrono::duration_cast<chrono::microseconds>(end - start).count();

            auto normalize = [](string s) {
                while (!s.empty() && isspace(s.back())) s.pop_back();
                return s;
                };

            string expn = normalize(c.expected);
            string gotn = normalize(got);
            bool ok = is_valid_answer(c, gotn);

            if (ok) {
                cout << GREEN << "AC" << RESET;
                ++pass;
                ++global_pass;
            }
            else {
                cout << RED << "WA" << RESET;
                ++fail;
                ++global_fail;
            }

            cout << "  Case " << case_id
                << " | N=" << c.N
                << " M=" << c.M
                << " | expected=" << (expn.empty() ? "<empty>" : (expn.size() > 20 ? expn.substr(0, 20) + "..." : expn))
                << " got=" << (gotn.empty() ? "<empty>" : (gotn.size() > 20 ? gotn.substr(0, 20) + "..." : gotn))
                << " | time=" << fixed << setprecision(3)
                << (dur_us / 1000.0) << " ms\n";

            if (!ok && c.A.size() <= 20) {
                int preview = min((int)c.A.size(), 20);
                cout << "  Clauses preview:\n";

                ostringstream is;

                for (int j = 0; j < preview; ++j) {
                    is << c.A[j] << " " << c.X[j] << " " << c.B[j] << " " << c.Y[j] << endl;
                }
                cout << is.str() << endl;

                if (preview < (int)c.A.size())
                    cout << "    ...\n";
            }
        }

        auto sub_end = clock::now();
        long long sub_us =
            chrono::duration_cast<chrono::microseconds>(sub_end - sub_start).count();

        cout << CYAN
            << "-------------------- SUBTASK " << sub << " SUMMARY --------------------\n"
            << RESET;
        cout << "Passed: " << pass << " / " << tests.size()
            << ", Failed: " << fail
            << ", Time: " << fixed << setprecision(3)
            << (sub_us / 1000.0) << " ms\n";
    }

    auto total_end = clock::now();
    long long total_us =
        chrono::duration_cast<chrono::microseconds>(total_end - total_start).count();

    cout << "\n"
        << CYAN
        << "==================== FINAL SUMMARY ====================\n"
        << RESET;
    cout << "Total passed: " << global_pass
        << ", Total failed: " << global_fail
        << ", Total cases: " << (global_pass + global_fail) << "\n";
    cout << "Total time: " << fixed << setprecision(3)
        << (total_us / 1000.0) << " ms\n";

    if (global_fail > 0) {
        cout << RED
            << "At least one WA -> Solution is incorrect on these tests."
            << RESET << "\n";
    }
    else {
        cout << GREEN
            << "All tests passed -> Solution looks correct on generated tests."
            << RESET << "\n";
    }

    return 0;
}
