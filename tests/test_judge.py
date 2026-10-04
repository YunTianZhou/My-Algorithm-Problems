"""Integration checks exercising the real compiler and runner."""
from pathlib import Path
import os
import pty
import re
import subprocess
import sys
import tempfile
import unittest

JUDGE = Path(__file__).resolve().parents[1] / "judge.py"


class JudgeTest(unittest.TestCase):
    def run_judge(self, solution, tests, *args, terminal=False):
        with tempfile.TemporaryDirectory(prefix="judge fixture with spaces ") as directory:
            problem = Path(directory)
            (problem / "format.cpp").write_text(solution)
            (problem / "oracle.hpp").write_text("class Oracle { public: int solve() { return 7; } };")
            (problem / "test.cpp").write_text(tests)
            if terminal:
                master, slave = pty.openpty()
                try:
                    result = subprocess.run(
                        [sys.executable, str(JUDGE), str(problem), *args],
                        stdout=slave, stderr=slave, timeout=30)
                    os.close(slave)
                    slave = -1
                    output = b""
                    while True:
                        try:
                            chunk = os.read(master, 65536)
                        except OSError:
                            break
                        if not chunk:
                            break
                        output += chunk
                    result.stdout, result.stderr = output.decode(), ""
                    return result
                finally:
                    os.close(master)
                    if slave >= 0:
                        os.close(slave)
            return subprocess.run(
                [sys.executable, str(JUDGE), str(problem), *args],
                capture_output=True, text=True, timeout=30)

    def test_direct_include_main_mutable_args_and_fresh_instance(self):
        result = self.run_judge('''
            int helper() { return 7; }
            class Solution {
                int calls = 0;
            public:
                int solve(vector<int>& values) { values[0] += ++calls; return values[0]; }
            };
            int main() { return 99; }
        ''', '''
            int helper() { return 9; }
            void run_tests(judge::Runner& r, const judge::Options&) {
                vector<int> values{4};
                r.test("first", make_tuple(values), 5);
                r.test("second", make_tuple(values), 5);
            }
        ''')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("2 passed, 0 failed", result.stdout)
        self.assertIn("Final verdict: AC", result.stdout)
        self.assertNotIn("\033[", result.stdout)

    def test_custom_checker_and_options(self):
        result = self.run_judge('''
            class Solution { public: string solve(int) { return "different"; } };
        ''', '''
            void run_tests(judge::Runner& r, const judge::Options& o) {
                for (int i = 0; i < o.cases; ++i)
                    r.test("seed=" + to_string(o.seed), make_tuple(i), string("reference"),
                           [](const string& got, const string&) { return got == "different"; });
            }
        ''', "--cases", "3", "--seed", "42")
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("3 passed, 0 failed", result.stdout)
        self.assertIn("seed=42", result.stdout)

    def test_wrong_answer_and_exception(self):
        result = self.run_judge('''
            class Solution { public: int solve(int x) {
                if (x) throw runtime_error("fixture error");
                return 0;
            } };
        ''', '''
            void run_tests(judge::Runner& r, const judge::Options&) {
                r.section("wrong answer");
                r.test("wrong", make_tuple(0), 1);
                r.section("exception");
                r.test("throws", make_tuple(1), 1);
                r.section("passing");
                r.test("continues", make_tuple(0), 0);
            }
        ''')
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn("| WA |", result.stdout)
        self.assertIn("| RE | fixture error", result.stdout)
        self.assertIn("1 passed, 2 failed", result.stdout)
        self.assertIn("Final verdict: RE", result.stdout)

    def test_compile_error(self):
        result = self.run_judge("not valid C++", "")
        self.assertEqual(result.returncode, 2)
        self.assertIn("Final verdict: CE", result.stdout)

    def test_timeout(self):
        result = self.run_judge('''
            class Solution { public: int solve() {
                std::this_thread::sleep_for(std::chrono::seconds(5)); return 0;
            } };
        ''', '''
            void run_tests(judge::Runner& r, const judge::Options&) {
                r.test("slow", make_tuple(), 0);
            }
        ''', "--timeout", "1")
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn("Time limit exceeded", result.stderr)
        self.assertIn("Final verdict: TLE", result.stdout)
        self.assertIn("Total runtime: ---", result.stdout)
        self.assertIn("Max single-case runtime: ---", result.stdout)

    def test_invalid_options(self):
        result = self.run_judge("", "", "--cases", "0")
        self.assertEqual(result.returncode, 2)
        self.assertIn("must be positive", result.stderr)

    def test_empty_suite_is_not_a_pass(self):
        result = self.run_judge(
            "class Solution { public: int solve() { return 0; } };",
            "void run_tests(judge::Runner&, const judge::Options&) {}")
        self.assertEqual(result.returncode, 2, result.stdout + result.stderr)
        self.assertIn("no test cases", result.stderr)
        self.assertIn("Final verdict: JE", result.stdout)

    def test_wrong_answer_verdict(self):
        result = self.run_judge(
            "class Solution { public: int solve() { return 0; } };",
            'void run_tests(judge::Runner& r, const judge::Options&) { r.test("wrong", make_tuple(), 1); }',
            "--color", "never")
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn("Final verdict: WA", result.stdout)
        self.assertIn("expected: 1", result.stdout)
        self.assertIn("received: 0", result.stdout)
        self.assertNotIn("\033[", result.stdout)

    def test_oracle_header_and_forced_color(self):
        result = self.run_judge("unused template", '''
            void run_tests(judge::Runner& r, const judge::Options&) {
                r.test("reference", make_tuple(), reference::Oracle().solve());
            }
        ''', "--oracle", "--color", "always")
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("\033[1;32m", result.stdout)
        self.assertIn("Final verdict: AC", result.stdout)

    def test_checker_exception_is_judge_error(self):
        result = self.run_judge(
            "class Solution { public: int solve() { return 0; } };", '''
            void run_tests(judge::Runner& r, const judge::Options&) {
                r.test("checker", make_tuple(), 0, [](int, int) -> bool {
                    throw runtime_error("broken checker");
                });
            }
        ''')
        self.assertEqual(result.returncode, 2, result.stdout + result.stderr)
        self.assertIn("Final verdict: JE", result.stdout)
        self.assertIn("broken checker", result.stderr)

    def test_signal_has_runtime_verdict(self):
        result = self.run_judge('''
            class Solution { public: int solve() { raise(SIGTERM); return 0; } };
        ''', '''
            void run_tests(judge::Runner& r, const judge::Options&) {
                r.test("crash", make_tuple(), 0);
            }
        ''')
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn("Final verdict: RE", result.stdout)

    def test_subtask_skips_after_failure_and_resets(self):
        result = self.run_judge('''
            class Solution { public: int solve(int x) {
                if (x == 99) throw runtime_error("must not execute");
                return x;
            } };
        ''', '''
            void run_tests(judge::Runner& r, const judge::Options&) {
                r.section("first", 3);
                r.test("wrong", make_tuple(0), 1);
                r.test("skipped-one", make_tuple(99), 99);
                r.test("skipped-two", make_tuple(99), 99);
                r.section("second", 1);
                r.test("next-subtask", make_tuple(5), 5);
            }
        ''')
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn("Final verdict: WA", result.stdout)
        self.assertIn("1 passed, 1 failed", result.stdout)
        self.assertIn("Skipped 2", result.stdout)
        self.assertNotIn("skipped-one", result.stdout)
        self.assertNotIn("must not execute", result.stdout)

    def test_case_timeout_and_crash_continue_next_subtask(self):
        result = self.run_judge('''
            class Solution { public: int solve(int x) {
                if (x == 1) for (;;) std::this_thread::sleep_for(std::chrono::seconds(1));
                if (x == 2) raise(SIGTERM);
                return x;
            } };
        ''', '''
            void run_tests(judge::Runner& r, const judge::Options&) {
                r.set_time_limit(chrono::milliseconds(100));
                r.section("timeout", 4);
                if (r.test("infinite", make_tuple(1), 1))
                    throw runtime_error("timeout must return false");
                r.section("crash", 2);
                r.test("signal", make_tuple(2), 2);
                r.section("healthy", 1);
                r.test("after-failures", make_tuple(0), 0);
            }
        ''')
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn("1 passed, 2 failed", result.stdout)
        self.assertIn("TLE 1", result.stdout)
        self.assertIn("RE 1", result.stdout)
        self.assertIn("Skipped 4", result.stdout)
        self.assertIn("exceeded 100 ms", result.stdout)

    def test_case_timeout_final_verdict(self):
        result = self.run_judge('''
            class Solution { public: int solve(int x) {
                if (x) this_thread::sleep_for(chrono::seconds(5));
                return x;
            } };
        ''', '''
            void run_tests(judge::Runner& r, const judge::Options&) {
                r.set_time_limit(chrono::milliseconds(100));
                r.section("slow", 1);
                r.test("sleep", make_tuple(1), 1);
                r.section("fast", 1);
                r.test("fast", make_tuple(0), 0);
            }
        ''')
        self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
        self.assertIn("Final verdict: TLE", result.stdout)
        self.assertIn("1 passed, 1 failed", result.stdout)
        self.assertIn("| TLE | ---", result.stdout)
        self.assertIn("Total runtime: ---", result.stdout)
        self.assertIn("Max single-case runtime: ---", result.stdout)

    def test_runtime_excludes_preparation_and_checker(self):
        result = self.run_judge('''
            class Solution { public: int solve(int delay) {
                this_thread::sleep_for(chrono::milliseconds(delay)); return delay;
            } };
        ''', '''
            void run_tests(judge::Runner& r, const judge::Options&) {
                r.preparing(0, 2);
                this_thread::sleep_for(chrono::milliseconds(400));
                r.preparing(1, 2);
                this_thread::sleep_for(chrono::milliseconds(400));
                r.preparing(2, 2);
                r.test("first", make_tuple(20), 20, [](int got, int expected) {
                    this_thread::sleep_for(chrono::milliseconds(400));
                    return got == expected;
                });
                r.test("second", make_tuple(50), 50);
            }
        ''')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        times = [float(n) for n in re.findall(r"\| AC \| ([\d.]+) ms", result.stdout)]
        total = float(re.search(r"Total runtime: ([\d.]+) ms", result.stdout)[1])
        maximum = float(re.search(r"Max single-case runtime: ([\d.]+) ms", result.stdout)[1])
        self.assertEqual(len(times), 2)
        self.assertAlmostEqual(total, sum(times), delta=0.003)
        self.assertAlmostEqual(maximum, max(times), delta=0.002)
        self.assertGreaterEqual(total, 70)
        self.assertLess(total, 600)
        self.assertEqual(result.stdout.count("Preparing cases"), 2)
        self.assertNotIn("\r", result.stdout)

    def test_terminal_preparation_redraws_progress(self):
        result = self.run_judge(
            'class Solution { public: int solve() { return 0; } };', '''
            void run_tests(judge::Runner& r, const judge::Options&) {
                r.preparing(0, 2);
                this_thread::sleep_for(chrono::milliseconds(60));
                r.preparing(1, 2);
                r.preparing(2, 2);
                r.test("sample", make_tuple(), 0);
            }
        ''', "--color", "never", terminal=True)
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertIn("\r  Preparing cases", result.stdout)
        self.assertIn("1/2 (50%)", result.stdout)
        self.assertIn("2/2 (100%)", result.stdout)


if __name__ == "__main__":
    unittest.main()
