## Subtask 1

Since $A_i=B_i$, only one position is shot in each turn. From our current
position $x$, the two choices $x-1$ and $x+1$ are different, so at least one
is safe.

Greedily move left if it is safe; otherwise move right. This avoids every
shot, so the minimum number of hits is $0$.

Time complexity: $O(N)$  
Space complexity: $O(N)$ for the returned path, with $O(1)$ auxiliary space

```cpp
class Solution {
public:
    pair<int, string> solve(int n, const vector<int>& A, const vector<int>& B) {
        int position = 0;
        string path(n, 'L');
        for (int i = 0; i < n; ++i) {
            if (position - 1 == A[i]) {
                ++position;
                path[i] = 'R';
            } else {
                --position;
            }
        }
        return {0, path};
    }
};
```

## Subtask 2

Use the same greedy rule, with a parity observation. After turn $i$, our
position has the same parity as $i$, since every move changes parity.

Because $B_i=A_i+1$, exactly one of the two shot positions has that parity.
The other shot cannot hit us. Thus, just as in Subtask 1, at most one of our
two possible next positions is blocked. A safe move always exists, and the
answer is again $0$.

Time complexity: $O(N)$  
Space complexity: $O(N)$ for the returned path, with $O(1)$ auxiliary space

```cpp
class Solution {
public:
    pair<int, string> solve(int n, const vector<int>& A, const vector<int>& B) {
        int position = 0;
        string path(n, 'L');
        for (int i = 0; i < n; ++i) {
            int left = position - 1;
            if (left == A[i] || left == B[i]) {
                ++position;
                path[i] = 'R';
            } else {
                --position;
            }
        }
        return {0, path};
    }
};
```

## Subtask 3

Let $dp[t][k]$ be the minimum number of hits after $t$ turns, having made
exactly $k$ right moves. We have made $t-k$ left moves, so the current
position is

$$
x=2k-t.
$$

Initialize $dp[0][0]=0$ and all other states to infinity. We can reach
$(t,k)$ by moving left from $(t-1,k)$ or right from $(t-1,k-1)$:

$$
dp[t][k]=\min\bigl(dp[t-1][k],dp[t-1][k-1]\bigr)
+[2k-t=A_t\ \lor\ 2k-t=B_t].
$$

Ignore predecessors outside $0\le k\le t-1$. Coincident shots still add
only one hit.

The answer is $\min_k dp[N][k]$. Start from a minimizing final state and
follow any predecessor that attains its value to reconstruct the path.
Keeping the whole table makes reconstruction straightforward for $N\le2000$.

Time complexity: $O(N^2)$  
Space complexity: $O(N^2)$

```cpp
class Solution {
public:
    pair<int, string> solve(int n, const vector<int>& A, const vector<int>& B) {
        const int INF = n + 1;
        vector<vector<int>> dp(n + 1, vector<int>(n + 1, INF));
        dp[0][0] = 0;
        for (int t = 1; t <= n; ++t) {
            for (int k = 0; k <= t; ++k) {
                int best = INF;
                if (k < t) best = min(best, dp[t - 1][k]);     // Left
                if (k > 0) best = min(best, dp[t - 1][k - 1]); // Right
                int position = 2 * k - t;
                int hit = position == A[t - 1] || position == B[t - 1];
                dp[t][k] = best + hit;
            }
        }

        int k = int(min_element(dp[n].begin(), dp[n].end()) - dp[n].begin());
        int answer = dp[n][k];
        string path(n, 'L');
        for (int t = n; t >= 1; --t) {
            int position = 2 * k - t;
            int hit = position == A[t - 1] || position == B[t - 1];
            if (k < t && dp[t - 1][k] + hit == dp[t][k]) continue;
            path[t - 1] = 'R';
            --k;
        }
        return {answer, path};
    }
};
```

## Subtask 4

Instead of updating every state, store consecutive states with equal costs
as intervals in an ordered map. This gives an $O(N\log N)$ solution directly
from the DP.

### Backward DP

Let $F_t(k)$ be the minimum number of hits on turns $t+1,\ldots,N$, after
turn $t$ with $k$ right moves. Initially $F_N(k)=0$.

Process turn $t$ backwards. First add its hit cost:

$$
G(k)=F_t(k)+[2k-t=A_t\ \lor\ 2k-t=B_t].
$$

Then choose the next move:

$$
F_{t-1}(k)=\min(G(k),G(k+1)).
$$

The two candidates correspond to moving left and right. The final answer
is $F_0(0)$.

A shot at position $x$ affects only $k=(x+t)/2$. Ignore it if $x\notin[-t,t]$
or $x+t$ is odd. Process equal shot positions only once.

For convenience, maintain costs over $k=0,\ldots,N$, with zero outside the
right boundary. Values at unreachable states do not affect the answer:
from a reachable state, both successor states are reachable at the next turn.

### Maintaining constant intervals

A map entry $(l,v)$ means that the interval starting at $l$ has cost $v$,
until the next map entry. Keep entries at $0$ and a zero sentinel at $N+1$.

To assign one cell $k$, split its interval at $k$ and $k+1$, update its value,
and merge adjacent equal intervals. This uses $O(\log N)$ time. Each shot
is just a point increment.

Now consider the minimum transition. Inside a constant interval,
$G(k)=G(k+1)$, so nothing changes. A change occurs only just before a
boundary $p$ where the cost drops:

$$
G(p-1)>G(p).
$$

For such a boundary, set the single cell $p-1$ to $G(p)$. Collect all these
changes before applying them, so every comparison uses the same $G$.

Also record $p$ for turn $t$: from state $k=p-1$, moving right is strictly
better. During reconstruction, choose right exactly when $k+1$ is recorded
for that turn; otherwise choose left. The recorded boundaries are sorted,
so membership can be checked with binary search.

### Bounding the number of interval updates

We still need to justify scanning the map on every turn. Use the potential

$$
\Phi=\sum_{k=0}^{N}F(k)\ge0.
$$

The shots increase $\Phi$ by at most $2$ per turn. A downward boundary of
height $h=G(p-1)-G(p)>0$ causes the minimum transition to decrease $\Phi$
by exactly $h$. Therefore, if $W_t$ is the sum of downward jump heights
before the transition at turn $t$,

$$
\sum_{t=1}^{N}W_t\le2N.
$$

To bound the map size, extend the cost function by zero on both sides.
Its total upward jump height equals its total downward jump height $W_t$.
Every nonzero jump has integer height at least $1$, so there are at most
$2W_t$ such boundaries, plus a constant number of sentinels.

Consequently, all map scans together take $O(N)$ time, and there are $O(N)$
point assignments and recorded downward boundaries in total. Each map
operation takes $O(\log N)$. Reconstruction takes at most $O(N\log N)$.

Time complexity: $O(N\log N)$  
Space complexity: $O(N)$

```cpp
class Solution {
public:
    pair<int, string> solve(int n, const vector<int>& A, const vector<int>& B) {
        // A map entry starts a constant-cost interval. n+1 is a zero sentinel
        map<int, int> cost{{0, 0}, {n + 1, 0}};
        
        auto split = [&](int k) {
            auto it = prev(cost.upper_bound(k));
            if (it->first == k) return it;
            return cost.emplace(k, it->second).first;
        };
        
        auto assign = [&](int k, int value) {
            auto it = split(k);
            auto after = split(k + 1);
            it->second = value;
            if (it != cost.begin() && prev(it)->second == value) cost.erase(it);
            if (after->first != n + 1 && prev(after)->second == after->second)
                cost.erase(after);
        };
        
        auto shoot = [&](int x, int t) {
            if (x < -t || x > t || (x + t) % 2) return;
            int k = (x + t) / 2;
            assign(k, prev(cost.upper_bound(k))->second + 1);
        };
        
        vector<vector<int>> right(n + 1);
        vector<pair<int, int>> changes;
        for (int t = n; t >= 1; --t) {
            shoot(A[t - 1], t);
            if (B[t - 1] != A[t - 1]) shoot(B[t - 1], t);
            changes.clear();
            for (auto it = next(cost.begin()); it != cost.end(); ++it) {
                if (prev(it)->second > it->second) {
                    changes.push_back({it->first - 1, it->second});
                    right[t].push_back(it->first);
                }
            }
            for (auto [k, value] : changes) assign(k, value);
        }
        
        int answer = cost.begin()->second, k = 0;
        string path(n, 'L');
        for (int t = 1; t <= n; ++t) {
            if (binary_search(right[t].begin(), right[t].end(), k + 1)) {
                path[t - 1] = 'R';
                ++k;
            }
        }
        
        return {answer, path};
    }
};
```

## General Solution

Optimize the same backward DP by storing its differences instead of an
ordered map of intervals.

### Moving negative differences

Let

$$
D[k]=G(k)-G(k-1),
$$

where $G(-1)=0$. A shot at right-move count $k$ changes just two entries:

$$
D[k]\mathrel{+}=1,\qquad D[k+1]\mathrel{-}=1.
$$

Since

$$
\min(G(k),G(k+1))=G(k)+\min(D[k+1],0),
$$

taking differences gives

$$
D_{\mathrm{new}}[k]=\max(D[k],0)+\min(D[k+1],0).
$$

Thus positive differences stay where they are, and negative differences
move one index to the left. The same formula holds at $k=0$ because
$D[0]=G(0)\ge0$; a negative entry never moves out of the array on the left.

Keep the indices of negative differences in a sorted vector. Insert any
new negative index after a shot, unless it is already present. A shot can
also cancel a negative difference; leave its index in the list until the
subsequent scan, where nonnegative entries are ignored.

Process the list in increasing order. When processing $p$, move `D[p]` to
`D[p-1]` and clear `D[p]`. Every destination is smaller than every
unprocessed source, so these in-place updates give the same result as
simultaneous shifts. Collect the destinations that remain negative for the
next turn. They are already sorted and distinct.

### Why sorted-vector insertions are still linear overall

The potential argument from Subtask 4 gives

$$
\sum_t W_t\le2N.
$$

Let $h_t$ be the number of actually negative entries after adding turn
$t$'s shots, and let $m_t$ be the number in the list before adding them.
Since every negative entry has integer magnitude at least $1$,

$$
h_t\le W_t,\qquad \sum_t h_t\le2N.
$$

That bounds the shifts, but **does not by itself bound vector insertion**.
Here is where the two-shot restriction is needed:

- Each distinct shot increments one difference entry, so at most two old
  negative entries can cease to be negative. Hence $m_t\le h_t+2$.
- Each shot decrements one difference entry, so at most two indices are
  inserted. Throughout the updates, the vector has at most
  $m_t+2\le h_t+4$ entries.
- Each insertion can shift $O(h_t+1)$ elements, but there are at most two
  insertions. Their binary searches, the full scan including stale entries,
  and construction of the next list also cost $O(h_t+1)$ for this turn.

Summing over turns gives

$$
\sum_t O(h_t+1)=O(N).
$$

This proves the implementation's total time bound, including maintaining
the sorted vector. With an unbounded number of shots per turn, repeated
linear-time insertions would require another argument or a different data
structure. The increasing-order shift itself remains correct; it is this
insertion-cost bound that uses the constant number of shots.

### Reconstructing the path

Just before shifting, moving right from state $k$ is strictly better when
$D[k+1]<0$. Record the negative indices for each turn, as in Subtask 4.

Store all records in a flat vector, with offsets separating the turns.
There are at most $\sum_t h_t\le2N$ records. Start at $k=0$ and replay turns
forwards, choosing right when the current turn contains $k+1$. A tie chooses
left. Scanning these records over all turns takes $O(N)$ time.

At the end of the backward DP, `diff[0]` is $F_0(0)$, the minimum number of
hits. Return it together with the reconstructed path.

Time complexity: $O(N)$ amortized  
Space complexity: $O(N)$

```cpp
class Solution {
public:
    pair<int, string> solve(int n, const vector<int>& A, const vector<int>& B) {
        // k = number of right moves. Position at turn t is 2*k - t
        // diff is the discrete derivative of the backward cost function
        vector<int> diff(n + 2), offset(n + 1);
        vector<int> negative, next, right;
        right.reserve(2 * n);

        auto shoot = [&](int position, int t) {
            if (position < -t || position > t || (position + t) % 2) return;
            int k = (position + t) / 2;
            ++diff[k];
            if (--diff[k + 1] < 0) {
                auto it = lower_bound(negative.begin(), negative.end(), k + 1);
                if (it == negative.end() || *it != k + 1)
                    negative.insert(it, k + 1);
            }
        };

        for (int t = n; t >= 1; --t) {
            shoot(A[t - 1], t);
            if (B[t - 1] != A[t - 1]) shoot(B[t - 1], t);
            offset[t] = int(right.size());
            next.clear();
            // Ascending order makes in-place shifts simultaneous: a move
            // affects only an index smaller than every unprocessed source
            for (int p : negative) {
                if (diff[p] >= 0) continue;
                right.push_back(p); // Right is better when k + 1 == p
                diff[p - 1] += diff[p];
                diff[p] = 0;
                if (diff[p - 1] < 0) next.push_back(p - 1);
            }
            negative.swap(next);
        }
        offset[0] = int(right.size());

        string path(n, 'L');
        int k = 0;
        for (int t = 1; t <= n; ++t) {
            for (int i = offset[t]; i < offset[t - 1]; ++i) {
                if (right[i] == k + 1) {
                    path[t - 1] = 'R';
                    ++k;
                    break;
                }
            }
        }
        return {diff[0], move(path)};
    }
};
```
