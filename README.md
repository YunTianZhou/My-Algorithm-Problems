# Algorithm problems

Each problem has the same structure:

```text
P# - XXX/
├── problem.md    # Statement
├── solution.md   # Explanation
├── format.cpp    # Your Solution class / submission template
├── oracle.cpp    # Reference implementation (oracle.hpp also supported)
└── test.cpp      # Cases, generators, and answer validation
```

Edit `format.cpp`, then run the shared judge:

```sh
python3 judge.py P3
python3 judge.py P3 --cases 10 --seed 42
python3 judge.py P3 --solution /path/to/my_submission.cpp
python3 judge.py P3 --oracle
```

Requires
Linux/macOS (POSIX processes), Python 3, and a C++17 compiler with `<bits/stdc++.h>` (g++ by default; set `CXX`
to choose another compiler). Builds use temporary directories and are cleaned
up automatically. IDs are case insensitive; a directory path also works.
`--solution` paths are relative to your current directory.

The judge directly calls `Solution::solve`; a submission's optional `main()`
is unused. A fresh instance and private arguments are used for each test.
`--oracle` runs `Oracle::solve` instead, as a harness smoke test. It compares
the oracle against itself and is not an independent correctness check.
The reference algorithms have been moved out of the test generators.

## Results

Each case shows its number, description, colored verdict, and execution time.
Wrong answers include expected/received values. The summary gives verdict and
skipped counts, **total runtime**, and **max single-case runtime**, followed by
a prominent **final verdict**. Runtime totals sum solution execution times;
case generation, reference answers, checking, process startup, and reporting
are excluded. Skipped cases contribute nothing. If any executed case has no
completed measurement (such as TLE or RE), its timing and both aggregate
timings display `---` rather than a misleading partial total.

Preparing cases shows a live progress bar in terminals. Redirected logs get
only start/end lines. P2/P3 prepare a batch per subtask; P1 prepares cases one
at a time to avoid retaining its potentially very large inputs.

| Verdict | Meaning | Exit code |
|---------|---------|-----------|
| AC | All cases accepted | 0 |
| WA | At least one wrong answer | 1 |
| RE | Submission threw an exception or crashed | 1 |
| TLE | A case or the entire test run exceeded its time limit | 1 |
| CE | Compilation failed | 2 |
| JE | Setup, generator, or checker failed; or no cases ran | 2 |

Each problem statement specifies a **2000 ms per-case time limit**, passed to
`runner.set_time_limit(chrono::milliseconds(2000))` by its `test.cpp`. Keep these
values in sync when changing a problem's limit. Cases run in child processes;
the wall-clock limit covers process startup, solution execution, and checking,
but excludes test generation, reference-answer calculation, and argument preparation. Displayed
case timings measure solution execution only.

After the first non-AC result (WA, RE, or TLE), the remaining cases in that
subtask are skipped. The next subtask still runs. P1 treats all cases as one
subtask. The summary counts skipped cases separately. If multiple kinds of
failure occur, the final verdict priority is RE, then TLE, then WA. Judge errors
stop the run. Child processes let crashes and infinite loops fail one subtask
without terminating the entire suite.

Color is automatic in terminals. Redirected output is plain text. Use
`--color always` or `--color never` to override; auto mode respects `NO_COLOR`.
`--cases` sets random cases **per subtask** (default 100). Use the same
`--seed` (default 1) and case count to reproduce failures. `--timeout` limits
the entire run, including generation (default 120 seconds), but not compilation.
The whole-run timeout terminates the judge and its case processes. This is a
local test tool without sandboxing or memory limits.

## Adding a problem

Create the five files above. Keep your submission in `format.cpp` as a
`Solution` class; put the reference algorithm in `oracle.cpp` as an `Oracle`
class with the same `solve` signature. For a two-integer addition problem:

```cpp
// oracle.cpp
class Oracle {
public:
    int solve(int a, int b) { return a + b; }
};
```

```cpp
// test.cpp
void run_tests(judge::Runner& runner, const judge::Options& options) {
    runner.set_time_limit(std::chrono::milliseconds(2000)); // Match problem.md.
    runner.section("All cases", options.cases + 1);
    if (!runner.test("sample", std::make_tuple(2, 3), 5)) return;
    std::mt19937 rng(options.seed);
    std::uniform_int_distribution<int> value(-100, 100);
    runner.preparing(0, options.cases);
    for (int i = 0; i < options.cases; ++i) {
        int a = value(rng), b = value(rng);
        auto expected = reference::Oracle().solve(a, b);
        runner.preparing(i + 1, options.cases);
        if (!runner.test("random " + std::to_string(i), std::make_tuple(a, b),
                         expected)) break;
    }
}
```

Run `python3 judge.py "path/to/problem"`. Tuple elements are arguments to
`solve`, in order. The third argument is the expected answer. Answers must
support `operator<<` for diagnostics and, by default, `operator==`. Problems
accepting multiple answers can provide a checker as the fourth argument:

```cpp
runner.test("assignment", std::make_tuple(n, edges), expected,
            [&](const auto& got, const auto&) { return is_valid(got); });
```

`runner.section("Subtask 1", case_count)` starts a subtask and resets its failure
state. `runner.test(...)` returns false on failure; break the case loop to avoid
further generation. Subsequent test calls in that section are also ignored.
Pass the planned case count for accurate skipped totals. Checkers execute in
the case process, so changes to their captured state do not reach the parent.
The default limit is 2000 ms, but each problem should set it explicitly.
Use `runner.preparing(completed, total)` before preparation and after each
case is ready to update the progress bar.

The shared wrapper includes the
oracle once in namespace `reference`, the submission in `submission`, and
tests in `problem`. Tests use `reference::Oracle` without including the oracle
again. Standard headers are preloaded outside these namespaces.

## Testing the judge

```sh
python3 -m unittest discover -s tests -v
```
