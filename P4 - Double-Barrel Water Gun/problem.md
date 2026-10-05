# P0004 Double-Barrel Water Gun

**Time limit: 1000 ms per test case.**

You and Bob are playing a game called "Double-Barreled Water Gun" on an infinitely long number line. In this round, Bob is responsible for shooting, while you are responsible for dodging.

The game consists of $N$ turns. Initially, you are at position $0$.  In each turn, you **must** move 1 unit of distance either to the left or to the right; subsequently, Bob fires streams of water at any two positions of his choice.

Clearly, this game is extremely unfair to you because Bob always knows your location and can hit you every time. Therefore, to make the game more interesting, Bob **explicitly tells you which two positions he will shoot at during the upcoming $N$ turns**.

Can you find  a moving path for the following $N$ turns that **minimizes the number times you get hit**?

## Problem Description

You are on an infinitely long number line. Initially, you are at position $0$.

Bob will show you his shooting plan $A_1, A_2, ..., A_N$, and $B_1, B_2, ..., B_N$, where $A_i$ and $B_i$ are the positions Bob will shoot at during the $i$th turn.

In the $i$th of the following $N$ turns:

- You move to the left or to the right by 1 unit of distance.
- Bob shoots at positions $A_i$ and $B_i$, if **at least one of the positions is your current position**, you will get hit in this turn.

Find the **minimum** number of turns you get hit.


Precisely, for all paths $P$ consist of integers that satisfies the following conditions:

- $P_0 = 0$
- For all $1 \le i \le n$, $|P_i - P_{i-1}| = 1$


Find the minimum value of $\sum_{i=1}^{n}{I_{hit}(P_i)}$, where $I_{hit}(P_i)$ is $1$ if $P_i = A_i \lor P_i = B_i$, and $0$ otherwise.

## Constraints

- $1 \le N \le 2 * 10^6$
- $-10^9 < A_i, B_i < 10^9$ $(1 \le i \le N)$

### Subtasks

| Subtask | Constraints |
|---------|--------------------------|
| 1 | For all $1 \le i \le N$, $A_i = B_i$ |
| 2 | For all $1 \le i \le N$, $A_i + 1 = B_i$ |
| 3 | $N \le 2000$ |
| 4 | $N \le 10^5$ |
| 5 | No additional constraints |

## Input Specification

```
N
A_1 B_1
A_2 B_2
...
A_N B_N
```

## Output Specifications

Output a single non-negative integer represents the minimum number of times you get hit.

In the next line, output a string $P$ of length $N$ representing the path you take to achieve the minimum number of hits:

- If you choose to move to the left in the $i$th turn, $P_i$ is `L`.
- If you choose to move to the right in the $i$th turn, $P_i$ is `R`.

If there are multiple solutions, you can output **any one of them**.

## Example 1

### Input

```
3
-1 2
-2 2
-1 1
```

### Output

```
1
RLL
```

### Explanation

No matter what path you choose, you will get it at least once:

- In the first turn, you must move right from $0 \rightarrow 1$ to avoid getting hit at $-1$.
- In the second turn, you must move left from $1 \rightarrow 0$ to avoid getting hit at $2$.
- In the third turn, no matter which direction you move, you will get hit at either $-1$ or $1$.

Therefore, the minimum number of times you get hit is $1$.

Notice that the path `RLL` is just one of the possible solutions. There are other paths such as `RRR`, `LLL`, and `RLR`. They also achieve the minimum number of hits.

## Example 2

### Input

```
5
1 1
-2 -2
-1 -1
0 0
3 3
```

### Output

```
0
LRRRL
```

### Explanation

In this case only one path can achieve the minimum number of hits.

## Example 3

### Input

```
3
-1 1
-2 2
-1 1
```

### Output

```
2
LRL
```

### Explanation

It is shown that no matter what path you choose, you will get hit at least twice.
