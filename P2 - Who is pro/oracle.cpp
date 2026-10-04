#include <bits/stdc++.h>
using namespace std;

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

class Oracle {
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

