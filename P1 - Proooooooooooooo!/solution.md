# Solution

Count the transitions from `Tim` to `Scifish`: each can end a chunk worth +1.
If the history ends in `Tim`, the last chunk must instead score -1. Skip that
trailing Tim run, the preceding Scifish message, and its preceding Tim run
before counting the remaining transitions. The skipped transition cannot form
an additional positive chunk because the final chunk needs both speakers.

Finally, compare the maximum score with K.

Time complexity: $O(N)$

```cpp
class Solution {
public:
    string solve(int n, int k, const vector<string>& messages) {
        int i = n - 1;
        int score = 0;

        if (messages[i] == "Tim") {
            score--;
            while (i > 0 && S[i] == "Tim") i--;
            i--;
            while (i > 0 && S[i] == "Tim") i--;
        }

        for (; i > 0; --i)
            if (messages[i] == "Scifish" && S[i - 1] == "Tim")
                score++;
        
        return score >= k ? "Yes" : "No";
    }
};
```
