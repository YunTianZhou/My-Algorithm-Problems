# P0002 Who is pro

Scifish and Tim have always argued about who is the true pro, but they could never reach an agreement. One day, Scifish decided to interview other people on the street to gather opinions.

Scifsh interviewed some people on the street. Some of them are Scifsh’s friends, some are Tim’s friends, and some he doesn’t know at all. In order to avoid offending anyone, each interviewee chose to compare only themselves with their best friends, and gave some opinions in the following form:

> If I am `<pro|noob>`, then `<friend_name>` is `<pro|noob>`.

For example:

```
Alice: If I am noob, then Scifish is pro.
```

It is guaranteed that **every person mentioned in the opinions was also interviewed**.

Each person is either a **pro** or a **noob**.

Determine whether it is possible that all of the following conditions hold simultaneously:
- Scifish is a noob.
- Tim is a pro.
- All the opinions are true.

If it is possible, output a valid assignment labeling every person as either **pro** or **noob**.

## Problem Description

There are $N$ people numbered from $1$ to $N$, person $1$ is Scifish, and person $N$ is Tim.

There are $M$ opinions, the i-th opinion has the form:

> If person $A_i$ is $X_i$, then person $B_i$ is $Y_i$

Where $X_i$ and $Y_i$ are either `pro` or `noob`.

## Constraints

- $3 \le N \le 67^3$
- $0 \le M \le 676767$
- $1 < A_i < N$ $(1 \le i \le M)$
- $1 < B_i < N$ $(1 \le i \le M)$
- $A_i \ne B_i$ $(1 \le i \le M)$
- $X_i$ is either `pro` or `noob`. $(1 \le i \le M)$
- $Y_i$ is either `pro` or `noob`. $(1 \le i \le M)$

### Subtasks

| Subtask | Constraints / Properties |
|---------|--------------------------|
| 1 | $N \le 20,\ M \le 50$ |
| 2 | For each $1 < j < N$, there exists **exactly** one $i$ such that $A_i = j$ |
| 3 | For each $1 \le i \le M$, there must exist at least one $1 \le j \le M$, such that:<ul><li>$A_i = A_j$</li><li>$B_i = B_j$</li><li>$X_i \ne X_j$</li><li>$Y_i \ne Y_j$</li></ul> |
| 4 | $N \le 3000,\ M \le 3000$ |
| 5 | No additional constraints |

## Input Specification

```
N M
A_1 X_1 B_1 Y_1
A_2 X_2 B_2 Y_2
...
A_M X_M B_M Y_M
```

## Output Specifications

If it is impossible, output:

```
Impossible
```

Otherwise, output:

```
Possible
```

followed by N lines, where the i-th line is either:

```
pro
```

or

```
noob
```

representing the role of person $i$.

Any valid solution is accepted.

## Example 1

### Input

```
3 1
2 pro 3 noob
```

### Output

```
Possible
noob
noob
pro
```

### Explanation

Note that person $2$ cannot be pro, otherwise according to the first opinion, person $1$ (Tim) must be noob, which doesn't met the question's condition.

## Example 2

### Input

```
3 2
2 noob 1 pro
2 pro 3 noob
```

### Output

```
Impossible
```

### Explanation

- If person $2$ is noob, person $1$ (Scifish) must be pro, which violates the problem's condition.

- If person $2$ is pro, person $3$ (Tim) must be noob, which violates the problem's condition.

Therefore, person $2$ can neither be a pro or noob, which is impossible.

## Example 3

### Input

```
5 3
2 noob 3 pro
3 noob 5 pro
2 pro 5 noob
4 pro 3 pro
```

### Output

```
Possible
noob
noob
pro
noob
pro
```

### Explanation

Note that person $4$ can also be a pro.

## Example 4

### Input

```
4 3
2 noob 3 pro
2 pro 3 noob
3 noob 2 noob
3 pro 2 pro
```

### Output

```
Impossible
```

### Explanation

No matter how to label person $2$ and person $3$, there will always be contradiction between opinions.
