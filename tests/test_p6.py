"""P6's separate updates/queries, editorial code, and stress tests."""
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
PROBLEM = ROOT / "P6 - Swipe"


class ProblemSixTest(unittest.TestCase):
    def test_editorial_against_brute(self):
        snippets = re.findall(r"```cpp\n(.*?)\n```", (PROBLEM / "solution.md").read_text(), re.S)
        self.assertEqual(len(snippets), 4)
        with tempfile.TemporaryDirectory(prefix="p6-validation-") as directory:
            header = Path(directory) / "editorial.hpp"
            header.write_text("\n".join(
                f"namespace editorial{i} {{\n{code}\n}}" for i, code in enumerate(snippets, 1)))
            binary = str(Path(directory) / "validate")
            compiled = subprocess.run(
                ["g++", "-std=c++17", "-O2", "-Wall", "-Wextra",
                 f'-DP6_EDITORIAL="{header}"', str(ROOT / "tests/p6_validation.cpp"), "-o", binary],
                capture_output=True, text=True, timeout=60)
            self.assertEqual(compiled.returncode, 0, compiled.stderr)
            result = subprocess.run([binary], capture_output=True, text=True, timeout=60)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn("all four editorial implementations", result.stdout)

    def test_full_size_judge(self):
        result = subprocess.run(
            [sys.executable, str(ROOT / "judge.py"), "P6", "--oracle", "--cases", "1"],
            capture_output=True, text=True, timeout=120)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("Final verdict: AC", result.stdout)
        self.assertIn("N=200000 Q=200000", result.stdout)
        self.assertNotIn("Preparing cases", result.stdout)

    def test_rejects_query_that_collapses_interval(self):
        with tempfile.TemporaryDirectory(prefix="p6-bad-solution-") as directory:
            solution = Path(directory) / "bad.cpp"
            solution.write_text('''class Solution { public:
                vector<int> solve(int, int q, vector<int> A, vector<char> T,
                                  vector<int> L, vector<int> R, vector<int> K) {
                    vector<int> result;
                    for (int i=0;i<q;++i) {
                        int l=L[i]-1,r=R[i];
                        int value=T[i]=='R'?A[r-1]:A[l];
                        if(T[i]=='K') {
                            vector<int> part(A.begin()+l,A.begin()+r);
                            sort(part.begin(),part.end());
                            result.push_back(part[K[i]-1]);
                        }
                        fill(A.begin()+l,A.begin()+r,value); // Deliberately wrong.
                    }
                    return result;
                }
            };''')
            result = subprocess.run(
                [sys.executable, str(ROOT / "judge.py"), "P6", "--cases", "1", "--solution", str(solution)],
                capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
            self.assertIn("Final verdict: WA", result.stdout)


if __name__ == "__main__":
    unittest.main()
