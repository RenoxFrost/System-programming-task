#include "benchmark.h"

namespace sp {

void printResultHeader() {
    std::cout << '\n'
              << std::left << std::setw(14) << "Container"
              << std::right
              << std::setw(11) << "Records"
              << std::setw(10) << "Read s"
              << std::setw(10) << "Sort s"
              << std::setw(10) << "Split s"
              << std::setw(10) << "Write s"
              << std::setw(11) << "Total s" << '\n'
              << std::string(76, '-') << '\n';
}

void printResult(const Result& r) {
    std::cout << std::left << std::setw(14) << r.container
              << std::right << std::setw(11) << r.records
              << std::fixed << std::setprecision(4)
              << std::setw(10) << r.read
              << std::setw(10) << r.sort
              << std::setw(10) << r.split
              << std::setw(10) << r.write
              << std::setw(11) << r.total() << '\n';
}

std::vector<Result> runAllContainers(const std::string& inputPath, GradeMode mode) {
    std::vector<Result> results;
    results.push_back(runPipeline<std::vector<Person>>(inputPath, mode, "out_vector"));
    printResult(results.back());
    results.push_back(runPipeline<std::list<Person>>(inputPath, mode, "out_list"));
    printResult(results.back());
    results.push_back(runPipeline<std::deque<Person>>(inputPath, mode, "out_deque"));
    printResult(results.back());
    return results;
}

} // namespace sp
