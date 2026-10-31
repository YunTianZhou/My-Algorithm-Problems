#include <bits/stdc++.h>
using namespace std;

class Oracle {
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
