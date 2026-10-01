# P0001 Prooooooooooo!

Scifish and Tim are friends, they really like to argue about who is pro. Interestingly, they all firmly believe that the other is.

This is how their chat looks like:
```
scifish: prooooooooooooo
tim: u prooooooooooo
scifish: me noob
scifish: ur proooooooooooooo
...
```

Scifish has been trying to prove that Tim is the pro, but Tim says what he said is absurd. Scifish have to find **reliable evidence** to prove that Tim is pro. Thus, he decided to look for evidence from their chat history. 

## Problem Description

There are $N$ lines of message $S_1, S_2, ..., S_N$, every message is about saying the other is pro. 

- If $S_i$ is `Scifish`, it is sent by Scifish.
- If $S_i$ is `Tim`, it is sent by Tim.

Scifish need to divide these messages in to **consecutive** chunks, such that each chunk satisfied the following conditions:

-  Scifish has sent at least one message.
- Tim has sent at least one message.

Each chunk will receive a reliability score.

- `1` If the last message is sent by scifish
- `-1` If the last message is sent by Tim

The total reliability score is the sum of each chunk.

Tim will believe he is pro if Scifish found a valid division such that its total reliability score is greater than or equal to $K$.

Is it possible for Scifish to make Tim believe that he is pro？

## Constraints

- $2 \le N \le 67^4$
- $0 \le K \le N$
- $S_i$ is either `Scifish` or `Tim`. $(1 \le i \le N)$
- There exist at least one `Scifish` among $S_i$. $(1 \le i \le N)$
- There exist at least one `Tim` among $S_i$. $(1 \le i \le N)$

## Input Specification

```
N K
S_1
S_2
...
S_N
```

## Output Specifications

If it is possible to make Tim believe he is pro, output `Yes`, otherwise output `No`.

## Example 1

### Input

```
5 1
Scifish
Tim
Scifish
Tim
Scifish
```

### Output

```
Yes
```

### Explanation

Divide the message as following:

- $S_1,\ S_2,\ S_3$
- $S_4,\ S_5$

The total reliability score is $2$, which is greater than $1$. Therefore, output `Yes`.

## Example 2

### Input

```
7 0
Scifish
Scifish
Scifish
Tim
Scifish
Tim
Tim
```

### Output

```
No
```

### Explanation

No matter how Scifish divide the messages, the total reliability score is at most $-1$, which is less than $0$. Therefore, output `No`.
