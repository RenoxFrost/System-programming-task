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

struct Result {
    std::string container;
    std::size_t records = 0;
    double      read    = 0.0;
    double      sort    = 0.0;
    double      split   = 0.0;
    double      write   = 0.0;
    std::size_t failed  = 0;
    std::size_t passed  = 0;

    double total() const { return read + sort + split + write; }
};

template <typename Container> struct ContainerName;
template <> struct ContainerName<std::vector<Person>> { static constexpr const char* value = "std::vector"; };
template <> struct ContainerName<std::list<Person>>   { static constexpr const char* value = "std::list";   };
template <> struct ContainerName<std::deque<Person>>  { static constexpr const char* value = "std::deque";  };

/// read -> sort -> split -> write, every stage timed separately.
template <typename Container>
Result runPipeline(const std::string& inputPath, GradeMode mode,
                   const std::string& outputPrefix) {
    Result result;
    result.container = ContainerName<Container>::value;

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
    splitStudents(all, failed, passed, mode);
    result.split = timer.seconds();

    timer.reset();
    writeStudents(outputPrefix + "_failed.txt", failed, mode);
    writeStudents(outputPrefix + "_passed.txt", passed, mode);
    result.write = timer.seconds();

    result.failed = failed.size();
    result.passed = passed.size();
    return result;
}

void printResultHeader();
void printResult(const Result& result);
std::vector<Result> runAllContainers(const std::string& inputPath, GradeMode mode);

} // namespace sp

#endif // SP_BENCHMARK_H
