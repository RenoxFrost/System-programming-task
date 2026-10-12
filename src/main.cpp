// =============================================================================
//  Systems Programming - Tasks 1-3, release v1.0
//  Student points analyzer.
//
//  final = 0.4 * homework(average|median) + 0.6 * exam
//  A student with fewer than 5.00 points is "failed", otherwise "passed".
// =============================================================================

#include "benchmark.h"
#include "exceptions.h"
#include "fileio.h"
#include "generator.h"
#include "person.h"
#include "processing.h"
#include "timer.h"

#include <deque>
#include <iostream>
#include <limits>
#include <list>
#include <string>
#include <vector>

namespace {

using namespace sp;

GradeMode g_mode = GradeMode::Average;

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int readInt(const std::string& prompt, int fallback) {
    std::cout << prompt;
    int value = 0;
    if (!(std::cin >> value)) {
        clearInput();
        return fallback;
    }
    return value;
}

std::string readLine(const std::string& prompt, const std::string& fallback) {
    std::cout << prompt;
    std::string line;
    clearInput();
    std::getline(std::cin, line);
    return line.empty() ? fallback : line;
}

// -----------------------------------------------------------------------------
void menuGenerate() {
    std::cout << "\nRecord counts: 1) 1 000  2) 10 000  3) 100 000  "
                 "4) 1 000 000  5) 10 000 000  6) all  7) custom\n";
    const int choice = readInt("Your choice: ", 0);
    const auto& counts = standardRecordCounts();

    std::vector<std::size_t> todo;
    if (choice >= 1 && choice <= 5)      todo.push_back(counts[static_cast<std::size_t>(choice - 1)]);
    else if (choice == 6)                todo = counts;
    else if (choice == 7)                todo.push_back(static_cast<std::size_t>(readInt("How many records? ", 1000)));
    else { std::cout << "Unknown choice.\n"; return; }

    const int hw = readInt("Homework results per student [5]: ", 5);
    for (std::size_t count : todo) {
        const std::string path = defaultFileName(count);
        std::cout << "  generating " << path << " ... " << std::flush;
        const double seconds = generateStudentFile(path, count, hw > 0 ? hw : 5);
        std::cout << "done in " << std::fixed << std::setprecision(3) << seconds << " s\n";
    }
}

// -----------------------------------------------------------------------------
void menuShowFile() {
    const std::string path = readLine("File name [Students.txt]: ", "Students.txt");
    std::vector<Person> students;
    readStudents(path, students);
    sortAlphabetically(students);
    printTable(students);
}

// -----------------------------------------------------------------------------
void menuManualInput() {
    const int n = readInt("How many students? ", 0);
    if (n <= 0) { std::cout << "Nothing to enter.\n"; return; }

    std::vector<Person> students;
    students.reserve(static_cast<std::size_t>(n));
    for (int i = 0; i < n; ++i) {
        std::cout << "\nStudent " << i + 1 << ":\n";
        Person person;
        std::cin >> person;
        students.push_back(person);
    }
    sortAlphabetically(students);
    printTable(students);

    if (readLine("Save the two result files? (y/n) [n]: ", "n") == "y") {
        std::vector<Person> failed;
        std::vector<Person> passed;
        splitCopyToTwo(students, failed, passed, g_mode, Impl::Algorithms);
        writeStudents("manual_failed.txt", failed, g_mode);
        writeStudents("manual_passed.txt", passed, g_mode);
        std::cout << "Written manual_failed.txt (" << failed.size() << ") and "
                     "manual_passed.txt (" << passed.size() << ").\n";
    }
}

// -----------------------------------------------------------------------------
void menuSingleRun() {
    const std::string path = readLine("Input file [students1000.txt]: ", "students1000.txt");

    std::cout << "Container: 1) std::vector  2) std::list  3) std::deque\n";
    const int container = readInt("Your choice [1]: ", 1);

    std::cout << "Strategy : 1) copy into two new containers  "
                 "2) move failed out and shrink the base\n";
    const Strategy strategy = (readInt("Your choice [1]: ", 1) == 2) ? Strategy::MoveFailed
                                                                     : Strategy::CopyToTwo;

    std::cout << "Code path: 1) hand written loops  2) <algorithm> library\n";
    const Impl impl = (readInt("Your choice [2]: ", 2) == 1) ? Impl::Loops : Impl::Algorithms;

    const std::string prefix = "out_single";
    Result result;
    switch (container) {
    case 2:  result = runPipeline<std::list<Person>>(path, g_mode, strategy, impl, prefix); break;
    case 3:  result = runPipeline<std::deque<Person>>(path, g_mode, strategy, impl, prefix); break;
    default: result = runPipeline<std::vector<Person>>(path, g_mode, strategy, impl, prefix); break;
    }

    printResultHeader();
    printResult(result);
    std::cout << "\nfailed: " << result.failed << "   passed: " << result.passed
              << "\nFiles written: " << prefix << "_failed.txt, " << prefix << "_passed.txt\n";
}

// -----------------------------------------------------------------------------
void menuBenchmark() {
    const std::string path = readLine("Input file [students1000.txt]: ", "students1000.txt");
    printResultHeader();
    const std::vector<Result> results = runAllCombinations(path, g_mode);

    if (readLine("Append the table to benchmark_results.md? (y/n) [y]: ", "y") != "n") {
        writeMarkdownReport("benchmark_results.md", results);
        std::cout << "Appended to benchmark_results.md\n";
    }
}

// -----------------------------------------------------------------------------
void menuMode() {
    std::cout << "Final grade from: 1) homework average  2) homework median\n";
    g_mode = (readInt("Your choice [1]: ", 1) == 2) ? GradeMode::Median : GradeMode::Average;
    std::cout << "Mode set to " << (g_mode == GradeMode::Average ? "average" : "median") << ".\n";
}

// -----------------------------------------------------------------------------
int runCommandLine(int argc, char** argv) {
    const std::string command = argv[1];

    if (command == "--generate" && argc >= 3) {
        const auto records = static_cast<std::size_t>(std::stoull(argv[2]));
        const int hw = (argc >= 4) ? std::stoi(argv[3]) : 5;
        const std::string path = defaultFileName(records);
        const double seconds = generateStudentFile(path, records, hw);
        std::cout << "Generated " << path << " in " << std::fixed << std::setprecision(3)
                  << seconds << " s\n";
        return 0;
    }

    if (command == "--bench" && argc >= 3) {
        printResultHeader();
        const bool writeFiles = !(argc >= 4 && std::string(argv[3]) == "--no-write");
        const std::vector<Result> results = runAllCombinations(argv[2], g_mode, writeFiles);
        writeMarkdownReport("benchmark_results.md", results);
        std::cout << "\nTable appended to benchmark_results.md\n";
        return 0;
    }

    if (command == "--run" && argc >= 6) {
        // --run FILE vector|list|deque 1|2 loops|algo
        const std::string file      = argv[2];
        const std::string container = argv[3];
        const Strategy strategy = (std::string(argv[4]) == "2") ? Strategy::MoveFailed
                                                                : Strategy::CopyToTwo;
        const Impl impl = (std::string(argv[5]) == "loops") ? Impl::Loops : Impl::Algorithms;

        Result result;
        if (container == "list")
            result = runPipeline<std::list<Person>>(file, g_mode, strategy, impl, "run_list");
        else if (container == "deque")
            result = runPipeline<std::deque<Person>>(file, g_mode, strategy, impl, "run_deque");
        else
            result = runPipeline<std::vector<Person>>(file, g_mode, strategy, impl, "run_vector");

        printResultHeader();
        printResult(result);
        std::cout << "failed: " << result.failed << "  passed: " << result.passed << '\n';
        return 0;
    }

    if (command == "--show" && argc >= 3) {
        std::vector<Person> students;
        readStudents(argv[2], students);
        sortAlphabetically(students);
        printTable(students);
        return 0;
    }

    std::cout << "Usage:\n"
              << "  students                         interactive menu\n"
              << "  students --generate N [HW]       create studentsN.txt\n"
              << "  students --bench FILE [--no-write]  run every combination\n"
              << "  students --run FILE CONTAINER STRATEGY IMPL   one single run\n"
              << "        CONTAINER vector|list|deque   STRATEGY 1|2   IMPL loops|algo\n"
              << "  students --show FILE             print a formatted table\n";
    return 1;
}

} // namespace

// =============================================================================
int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);

    try {
        if (argc > 1) return runCommandLine(argc, argv);

        while (true) {
            std::cout << "\n===== Student points analyzer  v1.0 =====\n"
                      << "  current final grade mode: "
                      << (g_mode == GradeMode::Average ? "homework average" : "homework median")
                      << "\n"
                      << "1. Generate random test files\n"
                      << "2. Read a file and show the table\n"
                      << "3. Enter students from the keyboard\n"
                      << "4. Process one file (choose container / strategy / code path)\n"
                      << "5. Full benchmark on one file (all combinations)\n"
                      << "6. Change the final grade mode\n"
                      << "0. Exit\n"
                      << "Your choice: ";

            int choice = 0;
            if (!(std::cin >> choice)) { clearInput(); std::cout << "Unknown choice.\n"; continue; }

            try {
                switch (choice) {
                case 1: menuGenerate();    break;
                case 2: menuShowFile();    break;
                case 3: menuManualInput(); break;
                case 4: menuSingleRun();   break;
                case 5: menuBenchmark();   break;
                case 6: menuMode();        break;
                case 0: std::cout << "Bye.\n"; return 0;
                default: std::cout << "Unknown choice.\n"; break;
                }
            } catch (const AppError& e) {
                std::cout << "\n[!] " << e.what() << '\n';
            }
        }
    } catch (const AppError& e) {
        std::cerr << "[!] " << e.what() << '\n';
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "[!] Unexpected error: " << e.what() << '\n';
        return 3;
    }
}
