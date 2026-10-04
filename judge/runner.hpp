#pragma once
#include <bits/stdc++.h>
#include <sys/wait.h>
#include <unistd.h>

namespace judge {
struct Options {
    int cases = 100;
    unsigned seed = 1;
};

template<class T> std::string preview(const T& value) {
    std::ostringstream out;
    out << value;
    auto text = out.str();
    if (text.empty()) return "<empty>";
    return text.size() > 100 ? text.substr(0, 100) + "..." : text;
}

class Runner {
    int passed = 0, wrong = 0, errors = 0, timeouts = 0, skipped = 0;
    int planned = 0, executed = 0;
    bool blocked = false, color;
    double total_ms = 0, max_ms = 0;
    bool timing_complete = true;
    bool progress_open = false;
    std::chrono::milliseconds time_limit{2000};
    using Clock = std::chrono::steady_clock;
    Clock::time_point progress_updated{};

    void close_progress() {
        if (progress_open) std::cout << '\n';
        progress_open = false;
    }

    static std::string runtime(double ms) {
        std::ostringstream out;
        out << std::fixed << std::setprecision(3) << ms << " ms";
        return out.str();
    }

    std::string paint(const std::string& text, const char* code) const {
        return color ? std::string("\033[") + code + "m" + text + "\033[0m" : text;
    }

    template<class Args> static auto invoke(Args& args) {
        Solution solver;
        return std::apply([&](auto&... values) { return solver.solve(values...); }, args);
    }

    // The child reports its verdict through its exit status. No answer
    // serialization is needed, so custom checkers and return types still work.
    template<class Args, class Expected, class Check>
    int evaluate(Args& args, const Expected& expected, Check& check, double& measured_ms) const {
        std::optional<decltype(invoke(args))> got;
        auto start = Clock::now();
        try {
            got.emplace(invoke(args));
        } catch (const std::exception& e) {
            std::cout << " | " << paint("RE", "1;35") << " | " << e.what() << " | ---\n";
            return 3;
        } catch (...) {
            std::cout << " | " << paint("RE", "1;35") << " | unknown exception | ---\n";
            return 3;
        }
        double ms = std::chrono::duration<double, std::milli>(Clock::now() - start).count();
        if (ms >= time_limit.count()) {
            std::cout << " | " << paint("TLE", "1;33")
                      << " | --- | exceeded " << time_limit.count() << " ms\n";
            return 4;
        }
        bool ok = check(*got, expected);
        measured_ms = ms;
        std::cout << " | " << paint(ok ? "AC" : "WA", ok ? "1;32" : "1;31")
                  << " | " << std::fixed << std::setprecision(3) << ms << " ms\n";
        if (!ok)
            std::cout << "        expected: " << preview(expected)
                      << "\n        received: " << preview(*got) << '\n';
        return ok ? 0 : 1;
    }

public:
    explicit Runner(bool use_color = false) : color(use_color) {}

    // Update in place on terminals; redirected logs get only start/end lines.
    void preparing(int done, int total) {
        if (total <= 0 || done < 0 || done > total)
            throw std::invalid_argument("invalid preparation progress");
        bool live = isatty(STDOUT_FILENO);
        auto now = Clock::now();
        if (!live && done != 0 && done != total) return;
        if (live && progress_open && done != total &&
            now - progress_updated < std::chrono::milliseconds(50)) return;
        constexpr int width = 24;
        int filled = static_cast<long long>(done) * width / total;
        std::cout << (progress_open ? "\r" : "")
                  << "  Preparing cases [" << std::string(filled, '#')
                  << std::string(width - filled, '-') << "] "
                  << done << '/' << total << " ("
                  << static_cast<long long>(done) * 100 / total << "%)" << std::flush;
        progress_open = true;
        progress_updated = now;
        if (!live || done == total) close_progress();
    }

    void set_time_limit(std::chrono::milliseconds limit) {
        if (limit.count() <= 0) throw std::invalid_argument("time limit must be positive");
        time_limit = limit;
        std::cout << "  Time limit: " << limit.count() << " ms per case\n";
    }

    void section(const std::string& name, int cases = 0) {
        close_progress();
        planned = cases;
        executed = 0;
        blocked = false;
        std::cout << '\n' << paint("── " + name + " ──", "1;36") << '\n';
    }

    // Return false on the first failure so generators can stop the subtask.
    // Further calls in a failed section are ignored until section() resets it.
    template<class Args, class Expected, class Check = std::equal_to<>>
    bool test(const std::string& label, Args args, const Expected& expected,
              Check check = {}) {
        if (blocked) {
            if (!planned) ++skipped;
            return false;
        }
        close_progress();
        ++executed;
        std::cout << "  " << std::setw(4) << passed + wrong + errors + timeouts + 1
                  << "  " << label << std::flush;
        std::cerr.flush();
        // Only the child's measured solve time is returned, never fork/wait,
        // generation, oracle, checker, or reporting time.
        int timing_pipe[2];
        if (pipe(timing_pipe) < 0) throw std::runtime_error("cannot create timing pipe");
        auto start = Clock::now();
        pid_t child = fork();
        if (child < 0) {
            close(timing_pipe[0]);
            close(timing_pipe[1]);
            throw std::runtime_error("cannot start test process");
        }
        if (child == 0) {
            close(timing_pipe[0]);
            double measured_ms = -1;
            int code;
            try {
                code = evaluate(args, expected, check, measured_ms);
            } catch (const std::exception& e) {
                std::cerr << "Judge error: " << e.what() << '\n';
                code = 2;
            } catch (...) {
                std::cerr << "Judge error: unknown checker exception\n";
                code = 2;
            }
            std::cout.flush();
            std::cerr.flush();
            ssize_t sent;
            do { sent = write(timing_pipe[1], &measured_ms, sizeof(measured_ms)); }
            while (sent < 0 && errno == EINTR);
            close(timing_pipe[1]);
            _exit(code);
        }
        close(timing_pipe[1]);
        int status = 0, code = 0;
        while (true) {
            pid_t result = waitpid(child, &status, WNOHANG);
            if (result == child) {
                if (WIFEXITED(status)) code = WEXITSTATUS(status);
                else {
                    code = 3;
                    std::cout << " | " << paint("RE", "1;35")
                              << " | --- | signal " << WTERMSIG(status) << '\n';
                }
                break;
            }
            if (result < 0 && errno != EINTR) {
                kill(child, SIGKILL);
                while (waitpid(child, &status, 0) < 0 && errno == EINTR) {}
                close(timing_pipe[0]);
                throw std::runtime_error("cannot wait for test process");
            }
            if (Clock::now() - start >= time_limit) {
                kill(child, SIGKILL);
                while (waitpid(child, &status, 0) < 0 && errno == EINTR) {}
                code = 4;
                std::cout << " | " << paint("TLE", "1;33")
                          << " | --- | exceeded " << time_limit.count() << " ms\n";
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        double measured_ms = -1;
        ssize_t received = 0;
        if (code == 0 || code == 1) {
            do { received = read(timing_pipe[0], &measured_ms, sizeof(measured_ms)); }
            while (received < 0 && errno == EINTR);
        }
        close(timing_pipe[0]);
        if (received == static_cast<ssize_t>(sizeof(measured_ms)) &&
            measured_ms >= 0 && std::isfinite(measured_ms)) {
            total_ms += measured_ms;
            max_ms = std::max(max_ms, measured_ms);
        } else timing_complete = false;
        if (code == 2) throw std::runtime_error("case evaluation failed");
        if (code == 0) ++passed;
        else if (code == 1) ++wrong;
        else if (code == 4) ++timeouts;
        else ++errors;
        if (code != 0) {
            blocked = true;
            int remaining = std::max(0, planned - executed);
            skipped += remaining;
            std::cout << paint("  Skipping remaining cases in this subtask", "33");
            if (planned) std::cout << " (" << remaining << ")";
            std::cout << ".\n";
        }
        return code == 0;
    }

    // Internal codes consumed by judge.py: 0 AC, 1 WA, 2 JE, 3 RE, 4 TLE.
    int finish() {
        close_progress();
        if (passed + wrong + errors + timeouts == 0) {
            std::cerr << "Judge error: no test cases were run\n";
            return 2;
        }
        std::cout << '\n' << paint("────────────────────────────────────────────────", "36")
                  << "\nSummary: " << passed << " passed, " << wrong + errors + timeouts << " failed.\n"
                  << "  " << paint("AC " + std::to_string(passed), "32")
                  << "  ·  " << paint("WA " + std::to_string(wrong), "31")
                  << "  ·  " << paint("RE " + std::to_string(errors), "35")
                  << "  ·  " << paint("TLE " + std::to_string(timeouts), "33")
                  << "  ·  Skipped " << skipped
                  << "\nTotal runtime: " << (timing_complete ? runtime(total_ms) : "---")
                  << "\nMax single-case runtime: " << (timing_complete ? runtime(max_ms) : "---")
                  << '\n';
        return errors ? 3 : timeouts ? 4 : wrong ? 1 : 0;
    }
};
} // namespace judge
