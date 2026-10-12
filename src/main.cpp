// =============================================================================
//  Systems Programming - Task Nr. 2, release v0.2
//
//  What changed since v0.1
//    * the single main.cpp was split into *.h / *.cpp units;
//    * every error path now throws a typed exception (see exceptions.h);
//    * random student files can be generated (10k, 100k, 1M, 10M records);
//    * students are divided into "failed" (< 5.00) and "passed" (>= 5.00)
//      and written into two separate files;
//    * every stage of the run is timed with <chrono>.
//
//  Container used in this release: std::vector only.
// =============================================================================

#include "exceptions.h"
#include "fileio.h"
#include "generator.h"
#include "person.h"
#include "processing.h"
#include "timer.h"

#include <iomanip>
#include <iostream>
#include <limits>
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

/// read -> sort -> split -> write, with every stage measured separately.
void menuProcessFile() {
    const std::string path = readLine("Input file [students10000.txt]: ", "students10000.txt");

    std::vector<Person> all;
    std::vector<Person> failed;
    std::vector<Person> passed;

    Timer timer;
    readStudents(path, all);
    const double tRead = timer.seconds();

    timer.reset();
    sortByPoints(all, g_mode);
    const double tSort = timer.seconds();

    timer.reset();
    splitStudents(all, failed, passed, g_mode);
    const double tSplit = timer.seconds();

    timer.reset();
    writeStudents("failed.txt", failed, g_mode);
    writeStudents("passed.txt", passed, g_mode);
    const double tWrite = timer.seconds();

    std::cout << std::fixed << std::setprecision(4)
              << "\nRecords : " << all.size()
              << "\n  read  : " << tRead  << " s"
              << "\n  sort  : " << tSort  << " s"
              << "\n  split : " << tSplit << " s"
              << "\n  write : " << tWrite << " s"
              << "\n  TOTAL : " << tRead + tSort + tSplit + tWrite << " s"
              << "\n\nfailed: " << failed.size() << " -> failed.txt"
              << "\npassed: " << passed.size() << " -> passed.txt\n";
}

} // namespace

int main() {
    std::ios::sync_with_stdio(false);

    while (true) {
        std::cout << "\n===== Student points analyzer  v0.2 =====\n"
                  << "1. Generate random test files\n"
                  << "2. Read a file and show the table\n"
                  << "3. Enter students from the keyboard\n"
                  << "4. Process a file: sort, split, write two files, measure time\n"
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
            case 3: {
                const int n = readInt("How many students? ", 0);
                std::vector<Person> students;
                for (int i = 0; i < n; ++i) {
                    std::cout << "\nStudent " << i + 1 << ":\n";
                    Person person;
                    std::cin >> person;
                    students.push_back(person);
                }
                sortAlphabetically(students);
                printTable(students);
                break;
            }
            case 4: menuProcessFile(); break;
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
