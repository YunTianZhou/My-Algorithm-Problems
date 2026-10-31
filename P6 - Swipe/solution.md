## Subtask 1

Simulate swipes directly. For a `K` operation, copy its subarray, sort the
copy, and return its $k$-th element. Read a swipe's endpoint before
modifying the array; queries must leave the array unchanged.

Time complexity: $O(QN\log N)$  
Space complexity: $O(N+Q)$, including the returned answers

```cpp
class Solution {
public:
    vector<int> solve(int n, int q, vector<int> A,
                      vector<char> T, vector<int> L, vector<int> R, vector<int> K) {
        vector<int> answer;
        for (int i = 0; i < q; ++i) {
            int l = L[i] - 1, r = R[i];
            if (T[i] == 'K') {
                vector<int> part(A.begin() + l, A.begin() + r);
                sort(part.begin(), part.end());
                answer.push_back(part[K[i] - 1]);
            } else {
                int value = T[i] == 'L' ? A[l] : A[r - 1];
                fill(A.begin() + l, A.begin() + r, value);
            }
        }
        return answer;
    }
};
```

## Subtask 2

Use a lazy segment tree over positions. Each node stores its smallest
$10$ elements in sorted order, including duplicates, or all its elements
if the interval has fewer than $10$ positions.

Merge by taking the first $10$ elements of the children's sorted lists.
Any omitted element already has $10$ elements no larger than it in its
child, so it is unnecessary for the parent's first $10$ elements.

A range assignment replaces a node's list with copies of the assigned
value and sets a lazy tag. For a swipe, first query the chosen endpoint,
then assign its value to the interval. For a `K` operation, merge the
covering nodes' lists and return entry $k-1$. This is sufficient since
$k\le10$.

Time complexity: $O(10N+10Q\log N)$  
Space complexity: $O(10N+Q)$

```cpp
class Solution {
    struct Small { array<int, 10> value{}; int size = 0; };
    vector<Small> tree;
    vector<int> lazy;
    Small merge(const Small& a, const Small& b) {
        Small result;
        int i = 0, j = 0;
        while (result.size < 10 && (i < a.size || j < b.size)) {
            if (j == b.size || (i < a.size && a.value[i] <= b.value[j]))
                result.value[result.size++] = a.value[i++];
            else result.value[result.size++] = b.value[j++];
        }
        return result;
    }
    void assign(int p, int l, int r, int value) {
        lazy[p] = value;
        tree[p].size = min(10, r - l + 1);
        fill_n(tree[p].value.begin(), tree[p].size, value);
    }
    void push(int p, int l, int r) {
        if (!lazy[p] || l == r) return;
        int m = (l + r) / 2;
        assign(2*p, l, m, lazy[p]);
        assign(2*p+1, m+1, r, lazy[p]);
        lazy[p] = 0;
    }
    void build(int p, int l, int r, const vector<int>& A) {
        if (l == r) { assign(p, l, r, A[l - 1]); return; }
        int m = (l + r) / 2;
        build(2*p, l, m, A); build(2*p+1, m+1, r, A);
        tree[p] = merge(tree[2*p], tree[2*p+1]);
    }
    void update(int p, int l, int r, int a, int b, int value) {
        if (a <= l && r <= b) { assign(p, l, r, value); return; }
        push(p, l, r);
        int m = (l + r) / 2;
        if (a <= m) update(2*p, l, m, a, b, value);
        if (b > m) update(2*p+1, m+1, r, a, b, value);
        tree[p] = merge(tree[2*p], tree[2*p+1]);
    }
    Small query(int p, int l, int r, int a, int b) {
        if (a <= l && r <= b) return tree[p];
        push(p, l, r);
        int m = (l + r) / 2;
        if (b <= m) return query(2*p, l, m, a, b);
        if (a > m) return query(2*p+1, m+1, r, a, b);
        return merge(query(2*p, l, m, a, b), query(2*p+1, m+1, r, a, b));
    }
public:
    vector<int> solve(int n, int q, vector<int> A,
                      vector<char> T, vector<int> L, vector<int> R, vector<int> K) {
        tree.assign(4*n, {}); lazy.assign(4*n, 0);
        build(1, 1, n, A);
        vector<int> answer;
        for (int i = 0; i < q; ++i) {
            if (T[i] == 'K') answer.push_back(query(1,1,n,L[i],R[i]).value[K[i]-1]);
            else {
                int x = T[i] == 'L' ? L[i] : R[i];
                int value = query(1,1,n,x,x).value[0];
                update(1,1,n,L[i],R[i],value);
            }
        }
        return answer;
    }
};
```

## Subtask 3

Store constant segments in an ordered map. Represent a segment $[s,e]$
of value $v$ by a **weighted point** at position $s$, with value $v$ and
weight $e-s+1$. Maintain one position Fenwick tree for each value $1,\ldots,10$.

Before **every operation**, split at $l$ and $r+1$. Splitting $[s,e]$ at
$p$ transfers weight $e-p+1$ from $s$ to $p$, without changing any array
value. After splitting, $[l,r]$ is a union of complete segments, so the
sum of weights with starts in $[l,r]$ equals the interval's element count.

For a query, count each value using its Fenwick tree and scan values in
increasing order until the cumulative count reaches $k$. For a swipe,
read the chosen endpoint's value, remove the covered weighted points,
and insert one point at $l$ of weight $r-l+1$.

Each operation creates at most two segments by splitting, and each swipe
inserts one more. Thus at most $N+3Q$ segments can be removed in total.
Queries only refine the representation: they do not change values or
collapse their intervals. Adjacent equal segments need not be merged.

Time complexity: $O((N+10Q)\log(N+1))$  
Space complexity: $O(10N+Q)$

```cpp
class Solution {
public:
    vector<int> solve(int n, int q, vector<int> A,
                      vector<char> T, vector<int> L, vector<int> R, vector<int> K) {
        vector<vector<int>> bit(11, vector<int>(n + 1));
        auto add = [&](int value, int p, int delta) {
            for (; p <= n; p += p & -p) bit[value][p] += delta;
        };
        auto prefix = [&](int value, int p) {
            int total = 0;
            for (; p; p -= p & -p) total += bit[value][p];
            return total;
        };
        map<int, int> segments;
        for (int l = 1, r; l <= n; l = r + 1) {
            r = l;
            while (r < n && A[r] == A[l - 1]) ++r;
            segments[l] = A[l - 1];
            add(A[l - 1], l, r - l + 1);
        }
        segments[n + 1] = 0;
        auto split = [&](int p) {
            auto it = prev(segments.upper_bound(p));
            if (it->first == p) return it;
            int weight = next(it)->first - p;
            add(it->second, it->first, -weight);
            add(it->second, p, weight);
            return segments.emplace(p, it->second).first;
        };
        vector<int> answer;
        for (int i = 0; i < q; ++i) {
            auto begin = split(L[i]), end = split(R[i] + 1);
            if (T[i] == 'K') {
                int k = K[i];
                for (int value = 1; value <= 10; ++value) {
                    int count = prefix(value, R[i]) - prefix(value, L[i] - 1);
                    if (k <= count) { answer.push_back(value); break; }
                    k -= count;
                }
            } else {
                int value = T[i] == 'L' ? begin->second : prev(end)->second;
                for (auto it = begin; it != end; ++it)
                    add(it->second, it->first, -(next(it)->first - it->first));
                segments.erase(begin, end);
                segments[L[i]] = value;
                add(value, L[i], R[i] - L[i] + 1);
            }
        }
        return answer;
    }
};
```

## General Solution

Use the same weighted segments, but replace the separate Fenwick trees
with **offline divide-and-conquer on values** and one position Fenwick tree.
Swipes only copy existing values, so compress the initial distinct values
into sorted ranks.

### 1. Record events in operation order

An update event $(p,v,d)$ adds weight $d$ at position $p$ for value rank $v$.
Record initial segment weights, weight transfers caused by splits, and
removals/insertions caused by swipes. After splitting a query's boundaries,
record its $(l,r,k)$ and answer index.

Splits are required for queries too: otherwise a segment starting before
$l$ or extending beyond $r$ would be counted incorrectly. They change only
the representation, so later queries still see the same array values.
The segment-count argument from Subtask 3 gives $O(N+Q)$ total events.

### 2. Divide the answer range

A recursive call handles value ranks $[lo,hi]$ and its events in their
original relative order. At a leaf, all its queries have that value as
their answer. Otherwise, let $mid=(lo+hi)/2$ and scan the events:

- Apply an update to the Fenwick tree if its value is at most $mid$.
- For a query, let $c$ be the weight in $[l,r]$. If $k\le c$, send the query
  left. Otherwise send it right with rank $k-c$.
- Send each update to the half containing its value.

Undo the applied updates, stably partition the events, and recurse on the
two halves. **Preserving order** ensures that each query sees exactly its
preceding updates. **Rollback** ensures that each recursive call starts
with an empty Fenwick tree. Calls containing no queries can be skipped.

### 3. Correctness and complexity

At each query event, its boundaries are segment boundaries, so weighted
point counts equal element counts. During a recursive scan, the Fenwick
tree contains exactly the weights for values in the lower half at that
time. Comparing $k$ with their count therefore chooses the correct half;
subtracting that count gives the correct rank within the upper half.
Induction down the recursion proves every answer is correct.

Each event appears in one recursive call per level. There are
$O(\log(N+1))$ levels, and each Fenwick operation costs $O(\log(N+1))$.
The implementation reuses one partition buffer and direction array,
avoiding a separate allocation at every recursive call.

Time complexity: $O((N+Q)\log^2(N+1))$  
Space complexity: $O(N+Q)$

```cpp
class Solution {
    // Update: (position, value rank, weight change, -1).
    // Query:  (left, right, remaining rank, answer index).
    struct Event { int a, b, k, id; };
    int n;
    vector<int> bit, values, answer;
    vector<Event> events, buffer;
    vector<unsigned char> goes_left;

    void add(int p, int delta) {
        for (; p <= n; p += p & -p) bit[p] += delta;
    }
    int prefix(int p) const {
        int total = 0;
        for (; p; p -= p & -p) total += bit[p];
        return total;
    }
    void divide(int low, int high, int begin, int end, int queries) {
        if (!queries) return;
        if (low == high) {
            for (int i = begin; i < end; ++i)
                if (events[i].id >= 0) answer[events[i].id] = values[low];
            return;
        }
        int mid = (low + high) / 2, left_size = 0, left_queries = 0;
        for (int i = begin; i < end; ++i) {
            auto& e = events[i];
            bool left;
            if (e.id < 0) {
                left = e.b <= mid;
                if (left) add(e.a, e.k);
            } else {
                int count = prefix(e.b) - prefix(e.a - 1);
                left = e.k <= count;
                if (left) ++left_queries;
                else e.k -= count;
            }
            goes_left[i] = left;
            left_size += left;
        }
        // Restore an empty Fenwick tree before either recursive call.
        for (int i = begin; i < end; ++i)
            if (goes_left[i] && events[i].id < 0) add(events[i].a, -events[i].k);
        int middle = begin + left_size, l = begin, r = middle;
        for (int i = begin; i < end; ++i)
            buffer[goes_left[i] ? l++ : r++] = events[i];
        copy(buffer.begin() + begin, buffer.begin() + end, events.begin() + begin);
        divide(low, mid, begin, middle, left_queries);
        divide(mid + 1, high, middle, end, queries - left_queries);
    }

public:
    vector<int> solve(int n_, int q, vector<int> A,
                      vector<char> T, vector<int> L, vector<int> R, vector<int> K) {
        n = n_;
        if (find(T.begin(), T.end(), 'K') == T.end()) return {};
        values = A;
        sort(values.begin(), values.end());
        values.erase(unique(values.begin(), values.end()), values.end());
        for (int& a : A) a = int(lower_bound(values.begin(), values.end(), a) - values.begin());
        events.clear();
        events.reserve(size_t(n) + 6ULL * q);
        map<int, int> segments; // Left endpoint -> value rank; next key ends the segment.
        for (int l = 1, r; l <= n; l = r + 1) {
            r = l;
            while (r < n && A[r] == A[l - 1]) ++r;
            segments[l] = A[l - 1];
            events.push_back({l, A[l - 1], r - l + 1, -1});
        }
        segments[n + 1] = -1;
        auto split = [&](int p) {
            auto it = prev(segments.upper_bound(p));
            if (it->first == p) return it;
            int weight = next(it)->first - p;
            events.push_back({it->first, it->second, -weight, -1});
            events.push_back({p, it->second, weight, -1});
            return segments.emplace(p, it->second).first;
        };
        int queries = 0;
        for (int i = 0; i < q; ++i) {
            auto begin = split(L[i]), end = split(R[i] + 1);
            if (T[i] == 'K') events.push_back({L[i], R[i], K[i], queries++});
            else {
                int value = T[i] == 'L' ? begin->second : prev(end)->second;
                for (auto it = begin; it != end; ++it)
                    events.push_back({it->first, it->second, -(next(it)->first - it->first), -1});
                segments.erase(begin, end);
                segments[L[i]] = value;
                events.push_back({L[i], value, R[i] - L[i] + 1, -1});
            }
        }
        answer.assign(queries, 0);
        bit.assign(n + 1, 0);
        buffer.resize(events.size());
        goes_left.resize(events.size());
        divide(0, int(values.size()) - 1, 0, int(events.size()), queries);
        return answer;
    }
};
```
