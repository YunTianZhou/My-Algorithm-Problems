"""P4 oracle, generator, and real-judge regression checks."""
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


class ProblemFourTest(unittest.TestCase):
    def test_independent_dp_and_patterns(self):
        with tempfile.TemporaryDirectory(prefix="p4-validation-") as directory:
            binary = str(Path(directory) / "validate")
            compiled = subprocess.run(
                ["g++", "-std=c++17", "-O2", "-Wall", "-Wextra",
                 str(ROOT / "tests/p4_validation.cpp"), "-o", binary],
                capture_output=True, text=True, timeout=30)
            self.assertEqual(compiled.returncode, 0, compiled.stderr)
            result = subprocess.run([binary], capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn("independent quadratic DP", result.stdout)

    def test_real_judge_including_large_streamed_cases(self):
        result = subprocess.run(
            [sys.executable, str(ROOT / "judge.py"), "P4", "--oracle", "--cases", "1"],
            capture_output=True, text=True, timeout=60)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("Final verdict: AC", result.stdout)
        self.assertIn("N=2000000", result.stdout)
        self.assertNotIn("Preparing cases", result.stdout)

    def test_pair_diagnostics_for_invalid_submission(self):
        with tempfile.TemporaryDirectory(prefix="p4-bad-solution-") as directory:
            solution = Path(directory) / "bad.cpp"
            solution.write_text('''class Solution { public:
                pair<int, string> solve(int, const vector<int>&, const vector<int>&) {
                    return {0, ""};
                }
            };''')
            result = subprocess.run(
                [sys.executable, str(ROOT / "judge.py"), "P4", "--cases", "1",
                 "--solution", str(solution)],
                capture_output=True, text=True, timeout=30)
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn("Final verdict: WA", result.stdout)
        self.assertIn("received: (0, <empty>)", result.stdout)
        self.assertIn("0 passed, 5 failed", result.stdout)


if __name__ == "__main__":
    unittest.main()
