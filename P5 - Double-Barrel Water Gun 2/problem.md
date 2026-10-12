# P0005 Double-Barrel Water Gun 2

**Time limit: 1000 ms per test case.**

You and Bob are playing a game called "Double-Barreled Water Gun" on an infinitely long number line. But this time, you  are responsible for shooting, while Bob is responsible for dodging.

The game consists of $N$ turns. Initially, Bob  are at position $0$.  In each turn, he **must** move 1 unit of distance either to the left or to the right; subsequently, you can fires streams of water at any two positions you want.

In the previous game, you were responsible for dodging and discovered an optimal strategy that helped you win. Consequently, in this game, Bob will use that same strategy against you.

Before the game begins, you **must inform Bob which two positions you will target in each of the next $N$ turns**. Bob will then choose a movement path based on your shooting plan to **minimize the number of total times he gets hit**.

Can you find a shooting plan for the following $N$ turns that **maximizes the total number of times Bob gets hit when he plays optimally**?

## Problem Description

Bob is on an infinitely long number line. Initially, he is at position $0$.

You need to construct a shooting plan $A_1, A_2, ..., A_N$, and $B_1, B_2, ..., B_N$, where $A_i$ and $B_i$ are the positions you will shoot at during the $i$th turn. **Bob will know your shooting plan before the game starts.**

In the $i$th of the following $N$ turns:

- Bob can move to the left or to the right by 1 unit of distance.
- You shoot at positions $A_i$ and $B_i$. If **at least one of the positions is Bob's current position**, he will get hit in this turn.

Find a shooting plan that **maximizes** number of turns Bob gets hit, when he is trying **minimizing** the total number of times he gets hit.


Precisely, for all valid shooting plans $A_1, A_2, ..., A_N$, $B_1, B_2, ..., B_N$ cosist of integers, find the maximum value of $H$, where its value is determined as follows:

- Among all Bob's possible movement paths $P$ consist of integers that satisfies the following conditions:

  - $P_0 = 0$

  - For all $1 \le i \le n$, $|P_i - P_{i-1}| = 1$

- $H$ is the minimum possible $\sum_{i=1}^{n}{I_{hit}(P_i)}$, where $I_{hit}(P_i)$ is $1$ if $P_i = A_i \lor P_i = B_i$, and $0$ otherwise.

## Constraints

- $1 \le N \le 10^6$


## Input Specification

```
N
```

## Output Specifications

Output a single non-negative integer represents the **maximum total number of times you can hit Bob when he plays optimally**.

For each of the  following $N$ lines, output the to two positions you will shoot at each turn.

 Specifically, the $i$th line should contain two integers $A_i$ and $B_i$ separated by a space:

```
A_i B_i
```

If there are multiple optimal shooting plans, you can output **any one of them**.

## Example 1

### Input

```
1
```

### Output

```
1
-1 1
```

### Explanation

Regardless of whether Bob moves left or right in the first turn, he will be hit at position $-1$ or $1$.

Since the game only consists a single turn, $1$ is the maximum number of hits you can achieve.

