# P0003 Elevator

There is a skyscraper with infinite floors, both above and underground. Each floor is numbered using an integer. 

Floor $0$ is the ground floor, floors above it are positive, and floors under it are negative. Floor $i$ is **directly above** floor $i - 1$, and **directly below** floor $i + 1$.

Unfortunately, the skyscraper only has one elevator, and there are no stairs. The elevator is the only way for residents to move between floors.

Luckily, the elevator is extremely fast. It takes only $1$ millisecond to move between any two **adjacent** floors, and it takes negligible time for residents to enter or leave the elevator. The elevator is also very sturdy and can hold any number of residents at the same time.

The designer wants to program the elevator so that when there are $N$ residents waiting fot the elevator, and the $i$-th resident wants to move from floor $S_i$ to floor $T_i$, the elevator will choose the path that gets all residents to their destination **as fast as possible**.

However, due to his lack of programming skills, his program design has drew many complaints from the residents.

```cpp
for each i in shuffled(1...N):
	goto A[i]
	wait until resident i enters
	goto B[i]
	wait until resident i leaves
```

> The designer's program

Therefor, the designer finds you to help him design a program for the elevator, that can determine the **minimum total time** to take all residents to their destination.

## Problem Description

There are $N$ residents numbered from $1$ to $N$, resident $i$ wants to move from floor $A_i$ to floor $B_i$.

The elevator is initially on floor $0$. It takes $1$ millisecond to move between any two **adjacent** floors, and no time for residents to enter or leave the elevator.

Find the minimum time (in milliseconds) to get all residents to their destination.

Precisely, for all plans $P$ consist of integers that satisfies the following conditions:

- $P_1 = 0$
- For each $1 \le i \le N$, there exist $1 \le x \lt y \le |P|$, such that $A_i = P_x$ and $B_i = P_y$.

Find the minimum value of $\sum_{i=2}^{|P|} |P_i - P_{i - 1}|$

## Constraints

- $1 \le N \le 10^5$
- $-10^9 < A_i, B_i < 10^9$ $(1 \le i \le N)$

### Subtasks

| Subtask | Constraints |
|---------|--------------------------|
| 1 | For all $1 \le i \le N$, $A_i = B_i$ |
| 2 | $0 \le A_i, B_i \le 10^5$ |
| 3 | $0 \le A_i, B_i \le 10^9$ |
| 4 | $N \le 10$ |
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

Output a single non-negative integer represents the minimum time (in milliseconds) it takes to transport all residents.

## Example 1

### Input

```
4
1 2
3 4
4 1
2 5
```

### Output

```
9
```

### Explanation

The only optimum path is to first rise from floor $0$ to $5$, pick all $4$ residents and send residents $1$, $2$, and $4$ to their destination.
Then descend to send resident $3$ to floor $1$.

Path: $0 \rightarrow 1 \rightarrow 2 \rightarrow 3 \rightarrow 4 \rightarrow 5 \rightarrow 1$.

## Example 2

### Input

```
2
3 1
-5 -6
```

### Output

```
12
```

### Explanation

First go up to pick up resident $1$, then go down for resident $2$ and send resident $1$ by the way.

## Example 3

### Input

```
3
10 0
-10 0
100 90
```

### Output

```
150
```

### Explanation

Transport the first $2$ residents first, and resident $3$ lastly.
