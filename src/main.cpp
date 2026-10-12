// =============================================================================
//  Systems Programming - Task Nr. 2, release v0.25
//  Same program as v0.2, but the whole pipeline is a template, so the very
//  same code runs with std::vector, std::list and std::deque and the three
//  containers can be compared on identical data.
// =============================================================================

#include "benchmark.h"
#include "exceptions.h"
#include "fileio.h"
#include "generator.h"
#include "person.h"
#include "processing.h"

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
    if (!(std::cin >> value)) { clearInput(); return fallback; }
    return value;
}

std::string readLine(const std::string& prompt, const std::string& fallback) {
    std::cout << prompt;
    std::string line;
    clearInput();
    std::getline(std::cin, line);
    return line.empty() ? fallback : line;
}

void menuGenerate() {
    std::cout << "\n1) 10 000  2) 100 000  3) 1 000 000  4) 10 000 000  5) all four  6) custom\n";
    const int choice = readInt("Your choice: ", 0);
    const std::vector<std::size_t> counts{10000, 100000, 1000000, 10000000};

    std::vector<std::size_t> todo;
    if (choice >= 1 && choice <= 4) todo.push_back(counts[static_cast<std::size_t>(choice - 1)]);
    else if (choice == 5)           todo = counts;
    else if (choice == 6)           todo.push_back(static_cast<std::size_t>(readInt("How many records? ", 1000)));
    else { std::cout << "Unknown choice.\n"; return; }

    for (std::size_t count : todo) {
        const std::string path = defaultFileName(count);
        std::cout << "  generating " << path << " ... " << std::flush;
        std::cout << "done in " << std::fixed << std::setprecision(3)
                  << generateStudentFile(path, count, 5) << " s\n";
    }
}

void menuOneContainer() {
    const std::string path = readLine("Input file [students10000.txt]: ", "students10000.txt");
    std::cout << "Container: 1) std::vector  2) std::list  3) std::deque\n";
    const int container = readInt("Your choice [1]: ", 1);

    Result result;
    switch (container) {
    case 2:  result = runPipeline<std::list<Person>>(path, g_mode, "out_list");   break;
    case 3:  result = runPipeline<std::deque<Person>>(path, g_mode, "out_deque"); break;
    default: result = runPipeline<std::vector<Person>>(path, g_mode, "out_vector"); break;
    }
    printResultHeader();
    printResult(result);
    std::cout << "\nfailed: " << result.failed << "   passed: " << result.passed << '\n';
}

} // namespace

int main() {
    std::ios::sync_with_stdio(false);

    while (true) {
        std::cout << "\n===== Student points analyzer  v0.25 =====\n"
                  << "1. Generate random test files\n"
                  << "2. Read a file and show the table\n"
                  << "3. Process one file with a chosen container\n"
                  << "4. Compare all three containers on one file\n"
                  << "5. Change the final grade mode (now: "
                  << (g_mode == GradeMode::Average ? "average" : "median") << ")\n"
                  << "0. Exit\n"
                  << "Your choice: ";

        int choice = 0;
        if (!(std::cin >> choice)) { clearInput(); continue; }

        try {
            switch (choice) {
            case 1: menuGenerate(); break;
            case 2: {
                const std::string path = readLine("File [Students.txt]: ", "Students.txt");
                std::vector<Person> students;
                readStudents(path, students);
                sortAlphabetically(students);
                printTable(students);
                break;
            }
            case 3: menuOneContainer(); break;
            case 4: {
                const std::string path = readLine("Input file [students10000.txt]: ",
                                                  "students10000.txt");
                printResultHeader();
                runAllContainers(path, g_mode);
                break;
            }
            case 5:
                std::cout << "1) average  2) median: ";
                g_mode = (readInt("", 1) == 2) ? GradeMode::Median : GradeMode::Average;
                break;
            case 0: std::cout << "Bye.\n"; return 0;
            default: std::cout << "Unknown choice.\n"; break;
            }
        } catch (const AppError& e) {
            std::cout << "\n[!] " << e.what() << '\n';
        } catch (const std::exception& e) {
            std::cout << "\n[!] Unexpected error: " << e.what() << '\n';
        }
    }
}
