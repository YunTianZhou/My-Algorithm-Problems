#!/usr/bin/env python3
"""Compile an existing Solution with a problem's tests and the shared runner."""
import argparse
import json
import os
from pathlib import Path
import shlex
import signal
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parent


def paint(text, code, color):
    return f"\033[{code}m{text}\033[0m" if color else text


def verdict(code, color):
    names = {"AC": "Accepted", "WA": "Wrong Answer", "RE": "Runtime Error",
             "TLE": "Time Limit Exceeded", "CE": "Compilation Error", "JE": "Judge Error"}
    colors = {"AC": "1;32", "WA": "1;31", "RE": "1;35", "TLE": "1;33",
              "CE": "1;31", "JE": "1;35"}
    print(paint(f"\nFinal verdict: {code} — {names[code]}", colors[code], color), flush=True)
    return 0 if code == "AC" else 2 if code in ("CE", "JE") else 1


class Parser(argparse.ArgumentParser):
    def error(self, message):
        self.print_usage(sys.stderr)
        print(f"{self.prog}: error: {message}", file=sys.stderr)
        verdict("JE", False)
        self.exit(2)


def positive(value):
    number = int(value)
    if number <= 0:
        raise argparse.ArgumentTypeError("must be positive")
    return number


def seed_value(value):
    number = int(value)
    if not 0 <= number <= 0xFFFFFFFF:
        raise argparse.ArgumentTypeError("must be between 0 and 4294967295")
    return number


def main():
    parser = Parser(description=__doc__)
    parser.add_argument("problem", help="problem ID (P3) or directory")
    target = parser.add_mutually_exclusive_group()
    target.add_argument("--solution", type=Path, help="submission file (default: format.cpp)")
    target.add_argument("--oracle", action="store_true", help="smoke-test the reference implementation")
    parser.add_argument("--color", choices=("auto", "always", "never"), default="auto",
                        help="color output (default: auto; respects NO_COLOR)")
    parser.add_argument("--cases", type=positive, default=100,
                        help="random cases per subtask (default: 100)")
    parser.add_argument("--seed", type=seed_value, default=1,
                        help="reproducible random seed (default: 1)")
    parser.add_argument("--timeout", type=positive, default=120,
                        help="timeout for the entire test run in seconds (default: 120)")
    args = parser.parse_args()
    color = args.color == "always" or (args.color == "auto" and sys.stdout.isatty()
                                      and "NO_COLOR" not in os.environ)

    problem = Path(args.problem).resolve()
    if not problem.is_dir():
        matches = [p for p in ROOT.iterdir() if p.is_dir()
                   and p.name.split(" - ")[0].lower() == args.problem.lower()]
        if len(matches) != 1:
            parser.error(f"unknown problem: {args.problem}")
        problem = matches[0]
    oracle = problem / "oracle.cpp"
    if not oracle.is_file():
        oracle = problem / "oracle.hpp"
    solution = oracle if args.oracle else (args.solution or problem / "format.cpp").resolve()
    tests = problem / "test.cpp"
    for path in (solution, tests, oracle):
        if not path.is_file():
            parser.error(f"missing file: {path}")

    # Separate namespaces prevent generator helpers from colliding with a
    # submission's globals. Preload standard headers outside those namespaces.
    # Renaming main allows the existing format.cpp submission style as well.
    submission = 'using Solution = reference::Oracle;' if args.oracle else f'''namespace submission {{
#define main submission_main
#include {json.dumps(str(solution))}
#undef main
}}
using submission::Solution;
'''
    source = f'''#include <bits/stdc++.h>
using namespace std;
namespace reference {{
#include {json.dumps(str(oracle))}
}}
{submission}
#include {json.dumps(str(ROOT / "judge/runner.hpp"))}
namespace problem {{
#include {json.dumps(str(tests))}
}}
int main() {{
    judge::Options options{{{args.cases}, {args.seed}u}};
    judge::Runner runner({str(color).lower()});
    try {{
        problem::run_tests(runner, options);
    }} catch (const std::exception& e) {{
        std::cerr << "Judge error: " << e.what() << '\\n';
        return 2;
    }} catch (...) {{
        std::cerr << "Judge error: unknown exception\\n";
        return 2;
    }}
    return runner.finish();
}}
'''
    print(paint(f"\n  JUDGE · {problem.name}", "1;36", color))
    print(f"  File: {solution.name}  |  Seed: {args.seed}  |  Cases/subtask: {args.cases}", flush=True)
    try:
        with tempfile.TemporaryDirectory(prefix="algorithm-judge-") as directory:
            build = Path(directory)
            wrapper = build / "main.cpp"
            binary = build / "judge"
            wrapper.write_text(source)
            compiler = shlex.split(os.environ.get("CXX", "g++"))
            result = subprocess.run([*compiler, "-std=c++17", "-O2", "-Wall", "-Wextra",
                                     str(wrapper), "-o", str(binary)])
            if result.returncode:
                return verdict("CE", color)
            with subprocess.Popen([str(binary)], start_new_session=True) as process:
                try:
                    returncode = process.wait(timeout=args.timeout)
                except subprocess.TimeoutExpired:
                    os.killpg(process.pid, signal.SIGKILL)
                    process.wait()
                    raise
            if returncode < 0:
                print(f"Runtime error: signal {-returncode}", file=sys.stderr)
                return verdict("RE", color)
            return verdict({0: "AC", 1: "WA", 2: "JE", 3: "RE", 4: "TLE"}.get(returncode, "RE"), color)
    except subprocess.TimeoutExpired:
        print(f"Time limit exceeded: {args.timeout}s for the whole run", file=sys.stderr)
        print("\nTotal runtime: ---\nMax single-case runtime: ---")
        return verdict("TLE", color)
    except OSError as error:
        print(f"Judge error: {error}", file=sys.stderr)
        return verdict("JE", color)


if __name__ == "__main__":
    sys.exit(main())
