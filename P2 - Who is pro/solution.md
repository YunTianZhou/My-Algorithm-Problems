## Subtask 1

Enumerate all possible answers, and check if all requirements are met.

Time complexity: $O(M * 2^N)$

```cpp
class Solution {
public:
    string solve(int n, int m, const vector<int>& A, const vector<int>& B,
                               const vector<string>& X, const vector<string>& Y) {
        // Aviod getting TLE on other subtasks
        if (n > 20 || m > 50) return "";

        for (int i = 0; i < (1 << (n - 2)); i++) {
            int mask = 1 + (i << 1);

            bool f = true;
            for (int j = 0; j < m; j++) {
                int a = A[j] - 1;
                int b = B[j] - 1;
                int x = X[j] == "pro" ? 0 : 1;
                int y = Y[j] == "pro" ? 0 : 1;
                if ((mask >> a & 1) == x && (mask >> b & 1) != y) {
                    f = false;
                    break;
                }
            }

            if (f) {
                string ans;
                for (int j = 0; j < n; j++)
                    ans.push_back((mask >> j & 1) == 0 ? 'P' : 'N');
                return ans;
            }
        }

        return "";
    }
};
```

## Subtask 2

Note that the conditions are in the form of:

```
If person i is `<pro|noob>`, then person j is `<pro|noob>`.
```

That means if the `if` part doesn't met, the condition is useless. 

Since every person only has have one opinion, for each $i$, if $X_i$ is `pro`, we make the person $A_i$ a `noob`, and vise versa. This ensures all opinions met.

Since Tim and Scifish don't have opinions, the solution always exist by following the procedure above.

Time complexity: $O(N + M)$

**Note: This algorithm only work under the special constrictions of this subtask.**

```cpp
class Solution {
public:
    string solve(int n, int m, const vector<int>& A, const vector<int>& B,
                               const vector<string>& X, const vector<string>& Y) {
        string ans(n, 'P');
        ans[0] = 'N';
        for (int i = 0; i < m; i++)
            if (X[i] == "pro")
                ans[A[i] - 1] = 'N';
        return ans;
    }
};
```

## Subtask 3

Due to the substack's constraints, if $X_i = Y_i$, then these must exist:

```
If A_i if pro, then B_i is pro.
If A_i if noob, then B_i is noob.
```

which implies that person $A_i$ and person $B_i$ have the **same** role.

Similarly, if $X_i \ne Y_i$, due to the special constraints, it implies that person $A_i$ and person $B_i$ have **different** roles. 

Note that all the conditions are actually about if two person's role are the same or different. This is a classic problem that can be solved by building a graph and applying the coloring trick.

Time complexity: $O(N + M)$

**Note: This algorithm only work under the special constrictions of this subtask.**

```cpp
class Solution {
public:
    string solve(int n, int m, const vector<int>& A, const vector<int>& B,
                               const vector<string>& X, const vector<string>& Y) {
        vector<vector<pair<int, bool>>> G(n);
        for (int i = 0; i < m; i++) {
            bool type = X[i] == Y[i];
            G[A[i] - 1].push_back({ B[i] - 1, type });
            G[B[i] - 1].push_back({ A[i] - 1, type });
        }

        vector<int> color(n, -1);
        function<bool(int, int)> dfs = [&](int a, int c) -> bool {
            if (color[a] != -1)
                return c != color[a];
            color[a] = c;
            for (auto [b, t] : G[a])
                if (dfs(b, t ? c : (1 - c)))
                    return true;
            return false;
        };

        for (int a = 0; a < n; a++)
            if (color[a] == -1)
                if (dfs(a, 0))
                    return "";

        string ans(n, 'N');
        for (int i = 1; i < n; i++)
            ans[i] = color[i] == 0 ? 'P' : 'N';

        return ans;
    }
};
```

## Subtask 4

**Let $0$ represent `noob` and $1$ represent `pro`.**

Since each person have only two states (`pro` or `noob`), for the i-th person, create two nodes:

- $(i, 0)$ The i-th person is `noob` 
- $(i, 1)$ The i-th person is `pro`

Each node can either be **true** or **false**, we call it the **state** of this node.

Note that the opinions are propositions:

```
If person i is x, then person j is y.
```

which implies to:

```
If node (i, x) is true, then node node (j, y) must be true.
If node (j, 1 - y) is true, then node node (i, 1 - x) must be true.
```

**Note: $1 - x$ is the reverse role of role $x$**

Therefore, for each opinion numbered $j$, have:

- $(A_j, X_j) \rightarrow (B_j, Y_j)$
- $(B_j, 1 - Y_j) \rightarrow (A_j, 1 - X_j)$

We can use add a direct edge from node $a$ to $b$ to describe the relationship $a \rightarrow b$.

---

Initially, we set the states of all nodes to **false**.

For each person $i$, we first guess a role $x$, then start a bfs/dfs search at node $(i, x)$, marking all the reachable nodes **true** (due to the transitivity
of propositions).

After that, the role $x$ is **good** to person $i$ if the following requirements are met.

- Node $(1, 1)$ is **false**
- Node $(N, 0)$ is **false**
- Node $(i, 0)$ and node $(i, 1)$ are not both **true** $(1 \le i \le N)$

If role $x$ is **good**, let $x$ be the role of person $i$, and move on to the next node.

If role $x$ is not **good**, test role $1 - x$. If it is **good**,  let $1 - x$ be the role of person $i$, and move on to the next node.

If neither $x$ and $1 - x$ are **good**, that means solution doesn't exist.

**Note: Always restore the state of nodes if the result is not good.**

It can be proven that (idk exactly how to prove |l^_^l|):

- If the task is impossible, if you completely follow the procedure above, there **always** exist a person $i$, such that taking the role $0$ and $1$ are both not **good**.
- If the task is possible, the procedure above can **always** find a valid solution.

Time complexity: $O(N * (N + M))$

```cpp
class Solution {
public:
    string solve(int n, int m, const vector<int>& A, const vector<int>& B,
                               const vector<string>& X, const vector<string>& Y) {
        // Aviod getting TLE on other subtasks
        if (n > 3000 || m > 3000) return "";
        
        vector<vector<int>> G(n * 2);
        for (int i = 0; i < m; i++) {
            int a = (A[i] - 1) * 2, b = (B[i] - 1)  * 2;
            int x = X[i] == "pro", y = Y[i] == "pro";
            G[a + x].push_back(b + y);
            G[b + (1 - y)].push_back(a + (1 - x));
        }

        vector<bool> vis(n * 2, false);

        function<bool(int)> try_assign = [&](int start) {
            vector<bool> nxt_vis = vis;

            stack<int> st;
            st.push(start);

            while (!st.empty()) {
                int a = st.top();
                st.pop();
                nxt_vis[a] = true;

                for (int b : G[a])
                    if (!nxt_vis[b])
                        st.push(b);
            }

            // Invalid: Scifish is pro or Tim is noob
            if (nxt_vis[0 * 2 + 1] || nxt_vis[(n - 1) * 2])
                return false;

            for (int a = 0; a < n; a++)
                if (nxt_vis[a * 2] && nxt_vis[a * 2 + 1])
                    return false;

            vis = move(nxt_vis);
            return true;
        };

        for (int a = 0; a < n; a++)
            if (!try_assign(a * 2) && !try_assign(a * 2 + 1))
                return "";

        string ans(n, 'P');
        for (int a = 0; a < n; a++)
            if (vis[a * 2])
                ans[a] = 'N';

        return ans;
    }
};
```

## General Solution

**Let $0$ represent `noob` and $1$ represent `pro`.**

This is a template problem of [**2-SAT**](https://cp-algorithms.com/graph/2SAT.html).

Note that there are two additional clauses:

- $\lnot P_1 \lor \lnot P_1$
- $P_N \lor P_N$

Time complexity: $O(N + M)$

**Note: The recursion version can easily lead to a stack overflow. See the safer version below.**

```cpp
class Solution {
public:
    string solve(int n, int m, const vector<int>& A, const vector<int>& B,
                               const vector<string>& X, const vector<string>& Y) {
        vector<vector<int>> G(n * 2);
        for (int i = 0; i < m; i++) {
            int a = (A[i] - 1) * 2, b = (B[i] - 1)  * 2;
            int x = X[i] == "pro", y = Y[i] == "pro";
            G[a + x].push_back(b + y);
            G[b + (1 - y)].push_back(a + (1 - x));
        }

        // Ensures Scifish is noob and Tim is pro
        G[0 * 2 + 1].push_back(0 * 2);
        G[(n - 1) * 2].push_back((n - 1) * 2 + 1);

        // Tarjan: find SCC
        vector<int> tin(n * 2, -1), low(n * 2, 0), scc(n * 2, 0);
        vector<bool> in_stack(n * 2, false);
        stack<int> st;
        int timer = 0, cnt = 0;

        function<void(int)> dfs = [&](int a) {
            tin[a] = low[a] = timer++;
            in_stack[a] = true;
            st.push(a);

            for (int b : G[a]) {
                if (tin[b] == -1) {
                    dfs(b);
                    low[a] = min(low[a], low[b]);
                }
                else if (in_stack[b]) {
                    low[a] = min(low[a], tin[b]);
                }
            }

            if (low[a] >= tin[a]) {
                while (true) {
                    int x = st.top();
                    st.pop();
                    in_stack[x] = false;
                    scc[x] = cnt;
                    if (x == a) break;
                }
                cnt++;
            }
        };

        for (int a = 0; a < n * 2; a++)
            if (tin[a] == -1)
                dfs(a);

        // Construct solution
        string ans(n, 'P');
        for (int i = 0; i < n; i++) {
            int a = scc[i * 2], b = scc[i * 2 + 1];
            if (a == b) return "";
            ans[i] = a < b ? 'N' : 'P';
        }

        return ans;
    }
};
```

### Safer Version

```cpp
class Solution {
public:
    string solve(int n, int m, const vector<int>& A, const vector<int>& B,
                               const vector<string>& X, const vector<string>& Y) {
        vector<vector<int>> G(n * 2);
        for (int i = 0; i < m; i++) {
            int a = (A[i] - 1) * 2, b = (B[i] - 1)  * 2;
            int x = X[i] == "pro", y = Y[i] == "pro";
            G[a + x].push_back(b + y);
            G[b + (1 - y)].push_back(a + (1 - x));
        }

        // Ensures Scifish is noob and Tim is pro
        G[0 * 2 + 1].push_back(0 * 2);
        G[(n - 1) * 2].push_back((n - 1) * 2 + 1);

        // Tarjan: find SCC (Safer)
        vector<int> tin(n * 2, -1), low(n * 2), scc(n * 2);
        vector<bool> in_stack(n * 2, false);
        stack<int> st;

        int timer = 0, cnt = 0;

        struct Frame {
            int a;
            int ei;  // next node's index
        };

        for (int start = 0; start < n * 2; start++) {
            if (tin[start] != -1) continue;

            stack<Frame> dfs;
            dfs.push({ start, 0 });

            while (!dfs.empty()) {
                auto& f = dfs.top();
                int a = f.a;

                if (f.ei == 0) {
                    tin[a] = low[a] = timer++;
                    st.push(a);
                    in_stack[a] = true;
                }

                if (f.ei < (int)G[a].size()) {
                    int b = G[a][f.ei++];
                    if (tin[b] == -1) {
                        dfs.push({ b, 0 });
                    }
                    else if (in_stack[b]) {
                        low[a] = min(low[a], tin[b]);
                    }
                }
                else {
                    if (low[a] >= tin[a]) {
                        while (true) {
                            int x = st.top(); st.pop();
                            in_stack[x] = 0;
                            scc[x] = cnt;
                            if (x == a) break;
                        }
                        cnt++;
                    }

                    dfs.pop();
                    if (!dfs.empty()) {
                        low[dfs.top().a] = min(low[dfs.top().a], low[a]);
                    }
                }
            }
        }

        // Construct solution
        string ans(n, 'P');
        for (int i = 0; i < n; i++) {
            int a = scc[i * 2], b = scc[i * 2 + 1];
            if (a == b) return "";
            ans[i] = a < b ? 'N' : 'P';
        }

        return ans;
    }
};
```
