#ifndef SP_BENCHMARK_H
#define SP_BENCHMARK_H

#include "fileio.h"
#include "person.h"
#include "processing.h"
#include "timer.h"

#include <deque>
#include <iomanip>
#include <iostream>
#include <list>
#include <string>
#include <vector>

namespace sp {

/// Timings of one complete run, in seconds.
struct Result {
    std::string container;
    Strategy    strategy = Strategy::CopyToTwo;
    Impl        impl     = Impl::Loops;
    std::size_t records  = 0;
    double      read     = 0.0;
    double      sort     = 0.0;
    double      split    = 0.0;
    double      write    = 0.0;
    std::size_t failed   = 0;
    std::size_t passed   = 0;

    double total() const { return read + sort + split + write; }
};

inline const char* strategyName(Strategy s) {
    return (s == Strategy::CopyToTwo) ? "1 (copy to two)" : "2 (move failed)";
}

inline const char* implName(Impl i) {
    return (i == Impl::Loops) ? "loops" : "algorithms";
}

template <typename Container> struct ContainerName;
template <> struct ContainerName<std::vector<Person>> { static constexpr const char* value = "std::vector"; };
template <> struct ContainerName<std::list<Person>>   { static constexpr const char* value = "std::list";   };
template <> struct ContainerName<std::deque<Person>>  { static constexpr const char* value = "std::deque";  };

// =============================================================================
//  One full run: read -> sort -> split -> write two files.
//  Every stage is timed separately, which is what the task asks for.
// =============================================================================
template <typename Container>
Result runPipeline(const std::string& inputPath,
                   GradeMode mode,
                   Strategy strategy,
                   Impl impl,
                   const std::string& outputPrefix,
                   bool writeFiles = true) {
    Result result;
    result.container = ContainerName<Container>::value;
    result.strategy  = strategy;
    result.impl      = impl;

    Container all;
    Container failed;
    Container passed;

    Timer timer;
    readStudents(inputPath, all);
    result.read    = timer.seconds();
    result.records = all.size();

    timer.reset();
    sortByPoints(all, mode);
    result.sort = timer.seconds();

    timer.reset();
    if (strategy == Strategy::CopyToTwo) {
        splitCopyToTwo(all, failed, passed, mode, impl);
    } else {
        splitMoveFailed(all, failed, mode, impl);
    }
    result.split = timer.seconds();

    const Container& passedRef = (strategy == Strategy::CopyToTwo) ? passed : all;
    result.failed = failed.size();
    result.passed = passedRef.size();

    if (writeFiles) {
        timer.reset();
        writeStudents(outputPrefix + "_failed.txt", failed, mode);
        writeStudents(outputPrefix + "_passed.txt", passedRef, mode);
        result.write = timer.seconds();
    }

    return result;
}

/// Prints the header of the results table.
void printResultHeader();

/// Prints one measured run.
void printResult(const Result& result);

/// Runs every container / strategy / implementation combination on one file.
std::vector<Result> runAllCombinations(const std::string& inputPath,
                                       GradeMode mode,
                                       bool writeFiles = true);

/// Writes a Markdown version of the table, ready to paste into README.md.
void writeMarkdownReport(const std::string& path,
                         const std::vector<Result>& results);

} // namespace sp

#endif // SP_BENCHMARK_H
