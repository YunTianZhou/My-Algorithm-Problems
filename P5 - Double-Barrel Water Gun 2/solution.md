## Observation

The maximum guaranteed number of hits is

$$
\boxed{\left\lfloor\log_2(N+1)\right\rfloor}.
$$

We first prove that Bob can always achieve at most this many hits, regardless
of the announced shooting plan. Then we give two fixed constructions that
force this bound: squeezing from both sides, and the one-sided sweep used
by the oracle. Neither construction depends on Bob's actual moves.

## Upper Bound

Fix any shooting plan. Let $d_t(k)$ be the minimum number of hits Bob needs
to reach turn $t$ having made exactly $k$ right moves. His position is
$2k-t$, and $0\le k\le t$.

Assign this state the weight

$$
w_t(k)=2^{-d_t(k)},\qquad W_t=\sum_{k=0}^{t}w_t(k).
$$

Initially $d_0(0)=0$, so $W_0=1$. We will prove that **$W_t\ge W_{t-1}$**.

### 1. Movement creates at least one maximum state's worth of weight

Write $w_k=w_{t-1}(k)$ for $0\le k<t$, and set weights outside this range
to zero. Before this turn's shots, a new state $k$ comes from either a left
move from $k$ or a right move from $k-1$. Its minimum cost is the smaller
predecessor cost, so its weight is

$$
u_k=\max(w_k,w_{k-1}),\qquad 0\le k\le t.
$$

Choose an index $r$ attaining $M=\max_k w_k$. For $0\le k\le r$, use
$u_k\ge w_k$; for $r+1\le k\le t$, use $u_k\ge w_{k-1}$. Summing gives

$$
\begin{aligned}
\sum_{k=0}^{t}u_k
&\ge \sum_{k=0}^{r}w_k+\sum_{k=r+1}^{t}w_{k-1}\\
&=W_{t-1}+w_r\\
&=W_{t-1}+M.
\end{aligned}
$$

The maximum-weight old state is counted twice, corresponding to its two
possible next moves.

### 2. Two shots remove at most that much weight

A shot can affect at most one reachable state. At that state, the cost
increases by one, so its weight changes from $u_k$ to $u_k/2$.

Every $u_k\le M$, so one affected state loses at most $M/2$. There are at
most two distinct affected states. Duplicate shots count only once, and
shots outside the reachable range or on the wrong parity affect no state.
Consequently,

$$
W_t\ge\sum_k u_k-M\ge W_{t-1}.
$$

Induction yields $W_N\ge1$.

### 3. Convert the weight bound into a hit bound

Let $H=\min_k d_N(k)$ be Bob's optimal number of hits against this plan.
Every $d_N(k)\ge H$, so every final weight is at most $2^{-H}$. There are
exactly $N+1$ reachable final positions. Therefore,

$$
1\le W_N\le(N+1)2^{-H}.
$$

It follows that $2^H\le N+1$, and hence

$$
H\le\left\lfloor\log_2(N+1)\right\rfloor.
$$

This holds for every fixed shooting plan. In particular, no construction
can force a larger answer. The fact that there are only **two shots per
turn** is essential to the weight-loss bound.

## Construction 1: Squeeze from Both Sides

Divide the turns into phases

$$
[1,1],\ [2,3],\ [4,7],\ [8,15],\ldots.
$$

A phase starts at $L$, where $L$ is a power of two, and ends at $2L-1$.
During this phase, shoot at

$$
A_t=-(2L-t),\qquad B_t=2L-t.
$$

The shots start at $-L$ and $L$, then move inward by one position each
turn, ending at $-1$ and $1$.

### Why every completed phase forces a hit

Consider an arbitrary path. Immediately before the phase, at turn $L-1$,
Bob satisfies $|P_{L-1}|\le L-1$. Let $r_t=2L-t$ be the radius of the
shooting positions.

Suppose Bob avoids all shots in this phase. At its first turn,
$|P_L|\le L=r_L$. Avoiding both endpoints implies $|P_L|<r_L$.
Both $P_L$ and $r_L$ have the parity of $L$, so actually
$|P_L|\le r_L-2$.

Inductively, if $|P_{t-1}|\le r_{t-1}-2$, then after one move,

$$
|P_t|\le r_{t-1}-1=r_t.
$$

To avoid the two shots at $\pm r_t$, Bob must again satisfy
$|P_t|<r_t$, or equivalently $|P_t|\le r_t-2$ by parity. Thus he cannot
jump through either inward-moving boundary without being hit.

At the last turn $t=2L-1$, the radius is $1$. Avoidance would require
$|P_t|<1$, but $t$ is odd and Bob's position must be odd. This is impossible.
Every completed phase therefore contributes at least one hit, regardless
of how Bob behaved in previous phases.

### Number of completed phases

A phase is completed when $2L-1\le N$. The number of completed phases is
$\lfloor\log_2(N+1)\rfloor$. Their turn ranges are disjoint, so the hits
forced by different phases are distinct.

If the last phase is incomplete, continue the same construction for the
remaining turns. The completed phases already force the claimed answer,
and the upper bound proves that no additional hit can be unavoidable.

Time complexity: $O(N)$  
Space complexity: $O(N)$ for the returned arrays, with $O(1)$ auxiliary space

```cpp
class Solution {
public:
    tuple<int, vector<int>, vector<int>> solve(int n) {
        vector<int> A(n), B(n);
        int answer = 0, L = 1;
        for (int t = 1; t <= n; ++t) {
            if (t == 2 * L) L *= 2;
            int radius = 2 * L - t;
            A[t - 1] = -radius;
            B[t - 1] = radius;
            if (t == 2 * L - 1) ++answer;
        }
        return {answer, A, B};
    }
};
```

## Construction 2: The Oracle's Sweep

Use the same phases $[L,2L-1]$. Instead of squeezing from two sides, shoot
at two adjacent positions of the reachable parity, sweeping to the right:

$$
A_t=3t-4L,\qquad B_t=A_t+2.
$$

For example, the first three phases are:

| Phase | Shot pairs, in turn order |
|---|---|
| $[1,1]$ | $(-1,1)$ |
| $[2,3]$ | $(-2,0),\ (1,3)$ |
| $[4,7]$ | $(-4,-2),\ (-1,1),\ (2,4),\ (5,7)$ |

### Why every completed phase forces a hit

Let $t=L+s$, where $0\le s<L$, and let $k_t$ be Bob's number of right
moves through turn $t$. Since $P_t=2k_t-t$, the two shots correspond to

$$
k_t=\frac{A_t+t}{2}=2s
\quad\text{or}\quad
k_t=\frac{B_t+t}{2}=2s+1.
$$

The quantity $k_t$ never decreases: it stays unchanged after a left move
and increases by one after a right move.

Suppose Bob avoids all shots in this phase. At $s=0$, avoiding $k_L=0,1$
requires $k_L\ge2$. If he has avoided shots through $s-1$, induction gives
$k_{L+s-1}\ge2s$. Because $k$ cannot decrease, avoiding the next two
values $2s$ and $2s+1$ requires

$$
k_{L+s}\ge2s+2.
$$

At $s=L-1$, this would require $k_{2L-1}\ge2L$. But Bob has made only
$2L-1$ moves, so he cannot have made $2L$ right moves. This contradiction
proves that the phase forces a hit.

There are again $\lfloor\log_2(N+1)\rfloor$ completed phases. Combined
with the universal upper bound, this proves optimality of the oracle's
construction as well.

The oracle increments its phase count at each new power of two. If the
last phase has not reached $2L-1$, it subtracts that unfinished phase at
the end. This is equivalent to counting completed phases directly.

Time complexity: $O(N)$  
Space complexity: $O(N)$ for the returned arrays, with $O(1)$ auxiliary space

```cpp
class Solution {
public:
    tuple<int, vector<int>, vector<int>> solve(int n) {
        vector<int> A(n), B(n);
        int answer = 1, L = 1;
        for (int t = 1; t <= n; ++t) {
            if (t == 2 * L) {
                ++answer;
                L *= 2;
            }
            A[t - 1] = 3 * t - 4 * L;
            B[t - 1] = A[t - 1] + 2;
        }
        if (n + 1 < 2 * L) --answer;
        return {answer, A, B};
    }
};
```
