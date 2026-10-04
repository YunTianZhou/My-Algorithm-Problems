#include <bits/stdc++.h>
using namespace std;

int getMaxScore(int n, const vector<string>& S) {
    int i = n - 1;
    int score = 0;

    if (S[i] == "Tim") {
        score--;
        while (i > 0 && S[i] == "Tim") i--;
        i--;
        while (i > 0 && S[i] == "Tim") i--;
    }

    for (; i > 0; --i)
        if (S[i] == "Scifish" && S[i - 1] == "Tim")
            score++;

    return score;
}

class Oracle {
public:
    string solve(int n, int k, const vector<string>& messages) {
        return getMaxScore(n, messages) >= k ? "Yes" : "No";
    }
};
