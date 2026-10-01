## Subtask 1

Since $A_i=B_i$, we only need to visit every requested floor.

Let $L=\min(0,A_1,\ldots,A_N)$ and $R=\max(0,A_1,\ldots,A_N)$. Visit one extreme, then the other. Going to the nearer extreme first gives

$$
(R-L)+\min(-L,R).
$$

Time complexity: $O(N)$  
Space complexity: $O(1)$

```cpp
class Solution {
public:
    long long solve(int n, const vector<int>& A, const vector<int>& B) {
        long long left = 0, right = 0;
        for (int floor : A) {
            left = min(left, (long long)floor);
            right = max(right, (long long)floor);
        }
        return right - left + min(-left, right);
    }
};
```

## Subtask 2

All floors are non-negative. Let $H=\max_i\max(A_i,B_i)$. The elevator must climb from $0$ to $H$, which automatically serves every upward request.

A downward request $a\to b$ requires a descent across $[b,a]$. Merge overlapping or touching downward intervals into disjoint segments $[l_i,r_i]$, ordered from bottom to top.

There are two ways to handle each segment:

- During the ascent, go from $r_i$ back to $l_i$, then return upward. This adds $2(r_i-l_i)$.
- Leave it for the final descent from $H$.

If the final descent stops at $l_i$, all segments below it must be handled during the ascent, and all segments from $i$ upward are handled on the final descent. The cost is

$$
H+2\sum_{j \lt i}(r_j-l_j)+(H-l_i).
$$

Also try making no final descent, which costs $H+2\sum_j(r_j-l_j)$. There is no benefit to stopping partway through a gap or a merged segment: a gap can be skipped, and any unfinished segment requires descending to its bottom.

Since $H\le10^5$, use a difference array on the floors to find the merged segments. A running total of their lengths lets us check every useful return distance in one scan.

Time complexity: $O(N+H)$  
Space complexity: $O(H)$

```cpp
class Solution {
public:
    long long solve(int n, const vector<int>& A, const vector<int>& B) {
        int highest = 0;
        for (int i = 0; i < n; ++i)
            highest = max({highest, A[i], B[i]});

        vector<int> difference(highest + 1, 0);
        for (int i = 0; i < n; ++i) {
            if (A[i] > B[i]) {
                ++difference[B[i]];
                --difference[A[i]];
            }
        }

        long long answer = 2LL * highest;
        long long covered = 0;
        int active = 0;
        for (int floor = 0; floor < highest; ++floor) {
            int previous = active;
            active += difference[floor];

            // A merged segment begins here. Try ending the final descent here.
            if (previous == 0 && active > 0)
                answer = min(answer, 2LL * highest - floor + 2 * covered);

            // Each covered unit gap contributes once to the union length.
            if (active > 0) ++covered;
        }

        // Handle every downward segment during the ascent, then stop at H.
        return min(answer, highest + 2 * covered);
    }
};
```

## Subtask 3

Use the same idea as Subtask 2. The floors are now too large for a difference array, so sort the downward intervals by their left endpoints and merge them directly.

Scan the merged segments, maintaining the total length of earlier segments. The candidate costs are unchanged.

Time complexity: $O(N\log N)$  
Space complexity: $O(N)$

```cpp
class Solution {
public:
    long long solve(int n, const vector<int>& A, const vector<int>& B) {
        using int64 = long long;
        int64 highest = 0;
        vector<pair<int64, int64>> downward;
        for (int i = 0; i < n; ++i) {
            highest = max({highest, (int64)A[i], (int64)B[i]});
            if (A[i] > B[i]) downward.push_back({B[i], A[i]});
        }
        sort(downward.begin(), downward.end());

        vector<pair<int64, int64>> segments;
        for (auto [left, right] : downward) {
            if (segments.empty() || segments.back().second < left)
                segments.push_back({left, right});
            else
                segments.back().second = max(segments.back().second, right);
        }

        int64 answer = 2 * highest;
        int64 covered = 0;
        for (auto [left, right] : segments) {
            answer = min(answer, 2 * highest - left + 2 * covered);
            covered += right - left;
        }
        return min(answer, highest + 2 * covered);
    }
};
```

## Subtask 4

With $N\le10$, enumerate pickup and delivery orders using DP. Each resident has three states:

- $0$: waiting;
- $1$: in the elevator;
- $2$: delivered.

Encode these states as the digits of a base-3 integer $mask$. Let $dp(mask,last)$ be the minimum remaining distance, where $last$ identifies the elevator's current floor among the $2N$ pickup/delivery locations and the initial floor $0$.

For every resident who is not delivered, try their next event: go to $A_i$ to pick them up, or to $B_i$ to drop them off. Increment their base-3 digit and recurse. When every digit is $2$, no further travel is needed.

Every feasible plan gives an order of these events with each pickup before its delivery. Conversely, every such order gives a feasible route, so trying all orders finds the optimum. Events at the same floor cost zero, including requests with $A_i=B_i$.

Time complexity: $O(N^2\cdot3^N)$  
Space complexity: $O(N\cdot3^N)$

```cpp
class Solution {
public:
    long long solve(int n, const vector<int>& A, const vector<int>& B) {
        using int64 = long long;
        const int64 INF = 1LL << 62;

        vector<int> power(n + 1, 1);
        for (int i = 0; i < n; ++i) power[i + 1] = power[i] * 3;

        // Event 2*i is pickup; event 2*i+1 is delivery; event 2*n is start.
        vector<int64> floor(2 * n + 1, 0);
        for (int i = 0; i < n; ++i) {
            floor[2 * i] = A[i];
            floor[2 * i + 1] = B[i];
        }

        int complete = power[n] - 1;
        vector<vector<int64>> memo(power[n], vector<int64>(2 * n + 1, -1));
        function<int64(int, int)> dp = [&](int mask, int last) -> int64 {
            if (mask == complete) return 0;
            int64& answer = memo[mask][last];
            if (answer != -1) return answer;
            answer = INF;

            for (int i = 0; i < n; ++i) {
                int state = mask / power[i] % 3;
                if (state == 2) continue;
                int next = 2 * i + state;
                int64 cost = abs(floor[next] - floor[last])
                           + dp(mask + power[i], next);
                answer = min(answer, cost);
            }
            return answer;
        };

        return dp(0, 2 * n);
    }
};
```

## General Solution

Think of the floors as points on a line. Since the elevator has unlimited capacity, we can always pick up a resident on our first visit to their floor and drop them off on the first later visit to their destination. We only need to choose the elevator's route.

Let $L$ and $R$ be the smallest and largest floors appearing in any request. First move directly from $0$ to the nearest point $s$ in $[L,R]$, paying $|s|$. All remaining movement can stay inside this interval.

We consider routes that reach $L$ before $R$. Negating every floor handles the opposite order.

### 1. The cost of backtracking

Suppose we must start at $x$, finish at $y$, and serve requests entirely inside $[x,y]$, where $x \le y$.

The basic cost is $y-x$. Every upward request is automatically satisfied. A downward request $a \to b$ requires travelling downward across $[b,a]$, then recovering that distance to finish at $y$.

Merge the intervals of the downward requests. For each merged interval $[l,r]$, travel to $r$, go back to $l$, then continue upward. Thus:

$$
\text{cost}=(y-x)+2\cdot\text{length of the union of downward intervals}.
$$

Each covered segment must be crossed downward at least once, so this construction is optimal. Overlapping requests share the same backtracking; their lengths must not be added separately.

### 2. Two useful turning points

Let $p \ge s$ be the highest floor reached on the initial excursion before going down to $L$, and let $q$ be the final floor after coming down from $R$.

The key structural result is that an optimal route can be chosen with this shape: an initial excursion, a climb from $L$ to $R$ with necessary backtracking, and a final descent. When the two descents do not overlap, suitable boundaries can be chosen so that no downward request crosses either boundary. This is the canonical-route result in [*Ride Sharing with a Vehicle of Unlimited Capacity*](https://arxiv.org/pdf/1507.02414), Section 3.2, Theorems 12 and 19.

The saved implementation also considers a final climb from $q$ to a prescribed finishing floor. Here we can stop at $q$: upward requests have already been served during the climb from $L$ to $R$, and a final climb cannot help any remaining downward request.

**Case A: The descents overlap ($q \le p$).**

Use the route

$$
s \to p \to L \to R \to q.
$$

A downward request $a \to b$ is served either by $p \to L$ when $a \le p$, or by $R \to q$ when $b \ge q$.

For a fixed $q$, every request with $b \lt q$ must therefore have $a \le p$. Choose the smallest possible peak:

$$
p=\max\bigl(s,q,\max_{a \gt b,\ b \lt q} a\bigr),
$$

ignoring the last term if there is no such request. The route costs

$$
(p-s)+(p-L)+(R-L)+(R-q)
=2(R-L)-s+2p-q.
$$

Sort downward requests by $b$ and sweep $q$ upward, maintaining the largest relevant $a$.

**Case B: There is a gap ($p<q$).**

Use an initial excursion $s \to p \to L \to p$, solve the requests inside $[p,q]$, then continue $q \to R \to q$.

It suffices to consider boundaries satisfying:

- No downward request has $b \le p<a$.
- No downward request has $b \lt q \le a$.

These conditions put each downward request entirely in the initial descent, the middle interval, or the final descent. Let $U(p,q)$ be the union length of downward intervals inside $[p,q]$. The cost is

$$
(p-s)+2(p-L)+(q-p)+2U(p,q)+2(R-q)
=2(R-L)-s+2p-q+2U(p,q).
$$

**Only the smallest valid $p \ge s$ is needed.** Moving $p$ left by $d$ saves $2d$ outside the middle interval and adds at most $2d$ of backtracking inside it. Therefore, fixing this $p$ and trying every valid $q>p$ is enough.

Also include the boundary case $s=L$ with finish $R$, whose cost is $(R-L)+2U(L,R)$ from the first observation.

### 3. Computing the costs efficiently

Compress all request endpoints and $s$ into a sorted array $x$. Turning between consecutive coordinates gives no advantage, since no pickup or delivery occurs there.

For each downward interval $[b,a]$, use difference arrays to mark:

- $[b,a)$: invalid initial boundaries $p$; this also marks the gaps covered by the interval union.
- $(b,a]$: invalid final boundaries $q$ for Case B.

Let $\text{covered}[i]$ be the union length between $L$ and $x_i$. For valid boundaries $p=x_j$ and $q=x_i$,

$$
U(p,q)=\text{covered}[i]-\text{covered}[j].
$$

Both cases now need only a linear sweep after sorting. Run the same algorithm with all coordinates negated, take the smaller result, and include the initial cost $|s|$.

The route construction gives a feasible plan for every candidate. The structural result guarantees that some optimum is among these cases in one of the two orientations; the sweeps minimize their costs.

Time complexity: $O(N\log N)$  
Space complexity: $O(N)$

**Note:** Use `long long` for distances. Requests with $A_i=B_i$ still require visiting that floor, but need no backtracking; pickup and delivery can occur at the same visit with zero extra time.

```cpp
class Solution {
    using int64 = long long;
    using Request = pair<int64, int64>;
    static constexpr int64 INF = 1LL << 62;

    int64 solveDirection(const vector<Request>& requests) {
        int64 left = INF, right = -INF;
        vector<int64> floors;
        vector<Request> downward; // (destination, origin)

        for (auto [a, b] : requests) {
            left = min({left, a, b});
            right = max({right, a, b});
            floors.push_back(a);
            floors.push_back(b);
            if (a > b) downward.push_back({b, a});
        }

        const int64 start = clamp(0LL, left, right);
        floors.push_back(start);
        sort(floors.begin(), floors.end());
        floors.erase(unique(floors.begin(), floors.end()), floors.end());
        sort(downward.begin(), downward.end());

        const int size = int(floors.size());
        auto indexOf = [&](int64 floor) {
            return int(lower_bound(floors.begin(), floors.end(), floor)
                       - floors.begin());
        };

        vector<int> peakDiff(size + 1), finishDiff(size + 1);
        for (auto [b, a] : downward) {
            int lo = indexOf(b), hi = indexOf(a);
            ++peakDiff[lo];       // b <= p < a
            --peakDiff[hi];
            ++finishDiff[lo + 1]; // b < q <= a
            --finishDiff[hi + 1];
        }

        vector<bool> blockedPeak(size), blockedFinish(size);
        vector<int64> covered(size, 0);
        int activePeak = 0, activeFinish = 0;
        for (int i = 0; i < size; ++i) {
            activePeak += peakDiff[i];
            activeFinish += finishDiff[i];
            blockedPeak[i] = activePeak > 0;
            blockedFinish[i] = activeFinish > 0;

            if (i > 0) {
                covered[i] = covered[i - 1];
                if (blockedPeak[i - 1])
                    covered[i] += floors[i] - floors[i - 1];
            }
        }

        // Case B uses the smallest valid initial peak.
        int peakIndex = indexOf(start);
        while (blockedPeak[peakIndex]) ++peakIndex;
        const int64 fixedPeak = floors[peakIndex];
        const int64 base = 2 * (right - left) - start;

        int64 best = INF;
        if (start == left)
            best = (right - left) + 2 * covered.back();

        int next = 0;
        int64 requiredPeak = start;
        for (int i = 0; i < size; ++i) {
            int64 finish = floors[i];

            // Case A: requests ending below finish must be served early.
            while (next < int(downward.size()) &&
                   downward[next].first < finish) {
                requiredPeak = max(requiredPeak, downward[next].second);
                ++next;
            }
            int64 peak = max(requiredPeak, finish);
            best = min(best, base + 2 * peak - finish);

            // Case B: only the middle interval needs extra backtracking.
            if (fixedPeak < finish && !blockedFinish[i]) {
                int64 middleUnion = covered[i] - covered[peakIndex];
                best = min(best, base + 2 * fixedPeak - finish
                                      + 2 * middleUnion);
            }
        }

        return abs(start) + best;
    }

public:
    long long solve(int n, const vector<int>& A, const vector<int>& B) {
        vector<Request> requests;
        for (int i = 0; i < n; ++i)
            requests.push_back({A[i], B[i]});

        int64 answer = solveDirection(requests);
        for (auto& [a, b] : requests) {
            a = -a;
            b = -b;
        }
        return min(answer, solveDirection(requests));
    }
};
```
