"""P5 maximin construction and special-checker regressions."""
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
PROBLEM = ROOT / "P5 - Double-Barrel Water Gun 2"


class ProblemFiveTest(unittest.TestCase):
    def test_exhaustive_plans_and_checker(self):
        with tempfile.TemporaryDirectory(prefix="p5-validation-") as directory:
            binary = str(Path(directory) / "validate")
            compiled = subprocess.run(
                ["g++", "-std=c++17", "-O2", "-Wall", "-Wextra",
                 str(ROOT / "tests/p5_validation.cpp"), "-o", binary],
                capture_output=True, text=True, timeout=30)
            self.assertEqual(compiled.returncode, 0, compiled.stderr)
            result = subprocess.run([binary], capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn("exhaustive shooting plans", result.stdout)

    def test_editorial_constructions(self):
        snippets = re.findall(r"```cpp\n(.*?)\n```", (PROBLEM / "solution.md").read_text(), re.S)
        self.assertEqual(len(snippets), 2)
        for index, source in enumerate(snippets):
            with self.subTest(construction=index), tempfile.TemporaryDirectory(prefix="p5-solution-") as directory:
                solution = Path(directory) / "solution.cpp"
                solution.write_text(source)
                result = subprocess.run(
                    [sys.executable, str(ROOT / "judge.py"), "P5", "--cases", "1",
                     "--solution", str(solution)],
                    capture_output=True, text=True, timeout=60)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertIn("Final verdict: AC", result.stdout)
                self.assertIn("N=1000000", result.stdout)
                self.assertNotIn("Preparing cases", result.stdout)

    def test_tuple_diagnostics(self):
        with tempfile.TemporaryDirectory(prefix="p5-bad-solution-") as directory:
            solution = Path(directory) / "bad.cpp"
            solution.write_text('''class Solution { public:
                tuple<int, vector<int>, vector<int>> solve(int) { return {0, {}, {}}; }
            };''')
            result = subprocess.run(
                [sys.executable, str(ROOT / "judge.py"), "P5", "--cases", "1",
                 "--solution", str(solution)],
                capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
            self.assertIn("Final verdict: WA", result.stdout)
            self.assertIn("received: (0, [], [])", result.stdout)


if __name__ == "__main__":
    unittest.main()
