# P0006 Swipe

**Time limit: 4000 ms per test case.**

Given an array of positive integers $A_1, A_2, \ldots, A_N$, process $Q$ operations in order. Each operation is one of the following:

- `L l r`: **Swipe left.** Change every element in $A_l, A_{l+1}, \ldots, A_r$ to the value of $A_l$ immediately before this operation.
- `R l r`: **Swipe right.** Change every element in $A_l, A_{l+1}, \ldots, A_r$ to the value of $A_r$ immediately before this operation.
- `K l r k`: Output the **$k$-th smallest element** in the current subarray $A_l, A_{l+1}, \ldots, A_r$. Equal values are counted separately.

**All swipe changes are permanent. A `K` operation does not change the array.**


## Constraints

- $1 \le N, Q \le 2 \cdot 10^5$
- $1 \le A_i \le 10^9$ for all $1 \le i \le N$
- Each operation's type is `L`, `R`, or `K`.
- $1 \le l \le r \le N$ for every operation.
- $1 \le k \le r-l+1$ for every `K` operation.

### Subtasks

| Subtask | Constraints |
|---------|--------------------------|
| 1 | $N, Q \le 3000$ |
| 2 | $k \le 10$ for every `K` operation |
| 3 | $A_i \le 10$ for all $1 \le i \le N$ |
| 4 | No additional constraints |

## Input Specification

```
N
A_1 A_2 ... A_N
Q
operation_1
operation_2
...
operation_Q
```

Each operation is given in one of these forms:

```
L l r
R l r
K l r k
```

All indices are 1-based. Swipe operations have no $k$ parameter.

## Output Specifications

Output a single line containing the answers to all `K` operations, in their input order, separated by spaces. Swipe operations produce no output.

If there are no `K` operations, output an empty line.

## Example 1

### Input

```
5
4 1 5 2 3
6
K 1 5 3
L 2 4
K 1 5 3
R 1 5
K 2 4 2
K 1 1 1
```

### Output

```
3 1 3 3
```

### Explanation

- Initially, sorting the whole array gives $[1,2,3,4,5]$, so the first answer is $3$.
- `L 2 4` copies the current $A_2=1$, making the array $[4,1,1,1,3]$. Its third smallest element is now $1$.
- `R 1 5` copies the current $A_5=3$, making every element $3$. Both remaining answers are $3$.

## Example 2

### Input

```
6
2 8 2 5 8 1
6
R 2 4
K 1 6 4
L 1 3
K 2 5 3
R 3 6
K 1 6 5
```

### Output

```
5 5 2
```

### Explanation

- After `R 2 4`, the array is $[2,5,5,5,8,1]$. Its fourth smallest element is $5$.
- After `L 1 3`, the array is $[2,2,2,5,8,1]$. The subarray $A_2,\ldots,A_5$ is $[2,2,5,8]$, whose third smallest element is $5$.
- After `R 3 6`, the array is $[2,2,1,1,1,1]$. Its fifth smallest element is $2$.

## Example 3

### Input

```
4
3 1 3 2
4
K 1 4 1
K 1 4 3
K 2 3 2
K 1 4 4
```

### Output

```
1 3 3 3
```

### Explanation

There are no swipes, so the array stays unchanged throughout. The sorted whole array is $[1,2,3,3]$, while the sorted subarray $A_2,A_3$ is $[1,3]$. Repeated queries do not collapse or otherwise modify their intervals.
