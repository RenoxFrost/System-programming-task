#include "benchmark.h"

#include "exceptions.h"

#include <fstream>

namespace sp {
namespace {

/// Above this many records the quadratic erase loop is not run any more.
constexpr std::size_t kQuadraticLimit = 200000;

/// Rough extrapolation from the measured 100 000 record run (about 19 s).
double estimateQuadraticSeconds(std::size_t records) {
    const double ratio = static_cast<double>(records) / 100000.0;
    return 19.0 * ratio * ratio;
}

} // namespace

void printResultHeader() {
    std::cout << '\n'
              << std::left << std::setw(13) << "Container"
              << std::setw(18) << "Strategy"
              << std::setw(13) << "Impl"
              << std::right
              << std::setw(11) << "Records"
              << std::setw(10) << "Read s"
              << std::setw(10) << "Sort s"
              << std::setw(10) << "Split s"
              << std::setw(10) << "Write s"
              << std::setw(11) << "Total s" << '\n'
              << std::string(106, '-') << '\n';
}

void printResult(const Result& r) {
    std::cout << std::left << std::setw(13) << r.container
              << std::setw(18) << strategyName(r.strategy)
              << std::setw(13) << implName(r.impl)
              << std::right << std::setw(11) << r.records
              << std::fixed << std::setprecision(4)
              << std::setw(10) << r.read
              << std::setw(10) << r.sort
              << std::setw(10) << r.split
              << std::setw(10) << r.write
              << std::setw(11) << r.total() << '\n';
}

std::vector<Result> runAllCombinations(const std::string& inputPath,
                                       GradeMode mode,
                                       bool writeFiles) {
    std::vector<Result> results;
    const Strategy strategies[] = {Strategy::CopyToTwo, Strategy::MoveFailed};
    const Impl impls[] = {Impl::Loops, Impl::Algorithms};

    // Strategy 2 written as a hand rolled erase loop is quadratic for vector
    // and deque: every erase shifts the whole tail. At 100 000 records it
    // already needs ~20 s, at 1 000 000 it would need roughly half an hour,
    // so above this limit that one combination is reported as skipped.
    std::size_t knownRecords = 0;

    for (Strategy strategy : strategies) {
        for (Impl impl : impls) {
            const bool quadratic = (strategy == Strategy::MoveFailed && impl == Impl::Loops);
            const bool skipQuadratic = quadratic && knownRecords > kQuadraticLimit;
            // The same two output files are reused by every combination:
            // the measurement is what matters here, not keeping 24 copies of
            // the same data on disk.
            if (skipQuadratic) {
                std::cout << std::left << std::setw(13) << "std::vector"
                          << std::setw(18) << strategyName(strategy)
                          << std::setw(13) << implName(impl)
                          << "  skipped - quadratic, would need about "
                          << std::fixed << std::setprecision(0)
                          << estimateQuadraticSeconds(knownRecords) << " s\n";
            } else {
                results.push_back(runPipeline<std::vector<Person>>(
                    inputPath, mode, strategy, impl, "bench_vector", writeFiles));
                knownRecords = results.back().records;
                printResult(results.back());
            }

            results.push_back(runPipeline<std::list<Person>>(
                inputPath, mode, strategy, impl, "bench_list", writeFiles));
            knownRecords = results.back().records;
            printResult(results.back());

            if (skipQuadratic) {
                std::cout << std::left << std::setw(13) << "std::deque"
                          << std::setw(18) << strategyName(strategy)
                          << std::setw(13) << implName(impl)
                          << "  skipped - quadratic, would need about "
                          << std::fixed << std::setprecision(0)
                          << estimateQuadraticSeconds(knownRecords) << " s\n";
            } else {
                results.push_back(runPipeline<std::deque<Person>>(
                    inputPath, mode, strategy, impl, "bench_deque", writeFiles));
                printResult(results.back());
            }
        }
    }
    return results;
}

void writeMarkdownReport(const std::string& path, const std::vector<Result>& results) {
    std::ofstream out(path, std::ios::app);
    if (!out) throw FileError("cannot open " + path + " for writing");

    out << "\n| Container | Strategy | Implementation | Records | Read s | Sort s | "
           "Split s | Write s | Total s |\n"
        << "|---|---|---|---:|---:|---:|---:|---:|---:|\n"
        << std::fixed << std::setprecision(4);

    for (const Result& r : results) {
        out << "| `" << r.container << "` | " << strategyName(r.strategy) << " | "
            << implName(r.impl) << " | " << r.records << " | " << r.read << " | "
            << r.sort << " | " << r.split << " | " << r.write << " | " << r.total()
            << " |\n";
    }
    out.flush();
}

} // namespace sp
